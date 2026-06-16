/* This file is part of the libdecimal project and is licensed under the MIT License.
 * Copyright © 2025–2026 Varga Labs, Toronto, ON, Canada 🇨🇦 */

#pragma once

// Portable unsigned 128-bit integer.
//
// On toolchains that provide the `__int128` builtin (GCC, Clang, including
// Apple Clang) `math::uint128` is simply `unsigned __int128`, so those
// platforms keep bit-for-bit identical behaviour and code generation.
//
// MSVC has no native 128-bit integer type, so there `math::uint128` is a
// two-limb struct that reproduces the subset of `unsigned __int128` semantics
// the library relies on (bitwise ops, shifts, full arithmetic including
// multiply/divide, and comparisons). Everything is `constexpr` so the struct
// works in the constant-evaluation contexts the headers use (e.g. the
// `fixed::traits_t` scale factors and the `bid::decompose` coefficient masks).
//
// Define DECIMAL_FORCE_PORTABLE_UINT128 to compile the struct even where the
// builtin exists; this is used by the test-suite to validate the portable
// implementation against the native type.

#include <cstdint>
#include <type_traits>

namespace math {

#if defined(__SIZEOF_INT128__) && !defined(DECIMAL_FORCE_PORTABLE_UINT128)

using uint128 = unsigned __int128;

#else

struct uint128 {
    std::uint64_t lo_ = 0;
    std::uint64_t hi_ = 0;

    constexpr uint128() = default;
    constexpr uint128(std::uint64_t lo, std::uint64_t hi) noexcept : lo_(lo), hi_(hi) {}

    // Implicit construction from any builtin integral, mirroring the implicit
    // widening of `unsigned __int128`. Negative signed values sign-extend, as
    // a conversion to `unsigned __int128` would.
    template <class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
    constexpr uint128(T value) noexcept
        : lo_(static_cast<std::uint64_t>(value)),
          hi_(is_negative(value) ? ~std::uint64_t{0} : std::uint64_t{0}) {}

    // Narrowing back to a builtin integral is explicit (keeps the low bits),
    // matching how `static_cast<uint64_t>(unsigned __int128)` truncates.
    template <class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
    constexpr explicit operator T() const noexcept { return static_cast<T>(lo_); }

    constexpr explicit operator bool() const noexcept { return lo_ != 0 || hi_ != 0; }

    // ---- bitwise ----------------------------------------------------------
    friend constexpr uint128 operator|(uint128 a, uint128 b) noexcept { return {a.lo_ | b.lo_, a.hi_ | b.hi_}; }
    friend constexpr uint128 operator&(uint128 a, uint128 b) noexcept { return {a.lo_ & b.lo_, a.hi_ & b.hi_}; }
    friend constexpr uint128 operator^(uint128 a, uint128 b) noexcept { return {a.lo_ ^ b.lo_, a.hi_ ^ b.hi_}; }
    constexpr uint128 operator~() const noexcept { return {~lo_, ~hi_}; }

    // ---- shifts -----------------------------------------------------------
    friend constexpr uint128 operator<<(uint128 a, unsigned s) noexcept {
        if (s == 0) return a;
        if (s >= 128) return uint128{0, 0};
        if (s >= 64) return uint128{0, a.lo_ << (s - 64)};
        return uint128{a.lo_ << s, (a.hi_ << s) | (a.lo_ >> (64 - s))};
    }
    friend constexpr uint128 operator>>(uint128 a, unsigned s) noexcept {
        if (s == 0) return a;
        if (s >= 128) return uint128{0, 0};
        if (s >= 64) return uint128{a.hi_ >> (s - 64), 0};
        return uint128{(a.lo_ >> s) | (a.hi_ << (64 - s)), a.hi_ >> s};
    }
    friend constexpr uint128 operator<<(uint128 a, int s) noexcept { return a << static_cast<unsigned>(s); }
    friend constexpr uint128 operator>>(uint128 a, int s) noexcept { return a >> static_cast<unsigned>(s); }

    // ---- additive ---------------------------------------------------------
    friend constexpr uint128 operator+(uint128 a, uint128 b) noexcept {
        const std::uint64_t lo = a.lo_ + b.lo_;
        const std::uint64_t carry = (lo < a.lo_) ? 1u : 0u;
        return uint128{lo, a.hi_ + b.hi_ + carry};
    }
    friend constexpr uint128 operator-(uint128 a, uint128 b) noexcept {
        const std::uint64_t lo = a.lo_ - b.lo_;
        const std::uint64_t borrow = (a.lo_ < b.lo_) ? 1u : 0u;
        return uint128{lo, a.hi_ - b.hi_ - borrow};
    }

