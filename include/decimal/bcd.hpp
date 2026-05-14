#pragma once

#include <array>
#include <bit>
#include <compare>
#include <concepts>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>

#include "bid.hpp"

namespace math::bcd::impl {
    using category = math::bid::category;

    template<class T> concept signed_integral_t = std::integral<T> && std::is_signed_v<T>;
    template<class T> concept storage_t = std::is_trivially_copyable_v<T> && (sizeof(T) == 4 || sizeof(T) == 8 || sizeof(T) == 16);

    constexpr bool is_special(category kind) { return kind == category::nan || kind == category::pinf || kind == category::ninf; }
    constexpr bool is_finite(category kind) { return kind == category::zero || kind == category::positive || kind == category::negative; }

    template<storage_t significand_t> inline constexpr std::size_t bytes_n = sizeof(significand_t);
    template<storage_t significand_t> inline constexpr std::size_t digits_n = 2 * sizeof(significand_t);

    template<storage_t significand_t> using bytes_t = std::array<std::uint8_t, bytes_n<significand_t>>;
    template<storage_t significand_t> using digits_t = std::array<std::uint8_t, digits_n<significand_t>>;

    template<storage_t significand_t> [[nodiscard]] inline bytes_t<significand_t> to_bytes(significand_t const& value) {
        return std::bit_cast<bytes_t<significand_t>>(value);
    }
    template<storage_t significand_t> [[nodiscard]] inline significand_t from_bytes(bytes_t<significand_t> const& bytes) {
        return std::bit_cast<significand_t>(bytes);
    }

    template<storage_t significand_t> constexpr void clear(significand_t& value) {
        value = from_bytes<significand_t>({});
    }

    template<storage_t significand_t> [[nodiscard]] constexpr std::uint8_t get_digit(significand_t const& value, std::size_t i) {
        const auto b = to_bytes(value);
        const auto x = b[i >> 1];
        return (i & 1) ? static_cast<std::uint8_t>((x >> 4) & 0x0f) : static_cast<std::uint8_t>(x & 0x0f);
    }
    template<storage_t significand_t> constexpr void set_digit(significand_t& value, std::size_t i, std::uint8_t d) {
        auto b = to_bytes(value);
        auto& x = b[i >> 1];
        d &= 0x0f;
        if (i & 1) x = static_cast<std::uint8_t>((x & 0x0f) | (d << 4));
        else x = static_cast<std::uint8_t>((x & 0xf0) | d);
        value = from_bytes<significand_t>(b);
    }

    template<storage_t significand_t> [[nodiscard]] constexpr digits_t<significand_t> unpack(significand_t const& value) {
        digits_t<significand_t> out{};
        for (std::size_t i = 0; i < digits_n<significand_t>; ++i) out[i] = get_digit(value, i);
        return out;
    }
    template<storage_t significand_t> [[nodiscard]] constexpr significand_t pack(digits_t<significand_t> const& in) {
        significand_t out{};
        clear(out);
        for (std::size_t i = 0; i < digits_n<significand_t>; ++i) set_digit(out, i, in[i]);
        return out;
    }

    template<storage_t significand_t> [[nodiscard]] constexpr std::size_t used_digits(significand_t const& value) {
        for (std::size_t i = digits_n<significand_t>; i > 0; --i)
            if (get_digit(value, i - 1) != 0) return i;
        return 0;
    }
    template<storage_t significand_t> [[nodiscard]] constexpr bool is_zero(significand_t const& value) {
        return used_digits(value) == 0;
    }
    template<storage_t significand_t> [[nodiscard]] constexpr unsigned trailing_zeroes10(significand_t const& value) {
        unsigned n = 0;
        while (n < digits_n<significand_t> && get_digit(value, n) == 0) ++n;
        return n;
    }

