

#pragma once

#include <cstdint>
#include <compare>
#include <concepts>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include "bid.hpp"

namespace math::scaled::impl {
    template<class T> concept signed_integral_t = std::integral<T> && std::is_signed_v<T>;
    template<class T> concept unsigned_integral_t = std::integral<T> && !std::is_signed_v<T>;
    template<class T> concept integral_t = std::integral<T>;
    template<integral_t T> constexpr T abs_t(T value) {
        if constexpr (std::is_signed_v<T>)
            return value < 0 ? -value : value;
        else return value;
    }

    template<integral_t T> constexpr T pow10(unsigned exponent) {
        T value = static_cast<T>(1);
        for (unsigned i = 0; i < exponent; ++i)
            value *= static_cast<T>(10);
        return value;
    }

    template<integral_t T> constexpr unsigned trailing_zeroes10(T value) {
        if (value == 0) return 0;
        unsigned count = 0;
        while (value % static_cast<T>(10) == 0)
            value /= static_cast<T>(10), ++count;
        return count;
    }

    template<integral_t significand_t, integral_t exponent_t>
    constexpr void normalize(significand_t& significand, exponent_t& exponent) {
        if (significand == 0) {
            exponent = 0;
            return;
        }
        const unsigned zeroes = trailing_zeroes10(abs_t(significand));
        if (zeroes == 0) return;
        for (unsigned i = 0; i < zeroes; ++i)
            significand /= static_cast<significand_t>(10), ++exponent;
    }

    template<integral_t significand_t, integral_t exponent_t>
    constexpr auto widen(significand_t value) {
    #if defined(__SIZEOF_INT128__)
        if constexpr (sizeof(significand_t) <= 8) {
            if constexpr (std::is_signed_v<significand_t>) 
                return static_cast<__int128>(value);
            else return static_cast<unsigned __int128>(value);
        } else return value;
    #else
        return value;
    #endif
    }

    template<integral_t significand_t, integral_t exponent_t>
    constexpr significand_t checked_cast(auto value) {
        using limit_t = std::numeric_limits<significand_t>;
        if (value > static_cast<decltype(value)>(limit_t::max())) {
            throw std::overflow_error("decimal_t: significand overflow");
        }
        if constexpr (std::is_signed_v<significand_t>) {
            if (value < static_cast<decltype(value)>(limit_t::lowest())) {
                throw std::overflow_error("decimal_t: significand underflow");
            }
        }
        return static_cast<significand_t>(value);
    }

    template<integral_t significand_t, integral_t exponent_t>
    constexpr void align(significand_t lhs_significand, exponent_t lhs_exponent, significand_t rhs_significand, exponent_t rhs_exponent,
        auto& lhs_aligned, auto& rhs_aligned, exponent_t& exponent_out) {

        exponent_out = lhs_exponent < rhs_exponent ? lhs_exponent : rhs_exponent;
        const auto lhs_scale = static_cast<unsigned>(lhs_exponent - exponent_out);
        const auto rhs_scale = static_cast<unsigned>(rhs_exponent - exponent_out);
        lhs_aligned = widen<significand_t, exponent_t>(lhs_significand);
        rhs_aligned = widen<significand_t, exponent_t>(rhs_significand);
        for (unsigned i = 0; i < lhs_scale; ++i) lhs_aligned *= 10;
        for (unsigned i = 0; i < rhs_scale; ++i) rhs_aligned *= 10;
    }
} // namespace impl