    // ---- multiply ---------------------------------------------------------
    friend constexpr uint128 operator*(uint128 a, uint128 b) noexcept {
        const std::uint64_t a0 = a.lo_ & 0xffffffffull, a1 = a.lo_ >> 32;
        const std::uint64_t b0 = b.lo_ & 0xffffffffull, b1 = b.lo_ >> 32;
        const std::uint64_t p00 = a0 * b0;
        const std::uint64_t p01 = a0 * b1;
        const std::uint64_t p10 = a1 * b0;
        const std::uint64_t p11 = a1 * b1;
        const std::uint64_t mid = (p00 >> 32) + (p01 & 0xffffffffull) + (p10 & 0xffffffffull);
        const std::uint64_t lo = (p00 & 0xffffffffull) | (mid << 32);
        std::uint64_t hi = p11 + (p01 >> 32) + (p10 >> 32) + (mid >> 32);
        hi += a.lo_ * b.hi_ + a.hi_ * b.lo_;  // high cross terms (mod 2^64)
        return uint128{lo, hi};
    }

    // ---- divide / modulo (schoolbook long division) -----------------------
    static constexpr void divmod(uint128 n, uint128 d, uint128& q, uint128& r) noexcept {
        q = uint128{0, 0};
        r = uint128{0, 0};
        if (d.lo_ == 0 && d.hi_ == 0) return;  // division by zero: undefined, return 0/0
        for (int i = 127; i >= 0; --i) {
            r = r << 1u;
            const std::uint64_t bit = (i >= 64) ? ((n.hi_ >> (i - 64)) & 1u) : ((n.lo_ >> i) & 1u);
            r.lo_ |= bit;
            if (!(r < d)) {
                r = r - d;
                if (i >= 64) q.hi_ |= (std::uint64_t{1} << (i - 64));
                else q.lo_ |= (std::uint64_t{1} << i);
            }
        }
    }
    friend constexpr uint128 operator/(uint128 a, uint128 b) noexcept { uint128 q, r; divmod(a, b, q, r); return q; }
    friend constexpr uint128 operator%(uint128 a, uint128 b) noexcept { uint128 q, r; divmod(a, b, q, r); return r; }

    // ---- compound assignment ---------------------------------------------
    constexpr uint128& operator+=(uint128 o) noexcept { return *this = *this + o; }
    constexpr uint128& operator-=(uint128 o) noexcept { return *this = *this - o; }
    constexpr uint128& operator*=(uint128 o) noexcept { return *this = *this * o; }
    constexpr uint128& operator/=(uint128 o) noexcept { return *this = *this / o; }
    constexpr uint128& operator%=(uint128 o) noexcept { return *this = *this % o; }
    constexpr uint128& operator|=(uint128 o) noexcept { return *this = *this | o; }
    constexpr uint128& operator&=(uint128 o) noexcept { return *this = *this & o; }
    constexpr uint128& operator^=(uint128 o) noexcept { return *this = *this ^ o; }
    constexpr uint128& operator<<=(unsigned s) noexcept { return *this = *this << s; }
    constexpr uint128& operator>>=(unsigned s) noexcept { return *this = *this >> s; }

    // ---- comparisons ------------------------------------------------------
    friend constexpr bool operator==(uint128 a, uint128 b) noexcept { return a.lo_ == b.lo_ && a.hi_ == b.hi_; }
    friend constexpr bool operator!=(uint128 a, uint128 b) noexcept { return !(a == b); }
    friend constexpr bool operator<(uint128 a, uint128 b) noexcept { return a.hi_ < b.hi_ || (a.hi_ == b.hi_ && a.lo_ < b.lo_); }
    friend constexpr bool operator>(uint128 a, uint128 b) noexcept { return b < a; }
    friend constexpr bool operator<=(uint128 a, uint128 b) noexcept { return !(b < a); }
    friend constexpr bool operator>=(uint128 a, uint128 b) noexcept { return !(a < b); }

private:
    template <class T>
    static constexpr bool is_negative(T value) noexcept {
        if constexpr (std::is_signed_v<T>) return value < 0;
        else return false;
    }
};

#endif  // builtin __int128 vs portable

}  // namespace math