    template<storage_t significand_t> constexpr void shift_left10(significand_t& value, unsigned count) {
        if (!count || is_zero(value)) return;
        const auto size = used_digits(value);
        if (size + count > digits_n<significand_t>) throw std::overflow_error("bcd::decimal_t: capacity overflow");
        auto d = unpack(value);
        for (std::size_t i = size; i > 0; --i) d[i + count - 1] = d[i - 1];
        for (unsigned i = 0; i < count; ++i) d[i] = 0;
        for (std::size_t i = size + count; i < digits_n<significand_t>; ++i) d[i] = 0;
        value = pack<significand_t>(d);
    }
    template<storage_t significand_t> constexpr void shift_right10(significand_t& value, unsigned count = 1) {
        if (!count || is_zero(value)) return;
        auto d = unpack(value);
        if (count >= digits_n<significand_t>) return clear(value);
        for (std::size_t i = 0; i + count < digits_n<significand_t>; ++i) d[i] = d[i + count];
        for (std::size_t i = digits_n<significand_t> - count; i < digits_n<significand_t>; ++i) d[i] = 0;
        value = pack<significand_t>(d);
    }

    template<storage_t significand_t> [[nodiscard]] constexpr int compare(significand_t const& lhs, significand_t const& rhs) {
        const auto ln = used_digits(lhs), rn = used_digits(rhs);
        if (ln < rn) return -1;
        if (ln > rn) return +1;
        for (std::size_t i = ln; i > 0; --i) {
            auto a = get_digit(lhs, i - 1), b = get_digit(rhs, i - 1);
            if (a < b) return -1;
            if (a > b) return +1;
        }
        return 0;
    }

    template<storage_t significand_t, std::unsigned_integral T> constexpr void from_uint(significand_t& out, T value) {
        clear(out);
        std::size_t i = 0;
        while (value) {
            if (i >= digits_n<significand_t>) throw std::overflow_error("bcd::decimal_t: capacity overflow");
            set_digit(out, i++, static_cast<std::uint8_t>(value % 10));
            value /= 10;
        }
    }

    template<class T> [[nodiscard]] constexpr auto abs_t(T value) {
        if constexpr (std::is_signed_v<T>) return value < 0 ? static_cast<std::make_unsigned_t<T>>(-value) : static_cast<std::make_unsigned_t<T>>(value);
        else return value;
    }

    template<storage_t significand_t> [[nodiscard]] inline std::string to_string(significand_t const& value) {
        const auto n = used_digits(value);
        if (!n) return "0";
        std::string out;
        out.resize(n);
        for (std::size_t i = 0; i < n; ++i) out[n - 1 - i] = static_cast<char>('0' + get_digit(value, i));
        return out;
    }

    template<storage_t significand_t> [[nodiscard]] constexpr long double to_long_double(significand_t const& value) {
        long double x = 0;
        const auto n = used_digits(value);
        for (std::size_t i = n; i > 0; --i) x = x * 10.0L + static_cast<long double>(get_digit(value, i - 1));
        return x;
    }

    template<storage_t significand_t> constexpr void add(significand_t& lhs, significand_t const& rhs) {
        const auto ln = used_digits(lhs), rn = used_digits(rhs), n = ln > rn ? ln : rn;
        if (n == digits_n<significand_t>) {
            std::uint8_t c = 0;
            for (std::size_t i = 0; i < n; ++i) {
                const std::uint8_t s = static_cast<std::uint8_t>(get_digit(lhs, i) + get_digit(rhs, i) + c);
                c = static_cast<std::uint8_t>(s / 10);
            }
            if (c) throw std::overflow_error("bcd::decimal_t: capacity overflow");
        }
        std::uint8_t carry = 0;
        for (std::size_t i = 0; i < digits_n<significand_t>; ++i) {
            const std::uint8_t s = static_cast<std::uint8_t>(get_digit(lhs, i) + get_digit(rhs, i) + carry);
            set_digit(lhs, i, static_cast<std::uint8_t>(s % 10));
            carry = static_cast<std::uint8_t>(s / 10);
        }
        if (carry) throw std::overflow_error("bcd::decimal_t: capacity overflow");
    }

    template<storage_t significand_t> constexpr void sub(significand_t& lhs, significand_t const& rhs) {
        if (compare(lhs, rhs) < 0) throw std::domain_error("bcd::decimal_t: negative magnitude in sub");
        std::int16_t borrow = 0;
        for (std::size_t i = 0; i < digits_n<significand_t>; ++i) {
            std::int16_t a = static_cast<std::int16_t>(get_digit(lhs, i)) - borrow;
            std::int16_t b = static_cast<std::int16_t>(get_digit(rhs, i));
            if (a < b) a += 10, borrow = 1; else borrow = 0;
            set_digit(lhs, i, static_cast<std::uint8_t>(a - b));
        }
    }

