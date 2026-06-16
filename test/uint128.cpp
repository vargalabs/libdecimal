/* This file is part of the libdecimal project and is licensed under the MIT License.
 * Copyright © 2025–2026 Varga Labs, Toronto, ON, Canada 🇨🇦 */

// Exercises math::uint128. On GCC/Clang this is `unsigned __int128`; on MSVC it
// is the portable two-limb struct. The checks below are oracle-free algebraic
// invariants, so they validate whichever implementation is active — in
// particular they give direct coverage of the portable struct on MSVC.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <cstdint>
#include <random>

#include <doctest/all>
#include <decimal/uint128.hpp>

using u128 = math::uint128;

static u128 make(std::uint64_t hi, std::uint64_t lo) {
    return (u128{hi} << 64u) | u128{lo};
}

TEST_CASE("uint128: construction and narrowing") {
    u128 a = make(0x0123456789abcdefULL, 0xfedcba9876543210ULL);
    CHECK(static_cast<std::uint64_t>(a) == 0xfedcba9876543210ULL);
    CHECK(static_cast<std::uint64_t>(a >> 64u) == 0x0123456789abcdefULL);
    CHECK(static_cast<std::uint64_t>(u128{42}) == 42u);
}

TEST_CASE("uint128: shift assembles 128-bit value") {
    u128 one = 1;
    u128 high = one << 113u;            // a bit only representable in 128 bits
    CHECK(static_cast<std::uint64_t>(high) == 0);
    CHECK(static_cast<std::uint64_t>(high >> 113u) == 1);
    CHECK((high >> 113u) == one);
}

TEST_CASE("uint128: additive identities and carry") {
    u128 lo_max = make(0, ~0ULL);
    u128 sum = lo_max + u128{1};        // carries into the high limb
    CHECK(static_cast<std::uint64_t>(sum) == 0);
    CHECK(static_cast<std::uint64_t>(sum >> 64u) == 1);
    CHECK(sum - u128{1} == lo_max);
}

TEST_CASE("uint128: 64x64 widening multiply has no truncation") {
    u128 a = 0x100000000ULL;            // 2^32
    u128 b = 0x100000000ULL;            // 2^32
    u128 p = a * b;                     // 2^64 -> needs the high limb
    CHECK(static_cast<std::uint64_t>(p) == 0);
    CHECK(static_cast<std::uint64_t>(p >> 64u) == 1);
}

TEST_CASE("uint128: divmod invariants over random values") {
    std::mt19937_64 rng(12345);
    for (int i = 0; i < 200000; ++i) {
        u128 n = make(rng(), rng());
        u128 d = make(rng() & ((1ULL << (rng() % 40)) ? ~0ULL : 0ULL), rng());
        if (d == u128{0}) d = u128{1};
        u128 q = n / d;
        u128 r = n % d;
        CHECK(r < d);                   // remainder strictly below divisor
        CHECK(q * d + r == n);          // division identity
    }
}

TEST_CASE("uint128: multiply/divide round-trip") {
    std::mt19937_64 rng(999);
    for (int i = 0; i < 200000; ++i) {
        u128 a = u128{rng()};           // < 2^64
        u128 b = u128{(rng() % 0xffffffffULL) + 1};  // nonzero, < 2^32
        u128 p = a * b;
        CHECK(p / b == a);
        CHECK(p % b == u128{0});
    }
}

TEST_CASE("uint128: comparison is total and consistent") {
    std::mt19937_64 rng(7);
    for (int i = 0; i < 100000; ++i) {
        u128 a = make(rng(), rng());
        u128 b = make(rng(), rng());
        CHECK((a < b) == (b > a));
        CHECK((a <= b) == !(a > b));
        CHECK((a == b) == (!(a < b) && !(b < a)));
        CHECK((a != b) == (a < b || b < a));
    }
}

TEST_CASE("uint128: bitwise relations") {
    std::mt19937_64 rng(55);
    for (int i = 0; i < 100000; ++i) {
        u128 a = make(rng(), rng());
        u128 b = make(rng(), rng());
        CHECK((a | b) == ((a & b) | (a ^ b)));
        CHECK((a & ~a) == u128{0});
        CHECK((a | ~a) == ~u128{0});
    }
}
