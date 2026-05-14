/* This file is part of the LIBDECIMAL project and is licensed under the MIT License.
 * Copyright © 2025–2026 Varga Labs, Toronto, ON, Canada 🇨🇦 Contact: info@vargalabs.com */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <string>
#include <string_view>
#include <doctest/all>
#include <decimal/bid.hpp>

namespace test {
    using dec32_t  = math::decimal_t<std::uint32_t>;
    using dec64_t  = math::decimal_t<std::uint64_t>;
    using dec128_t = math::decimal_t<math::uint128_t>;

    template <class decimal_t>
    std::string as_string(decimal_t value) {
        return static_cast<std::string>(value);
    }

    inline double as_double(dec32_t value) {
        return static_cast<double>(value);
    }

    inline long double as_long_double(dec64_t value) {
        return static_cast<long double>(value);
    }

    inline long double as_long_double(dec128_t value) {
        return static_cast<long double>(value);
    }
}

TEST_SUITE("import/export::decimal32") {
    TEST_CASE("import from const char*") {
        const test::dec32_t x{"123.456"};
        CHECK(test::as_string(x) == "123.456");
    }

    TEST_CASE("import from std::string") {
        const std::string s{"123.456"};
        const test::dec32_t x{s};
        CHECK(test::as_string(x) == "123.456");
    }

    TEST_CASE("import from std::string_view") {
        const std::string_view s{"123.456"};
        const test::dec32_t x{s};
        CHECK(test::as_string(x) == "123.456");
    }

    TEST_CASE("import from significand and exponent") {
        const test::dec32_t x{123456u, -3};
        CHECK(test::as_string(x) == "123.456");
    }

    TEST_CASE("import from float via constructor") {
        const test::dec32_t x{12.5};
        CHECK(test::as_string(x) == "12.5");
    }

    TEST_CASE("import from float via static from") {
        const auto x = test::dec32_t::from(12.5f);
        CHECK(test::as_string(x) == "12.5");
    }

    TEST_CASE("import from double via static from") {
        const auto x = test::dec32_t::from(12.5);
        CHECK(test::as_string(x) == "12.5");
    }

    TEST_CASE("export to std::string") {
        const test::dec32_t x{"123.456"};
        const std::string s = static_cast<std::string>(x);
        CHECK(s == "123.456");
    }

    TEST_CASE("export to double") {
        const test::dec32_t x{"123.456"};
        CHECK(test::as_double(x) == doctest::Approx(123.456));
    }

    TEST_CASE("bid round-trip through raw representation") {
        const test::dec32_t x{"123.456"};
        const auto raw = static_cast<math::bid_t<std::uint32_t>>(x);
        const auto y   = test::dec32_t::set(raw);
        CHECK(y == x);
        CHECK(test::as_string(y) == "123.456");
    }

    TEST_CASE("dpd round-trip") {
        const test::dec32_t x{"123.456"};
        const auto dpd = static_cast<math::dpd_t<std::uint32_t>>(x);
        const test::dec32_t y{dpd};
        CHECK(y == x);
        CHECK(test::as_string(y) == "123.456");
    }
}

TEST_SUITE("import/export::decimal64") {
    TEST_CASE("import from const char*") {
        const test::dec64_t x{"123.456"};
        CHECK(test::as_string(x) == "123.456");
    }

    TEST_CASE("import from std::string") {
        const std::string s{"123.456"};
        const test::dec64_t x{s};
        CHECK(test::as_string(x) == "123.456");
    }

    TEST_CASE("import from std::string_view") {
        const std::string_view s{"123.456"};
        const test::dec64_t x{s};
        CHECK(test::as_string(x) == "123.456");
    }

    TEST_CASE("import from significand and exponent") {
        const test::dec64_t x{123456ull, -3};
        CHECK(test::as_string(x) == "123.456");
    }

    TEST_CASE("import from long double via constructor") {
        const test::dec64_t x{12.5L};
        CHECK(test::as_string(x) == "12.5");
    }

    TEST_CASE("import from float via static from") {
        const auto x = test::dec64_t::from(12.5f);
        CHECK(test::as_string(x) == "12.5");
    }

    TEST_CASE("import from double via static from") {
        const auto x = test::dec64_t::from(12.5);
        CHECK(test::as_string(x) == "12.5");
    }

    TEST_CASE("import from long double via static from") {
        const auto x = test::dec64_t::from(12.5L);
        CHECK(test::as_string(x) == "12.5");
    }

    TEST_CASE("export to std::string") {
        const test::dec64_t x{"123.456"};
        const std::string s = static_cast<std::string>(x);
        CHECK(s == "123.456");
    }

    TEST_CASE("export to long double") {
        const test::dec64_t x{"123.456"};
        CHECK(test::as_long_double(x) == doctest::Approx(123.456L));
    }

    TEST_CASE("bid round-trip through raw representation") {
        const test::dec64_t x{"123.456"};
        const auto raw = static_cast<math::bid_t<std::uint64_t>>(x);
        const auto y   = test::dec64_t::set(raw);
        CHECK(y == x);
        CHECK(test::as_string(y) == "123.456");
    }

    TEST_CASE("dpd round-trip") {
        const test::dec64_t x{"123.456"};
        const auto dpd = static_cast<math::dpd_t<std::uint64_t>>(x);
        const test::dec64_t y{dpd};
        CHECK(y == x);
        CHECK(test::as_string(y) == "123.456");
    }
}