    template<storage_t significand_t> [[nodiscard]] constexpr significand_t mul(significand_t const& lhs, significand_t const& rhs) {
        auto a = unpack(lhs), b = unpack(rhs);
        std::array<std::uint16_t, digits_n<significand_t> * 2> acc{};
        for (std::size_t i = 0; i < digits_n<significand_t>; ++i)
            for (std::size_t j = 0; j < digits_n<significand_t>; ++j)
                acc[i + j] = static_cast<std::uint16_t>(acc[i + j] + a[i] * b[j]);
        for (std::size_t i = 0; i + 1 < acc.size(); ++i) acc[i + 1] = static_cast<std::uint16_t>(acc[i + 1] + acc[i] / 10), acc[i] %= 10;
        for (std::size_t i = digits_n<significand_t>; i < acc.size(); ++i) if (acc[i]) throw std::overflow_error("bcd::decimal_t: capacity overflow");
        digits_t<significand_t> out{};
        for (std::size_t i = 0; i < digits_n<significand_t>; ++i) out[i] = static_cast<std::uint8_t>(acc[i]);
        return pack<significand_t>(out);
    }

    template<storage_t significand_t> constexpr void mul10_add(significand_t& x, std::uint8_t d) {
        shift_left10(x, 1);
        set_digit(x, 0, d);
    }

    template<storage_t significand_t> [[nodiscard]] constexpr significand_t div(significand_t const& lhs, significand_t const& rhs) {
        if (is_zero(rhs)) throw std::domain_error("bcd::decimal_t: division by zero");
        if (compare(lhs, rhs) < 0) return significand_t{};
        significand_t q{}, r{};
        std::array<std::uint8_t, digits_n<significand_t>> q_msd{};
        std::size_t q_size = 0;
        const auto ln = used_digits(lhs);
        for (std::size_t i = ln; i > 0; --i) {
            mul10_add(r, get_digit(lhs, i - 1));
            std::uint8_t digit = 0;
            while (compare(r, rhs) >= 0) sub(r, rhs), ++digit;
            q_msd[q_size++] = digit;
        }
        std::size_t first = 0;
        while (first < q_size && q_msd[first] == 0) ++first;
        for (std::size_t i = first; i < q_size; ++i) {
            if (i - first >= digits_n<significand_t>) throw std::overflow_error("bcd::decimal_t: capacity overflow");
            set_digit(q, q_size - 1 - i, q_msd[i]);
        }
        return q;
    }

    template<storage_t significand_t, signed_integral_t exponent_t>
    constexpr void normalize(category& kind, significand_t& significand, exponent_t& exponent) {
        if (is_special(kind)) return clear(significand), exponent = 0, void();
        if (is_zero(significand)) return kind = category::zero, exponent = 0, void();
        if (kind == category::zero) kind = category::positive;
        const unsigned n = trailing_zeroes10(significand);
        if (n) shift_right10(significand, n), exponent = static_cast<exponent_t>(exponent + static_cast<exponent_t>(n));
    }

    template<storage_t significand_t, signed_integral_t exponent_t>
    constexpr void align(significand_t lhs_significand, exponent_t lhs_exponent, significand_t rhs_significand, exponent_t rhs_exponent,
        significand_t& lhs_aligned, significand_t& rhs_aligned, exponent_t& exponent_out) {
        exponent_out = lhs_exponent < rhs_exponent ? lhs_exponent : rhs_exponent;
        lhs_aligned = lhs_significand, rhs_aligned = rhs_significand;
        shift_left10(lhs_aligned, static_cast<unsigned>(lhs_exponent - exponent_out));
        shift_left10(rhs_aligned, static_cast<unsigned>(rhs_exponent - exponent_out));
    }
} // namespace math::bcd::impl


namespace math::bcd {
    using category = math::bid::category;

