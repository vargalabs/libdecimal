/* This file is part of the libdecimal project and is licensed under the MIT License.
 * Copyright © 2025–2026 Varga Labs, Toronto, ON, Canada 🇨🇦 Contact: info@vargalabs.com */

#include <decimal/bid.hpp>
#include <iostream>

int main() {
    using namespace math::literals;

    // IEEE 754-2008 Binary Integer Decimal (BID) via Intel LIBBID.
    // Use this representation when you need full decimal semantics:
    // NaN/Inf, correct rounding, transcendentals, and auditability.

    math::decimal_t<uint64_t> price(12345, -2);  // 123.45
    math::decimal_t<uint64_t> fee_rate = 0.0025_dec; // 0.25 %

    auto fee = price * fee_rate;
    auto total = price + fee;

    std::cout << "BID example\n";
    std::cout << "  price:     " << price << '\n';
    std::cout << "  fee_rate:  " << fee_rate << '\n';
    std::cout << "  fee:       " << fee << '\n';
    std::cout << "  total:     " << total << '\n';

    // Transcendentals are available on the BID path.
    auto exp_value = math::exp(price);
    std::cout << "  exp(price): " << exp_value << '\n';

    return 0;
}
