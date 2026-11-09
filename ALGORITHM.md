# The `.nam` Block Codec — Format Specification

This document specifies the compression format produced and consumed by this
project. It is written so that an independent implementation (in any language)
can decode a stream produced by this encoder, and produce a stream that this
decoder accepts.

Everything here was derived from the implementation in `Compress_gray.cpp`,
`Bits.cpp` and `RBlock.cpp`. Where the implementation has quirks, they are
recorded in [Known issues](#known-issues) rather than silently smoothed over.

A reference test vector is included in `tests/vectors/`, and
`tools/verify_vector.py` contains a from-scratch decoder written only from
this specification. That script is checked against the vector as part of the
build, which keeps this document honest.

---

## 1. Overview

The codec is a **lossy, block-based image codec** for 8-bit greyscale images.

The encoder scans the image and groups pixels into rectangular *blocks*. A
block is accepted when every pixel inside it can be approximated, within a
tolerance `sigma`, by the **bilinear interpolation of the block's four corner
pixels**. Only the four corner colours need to be stored; the interior is
reconstructed by interpolation.

Pixels that cannot be grouped into any block are stored as **isolated pixels**
with their exact colour.

The encoder therefore produces two kinds of records:

* **Rectangle** — a region described by its four corner colours.
* **Isolated pixel** — a single pixel stored verbatim.

The compressed stream is a flat sequence of ASCII characters `'0'` and `'1'`,
one bit per character. It is not byte-packed: one character carries one bit.

---

## 2. Input constraints

| Property | Requirement |
|---|---|
| Colour space | Greyscale, 8 bits per pixel |
| OpenCV type | `CV_8UC1` |
| Channels | 1. The encoder folds the channel count into the stored width, so a multi-channel image is accepted but is treated as a single interleaved buffer; this is **not** a colour codec. |
| Dimensions | `rows` and `cols` are stored as 16-bit unsigned values, so the maximum is 65535 rows/columns. |

The decoder always allocates a `CV_8UC1` image of the stored dimensions.

---

## 3. Encoder algorithm

### 3.1 Block detection

For a candidate rectangle with corners `lt` (top-left) and `rb` (bottom-right),
the region is a valid block when, for every pixel `(i, j)` in the rectangle:

```
|img[i][j] - predicted(i, j)| <= sigma
```

where `predicted` is the bilinear interpolation of the four corner colours.

The interpolation is separable. Writing the corner colours as

```
c00 = img[lt.x][lt.y]   c01 = img[lt.x][rb.y]
c10 = img[rb.x][lt.y]   c11 = img[rb.x][rb.y]
```

the predicted value is:

```
gamma_i = (i - lt.x) / (rb.x - lt.x)      (0 when rb.x == lt.x)
gamma_j = (j - lt.y) / (rb.y - lt.y)      (0 when rb.y == lt.y)

left(i)   = c00 + (c10 - c00) * gamma_i
right(i)  = c01 + (c11 - c01) * gamma_i

predicted(i, j) = left(i) + (right(i) - left(i)) * gamma_j
```

Note that the C++ implementation truncates the intermediate values to `uchar`
as it goes: `left` and `right` are stored in a `uchar[2][3]`, so they are
truncated to integers before being used for the final interpolation. A faithful
implementation must reproduce this truncation. See
[Known issues](#known-issues-1).

### 3.2 Growing a block

Starting from an uncovered pixel `lt`, the encoder grows the rectangle in three
steps, in this order:

1. **Diagonal**: while the candidate `rb = (rb.x + 1, rb.y + 1)` is a valid block, advance both coordinates.
2. **Horizontal**: while the candidate `rb = (rb.x, rb.y + 1)` is a valid block, advance the column.
3. **Vertical**: while the candidate `rb = (rb.x + 1, rb.y)` is a valid block, advance the row.

If the resulting rectangle is a single pixel (`lt == rb`), the pixel is
recorded as an **isolated pixel**. Otherwise it is recorded as a **rectangle**.

Every pixel of the accepted rectangle is then marked as covered, and the scan
resumes at the bottom-right column of that rectangle.

### 3.3 The encoding map

Alongside the block list, the encoder maintains a per-pixel map with four
values:

| Value | Meaning |
|---|---|
| `0` | Untouched — no block covers this pixel |
| `1` | **Rectangle start** — the top-left corner of a rectangle |
| `2` | **Rectangle end** — the bottom-right corner of a rectangle |
| `3` | **Isolated pixel** |

The map is what lets the decoder rediscover the rectangle extents without
storing them explicitly.

---

## 4. Stream format

All fields are written most-significant-bit first. The stream is a sequence of
ASCII `'0'`/`'1'` characters; one character is one bit.

### 4.1 Header

| Field | Width | Description |
|---|---|---|
| `rows` | 16 bits | Image height |
| `cols` | 16 bits | Image width in pixels (already multiplied by the channel count) |

### 4.2 Body

The body is a sequence of **rows**. Each row is either empty or contains a
list of encoded pixels.

A row that contains **no** encoded pixel is written as a single `0` bit
(`data.add(0)` at the last column of the row).

A row that contains encoded pixels is written as a sequence of records. Each
record is:

```
+--------+------------+-----------------+------------------+
| marker | kind code  | position delta  | colour payload   |
| 1 bit  | 1 or 2 bits| b bits          | 8 or 32 bits     |
+--------+------------+-----------------+------------------+
```

**Marker** — a single `1` bit, indicating that an encoded pixel follows.

**Kind code** — a variable-length code:

| Code | Meaning | Colour payload |
|---|---|---|
| `1` | Rectangle start | 4 × 8 bits — the corner colours `c00, c01, c10, c11` |
| `01` | Rectangle end | none |
| `00` | Isolated pixel | 1 × 8 bits — the pixel colour |

Read as a bit sequence: read one bit; if it is `1` the kind is a rectangle
start. Otherwise read a second bit; `1` means rectangle end and `0` means
isolated pixel.

**Position delta** — the horizontal distance from the previous encoded pixel in
the same row. The number of bits `b` is:

```
b = ceil(log2(cols - col - (isNull ? 0 : 1)))
b = max(b, 1)
```

where `col` is the column of the previous encoded pixel in this row and
`isNull` is true when no encoded pixel has yet been written in this row.
`log2` here means the real logarithm; `b` is the smallest integer with
`2^b >= (cols - col - !isNull)`.

The absolute column is then:

```
col = col + gap + (isNull ? 0 : 1)
```

So the first encoded pixel in a row encodes `gap = col`, and every later one
encodes `gap = col - previous_col - 1`.

**Colour payload** — 8 bits per colour for an 8-bit image. The bit depth comes
from the source image; `getDepth()` returns 8, 16, 32 or 64 depending on the
OpenCV depth. For the `CV_8UC1` images this codec targets, that is 8 bits.

#### Row termination

A row ends when either:

* the next bit is `0` (an empty row marker), or
* the last column (`cols - 1`) has been reached, in which case the decoder
  implicitly starts a new row.

---

## 5. Decoder algorithm

### 5.1 Parsing the body

Walk the body, maintaining `row`, `col` and `isNull`:

1. If the next bit is `0`, the current row is finished. Advance `row`, reset
   `col` to 0 and `isNull` to true.
2. Otherwise the next bit is the `1` marker. Read the kind code, then the
   position delta, then the colour payload.
3. Record `encoded_img[row][col] = kind` and, for kinds `1` and `3`, push a
   block holding the position and the colours.
4. If `col == cols - 1`, advance `row`, reset `col` to 0 and `isNull` to true.

### 5.2 Rediscovering the block extents

The decoder must recover each rectangle's bottom-right corner, because only the
top-left corner and the four colours were stored.

For a candidate corner `rb` relative to the block's top-left corner `lt`, the
corner is accepted when:

* `rb` is not already covered by another block, and
* `encoded_img[rb]` is `0` (untouched) or `2` (rectangle end), and
* every pixel in the column between `lt` and `rb` is either unmarked or is the
  block's own top-left corner, and
* every pixel in the row between `lt` and `rb` is either unmarked or is the
  block's own top-left corner.

The extent is then grown in the same two steps the encoder used:

1. **Horizontal**: while the corner `(lt.x, rb.y + 1)` is accepted, advance the column.
2. **Vertical**: while the corner `(rb.x + 1, rb.y)` is accepted, advance the row.

Note that the vertical step tests the corner at the **already extended** column
`rb.y`, not at the original left column. This is what the C++ implementation
does, and reproducing it exactly is required for a bit-exact decode.

### 5.3 Reconstruction

For an isolated pixel, write the stored colour directly.

For a rectangle, fill the region with the same bilinear interpolation used in
the encoder, truncating intermediate values to `uchar`:

```
for each row i in [lt.x, rb.x]:
    left  = truncate(c00 + (c10 - c00) * gamma_i)
    right = truncate(c01 + (c11 - c01) * gamma_i)
    for each column j in [lt.y, rb.y]:
        img[i][j] = truncate(left + (right - left) * gamma_j)
```

---

## 6. Quality metric

The PSNR used throughout the project is computed over the raw 8-bit samples:

```
MSE = (1 / N) * sum over all pixels of (img1[i][j] - img2[i][j])^2

PSNR = 10 * log10(255^2 * N / MSE)
```

where `N = rows * cols * channels`. This is the standard PSNR definition for
8-bit images with `MAX = 255`.

Note that the implementation computes the sum of squared differences and
multiplies by `255^2 * N` inside the logarithm, which is algebraically the same
as the formula above.

---

## 7. Reference test vector

`tests/vectors/` contains a complete, self-contained vector:

| File | Description |
|---|---|
| `selftest.nam` | A compressed stream of a deterministic 64 × 96 synthetic image at `sigma = 20`. 4956 bits. |
| `selftest_decoded.pgm` | The expected decoded image, as a binary (P5) greyscale PGM. |

The source image is generated deterministically by the tool:

```
base    = (x * 255) / (cols - 1)
checker = (((x / 8) + (y / 8)) % 2) ? 40 : 0
value   = (base + checker) % 256
```

Decoding `selftest.nam` must yield exactly the pixels in `selftest_decoded.pgm`
(PSNR 52.7031 dB against the source, compression ratio 0.1008).

### Verifying the vector

With a Python 3 interpreter and no third-party packages:

```
python tools/verify_vector.py
```

The script re-implements the decoder described in this document, decodes the
stream and compares every pixel against the reference image. It prints:

```
stream       : selftest.nam (4956 bits)
decoded size : 64 x 96
OK: all 6144 pixels match the reference image
```

The same vector is also produced by the C++ tool itself:

```
compression_tool --selftest --output tests/vectors
```

which additionally asserts that the round-trip PSNR is at least 20 dB.

---

## 8. Known issues

These are quirks of the current implementation. They are documented so that
they are not mistaken for parts of the format, and so that a future
reimplementation can decide whether to reproduce them.

### 8.1 Truncation of intermediate interpolation values

The bilinear interpolation is performed in `double` precision, but the
intermediate edge values (`left`, `right`) are stored in a `uchar` array and are
therefore truncated to integers before the final interpolation step. The final
result is also truncated to `uchar`. A "mathematically exact" bilinear
interpolation will therefore differ from this implementation by up to one
level per step. The reference vector in `tests/vectors/` encodes the
truncating behaviour.

### 8.2 The stream is not byte-packed

Each bit occupies one ASCII character, so the file is eight times larger than
the bit count it represents. The reported "compressed bits" figure
(`getWriteLen()`) counts *bits*, while the file on disk is eight times that in
bytes. The `ratio` column in the benchmark output is therefore roughly eight
times smaller than the true on-disk compression ratio.

This is a straightforward optimisation (pack eight characters per byte) but it
would change the file format, so it has not been done here.

### 8.3 The decoder trusts the header

`rows` and `cols` are read from the stream and used to allocate the output
image and the working matrices without any upper bound or sanity check. A
corrupt or malicious stream can therefore request a very large allocation, or
cause out-of-bounds access if the body is shorter than the header claims.

The encoder is likewise unbounded: it writes the dimensions as 16-bit values,
so an image wider or taller than 65535 pixels would be silently truncated.

### 8.4 Colour images are not supported

`init_uncompress` always allocates a `CV_8UC1` result, and the colour-image
branches in the original code are commented out. The encoder folds the channel
count into `cols`, so feeding it a colour image produces a stream that decodes
to a greyscale image of the wrong dimensions.

### 8.5 The decoder assumes the block order matches

The decoder consumes blocks from the encoded-pixel list in the order they were
written, which is the raster scan order of the encoder. If the block list is
truncated or reordered, the decoder throws `blockNum out of range` rather than
recovering.

---

## 9. Reference implementation map

| Concept | Location |
|---|---|
| Block detection, growth, and the encoding map | `Compress_gray::encode()`, `Compress_gray::findRb()` |
| Rectangle validity test | `RBlock::isBlock()` |
| Stream writing | `Compress_gray::compress()` |
| Stream reading | `Compress_gray::uncompress()` |
| Extent recovery | `Compress_gray::findBlock()`, `Compress_gray::isPeer()` |
| Reconstruction | `RBlock::regain()` |
| Bit-level I/O | `Bits::write()`, `Bits::read()`, `Bits::add()` |
| PSNR | `Compress_gray::getPSNR()` |
