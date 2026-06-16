
[![CI](https://github.com/vargalabs/libdecimal/actions/workflows/ci.yml/badge.svg)](https://github.com/vargalabs/libdecimal/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/vargalabs/libdecimal/graph/badge.svg?token=F6YCNG3HEY)](https://codecov.io/gh/vargalabs/libdecimal)
[![MIT License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.19323619.svg)](https://doi.org/10.5281/zenodo.19323619)
[![GitHub release](https://img.shields.io/github/v/release/vargalabs/libdecimal.svg)](https://github.com/vargalabs/libdecimal/releases)
[![Documentation](https://img.shields.io/badge/docs-stable-blue)](https://vargalabs.github.io/libdecimal)

## Build Matrix

| Platform | Compilers |
|----------|-----------|
| Ubuntu 22.04 | ![gcc-13][200] ![gcc-14][201] |
| Ubuntu 24.04 | ![gcc-13][300] ![gcc-14][301] ![clang-18][351] ![clang-19][352] ![clang-20][353] |
| macOS 14 (x86_64, Rosetta) | ![apple-clang][400] |
| Windows (MSVC) | ![msvc][500] |

## libdecimal — Deterministic Decimal Arithmetic Built on Intel LIBBID
**libdecimal** is a **header-only, production-grade decimal arithmetic library** built for systems where correctness is not negotiable. If your P&L depends on the last digit — you’re in the right place. If your connectivity stack (e.g. Interactive Brokers) already relies on Intel LIBBID — this fits naturally. This is not a “numeric playground” — it’s infrastructure. For

* trading engines
* risk systems
* settlement pipelines
* cryptographic finance primitives

## Why This Exists

* Intel LIBBID is already in your stack — but the API is painful
* The build system failed for you, we had it covered
* Clean, modern C++ layer over LIBBID (no wrapper spaghetti)
* Exact BID binary representation for interop & persistence
* You are looking for an alternative to Boost.Decimal

**You get:**

* Exact base-10 semantics
* Deterministic cross-platform results
* Zero rounding surprises
* Precision suitable for financial systems
* Compile-time friendly (constexpr where it matters)
* Clean integration with modern C++ (C++20/23)

## Who This Is For

This is for engineers who:

* know why IEEE754 is insufficient for finance
* have debugged rounding errors at 3am
* care about **reproducibility under audit**
* build systems where mistakes cost real money

If you’re building:

* low-latency trading infra
* accounting engines
* blockchain / DeFi protocols
* risk or pricing systems



## Development

```bash
cmake -DCMAKE_BUILD_TYPE=Debug -Dlibdecimal_BUILD_TESTS=ON -S . -B build # configure (debug + tests)
cmake --build build --parallel                                           # build everything
cd build && ctest --output-on-failure                                    # run tests
cmake --build build --target docs_build                                  # install mkdocs locally
cmake --build build --target docs_serve                                  # run webserver @ http://127.0.0.1:9000/
```

## Installation
```bash
sudo apt install build-essential cmake
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
cmake --build build --parallel
sudo cmake --install build
```

## Usage
After installation, libdecimal is available as a standard CMake package:

```cmake
find_package(libdecimal REQUIRED)
add_executable(app main.cpp)
target_link_libraries(app PRIVATE libdecimal::libdecimal)
```

## Attribution
**LIBDECIMAL** builds on Intel’s LIBBID implementation of IEEE 754 decimal arithmetic. **The heavy lifting** — the arithmetic itself — comes from the work **of Marius Cornea, John Harrison, Cristina Anderson, and Evgeny Gvozdev**. **The** underlying **model** traces back **to Mike Cowlishaw** and the IEEE 754 standard.

**The template/macro contraption** that makes it usable in modern C++, **along with the full decomposition of BID into sign, significand, and exponent** — that **is on me**.


[200]: https://vargalabs.github.io/libdecimal/badges/ubuntu-22.04-gcc-13.svg
[201]: https://vargalabs.github.io/libdecimal/badges/ubuntu-22.04-gcc-14.svg
[300]: https://vargalabs.github.io/libdecimal/badges/ubuntu-24.04-gcc-13.svg
[301]: https://vargalabs.github.io/libdecimal/badges/ubuntu-24.04-gcc-14.svg
[351]: https://vargalabs.github.io/libdecimal/badges/ubuntu-24.04-clang-18.svg
[352]: https://vargalabs.github.io/libdecimal/badges/ubuntu-24.04-clang-19.svg
[353]: https://vargalabs.github.io/libdecimal/badges/ubuntu-24.04-clang-20.svg
[400]: https://vargalabs.github.io/libdecimal/badges/macos-14-apple-clang.svg
[500]: https://vargalabs.github.io/libdecimal/badges/windows-latest-msvc.svg