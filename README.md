# libdecimal
[![CI](https://github.com/vargalabs/libdecimal/actions/workflows/ci.yml/badge.svg)](https://github.com/vargalabs/libdecimal/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/vargalabs/libdecimal/graph/badge.svg?token=F6YCNG3HEY)](https://codecov.io/gh/vargalabs/libdecimal)
[![MIT License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.19323619.svg)](https://doi.org/10.5281/zenodo.19323619)
[![GitHub release](https://img.shields.io/github/v/release/vargalabs/libdecimal.svg)](https://github.com/vargalabs/libdecimal/releases)
[![Documentation](https://img.shields.io/badge/docs-stable-blue)](https://vargalabs.github.io/libdecimal)

Deterministic decimal arithmetic for C++
-----------------------------------------
**libdecimal** is a header-only C++ library for IEEE 754-2008 decimal floating-point arithmetic, built on Intel's LIBBID reference implementation. It wraps the BID (binary-integer-decimal) encoding in a typed, `constexpr`-friendly interface with exact base-10 semantics and deterministic cross-platform results. Decimal32 / Decimal64 / Decimal128 are supported, together with transcendental functions, string and binary-float conversions, and full decomposition into sign, significand, and exponent — plus direct access to the underlying BID bit patterns for interop and persistence. Lightweight fixed-point, scaled-integer, and BCD representations are also included.

| Layer | Role |
|---|---|
| **libdecimal** | Header-only C++ decimal arithmetic interface |
| [Intel LIBBID](https://www.intel.com/content/www/us/en/developer/articles/tool/intel-decimal-floating-point-math-library.html) | Vendored IEEE 754 decimal (BID) reference implementation |

## Supported platforms

| OS / Compiler | GCC 13        | GCC 14        | GCC 15    | Clang 17  | Clang 18     | Clang 19     | Clang 20     | Apple Clang | MSVC         |
|---------------|---------------|---------------|-----------|-----------|--------------|--------------|--------------|-------------|--------------|
| Ubuntu 22.04  | ![gcc13][200] | ![gcc14][201] | ![NA][NA] | ![NA][NA] | ![NA][NA]    | ![NA][NA]    | ![NA][NA]    | ![NA][NA]   | ![NA][NA]    |
| Ubuntu 24.04  | ![gcc13][300] | ![gcc14][301] | ![NA][NA] | ![NA][NA] | ![cl18][351] | ![cl19][352] | ![cl20][353] | ![NA][NA]   | ![NA][NA]    |
| macOS 14      | ![NA][NA]     | ![NA][NA]     | ![NA][NA] | ![NA][NA] | ![NA][NA]    | ![NA][NA]    | ![NA][NA]    | ![ac][400]  | ![NA][NA]    |
| Windows       | ![NA][NA]     | ![NA][NA]     | ![NA][NA] | ![NA][NA] | ![NA][NA]    | ![NA][NA]    | ![NA][NA]    | ![NA][NA]   | ![msvc][500] |

macOS is built for x86_64 and exercised under Rosetta 2: Intel's float128 (DPML) sources are x86_64-only, so libdecimal targets amd64 / x86_64 on every platform.

## Release packages

Native installers are attached to each tagged release. All packages are **x86_64 / amd64** (Intel's DPML is x86_64-only).

| Platform | Package |
|----------|---------|
| Debian / Ubuntu (amd64) | [libdecimal-2.2.3-Linux-amd64.deb][800] |
| Red Hat / Fedora (x86_64) | [libdecimal-2.2.3-Linux-x86_64.rpm][800] |
| macOS (x86_64) | [libdecimal-2.2.3-macOS-x86_64.pkg][802] |
| Windows (x64) | [libdecimal-2.2.3-Windows-x64.exe][803] |

Download them from the [Releases page](https://github.com/vargalabs/libdecimal/releases/latest). Each installer ships the headers, the Intel LIBBID static library, and the `libdecimal` CMake package files.

## Requirements

| Requirement | Minimum | Tested ceiling |
|---|---|---|
| C++ standard | C++23 | C++23 |
| CMake | 3.22 | — |
| Toolchains | GCC 13 · Clang 18 · Apple Clang · MSVC 19.4x | GCC 14 · Clang 20 |

## Quick start

```cmake
find_package(libdecimal REQUIRED)
add_executable(app main.cpp)
target_link_libraries(app PRIVATE libdecimal::libdecimal)
```

```cpp
#include <decimal/bid.hpp>
#include <iostream>

int main() {
    using namespace math::literals;

    math::decimal_t<uint64_t> price(12345, -2);       // 123.45
    math::decimal_t<uint64_t> fee_rate = 0.0025_dec;  // 0.25 %

    auto fee   = price * fee_rate;
    auto total = price + fee;

    std::cout << "price: " << price << "  fee: " << fee << "  total: " << total << '\n';
    std::cout << "exp(price): " << math::exp(price) << '\n';   // transcendentals on the BID path
}
```

## Building from source

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
sudo cmake --install build
```

The only build artifact is the vendored Intel LIBBID static library (`libbid_static`); everything libdecimal adds is header-only.

## Decimal representations

The `examples/` directory contains standalone programs demonstrating each representation:

| File | Representation |
|------|----------------|
| `examples/bid_example.cpp` | IEEE 754 BID via Intel LIBBID |
| `examples/fixed_example.cpp` | Compile-time fixed-point |
| `examples/bcd_example.cpp` | Binary-coded decimal |
| `examples/scaled_example.cpp` | Lightweight scaled-integer pair |

```bash
cmake -S . -B build -Dlibdecimal_BUILD_EXAMPLES=ON
cmake --build build --parallel
./build/bid_example
```

## Development

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -Dlibdecimal_BUILD_TESTS=ON  # configure (debug + tests)
cmake --build build --parallel                                           # build everything
ctest --test-dir build --output-on-failure                               # run the test suite
cmake --build build --target docs_serve                                  # serve docs @ http://127.0.0.1:9000/
```

## Attribution

**libdecimal** builds on Intel's LIBBID implementation of IEEE 754 decimal arithmetic. The heavy lifting — the arithmetic itself — comes from the work of **Marius Cornea, John Harrison, Cristina Anderson, and Evgeny Gvozdev**. The underlying model traces back to **Mike Cowlishaw** and the IEEE 754 standard.

The template/macro layer that makes it usable in modern C++, along with the full decomposition of BID into sign, significand, and exponent, is the contribution of this project.

<!-- Static "not applicable" badge (combination not built in CI). -->
[NA]: https://img.shields.io/badge/na-%E2%97%8B-lightgrey.svg

<!-- Ubuntu 22.04 -->
[200]: https://vargalabs.github.io/libdecimal/badges/ubuntu-22.04-gcc-13.svg
[201]: https://vargalabs.github.io/libdecimal/badges/ubuntu-22.04-gcc-14.svg

<!-- Ubuntu 24.04 -->
[300]: https://vargalabs.github.io/libdecimal/badges/ubuntu-24.04-gcc-13.svg
[301]: https://vargalabs.github.io/libdecimal/badges/ubuntu-24.04-gcc-14.svg
[351]: https://vargalabs.github.io/libdecimal/badges/ubuntu-24.04-clang-18.svg
[352]: https://vargalabs.github.io/libdecimal/badges/ubuntu-24.04-clang-19.svg
[353]: https://vargalabs.github.io/libdecimal/badges/ubuntu-24.04-clang-20.svg

<!-- macOS 14 (x86_64) -->
[400]: https://vargalabs.github.io/libdecimal/badges/macos-14-apple-clang.svg

<!-- Windows -->
[500]: https://vargalabs.github.io/libdecimal/badges/windows-latest-msvc.svg

<!-- PAckages -->
[800]: https://github.com/vargalabs/libdecimal/releases/download/v2.2.3/libdecimal-2.2.3-Linux-amd64.deb
[801]: https://github.com/vargalabs/libdecimal/releases/download/v2.2.3/libdecimal-2.2.3-Linux-x86_64.rpm
[802]: https://github.com/vargalabs/libdecimal/releases/download/v2.2.3/libdecimal-2.2.3-macOS-x86_64.pkg
[803]: https://github.com/vargalabs/libdecimal/releases/download/v2.2.3/libdecimal-2.2.3-Windows-x64.exe
[804]: https://github.com/vargalabs/libdecimal/archive/refs/tags/v2.2.3.zip
[805]: https://github.com/vargalabs/libdecimal/archive/refs/tags/v2.2.3.tar.gz