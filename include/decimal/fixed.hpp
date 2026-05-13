#pragma once

#include <compare>
#include <concepts>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include "bid.hpp"

namespace math::fixed::impl {
    using category = math::bid::category;
    template<class T> concept signed_integral_t = std::integral<T> && std::is_signed_v<T>;
    template<class T> concept unsigned_integral_t = std::integral<T> && !std::is_signed_v<T>;
    constexpr bool is_special(category kind) { return kind == category::nan || kind == category::pinf || kind == category::ninf; }
    constexpr bool is_finite(category kind) { return kind == category::zero || kind == category::positive || kind == category::negative; }

    template<class T> [[nodiscard]] constexpr auto abs_t(T value) {
        if constexpr (std::is_signed_v<T>) return value < 0 ? static_cast<std::make_unsigned_t<T>>(-value) : static_cast<std::make_unsigned_t<T>>(value);
        else return value;
    }

    template<unsigned_integral_t element_t, int exponent> struct traits_t {
        static constexpr int exponent_v = exponent;
        static constexpr unsigned scale_v = exponent < 0 ? static_cast<unsigned>(-exponent) : 0;
        using wide_t = std::conditional_t<(sizeof(element_t) <= sizeof(std::uint32_t)), std::uint64_t, unsigned __int128>;

        static constexpr wide_t pow10() {
            wide_t value = 1;
            for (unsigned i = 0; i < scale_v; ++i) value *= 10;
            return value;
        }
        static constexpr wide_t scale_factor_v = pow10();

        static constexpr element_t checked_cast(wide_t value) {
            if (value > static_cast<wide_t>(std::numeric_limits<element_t>::max()))
                throw std::overflow_error("fixed::decimal_t: overflow");
            return static_cast<element_t>(value);
        }
        static constexpr element_t checked_add(element_t lhs, element_t rhs) {
            const wide_t value = static_cast<wide_t>(lhs) + static_cast<wide_t>(rhs);
            return checked_cast(value);
        }
    };
} // namespace math::fixed::impl

namespace math::fixed {
    using category = math::bid::category;

