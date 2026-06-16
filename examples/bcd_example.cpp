/* This file is part of the libdecimal project and is licensed under the MIT License.
 * Copyright © 2025–2026 Varga Labs, Toronto, ON, Canada 🇨🇦 Contact: info@vargalabs.com */

#include <decimal/bcd.hpp>
#include <iostream>

int main() {
    // Binary Coded Decimal (BCD): pure C++, no Intel LIBBID dependency.
    // Useful for embedded systems or when you need digit-level access.

    using bcd64 = math::bcd::decimal_t<uint64_t>;

    bcd64 a{123456789ull};           // positive, exponent 0
    bcd64 b{"987654321", -3};        // 987654.321
    bcd64 sum = a + b;

    std::cout << "BCD example\n";
    std::cout << "  a:    " << a.str() << '\n';
    std::cout << "  b:    " << b.str() << '\n';
    std::cout << "  sum:  " << sum.str() << '\n';

    // String and digit-level inspection.
    std::cout << "  str:  " << a.str() << '\n';

    auto [kind, significand, exp] = a.decompose();
    std::cout << "  decomposed: kind=" << static_cast<int>(kind)
              << " sig=" << significand
              << " exp=" << exp << '\n';

    return 0;
}
