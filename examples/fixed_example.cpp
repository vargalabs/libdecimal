/* This file is part of the libdecimal project and is licensed under the MIT License.
 * Copyright © 2025–2026 Varga Labs, Toronto, ON, Canada 🇨🇦 Contact: info@vargalabs.com */

#include <decimal/fixed.hpp>
#include <iostream>

int main() {
    // Fixed-point with a compile-time exponent.
    // Perfect for currency where the scale is known up front (e.g., 4 dp).

    using price_t = math::fixed::decimal_t<uint64_t, -4>;  // 4 decimal places

    price_t apple{math::bid::category::positive, 12599};  // 1.2599
    price_t orange{math::bid::category::positive, 8750};  // 0.8750

    auto subtotal = apple + orange;
    auto tax = subtotal * price_t{math::bid::category::positive, 500};  // 5% -> 0.0500
    auto total = subtotal + tax;

    std::cout << "Fixed example (4 decimal places)\n";
    std::cout << "  apple:     " << apple.str() << '\n';
    std::cout << "  orange:    " << orange.str() << '\n';
    std::cout << "  subtotal:  " << subtotal.str() << '\n';
    std::cout << "  tax:       " << tax.str() << '\n';
    std::cout << "  total:     " << total.str() << '\n';

    // Compile-time scale introspection.
    static_assert(price_t::exponent_v == -4);
    static_assert(price_t::scale_factor_v == 10000);

    // Decompose into (kind, significand, exponent) for serialization.
    auto [kind, significand, exp] = total.decompose();
    std::cout << "  decomposed: kind=" << static_cast<int>(kind)
              << " sig=" << significand
              << " exp=" << exp << '\n';

    return 0;
}