    template <class element_t, int exponent> struct decimal_t {
        static_assert(impl::unsigned_integral_t<element_t>, "element_t must be unsigned integral");
        using element_type_t = element_t;
        using significand_t = element_t;
        using exponent_type_t = int;
        using category_type_t = category;
        using traits_t = impl::traits_t<element_t, exponent>;
        static constexpr int exponent_v = exponent;
        static constexpr auto scale_factor_v = traits_t::scale_factor_v;

        constexpr decimal_t() = default;
        constexpr decimal_t(category kind, element_t significand): kind(kind), significand(significand) { normalize(); }
        constexpr decimal_t(std::unsigned_integral auto value): kind(value == 0 ? category::zero : category::positive), significand(static_cast<element_t>(value)) { normalize(); }
        constexpr decimal_t(std::signed_integral auto value): kind(value < 0 ? category::negative : (value == 0 ? category::zero : category::positive)), significand(static_cast<element_t>(impl::abs_t(value))) { normalize(); }

        constexpr void normalize() {
            if (impl::is_special(kind)) return significand = 0, void();
            if (significand == 0) return kind = category::zero, void();
            if (kind == category::zero) kind = category::positive;
        }

        [[nodiscard]] constexpr bool is_zero() const { return kind == category::zero; }
        [[nodiscard]] constexpr bool is_finite() const { return impl::is_finite(kind); }
        [[nodiscard]] constexpr bool is_special() const { return impl::is_special(kind); }
        [[nodiscard]] constexpr auto as_tuple() const { return std::tuple{kind, significand, exponent}; }
        [[nodiscard]] constexpr auto decompose() const { return as_tuple(); }
        [[nodiscard]] inline std::string coefficient() const { return std::to_string(significand); }

        [[nodiscard]] inline std::string str() const {
            switch(kind) {
                case category::nan: return "nan";
                case category::pinf: return "+inf";
                case category::ninf: return "-inf";
                case category::zero: return "0e" + std::to_string(exponent);
                default: return std::string{kind == category::negative ? "-" : ""} + coefficient() + "e" + std::to_string(exponent);
            }
        }

        [[nodiscard]] explicit operator std::string() const { return str(); }

        [[nodiscard]] constexpr long double to_long_double() const {
            switch(kind) {
                case category::nan: return std::numeric_limits<long double>::quiet_NaN();
                case category::pinf: return std::numeric_limits<long double>::infinity();
                case category::ninf: return -std::numeric_limits<long double>::infinity();
                default: {
                    long double value = static_cast<long double>(significand);
                    if constexpr (exponent >= 0) {
                        for (int i = 0; i < exponent; ++i) value *= 10.0L;
                    } else {
                        for (int i = 0; i < -exponent; ++i) value /= 10.0L;
                    }
                    return kind == category::negative ? -value : value;
                }
            }
        }

        [[nodiscard]] explicit constexpr operator float() const { return static_cast<float>(to_long_double()); }
        [[nodiscard]] explicit constexpr operator double() const { return static_cast<double>(to_long_double()); }
        [[nodiscard]] explicit constexpr operator long double() const { return to_long_double(); }

        constexpr decimal_t& operator+=(decimal_t const& rhs) {
            if (!is_finite() || !rhs.is_finite()) throw std::domain_error("fixed::decimal_t: non-finite addition");
            if (kind == rhs.kind) significand = traits_t::checked_add(significand, rhs.significand);
            else {
                if (significand == rhs.significand) return kind = category::zero, significand = 0, *this;
                if (significand > rhs.significand) significand -= rhs.significand;
                else significand = static_cast<element_t>(rhs.significand - significand), kind = rhs.kind;
            }
            normalize();
            return *this;
        }

        constexpr decimal_t& operator-=(decimal_t const& rhs) {
            auto temp = rhs;
            if (temp.kind == category::positive) temp.kind = category::negative;
            else if (temp.kind == category::negative) temp.kind = category::positive;
            return *this += temp;
        }

        constexpr decimal_t& operator*=(decimal_t const& rhs) {
            if (!is_finite() || !rhs.is_finite()) throw std::domain_error("fixed::decimal_t: non-finite multiplication");
            if (is_zero() || rhs.is_zero()) return kind = category::zero, significand = 0, *this;
            using wide_t = typename traits_t::wide_t;
            wide_t value = static_cast<wide_t>(significand) * static_cast<wide_t>(rhs.significand);
            if constexpr (exponent < 0) value /= traits_t::scale_factor_v;
            else if constexpr (exponent > 0) value *= traits_t::scale_factor_v;
            significand = traits_t::checked_cast(value);
            kind = kind == rhs.kind ? category::positive : category::negative;
            normalize();
            return *this;
        }

        constexpr decimal_t& operator/=(decimal_t const& rhs) {
            if (!is_finite() || !rhs.is_finite()) throw std::domain_error("fixed::decimal_t: non-finite division");
            if (rhs.is_zero()) throw std::domain_error("fixed::decimal_t: division by zero");
            if (is_zero()) return *this;
            using wide_t = typename traits_t::wide_t;
            wide_t numerator = static_cast<wide_t>(significand);
            wide_t denominator = static_cast<wide_t>(rhs.significand);
            if constexpr (exponent < 0) numerator *= traits_t::scale_factor_v;
            else if constexpr (exponent > 0) denominator *= traits_t::scale_factor_v;
            significand = traits_t::checked_cast(numerator / denominator);
            kind = significand == 0 ? category::zero : (kind == rhs.kind ? category::positive : category::negative);
            normalize();
            return *this;
        }

        [[nodiscard]] friend constexpr decimal_t operator+(decimal_t lhs, decimal_t const& rhs) { lhs += rhs; return lhs; }
        [[nodiscard]] friend constexpr decimal_t operator-(decimal_t lhs, decimal_t const& rhs) { lhs -= rhs; return lhs; }
        [[nodiscard]] friend constexpr decimal_t operator*(decimal_t lhs, decimal_t const& rhs) { lhs *= rhs; return lhs; }
        [[nodiscard]] friend constexpr decimal_t operator/(decimal_t lhs, decimal_t const& rhs) { lhs /= rhs; return lhs; }
        [[nodiscard]] constexpr decimal_t operator+() const { return *this; }

        [[nodiscard]] constexpr decimal_t operator-() const {
            auto out = *this;
            if (out.kind == category::positive) out.kind = category::negative;
            else if (out.kind == category::negative) out.kind = category::positive;
            return out;
        }

        [[nodiscard]] friend constexpr bool operator==(decimal_t const& lhs, decimal_t const& rhs) {
            if (lhs.kind == category::nan || rhs.kind == category::nan) return false;
            if (lhs.kind != rhs.kind) return lhs.is_zero() && rhs.is_zero();
            return lhs.significand == rhs.significand;
        }

        [[nodiscard]] friend constexpr std::partial_ordering operator<=>(decimal_t const& lhs, decimal_t const& rhs) {
            if (lhs.kind == category::nan || rhs.kind == category::nan) return std::partial_ordering::unordered;
            if (lhs.kind == category::pinf) return rhs.kind == category::pinf ? std::partial_ordering::equivalent : std::partial_ordering::greater;
            if (lhs.kind == category::ninf) return rhs.kind == category::ninf ? std::partial_ordering::equivalent : std::partial_ordering::less;
            if (rhs.kind == category::pinf) return std::partial_ordering::less;
            if (rhs.kind == category::ninf) return std::partial_ordering::greater;
            if (lhs.kind == category::negative && rhs.kind != category::negative) return std::partial_ordering::less;
            if (lhs.kind != category::negative && rhs.kind == category::negative) return std::partial_ordering::greater;
            if (lhs.significand == rhs.significand) return std::partial_ordering::equivalent;
            if (lhs.kind == category::negative) return lhs.significand < rhs.significand ? std::partial_ordering::greater : std::partial_ordering::less;
            else return lhs.significand < rhs.significand ? std::partial_ordering::less : std::partial_ordering::greater;
        }

        category kind = category::zero;
        element_t significand = 0;
    };

    template<class element_t, int exponent>
    [[nodiscard]] inline auto decompose(decimal_t<element_t, exponent> const& x) {
        return x.decompose();
    }

    template<class element_t, int exponent>
    [[nodiscard]] inline auto checksum(decimal_t<element_t, exponent> const& x) -> std::uint64_t {
        std::uint64_t h = static_cast<std::uint64_t>(static_cast<unsigned char>(x.kind)) * 0x9e3779b97f4a7c15ULL;
        h = (h << 5) ^ (h >> 2) ^ static_cast<std::uint64_t>(x.significand);
        h ^= static_cast<std::uint64_t>(static_cast<std::int32_t>(exponent)) * 0xbf58476d1ce4e5b9ULL;
        return h;
    }
} // namespace math::fixed