    template <class significand_tt = std::uint64_t, class exponent_tt = std::int16_t>
        requires impl::storage_t<significand_tt> && impl::signed_integral_t<exponent_tt>
    struct decimal_t {
        using significand_t = significand_tt;
        using exponent_t = exponent_tt;
        using element_t = decimal_t<significand_t, exponent_tt>;
        using category_t = category;
        static inline constexpr std::size_t digits_n = impl::digits_n<significand_t>;

        constexpr decimal_t() = default;
        constexpr decimal_t(category kind, significand_t significand, exponent_t exponent = 0): kind(kind), significand(significand), exponent(exponent) { normalize(); }
        constexpr decimal_t(std::unsigned_integral auto value): kind(value == 0 ? category::zero : category::positive) { impl::from_uint(significand, value), normalize(); }
        constexpr decimal_t(std::signed_integral auto value): kind(value < 0 ? category::negative : (value == 0 ? category::zero : category::positive)) { impl::from_uint(significand, impl::abs_t(value)), normalize(); }

        explicit decimal_t(std::string_view digits, exponent_t exponent = 0): kind(category::positive), exponent(exponent) {
            if (digits.empty()) return;
            std::size_t first = 0;
            if (digits[0] == '-') kind = category::negative, first = 1;
            else if (digits[0] == '+') first = 1;
            for (std::size_t i = first; i < digits.size(); ++i) {
                const char c = digits[i];
                if (c < '0' || c > '9') throw std::invalid_argument("bcd::decimal_t: invalid digit");
                impl::shift_left10(significand, 1);
                impl::set_digit(significand, 0, static_cast<std::uint8_t>(c - '0'));
            }
            normalize();
        }

        constexpr void normalize() { impl::normalize(kind, significand, exponent); }
        [[nodiscard]] constexpr bool is_zero() const { return kind == category::zero; }
        [[nodiscard]] constexpr bool is_finite() const { return impl::is_finite(kind); }
        [[nodiscard]] constexpr bool is_special() const { return impl::is_special(kind); }
        [[nodiscard]] constexpr auto packed_bcd() const { return significand; }
        [[nodiscard]] constexpr auto as_tuple() const { return std::tuple{kind, significand, exponent}; }
        [[nodiscard]] constexpr auto decompose() const { return as_tuple(); }
        [[nodiscard]] inline std::string coefficient() const { return impl::to_string(significand); }

        [[nodiscard]] inline std::string str() const {
            switch(kind) {
                case category::nan:  return "nan";
                case category::pinf: return "+inf";
                case category::ninf: return "-inf";
                case category::zero: return "0e0";
                default: return std::string{kind == category::negative ? "-" : ""} + coefficient() + "e" + std::to_string(exponent);
            }
        }

        [[nodiscard]] explicit operator std::string() const { return str(); }

        [[nodiscard]] constexpr long double to_long_double() const {
            switch(kind) {
                case category::nan:  return std::numeric_limits<long double>::quiet_NaN();
                case category::pinf: return std::numeric_limits<long double>::infinity();
                case category::ninf: return -std::numeric_limits<long double>::infinity();
                default: {
                    long double value = impl::to_long_double(significand);
                    if (exponent >= 0) for (exponent_t i = 0; i < exponent; ++i) value *= 10.0L;
                    else for (exponent_t i = 0; i < -exponent; ++i) value /= 10.0L;
                    return kind == category::negative ? -value : value;
                }
            }
        }

        [[nodiscard]] explicit constexpr operator float() const { return static_cast<float>(to_long_double()); }
        [[nodiscard]] explicit constexpr operator double() const { return static_cast<double>(to_long_double()); }
        [[nodiscard]] explicit constexpr operator long double() const { return to_long_double(); }

