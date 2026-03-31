/* This file is part of the LIBDECIMAL project and is licensed under the MIT License.
 * Copyright © 2025–2026 Varga Labs, Toronto, ON, Canada 🇨🇦 Contact: info@vargalabs.com */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <cstdint>
#include <type_traits>

#include <doctest/all>
#include "decimal.hpp"

namespace {
    template <typename T> struct tolerance_t;
    template <> struct tolerance_t<uint32_t> {
        using decimal_t = math::decimal_t<uint32_t>;
        inline static const decimal_t value = decimal_t{"0.00001"};
    };

    template <> struct tolerance_t<uint64_t> {
        using decimal_t = math::decimal_t<uint64_t>;
        inline static const decimal_t value = decimal_t{"0.0000000000001"};
    };

    template <> struct tolerance_t<BID_UINT128> {
        using decimal_t = math::decimal_t<BID_UINT128>;
        inline static const decimal_t value = decimal_t{"0.000000000000000000001000000000"};
    };

    template <typename T>
    auto abs_t(const math::decimal_t<T>& x) -> math::decimal_t<T> {
        using decimal_t   = math::decimal_t<T>;
        using constants_t = math::constants<T>;
        return x < constants_t::zero ? -x : x;
    }

    template <typename T> void check_near(const math::decimal_t<T>& lhs, const math::decimal_t<T>& rhs, const char* expr_text) {
        using decimal_t = math::decimal_t<T>;
        const decimal_t err = abs_t<T>(lhs - rhs);
        CAPTURE(expr_text);
        CAPTURE(err);
        CAPTURE(tolerance_t<T>::value);
        CHECK(err <= tolerance_t<T>::value);
    }

    template <typename T> void check_algebraic_identities() {
        using decimal_t   = math::decimal_t<T>;
        using constants_t = math::constants<T>;

        const decimal_t zero = constants_t::zero;
        const decimal_t one  = constants_t::one;
        const decimal_t two  = decimal_t{"2"};
        const decimal_t four = decimal_t{"4"};
        const decimal_t five = decimal_t{"5"};
        const decimal_t ten  = decimal_t{"10"};
        const decimal_t one_eighty = decimal_t{"180"};

        /** exact alias identities */
        CHECK(constants_t::tau == constants_t::two_pi);
        if constexpr (std::is_same_v<T, uint32_t> || std::is_same_v<T, BID_UINT128>) {
            CHECK(constants_t::golden_ratio == constants_t::phi);
        }
        /** exact neutral-element identities */
        CHECK(constants_t::pi + zero == constants_t::pi);
        CHECK(constants_t::pi * one  == constants_t::pi);
        CHECK(constants_t::e  + zero == constants_t::e);
        CHECK(constants_t::e  * one  == constants_t::e);
        /** scaled circle constants */
        check_near<T>(constants_t::tau,        two  * constants_t::pi, "tau == 2*pi");
        check_near<T>(constants_t::two_pi,     two  * constants_t::pi, "two_pi == 2*pi");
        check_near<T>(constants_t::half_pi,    constants_t::pi / two,  "half_pi == pi/2");
        check_near<T>(constants_t::quarter_pi, constants_t::pi / four, "quarter_pi == pi/4");
        check_near<T>(two * constants_t::half_pi, constants_t::pi,     "2*half_pi == pi");
        check_near<T>(four * constants_t::quarter_pi, constants_t::pi, "4*quarter_pi == pi");
        /** reciprocal */
        check_near<T>(constants_t::inv_pi * constants_t::pi, one, "inv_pi * pi == 1");
        check_near<T>(constants_t::pi * constants_t::inv_pi, one, "pi * inv_pi == 1");
        /** degree/radian conversion */
        check_near<T>(constants_t::deg_to_rad * one_eighty, constants_t::pi, "deg_to_rad * 180 == pi");
        check_near<T>(constants_t::rad_to_deg * constants_t::pi, one_eighty, "rad_to_deg * pi == 180");
        check_near<T>(constants_t::deg_to_rad * constants_t::rad_to_deg, one, "deg_to_rad * rad_to_deg == 1");
        check_near<T>(constants_t::rad_to_deg * constants_t::deg_to_rad, one, "rad_to_deg * deg_to_rad == 1");
        /** logarithm base-change identities */
        check_near<T>(constants_t::log2e * constants_t::ln2, one,   "log2e * ln2 == 1");
        check_near<T>(constants_t::log10e * constants_t::ln10, one, "log10e * ln10 == 1");
        check_near<T>(constants_t::ln10 / constants_t::ln2, decimal_t{"3.32192809488736234787"}, "ln10 / ln2 == log2(10)");
        /** radicals */
        check_near<T>(constants_t::sqrt2 * constants_t::sqrt2, two,  "sqrt2^2 == 2");
        check_near<T>(constants_t::sqrt3 * constants_t::sqrt3, decimal_t{"3"}, "sqrt3^2 == 3");
        check_near<T>(constants_t::sqrt5 * constants_t::sqrt5, five, "sqrt5^2 == 5");
        /** cube roots */
        check_near<T>(
            constants_t::cube_root_2 * constants_t::cube_root_2 * constants_t::cube_root_2,
            two,
            "cube_root_2^3 == 2"
        );
        check_near<T>(
            constants_t::cube_root_3 * constants_t::cube_root_3 * constants_t::cube_root_3,
            decimal_t{"3"},
            "cube_root_3^3 == 3"
        );
        /** golden ratio: phi^2 = phi + 1 */
        if constexpr (std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t> || std::is_same_v<T, BID_UINT128> ) {
            check_near<T>(constants_t::phi * constants_t::phi, constants_t::phi + one, "phi^2 == phi + 1");
        }
        /** silver ratio: delta^2 = 2*delta + 1, where delta = 1 + sqrt(2) */
        check_near<T>(
            constants_t::silver_ratio * constants_t::silver_ratio,
            two * constants_t::silver_ratio + one,
            "silver_ratio^2 == 2*silver_ratio + 1"
        );
        /** simple ordering sanity */
        CHECK(constants_t::quarter_pi < constants_t::half_pi);
        CHECK(constants_t::half_pi    < constants_t::pi);
        CHECK(constants_t::pi         < constants_t::tau);
        CHECK(constants_t::zero       < constants_t::one);
    }

} // namespace

TEST_CASE("math::constants<uint32_t> algebraic identities") {
    check_algebraic_identities<uint32_t>();
}

TEST_CASE("math::constants<uint64_t> algebraic identities") {
    check_algebraic_identities<uint64_t>();
}

TEST_CASE("math::constants<BID_UINT128> algebraic identities") {
    check_algebraic_identities<BID_UINT128>();
}