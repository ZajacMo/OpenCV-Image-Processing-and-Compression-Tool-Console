#!/usr/bin/env python3
"""Convert this repository's legacy GBK-encoded sources to UTF-8.

The project was originally written with Visual Studio's system code page
(GBK / CP936) on a Chinese-locale Windows machine. Files that contain Chinese
comments were therefore stored as GBK bytes, which render as mojibake on
GitHub and in any UTF-8 aware editor.

This script performs the conversion in place:

  * ``.cpp`` / ``.h`` files become UTF-8 **with BOM** so that MSVC detects the
    encoding automatically (a BOM-less UTF-8 file is guessed as the system
    code page, which would re-introduce mojibake inside the IDE).
  * ``.md`` / ``.txt`` files become UTF-8 **without** BOM, which is what
    GitHub and CommonMark expect.
  * Files that are already valid UTF-8 are left untouched.

Usage::

    python tools/convert_gbk_to_utf8.py            # convert in place
    python tools/convert_gbk_to_utf8.py --check    # report only, change nothing

The script is idempotent: running it twice leaves the tree unchanged.
"""

from __future__ import annotations

import argparse
import pathlib
import sys

# Text files that may contain legacy-encoded bytes.
TEXT_SUFFIXES = {".cpp", ".h", ".md", ".txt", ".hpp", ".c", ".cc", ".rc"}

# Files whose encoding must carry a BOM for MSVC to pick it up automatically.
BOM_SUFFIXES = {".cpp", ".h", ".hpp", ".c", ".cc", ".rc"}

REPO_ROOT = pathlib.Path(__file__).resolve().parent.parent

# Directories that never contain hand-written sources.
SKIP_DIRS = {".git", ".vs", "Debug", "Release", "x64", "build", "images", "tests"}


def detect(raw: bytes) -> str:
    """Return the encoding of *raw*.

    Tries UTF-8 first (already converted), then GBK. Raises ``UnicodeDecodeError``
    if neither works.
    """
    try:
        raw.decode("utf-8")
    except UnicodeDecodeError:
        return "gbk"
    return "utf-8"


def convert(path: pathlib.Path, check_only: bool) -> bool:
    """Convert a single file. Returns True when the file was (or would be) changed."""
    raw = path.read_bytes()
    encoding = detect(raw)

    if encoding == "utf-8":
        # Already UTF-8. Only add a BOM where one is required but missing.
        if path.suffix.lower() in BOM_SUFFIXES and not raw.startswith(b"\xef\xbb\xbf"):
            if not check_only:
                path.write_bytes(b"\xef\xbb\xbf" + raw)
            print(f"  +BOM   {path.relative_to(REPO_ROOT)}")
            return True
        return False

    text = raw.decode("gbk")
    if path.suffix.lower() in BOM_SUFFIXES:
        payload = b"\xef\xbb\xbf" + text.encode("utf-8")
    else:
        payload = text.encode("utf-8")

    if not check_only:
        path.write_bytes(payload)

    print(f"  GBK->UTF-8 {'(BOM)' if path.suffix.lower() in BOM_SUFFIXES else '     '} "
          f"{path.relative_to(REPO_ROOT)}")
    return True


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--check",
        action="store_true",
        help="report which files would change without writing anything",
    )
    args = parser.parse_args()

    targets = sorted(
        p
        for p in REPO_ROOT.rglob("*")
        if p.is_file()
        and p.suffix.lower() in TEXT_SUFFIXES
        and not SKIP_DIRS & set(p.relative_to(REPO_ROOT).parts)
    )

    changed = 0
    print(f"Scanning {len(targets)} text files under {REPO_ROOT}")
    for path in targets:
        try:
            if convert(path, args.check):
                changed += 1
        except (UnicodeDecodeError, OSError) as exc:
            print(f"  !! {path.relative_to(REPO_ROOT)}: {exc}", file=sys.stderr)

    verb = "would change" if args.check else "changed"
    print(f"Done: {changed} file(s) {verb}.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
