/* This file is part of the libdecimal project and is licensed under the MIT License.
 * Copyright © 2025–2026 Varga Labs, Toronto, ON, Canada 🇨🇦 Contact: info@vargalabs.com */

#include <decimal/scaled.hpp>
#include <iostream>

int main() {
    // Scaled integer: a lightweight (significand, exponent) pair.
    // Ideal for wire formats, serialization, and hash keys.

    using scaled64 = math::scaled::decimal_t<int64_t>;

    scaled64 price{12345, -2};   // 123.45
    scaled64 qty{100, 0};        // 100
    scaled64 notional = price * qty;

    std::cout << "Scaled example\n";
    std::cout << "  price:    " << static_cast<std::string>(price) << '\n';
    std::cout << "  qty:      " << static_cast<std::string>(qty) << '\n';
    std::cout << "  notional: " << static_cast<std::string>(notional) << '\n';

    // Wire encode/decode as an integer pair.
    auto [sig, exp] = notional.as_pair();
    std::cout << "  wire pair: sig=" << sig << " exp=" << exp << '\n';

    // Checksum for hashing or verification.
    std::cout << "  checksum: " << math::scaled::checksum(notional) << '\n';

    return 0;
}