TEST_SUITE("import/export::decimal128") {
    TEST_CASE("import from const char*") {
        const test::dec128_t x{"123.456"};
        CHECK(test::as_string(x) == "123.456");
    }

    TEST_CASE("import from std::string") {
        const std::string s{"123.456"};
        const test::dec128_t x{s};
        CHECK(test::as_string(x) == "123.456");
    }

    TEST_CASE("import from std::string_view") {
        const std::string_view s{"123.456"};
        const test::dec128_t x{s};
        CHECK(test::as_string(x) == "123.456");
    }

    TEST_CASE("import from significand and exponent") {
        const test::dec128_t x{math::uint128_t{123456u}, -3};
        CHECK(test::as_string(x) == "123.456");
    }

    TEST_CASE("import from long double via constructor") {
        const test::dec128_t x{12.5L};
        CHECK(test::as_string(x) == "12.5");
    }

    TEST_CASE("import from float via static from") {
        const auto x = test::dec128_t::from(12.5f);
        CHECK(test::as_string(x) == "12.5");
    }

    TEST_CASE("import from double via static from") {
        const auto x = test::dec128_t::from(12.5);
        CHECK(test::as_string(x) == "12.5");
    }

    TEST_CASE("import from long double via static from") {
        const auto x = test::dec128_t::from(12.5L);
        CHECK(test::as_string(x) == "12.5");
    }

    TEST_CASE("export to std::string") {
        const test::dec128_t x{"123.456"};
        const std::string s = static_cast<std::string>(x);
        CHECK(s == "123.456");
    }

    TEST_CASE("export to long double") {
        const test::dec128_t x{"123.456"};
        CHECK(test::as_long_double(x) == doctest::Approx(123.456L));
    }

    TEST_CASE("bid round-trip through raw representation") {
        const test::dec128_t x{"123.456"};
        const auto raw = static_cast<math::bid_t<math::uint128_t>>(x);
        const auto y   = test::dec128_t::set(raw);
        CHECK(y == x);
        CHECK(test::as_string(y) == "123.456");
    }

    TEST_CASE("dpd round-trip") {
        const test::dec128_t x{"123.456"};
        const auto dpd = static_cast<math::dpd_t<math::uint128_t>>(x);
        const test::dec128_t y{dpd};
        CHECK(y == x);
        CHECK(test::as_string(y) == "123.456");
    }
}

TEST_SUITE("decompose::decimal32") {
    TEST_CASE("math::decompose returns sign mantissa exponent for positive value") {
        const test::dec32_t x{123456u, -3};
        const auto [signbit, mantissa, exponent] = math::decompose(x);
        CHECK(signbit == false);
        CHECK(mantissa == 123456u);
        CHECK(exponent == -3);
    }

    TEST_CASE("math::decompose returns sign mantissa exponent for negative value") {
        const test::dec32_t x{"-123.456"};
        const auto [signbit, mantissa, exponent] = math::decompose(x);
        CHECK(signbit == true);
        CHECK(mantissa == 123456u);
        CHECK(exponent == -3);
    }

    TEST_CASE("math::mantissa extracts positive significand") {
        const test::dec32_t x{123456u, -3};
        CHECK(math::mantissa(x) == 123456u);
    }

    TEST_CASE("math::mantissa rejects negative values") {
        const test::dec32_t x{"-123.456"};
        CHECK_THROWS(math::mantissa(x));
    }
}

