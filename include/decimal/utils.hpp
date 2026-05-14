#include <bit>
#include <cstdint>
#include <concepts>
#include <tuple>
#include <array>
#include <cmath>
#include "bid.hpp"

#pragma once

namespace math::utils {
    using category = math::bid::category;
    template<class value_t> struct ieee754_traits_t;
    template<> struct ieee754_traits_t<float> {
        using bits_t = std::uint32_t;
        static constexpr int mantissa_bits = 23, exponent_bits = 8, exponent_bias = 127;
        static constexpr bits_t exponent_mask = 0xFFu, fraction_mask = 0x7FFFFFu, sign_mask = 0x80000000u;
    };

    template<> struct ieee754_traits_t<double> {
        using bits_t = std::uint64_t;
        static constexpr int mantissa_bits = 52, exponent_bits = 11, exponent_bias = 1023;
        static constexpr bits_t exponent_mask = 0x7FFull, fraction_mask = 0xFFFFFFFFFFFFFull, sign_mask = 0x8000000000000000ull;
    };
    template<class value_t>  concept ieee754_binary_float_c = std::same_as<value_t, float> || std::same_as<value_t, double>;

    template <class value_t, class significand_t, class exponent_t> requires ieee754_binary_float_c<value_t> && std::unsigned_integral<significand_t> && std::signed_integral<exponent_t>
    inline std::tuple<category, significand_t, exponent_t> decompose(value_t value) {
        using traits_t = ieee754_traits_t<value_t>;
        using bits_t = typename traits_t::bits_t;

        const bits_t bits = std::bit_cast<bits_t>(value);
        const bool sign = (bits & traits_t::sign_mask) != 0;
        const bits_t exponent_field = (bits >> traits_t::mantissa_bits) & traits_t::exponent_mask, 
            fraction = bits & traits_t::fraction_mask;
        if (exponent_field == 0 && fraction == 0) return {category::zero, 0, 0}; // zero
        if (exponent_field == traits_t::exponent_mask) {
            if (fraction == 0) 
                return {sign ? category::ninf : category::pinf, 0, 0};
            else return {category::nan, 0, 0};
        }
        significand_t significand;
        exponent_t exponent;
        if (exponent_field == 0) { // subnormal: value = fraction * 2^(1 - bias - mantissa_bits)
            significand = static_cast<significand_t>(fraction);
            exponent = static_cast<exponent_t>(1 - traits_t::exponent_bias - traits_t::mantissa_bits);
        } else {   // normal: value = (2^mantissa_bits + fraction) * 2^(exponent - bias - mantissa_bits)
            const bits_t implicit_one = bits_t{1} << traits_t::mantissa_bits;
            significand = static_cast<significand_t>(implicit_one | fraction);
            exponent = static_cast<exponent_t>(static_cast<int>(exponent_field) - traits_t::exponent_bias - traits_t::mantissa_bits);
        }
        return {sign ? category::negative : category::positive, significand, exponent};
    }

    template<class value_t> struct pow10_t;
    template<> struct pow10_t<float> {
        static constexpr int min_exp = -16, max_exp = 16;
        static constexpr std::array<float, max_exp - min_exp + 1> value {
            1.0e-16f, 1.0e-15f, 1.0e-14f, 1.0e-13f, 1.0e-12f, 1.0e-11f, 1.0e-10f, 1.0e-9f,
            1.0e-8f,  1.0e-7f,  1.0e-6f,  1.0e-5f,  1.0e-4f,  1.0e-3f,  1.0e-2f,  1.0e-1f,
            1.0e+0f,
            1.0e+1f,  1.0e+2f,  1.0e+3f,  1.0e+4f,  1.0e+5f,  1.0e+6f,  1.0e+7f,  1.0e+8f,
            1.0e+9f,  1.0e+10f, 1.0e+11f, 1.0e+12f, 1.0e+13f, 1.0e+14f, 1.0e+15f, 1.0e+16f
        };
    };
    template<> struct pow10_t<double> {
        static constexpr int min_exp = -22, max_exp = 22;
        static constexpr std::array<double, max_exp - min_exp + 1> value{
            1.0e-22, 1.0e-21, 1.0e-20, 1.0e-19, 1.0e-18, 1.0e-17, 1.0e-16, 1.0e-15,
            1.0e-14, 1.0e-13, 1.0e-12, 1.0e-11, 1.0e-10, 1.0e-9,  1.0e-8,  1.0e-7,
            1.0e-6,  1.0e-5,  1.0e-4,  1.0e-3,  1.0e-2,  1.0e-1,
            1.0e+0,
            1.0e+1,  1.0e+2,  1.0e+3,  1.0e+4,  1.0e+5,  1.0e+6,  1.0e+7,  1.0e+8,
            1.0e+9,  1.0e+10, 1.0e+11, 1.0e+12, 1.0e+13, 1.0e+14, 1.0e+15, 1.0e+16,
            1.0e+17, 1.0e+18, 1.0e+19, 1.0e+20, 1.0e+21, 1.0e+22
        };
    };

    template<class value_t, std::integral exponent_t> inline value_t pow10(exponent_t exponent) {
        using table_t = pow10_t<value_t>;
        if (exponent >= table_t::min_exp && exponent <= table_t::max_exp)
            return table_t::value[static_cast<std::size_t>(exponent - table_t::min_exp)];
        return std::pow(static_cast<value_t>(10), static_cast<value_t>(exponent));
    }

    template<class value_t, std::unsigned_integral significand_t, std::signed_integral exponent_t>
        requires ieee754_binary_float_c<value_t> inline value_t make(significand_t significand, exponent_t exponent) {
        return static_cast<value_t>(significand) * pow10<value_t>(exponent);
    }
} // namespace math::utils