namespace math::scaled {
    using category = math::bid::category;
    template <class significand_t, class exponent_t = std::int16_t> struct decimal_t {
        static_assert(std::integral<significand_t>, "significand_t must be integral");
        static_assert(std::integral<exponent_t>, "exponent_t must be integral");

        using significand_type_t = significand_t;
        using exponent_type_t = exponent_t;
        using element_type_t = decimal_t<significand_t, exponent_t>;

        constexpr decimal_t() = default;
        constexpr decimal_t(significand_t significand, exponent_t exponent = 0): significand(significand), exponent(exponent) {
            normalize();
        }

        template<std::integral T> constexpr explicit decimal_t(T value)
            : significand(static_cast<significand_t>(value)), exponent(0) {
            normalize();
        }

        constexpr void normalize() { impl::normalize(significand, exponent); }
        [[nodiscard]] constexpr bool is_zero() const { return significand == 0; }
        [[nodiscard]] constexpr auto as_pair() const { return std::pair{significand, exponent}; }
        [[nodiscard]] constexpr long double to_long_double() const {
            long double value = static_cast<long double>(significand);
            if (exponent >= 0)
                for (exponent_t i = 0; i < exponent; ++i) value *= 10.0L;
            else for (exponent_t i = 0; i < -exponent; ++i) value /= 10.0L;
            return value;
        }
        [[nodiscard]] explicit constexpr operator float() const { return static_cast<float>(to_long_double()); }
        [[nodiscard]] explicit constexpr operator double() const { return static_cast<double>(to_long_double()); }
        [[nodiscard]] explicit constexpr operator long double() const { return to_long_double(); }
        [[nodiscard]] explicit constexpr operator std::string() const { return std::to_string(significand) + "e" + std::to_string(exponent); }

        constexpr decimal_t& operator+=(decimal_t const& rhs) {
            auto lhs_aligned = impl::widen<significand_t, exponent_t>(0);
            auto rhs_aligned = impl::widen<significand_t, exponent_t>(0);
            exponent_t out_exponent = 0;
            impl::align(significand, exponent, rhs.significand, rhs.exponent,lhs_aligned, rhs_aligned, out_exponent);
            significand = impl::checked_cast<significand_t, exponent_t>(lhs_aligned + rhs_aligned);
            exponent = out_exponent;
            normalize();
            return *this;
        }

        constexpr decimal_t& operator-=(decimal_t const& rhs) {
            auto lhs_aligned = impl::widen<significand_t, exponent_t>(0);
            auto rhs_aligned = impl::widen<significand_t, exponent_t>(0);
            exponent_t out_exponent=0;
            impl::align(significand, exponent,rhs.significand, rhs.exponent,lhs_aligned, rhs_aligned, out_exponent);
            significand = impl::checked_cast<significand_t, exponent_t>(lhs_aligned - rhs_aligned);
            exponent = out_exponent;
            normalize();
            return *this;
        }

        constexpr decimal_t& operator*=(decimal_t const& rhs) {
            const auto lhs = impl::widen<significand_t, exponent_t>(significand);
            const auto rhs_value = impl::widen<significand_t, exponent_t>(rhs.significand);
            significand = impl::checked_cast<significand_t, exponent_t>(lhs * rhs_value);
            exponent = static_cast<exponent_t>(exponent + rhs.exponent);
            normalize();
            return *this;
        }

        constexpr decimal_t& operator/=(decimal_t const& rhs) {
            if (rhs.significand == 0) throw std::domain_error("decimal_t: division by zero");
            significand = static_cast<significand_t>(significand / rhs.significand);
            exponent = static_cast<exponent_t>(exponent - rhs.exponent);
            normalize();
            return *this;
        }

        [[nodiscard]] friend constexpr decimal_t operator+(decimal_t lhs, decimal_t const& rhs) {
            lhs += rhs;
            return lhs;
        }

        [[nodiscard]] friend constexpr decimal_t operator-(decimal_t lhs, decimal_t const& rhs) {
            lhs -= rhs;
            return lhs;
        }

        [[nodiscard]] friend constexpr decimal_t operator*(decimal_t lhs, decimal_t const& rhs) {
            lhs *= rhs;
            return lhs;
        }

        [[nodiscard]] friend constexpr decimal_t operator/(decimal_t lhs, decimal_t const& rhs) {
            lhs /= rhs;
            return lhs;
        }

        [[nodiscard]] constexpr decimal_t operator+() const {
            return *this;
        }

        [[nodiscard]] constexpr decimal_t operator-() const requires std::is_signed_v<significand_t> {
            return decimal_t{-significand, exponent};
        }

        [[nodiscard]] friend constexpr bool operator==(decimal_t const& lhs, decimal_t const& rhs) {
            auto lhs_aligned = impl::widen<significand_t, exponent_t>(0);
            auto rhs_aligned = impl::widen<significand_t, exponent_t>(0);
            exponent_t out_exponent=0;
            impl::align(lhs.significand, lhs.exponent, rhs.significand, rhs.exponent, lhs_aligned, rhs_aligned, out_exponent);
            return lhs_aligned == rhs_aligned;
        }

        [[nodiscard]] friend constexpr std::strong_ordering operator<=>(decimal_t const& lhs, decimal_t const& rhs) {
            auto lhs_aligned = impl::widen<significand_t, exponent_t>(0);
            auto rhs_aligned = impl::widen<significand_t, exponent_t>(0);
            exponent_t out_exponent=0;
            impl::align(lhs.significand, lhs.exponent, rhs.significand, rhs.exponent, lhs_aligned, rhs_aligned, out_exponent);
            if (lhs_aligned < rhs_aligned) return std::strong_ordering::less;
            if (lhs_aligned > rhs_aligned) return std::strong_ordering::greater;
            return std::strong_ordering::equal;
        }

        significand_t significand;
        exponent_t exponent;        
    };

    template<class significand_t, class exponent_t> 
    [[nodiscard]] constexpr auto abs(decimal_t<significand_t, exponent_t> const& x) {
        if constexpr (std::is_signed_v<significand_t>)
            return x.significand < 0 ? decimal_t<significand_t, exponent_t>{static_cast<significand_t>(-x.significand), x.exponent} : x;
        else return x;
    }

    template<class significand_t, class exponent_t>
    [[nodiscard]] constexpr auto checksum(decimal_t<significand_t, exponent_t> const& x) -> std::uint64_t {
        if constexpr (sizeof(significand_t) <= sizeof(std::uint64_t))
            return static_cast<std::uint64_t>(x.significand) ^ (static_cast<std::uint64_t>(static_cast<std::uint32_t>(x.exponent)) << 32);
        else {
            #if defined(__SIZEOF_INT128__)
            auto value = static_cast<unsigned __int128>(x.significand);
            return static_cast<std::uint64_t>(value)
                 ^ static_cast<std::uint64_t>(value >> 64)
                 ^ (static_cast<std::uint64_t>(static_cast<std::uint32_t>(x.exponent)) * 0x9e3779b97f4a7c15ULL);
            #else
            return static_cast<std::uint64_t>(x.exponent);
            #endif
        }
    }

} // namespace math::scaled