TEST_SUITE("decompose::decimal64") {
    TEST_CASE("math::decompose returns sign mantissa exponent for positive value") {
        const test::dec64_t x{123456ull, -3};
        const auto [signbit, mantissa, exponent] = math::decompose(x);
        CHECK(signbit == false);
        CHECK(mantissa == 123456ull);
        CHECK(exponent == -3);
    }

    TEST_CASE("math::decompose returns sign mantissa exponent for negative value") {
        const test::dec64_t x{"-123.456"};
        const auto [signbit, mantissa, exponent] = math::decompose(x);
        CHECK(signbit == true);
        CHECK(mantissa == 123456ull);
        CHECK(exponent == -3);
    }

    TEST_CASE("math::mantissa extracts positive significand") {
        const test::dec64_t x{123456ull, -3};
        CHECK(math::mantissa(x) == 123456ull);
    }

    TEST_CASE("math::mantissa rejects negative values") {
        const test::dec64_t x{"-123.456"};
        CHECK_THROWS(math::mantissa(x));
    }
}

TEST_SUITE("decompose::decimal128") {
    TEST_CASE("math::decompose returns sign mantissa exponent for positive value") {
        const test::dec128_t x{math::uint128_t{123456u}, -3};
        const auto [signbit, mantissa, exponent] = math::decompose(x);
        CHECK(signbit == false);
        CHECK(mantissa == math::uint128_t{123456u});
        CHECK(exponent == -3);
    }

    TEST_CASE("math::decompose returns sign mantissa exponent for negative value") {
        const test::dec128_t x{"-123.456"};
        const auto [signbit, mantissa, exponent] = math::decompose(x);
        CHECK(signbit == true);
        CHECK(mantissa == math::uint128_t{123456u});
        CHECK(exponent == -3);
    }

    TEST_CASE("math::mantissa extracts positive significand") {
        const test::dec128_t x{math::uint128_t{123456u}, -3};
        CHECK(math::mantissa(x) == math::uint128_t{123456u});
    }

    TEST_CASE("math::mantissa rejects negative values") {
        const test::dec128_t x{"-123.456"};
        CHECK_THROWS(math::mantissa(x));
    }
}

