#!/usr/bin/env python3
"""Verify a reference test vector against the codec's documented format.

The vector in ``tests/vectors/`` consists of a compressed stream and the
expected decoded image. This script re-implements the decoder described in
``ALGORITHM.md`` from scratch, decodes the stream, and compares the result
against the expected image.

Its purpose is to let an implementer in another language check that their
decoder agrees with this one, without needing to build the C++ project.

Usage::

    python tools/verify_vector.py [vector-dir]
"""

from __future__ import annotations

import pathlib
import sys

REPO_ROOT = pathlib.Path(__file__).resolve().parent.parent
VECTOR_DIR = REPO_ROOT / "tests" / "vectors"


def read_bits(stream: pathlib.Path) -> str:
    """Return the stream as a string of '0'/'1' characters."""
    return stream.read_text(encoding="ascii").strip()


def decode(bits: str):
    """Decode a .nam stream, returning (rows, cols, pixels).

    ``pixels`` is a flat list of row-major greyscale values.
    """
    cursor = 0

    def read(n: int) -> int:
        nonlocal cursor
        value = 0
        for _ in range(n):
            value = (value << 1) | (1 if bits[cursor] == "1" else 0)
            cursor += 1
        return value

    rows = read(16)
    cols = read(16)

    # The encoder emits, per row, a sequence of encoded pixels. Each encoded
    # pixel starts with a "1" marker followed by a variable-length kind code
    # (1 bit selects kind 1 = rectangle start; a leading 0 selects kind 2 =
    # rectangle end or kind 3 = isolated pixel), then a position delta, then
    # the colour payload.
    #
    # A row that contains no encoded pixel at all is terminated by a single "0".
    def read_kind() -> int:
        # Mirrors Compress_gray::uncompress(): one bit selects kind 1,
        # otherwise a second bit distinguishes kinds 2 and 3.
        if read(1):
            return 1
        return 2 if read(1) else 3

    # First pass: recover the encoding map and the recorded corner colours.
    encoded = [[0] * cols for _ in range(rows)]
    blocks = []  # (row, col, colors)
    row = 0
    col = 0
    is_null = True

    while cursor < len(bits):
        if bits[cursor] == "0":
            # End of an encoded row.
            cursor += 1
            row += 1
            if row >= rows:
                break
            is_null = True
            col = 0
            continue

        cursor += 1  # the "1" marker
        kind = read_kind()

        # The width of the position delta depends on the remaining columns.
        b = 1
        while (1 << b) < (cols - col - (0 if is_null else 1)):
            b += 1

        gap = read(b)
        col += gap + (0 if is_null else 1)
        is_null = False

        encoded[row][col] = kind

        if kind == 3:
            colors = [read(8)]
        elif kind == 1:
            colors = [read(8) for _ in range(4)]
        else:
            colors = []

        if kind in (1, 3):
            blocks.append((row, col, colors))

        if col == cols - 1:
            row += 1
            is_null = True
            col = 0

    # Second pass: rebuild the blocks, mirroring Compress_gray::decode().
    pixels = [0] * (rows * cols)
    marked = [[False] * cols for _ in range(rows)]

    def gamma(a: int, b: int, i: int) -> float:
        if b == a:
            return 0.0
        return (i - a) / float(b - a)

    def median(a: float, b: float, rate: float) -> int:
        return int(a + (b - a) * rate)

    block_iter = iter(blocks)
    for i in range(rows):
        for j in range(cols):
            if marked[i][j]:
                continue
            kind = encoded[i][j]
            try:
                row_b, col_b, colors = next(block_iter)
            except StopIteration:
                return rows, cols, pixels  # truncated stream

            def is_peer(lt_x, lt_y, rb_x, rb_y):
                """Port of Compress_gray::isPeer."""
                if marked[rb_x][rb_y] or encoded[rb_x][rb_y] not in (0, 2):
                    return False
                # Walk down the column that contains the candidate corner.
                for y in range(lt_x, rb_x):
                    if marked[y][rb_y] or (
                        encoded[y][rb_y] and not (lt_x == y and lt_y == rb_y)
                    ):
                        return False
                # Walk across the row that contains the candidate corner.
                for x in range(lt_y, rb_y):
                    if marked[rb_x][x] or (
                        encoded[rb_x][x] and not (lt_x == rb_x and lt_y == x)
                    ):
                        return False
                return True

            rb_y = j
            rb_x = i
            if kind != 3:
                # Extend horizontally.
                while rb_y + 1 < cols and is_peer(i, j, i, rb_y + 1):
                    rb_y += 1
                # Extend vertically. Note that the C++ implementation tests the
                # corner at (rb.x + 1, rb.y), i.e. it uses the already extended
                # horizontal extent rather than the original left column.
                while rb_x + 1 < rows and is_peer(i, j, rb_x + 1, rb_y):
                    rb_x += 1

            if kind == 3:
                pixels[i * cols + j] = colors[0]
            else:
                c00, c01, c10, c11 = colors
                for y in range(i, rb_x + 1):
                    gi = gamma(i, rb_x, y)
                    left = median(c00, c10, gi)
                    right = median(c01, c11, gi)
                    for x in range(j, rb_y + 1):
                        gj = gamma(j, rb_y, x)
                        pixels[y * cols + x] = median(left, right, gj)

            for y in range(i, rb_x + 1):
                for x in range(j, rb_y + 1):
                    marked[y][x] = True

    return rows, cols, pixels


def main() -> int:
    vector_dir = pathlib.Path(sys.argv[1]) if len(sys.argv) > 1 else VECTOR_DIR
    stream = vector_dir / "selftest.nam"
    expected_path = vector_dir / "selftest_decoded.pgm"

    if not stream.exists():
        print(f"error: vector not found: {stream}")
        return 1

    bits = read_bits(stream)
    rows, cols, pixels = decode(bits)

    print(f"stream       : {stream.name} ({len(bits)} bits)")
    print(f"decoded size : {rows} x {cols}")

    if expected_path.exists():
        # Parse the binary PGM (P5) file.
        raw = expected_path.read_bytes()
        parts = []
        idx = 0
        while len(parts) < 4:
            while raw[idx:idx + 1].isspace():
                idx += 1
            if raw[idx:idx + 1] == b"#":
                while raw[idx:idx + 1] != b"\n":
                    idx += 1
                continue
            start = idx
            while not raw[idx:idx + 1].isspace():
                idx += 1
            parts.append(raw[start:idx])
        idx += 1
        data = raw[idx:]

        if (int(parts[1]), int(parts[2])) != (cols, rows):
            print(f"error: expected image is {parts[1]}x{parts[2]} (width x height), "
                  f"decoded {cols}x{rows}")
            return 1

        mismatches = sum(1 for a, b in zip(pixels, data) if a != b)
        if mismatches:
            print(f"FAIL: {mismatches} of {len(data)} pixels differ from the reference")
            return 1
        print(f"OK: all {len(data)} pixels match the reference image")
    else:
        print(f"note: {expected_path.name} not present; skipping pixel comparison")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
