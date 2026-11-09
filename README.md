# OpenCV Image Processing and Compression Tool (Console)

A block-based **lossy image codec** for 8-bit greyscale images, implemented in
C++ with [OpenCV](https://opencv.org). The encoder groups pixels into
rectangular blocks that can be reconstructed from their four corner colours by
bilinear interpolation; a single tolerance parameter (`sigma`) trades
compression ratio against image quality.

This is the console version of the project. A Qt GUI version lives in the
[Qt repository](https://github.com/ZajacMo/OpenCV-Image-Processing-and-Compression-Tool-Qt).

> **Looking to reuse the codec?** Read **[ALGORITHM.md](ALGORITHM.md)**. It
> specifies the on-disk format precisely enough to write an independent
> decoder, and includes a reference test vector plus a from-scratch decoder
> written only from that specification.

---

## Contents

- [Features](#features)
- [How the codec works](#how-the-codec-works)
- [Building](#building)
- [Usage](#usage)
- [Benchmark results](#benchmark-results)
- [Output artifacts](#output-artifacts)
- [Reference test vector](#reference-test-vector)
- [Project layout](#project-layout)
- [Limitations](#limitations)
- [License](#license)

---

## Features

- **Block-based compression** — the image is partitioned into rectangles whose
  interiors are reproducible from their corner colours.
- **Adjustable quality** — a single `sigma` parameter controls how tolerant
  the block matcher is, and therefore the compression ratio and PSNR.
- **Greyscale, 8-bit** — the format stores dimensions as 16-bit values and one
  byte per colour sample.
- **Bit-level stream I/O** — the compressed stream is a flat sequence of `'0'`
  and `'1'` characters.
- **Benchmark mode** — compresses and decompresses a whole sample set across a
  range of `sigma` values and reports ratio, timing and PSNR.
- **Self test** — round-trips a deterministic synthetic image and asserts a
  minimum quality, so the build can be verified without any input files.
- **No third-party dependencies** beyond OpenCV itself; the command line
  parser is hand-rolled.

---

## How the codec works

1. **Scan** the image in raster order.
2. **Grow a block** from the first uncovered pixel. A candidate rectangle is
   accepted when every pixel inside it is within `sigma` of the bilinear
   interpolation of its four corners. Growth proceeds diagonally, then
   horizontally, then vertically.
3. **Classify** — a rectangle of one pixel is stored as an *isolated pixel*;
   anything larger is stored as a *rectangle* carrying its four corner colours.
4. **Write** the dimensions, then for each row the encoded pixels, using a
   marker bit, a variable-length kind code, a variable-width position delta and
   the colour payload.
5. **Decode** by reading the stream back into a per-pixel *encoding map* (the
   values 0/1/2/3 mark untouched, rectangle start, rectangle end and isolated
   pixel), then rediscovering each rectangle's extent and filling it by
   interpolation.

`sigma` is the key parameter:

| `sigma` | Effect |
|---|---|
| Small (1–5) | Only near-uniform regions become blocks. High PSNR, little or no gain — the stream can be *larger* than the original. |
| Large (30–50) | Almost any region becomes a block. Large gains, visible quality loss. |

The full details, including the exact bit layout and the decoder's extent
recovery rules, are in **[ALGORITHM.md](ALGORITHM.md)**.

---

## Building

### Requirements

| Requirement | Version |
|---|---|
| Windows | 10 or later |
| Visual Studio | 2019 or later (C++17) |
| OpenCV | 4.x (developed and tested against 4.10.0) |
| CMake | 3.10 or later (only for the CMake build) |

The codec uses `QueryPerformanceCounter` for timing, so it is **Windows
only**. The build systems reflect this: the CMake configuration warns when
configured for another platform.

### Visual Studio

Open `test_openCV.sln` and build the `test_openCV` configuration, or from a
Developer Command Prompt:

```
msbuild test_openCV.sln /p:Configuration=Release
```

The OpenCV libraries must be on the library path and its headers on the
include path. The bundled `build_msvc.bat` shows the exact invocation used
during development:

```
build_msvc.bat
```

It produces `build/compression_tool.exe`.

### CMake

```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

CMake locates OpenCV through `find_package(OpenCV REQUIRED)`. Point it at a
non-standard installation with `-DOpenCV_DIR=/path/to/opencv/build`.

The executable lands in `build/` (or `build/Release/` for multi-config
generators).

### Running the self test

```
compression_tool --selftest
```

This needs no input files: it generates a synthetic 64 × 96 image, compresses
it, decompresses it, checks the dimensions and asserts the round-trip PSNR is
at least 20 dB. It exits non-zero on failure, so it works as a CI check. With
CMake it is registered as a test:

```
ctest --test-dir build -C Release --output-on-failure
```

---

## Usage

```
compression_tool [options]

  --input <dir>      Directory holding the source images (default: images)
  --output <dir>     Directory the compressed streams are written to (default: .)
  --images <list>    Comma separated image names, without extension
                     (default: the built-in sample set)
  --sigma <list>     Comma separated sigma values (default: 1,5,10,20,30,50)
  --selftest         Run a synthetic round-trip check and exit
  -h, --help         Show this message
```

Examples:

```
compression_tool --selftest
compression_tool --images lena512,f16 --sigma 5,20
compression_tool --input images --output out --sigma 10
```

With no arguments the tool runs the full benchmark: the ten sample images in
`images/` at six `sigma` values, which is 60 compress/decompress cycles.

The output is a tab-separated table:

```
image          original size   sigma   compressed bits   ratio   compress time (s)   decompress time (s)   PSNR (dB)
lena512.bmp    512*512         10      799631            0.3813  0.120487            0.010636              36.3887
```

**A note on `ratio` and `compressed bits`.** `compressed bits` counts the bits
of actual payload. `ratio` divides that by the raw bit count of the image, so
it is a *logical* compression ratio. The file written to disk is larger than
that by a factor of eight, because the stream stores one ASCII character per
bit rather than packing bits into bytes. See
[ALGORITHM.md §8.2](ALGORITHM.md#82-the-stream-is-not-byte-packed).

---

## Benchmark results

Measured on Windows 11 with MSVC 14.44 in Release mode, OpenCV 4.10.0.
Reproduce with `compression_tool` using the default arguments.

### lena512.bmp — a photographic image

| sigma | compressed bits | ratio | PSNR (dB) |
|---:|---:|---:|---:|
| 1 | 3282134 | 1.5650 | 59.23 |
| 5 | 1699187 | 0.8102 | 42.29 |
| 10 | 799631 | 0.3813 | 36.39 |
| 20 | 368150 | 0.1755 | 30.38 |
| 30 | 226626 | 0.1081 | 26.69 |
| 50 | 103842 | 0.0495 | 22.36 |

### baboonzyp512.bmp — a highly detailed image

| sigma | compressed bits | ratio | PSNR (dB) |
|---:|---:|---:|---:|
| 1 | 3512520 | 1.6749 | 67.26 |
| 5 | 3161627 | 1.5076 | 46.81 |
| 10 | 2543844 | 1.2130 | 38.48 |
| 20 | 1689296 | 0.8055 | 31.26 |
| 30 | 1194116 | 0.5694 | 27.30 |
| 50 | 652517 | 0.3111 | 22.56 |

### broke256.bmp — a piecewise-smooth image

| sigma | compressed bits | ratio | PSNR (dB) |
|---:|---:|---:|---:|
| 1 | 600231 | 1.1448 | 54.95 |
| 5 | 162361 | 0.3097 | 41.45 |
| 10 | 50716 | 0.0967 | 36.06 |
| 20 | 14391 | 0.0274 | 30.22 |
| 30 | 5985 | 0.0114 | 26.68 |
| 50 | 1588 | 0.0030 | 20.71 |

Observations:

- At `sigma = 1` the stream is **larger** than the raw image. The codec spends
  bits describing block structure without gaining anything, so small `sigma`
  values are not useful.
- Detail-heavy images (baboon) compress far worse than smooth ones (broke) at
  the same `sigma`, which is the expected behaviour for a corner-based
  predictor.
- Roughly, a `sigma` of 20 lands near 30 dB PSNR, which is a reasonable
  working point for visual use.

---

## Output artifacts

| Artifact | Description |
|---|---|
| `<name>_sigma<N>.nam` | The compressed stream for `<name>.bmp` at `sigma = N`. A text file of `'0'`/`'1'` characters. |
| `tests/vectors/selftest.nam` | The reference stream used by the self test and by `tools/verify_vector.py`. |
| `tests/vectors/selftest_decoded.pgm` | The expected decode of the above, as a binary (P5) greyscale PGM. |

The tool does not write decoded images in normal operation; only the self test
emits a PGM, so that the vector can be checked without an OpenCV dependency.

---

## Reference test vector

`tests/vectors/` holds a complete vector: a compressed stream plus the
expected decoded image. It exists so that an independent implementation can
confirm it agrees with this codec.

`tools/verify_vector.py` is a decoder written **only** from the specification
in `ALGORITHM.md`. It needs nothing but a Python 3 interpreter:

```
python tools/verify_vector.py
```

```
stream       : selftest.nam (4956 bits)
decoded size : 64 x 96
OK: all 6144 pixels match the reference image
```

Because that script is derived from the specification rather than from the C++
code, it doubles as a check that `ALGORITHM.md` is accurate: if the
specification and the implementation ever diverge, the pixel comparison fails.

---

## Project layout

```
├── Compress_gray.h/.cpp     Core codec: encode, decode, PSNR, timing
├── Bits.h/.cpp              Bit-level stream reader and writer
├── Block.h/.cpp             Abstract block base class
├── RBlock.h/.cpp            Rectangle block: validity test, marking, reconstruction
├── main.cpp                 Command line front-end and benchmark driver
├── ALGORITHM.md             Format specification
├── CMakeLists.txt           CMake build
├── build_msvc.bat           Convenience MSVC build script
├── tools/
│   ├── convert_gbk_to_utf8.py   Encoding conversion (see below)
│   └── verify_vector.py          Independent decoder for the reference vector
├── tests/vectors/           Reference test vector
├── images/                  Sample images
└── test_openCV.sln/.vcxproj Visual Studio project
```

### Source encoding

The sources were originally written in the Chinese system code page (GBK) and
are now UTF-8 with a BOM, which MSVC detects automatically. `tools/convert_gbk_to_utf8.py`
performs the conversion and is idempotent; `--check` reports which files would
change without touching them.

---

## Limitations

- **Windows only** — timing uses `QueryPerformanceCounter`.
- **Greyscale only** — colour images are not supported; the decoder always
  produces a `CV_8UC1` image. See
  [ALGORITHM.md §8.4](ALGORITHM.md#84-colour-images-are-not-supported).
- **Lossy** — reconstructed interiors are interpolations, not the original
  samples.
- **The stream is not byte-packed** — one ASCII character per bit, so files are
  eight times larger than the payload they carry.
- **The decoder trusts the header** — a corrupt stream can request an
  arbitrarily large allocation.
- **Single image per stream** — there is no container or multi-image format.
- **No entropy coding** — the stream is uncompressed bits; a general-purpose
  compressor applied afterwards would reduce it substantially.

The full list, with details, is in
[ALGORITHM.md §8](ALGORITHM.md#8-known-issues).

---

## License

MIT — see [LICENSE](LICENSE).

Copyright (c) 2025 Zajac Mo.