TEST_SUITE("import/export::cross precision round-trip") {
    TEST_CASE("same exact string imports and exports identically across precisions") {
        const auto s32  = test::as_string(test::dec32_t{"123.456"});
        const auto s64  = test::as_string(test::dec64_t{"123.456"});
        const auto s128 = test::as_string(test::dec128_t{"123.456"});

        CHECK(s32 == "123.456");
        CHECK(s64 == "123.456");
        CHECK(s128 == "123.456");
    }

    TEST_CASE("same exact significand/exponent pair exports identically across precisions") {
        CHECK(test::as_string(test::dec32_t{123456u, -3}) == "123.456");
        CHECK(test::as_string(test::dec64_t{123456ull, -3}) == "123.456");
        CHECK(test::as_string(test::dec128_t{math::uint128_t{123456u}, -3}) == "123.456");
    }

    TEST_CASE("decompose agrees across precisions for same exact value") {
        const auto [s32,  m32,  e32 ] = math::decompose(test::dec32_t{"-123.456"});
        const auto [s64,  m64,  e64 ] = math::decompose(test::dec64_t{"-123.456"});
        const auto [s128, m128, e128] = math::decompose(test::dec128_t{"-123.456"});

        CHECK(s32  == true);
        CHECK(s64  == true);
        CHECK(s128 == true);

        CHECK(m32  == 123456u);
        CHECK(m64  == 123456ull);
        CHECK(m128 == math::uint128_t{123456u});

        CHECK(e32  == -3);
        CHECK(e64  == -3);
        CHECK(e128 == -3);
    }
}
TEST_SUITE("import/export::parsing normalization") {
    TEST_CASE("normalizes leading and trailing zeroes") {
        CHECK(test::as_string(test::dec32_t{"000123.4500"}) == "123.4500");
        CHECK(test::as_string(test::dec64_t{"000123.4500"}) == "123.4500");
        CHECK(test::as_string(test::dec128_t{"000123.4500"}) == "123.4500");

        CHECK(test::as_string(test::dec32_t{"-000123.4500"}) == "-123.4500");
        CHECK(test::as_string(test::dec64_t{"-000123.4500"}) == "-123.4500");
        CHECK(test::as_string(test::dec128_t{"-000123.4500"}) == "-123.4500");
    }

    TEST_CASE("normalizes zero spellings") {
        CHECK(test::as_string(test::dec32_t{"0.000"}) == "0");
        CHECK(test::as_string(test::dec64_t{"0.000"}) == "0");
        CHECK(test::as_string(test::dec128_t{"0.000"}) == "0");

        CHECK(test::as_string(test::dec32_t{"-0"}) == "0");
        CHECK(test::as_string(test::dec64_t{"-0"}) == "0");
        CHECK(test::as_string(test::dec128_t{"-0"}) == "0");

        CHECK(test::dec32_t{"-0"} == test::dec32_t{"0"});
        CHECK(test::dec64_t{"-0"} == test::dec64_t{"0"});
        CHECK(test::dec128_t{"-0"} == test::dec128_t{"0"});
    }

    TEST_CASE("accepts explicit plus sign") {
        CHECK(test::as_string(test::dec32_t{"+123.45"}) == "123.45");
        CHECK(test::as_string(test::dec64_t{"+123.45"}) == "123.45");
        CHECK(test::as_string(test::dec128_t{"+123.45"}) == "123.45");
    }

    TEST_CASE("string roundtrip is stable for normalized finite values") {
        const test::dec32_t a{"00123.4500"};
        const test::dec64_t b{"00123.4500"};
        const test::dec128_t c{"00123.4500"};

        CHECK(test::as_string(test::dec32_t{test::as_string(a)}) == test::as_string(a));
        CHECK(test::as_string(test::dec64_t{test::as_string(b)}) == test::as_string(b));
        CHECK(test::as_string(test::dec128_t{test::as_string(c)}) == test::as_string(c));
    }
}
TEST_SUITE("import/export::current permissive parsing behavior") {
    TEST_CASE("malformed inputs currently produce nan") {
        CHECK(test::as_string(test::dec32_t{""}) == "nan");
        CHECK(test::as_string(test::dec32_t{"."}) == "0");
        CHECK(test::as_string(test::dec32_t{"-"}) == "nan");
        CHECK(test::as_string(test::dec32_t{"abc"}) == "nan");
        CHECK(test::as_string(test::dec32_t{"1..2"}) == "nan");

        CHECK(test::as_string(test::dec64_t{""}) == "nan");
        CHECK(test::as_string(test::dec64_t{"."}) == "0");
        CHECK(test::as_string(test::dec64_t{"-"}) == "nan");
        CHECK(test::as_string(test::dec64_t{"abc"}) == "nan");
        CHECK(test::as_string(test::dec64_t{"1..2"}) == "nan");

        CHECK(test::as_string(test::dec128_t{""}) == "nan");
        CHECK(test::as_string(test::dec128_t{"."}) == "0");
        CHECK(test::as_string(test::dec128_t{"-"}) == "nan");
        CHECK(test::as_string(test::dec128_t{"abc"}) == "nan");
        CHECK(test::as_string(test::dec128_t{"1..2"}) == "nan");
    }
    /* FIXME: Inter libbid fails, compile and see: `thirdparty.cpp`
    TEST_CASE("scientific notation is supported") {
        CHECK(test::as_string(test::dec32_t{"1e1"}) == "10");
        CHECK(test::as_string(test::dec32_t{"1e2"}) == "100");
        CHECK(test::as_string(test::dec32_t{1e1})  == "10");
        CHECK(test::as_string(test::dec32_t{1e2})  == "100");
    }*/
}

TEST_CASE("diagnose malformed input current behavior") {
    INFO("dec32 empty  => [" << test::as_string(test::dec32_t{""}) << "]");
    INFO("dec32 dot    => [" << test::as_string(test::dec32_t{"."}) << "]");
    INFO("dec32 minus  => [" << test::as_string(test::dec32_t{"-"}) << "]");
    INFO("dec32 abc    => [" << test::as_string(test::dec32_t{"abc"}) << "]");
    INFO("dec32 1..2   => [" << test::as_string(test::dec32_t{"1..2"}) << "]");
    INFO("dec32 1e3    => [" << test::as_string(test::dec32_t{"1e3"}) << "]");
    INFO("dec32 1E3    => [" << test::as_string(test::dec32_t{"1E3"}) << "]");

    CHECK(true);
}