        constexpr decimal_t& operator+=(decimal_t const& rhs) {
            if (!is_finite() || !rhs.is_finite()) throw std::domain_error("bcd::decimal_t: non-finite addition");
            significand_t lhs_aligned{}, rhs_aligned{};
            exponent_t out_exponent = 0;
            impl::align(significand, exponent, rhs.significand, rhs.exponent, lhs_aligned, rhs_aligned, out_exponent);
            if (kind == rhs.kind) impl::add(lhs_aligned, rhs_aligned), significand = lhs_aligned, exponent = out_exponent;
            else {
                const int cmp = impl::compare(lhs_aligned, rhs_aligned);
                if (cmp == 0) return kind = category::zero, impl::clear(significand), exponent = 0, *this;
                if (cmp > 0) impl::sub(lhs_aligned, rhs_aligned), significand = lhs_aligned, exponent = out_exponent;
                else impl::sub(rhs_aligned, lhs_aligned), significand = rhs_aligned, exponent = out_exponent, kind = rhs.kind;
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
            if (!is_finite() || !rhs.is_finite()) throw std::domain_error("bcd::decimal_t: non-finite multiplication");
            if (is_zero() || rhs.is_zero()) return kind = category::zero, impl::clear(significand), exponent = 0, *this;
            significand = impl::mul(significand, rhs.significand);
            exponent = static_cast<exponent_t>(exponent + rhs.exponent);
            kind = kind == rhs.kind ? category::positive : category::negative;
            normalize();
            return *this;
        }

        constexpr decimal_t& operator/=(decimal_t const& rhs) {
            if (!is_finite() || !rhs.is_finite()) throw std::domain_error("bcd::decimal_t: non-finite division");
            if (rhs.is_zero()) throw std::domain_error("bcd::decimal_t: division by zero");
            if (is_zero()) return *this;
            significand = impl::div(significand, rhs.significand);
            exponent = static_cast<exponent_t>(exponent - rhs.exponent);
            kind = impl::is_zero(significand) ? category::zero : (kind == rhs.kind ? category::positive : category::negative);
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
            if (!lhs.is_finite()) return true;
            significand_t lhs_aligned{}, rhs_aligned{};
            exponent_t out_exponent = 0;
            impl::align(lhs.significand, lhs.exponent, rhs.significand, rhs.exponent, lhs_aligned, rhs_aligned, out_exponent);
            return impl::compare(lhs_aligned, rhs_aligned) == 0;
        }

        [[nodiscard]] friend constexpr std::partial_ordering operator<=>(decimal_t const& lhs, decimal_t const& rhs) {
            if (lhs.kind == category::nan || rhs.kind == category::nan) return std::partial_ordering::unordered;
            if (lhs.kind == category::pinf) return rhs.kind == category::pinf ? std::partial_ordering::equivalent : std::partial_ordering::greater;
            if (lhs.kind == category::ninf) return rhs.kind == category::ninf ? std::partial_ordering::equivalent : std::partial_ordering::less;
            if (rhs.kind == category::pinf) return std::partial_ordering::less;
            if (rhs.kind == category::ninf) return std::partial_ordering::greater;
            significand_t lhs_aligned{}, rhs_aligned{};
            exponent_t out_exponent = 0;
            impl::align(lhs.significand, lhs.exponent, rhs.significand, rhs.exponent, lhs_aligned, rhs_aligned, out_exponent);
            if (lhs.kind == category::negative && rhs.kind != category::negative) return std::partial_ordering::less;
            if (lhs.kind != category::negative && rhs.kind == category::negative) return std::partial_ordering::greater;
            const int cmp = impl::compare(lhs_aligned, rhs_aligned);
            if (cmp == 0) return std::partial_ordering::equivalent;
            if (lhs.kind == category::negative) return cmp < 0 ? std::partial_ordering::greater : std::partial_ordering::less;
            else return cmp < 0 ? std::partial_ordering::less : std::partial_ordering::greater;
        }

        category kind = category::zero;
        significand_t significand{};
        exponent_t exponent = 0;
    };

    template<class significand_t, class exponent_t>
    [[nodiscard]] inline auto decompose(decimal_t<significand_t, exponent_t> const& x) { return x.decompose(); }

    template<class significand_t, class exponent_t>
    [[nodiscard]] inline auto checksum(decimal_t<significand_t, exponent_t> const& x) -> std::uint64_t {
        std::uint64_t h = static_cast<std::uint64_t>(static_cast<unsigned char>(x.kind)) * 0x9e3779b97f4a7c15ULL;
        const auto b = impl::to_bytes(x.significand);
        for (auto v : b) h = (h << 5) ^ (h >> 2) ^ v;
        return h ^ (static_cast<std::uint64_t>(static_cast<std::uint32_t>(x.exponent)) << 32);
    }
} // namespace math::bcd