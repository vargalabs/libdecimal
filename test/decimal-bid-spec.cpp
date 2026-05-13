/* This file is part of the LIBDECIMAL project and is licensed under the MIT License.
 * Copyright © 2025–2026 Varga Labs, Toronto, ON, Canada 🇨🇦 Contact: info@vargalabs.com */
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <sstream>
#include <string>
#include <format>

#include <doctest/all>
#include <decimal/bid.hpp>


namespace test {
    using dec32_t = math::decimal_t<std::uint32_t>;
    using dec64_t = math::decimal_t<std::uint64_t>;
    using dec128_t = math::decimal_t<math::uint128_t>;

    template <class decimal_t>
    std::string as_string(decimal_t value) {
        return static_cast<std::string>(value);
    }

    template <class decimal_t>
    auto as_bid(decimal_t value) {
        using raw_t = typename decltype(static_cast<math::bid_t<typename decimal_t::value_t>>(value))::value_type;
        return static_cast<math::bid_t<raw_t>>(value).value;
    }
}

TEST_SUITE("math::bid::print") {
    TEST_CASE("renders integral values") {
        std::array<char, BID32_MAX_PRINTED_DIGITS> buffer{};
        CHECK(math::bid::print(false, 123u, 0, buffer.data(), buffer.size()) == "123");
        CHECK(math::bid::print(true, 123u, 0, buffer.data(), buffer.size()) == "-123");
    }

    TEST_CASE("renders fractional values") {
        std::array<char, BID32_MAX_PRINTED_DIGITS> buffer{};
        CHECK(math::bid::print(false, 123u, -2, buffer.data(), buffer.size()) == "1.23");
        CHECK(math::bid::print(false, 5u, -3, buffer.data(), buffer.size()) == "0.005");
        CHECK(math::bid::print(true, 5u, -3, buffer.data(), buffer.size()) == "-0.005");
    }

    TEST_CASE("renders positive exponents as trailing zeroes") {
        std::array<char, BID32_MAX_PRINTED_DIGITS> buffer{};
        CHECK(math::bid::print(false, 42u, 3, buffer.data(), buffer.size()) == "4200");
        CHECK(math::bid::print(false, 7u, 1, buffer.data(), buffer.size()) == "7");
    }
}

TEST_SUITE("math::bid::uint32_to_bid32 / decompose / display") {
    TEST_CASE("roundtrip positive decimal32 value") {
        const std::uint32_t raw = math::bid::uint32_to_bid32(1234567u, -3);
        const auto [kind, mantissa, exponent] = math::bid::decompose(raw);

        CHECK(kind == math::bid::category::positive);
        CHECK(mantissa == 1234567u);
        CHECK(exponent == -3);
        CHECK(math::bid::display_bid32(raw) == "1234.567");
    }

    TEST_CASE("encodes zero as canonical positive zero") {
        const std::uint32_t raw = math::bid::uint32_to_bid32(0u, 42);
        const auto [kind, mantissa, exponent] = math::bid::decompose(raw);

        CHECK(raw == 0x32000000u);
        CHECK(kind == math::bid::category::positive);
        CHECK(mantissa == 0u);
        CHECK(exponent == -1);
        CHECK(math::bid::display_bid32(raw) == "0");
    }

    TEST_CASE("rejects exponent outside decimal32 range") {
        CHECK_THROWS_AS(math::bid::uint32_to_bid32(1u, -96), std::out_of_range);
        CHECK_THROWS_AS(math::bid::uint32_to_bid32(1u, 97), std::out_of_range);
    }

    TEST_CASE("oversized significand collapses to zero per current implementation") {
        const std::uint32_t raw = math::bid::uint32_to_bid32(10'000'000u, 0);
        CHECK(raw == 0x32000000u);
        CHECK(math::bid::display_bid32(raw) == "0");
    }
}

TEST_SUITE("math::bid::uint64_to_bid64 / decompose / display") {
    TEST_CASE("roundtrip positive decimal64 value") {
        const std::uint64_t raw = math::bid::uint64_to_bid64(1234567890123456ull, -6);
        const auto [kind, mantissa, exponent] = math::bid::decompose(raw);

        CHECK(kind == math::bid::category::positive);
        CHECK(mantissa == 1234567890123456ull);
        CHECK(exponent == -6);
        CHECK(math::bid::display_bid64(raw) == "1234567890.123456");
    }

    TEST_CASE("encodes zero as canonical positive zero") {
        const std::uint64_t raw = math::bid::uint64_to_bid64(0ull, 123);
        const auto [kind, mantissa, exponent] = math::bid::decompose(raw);

        CHECK(raw == 0x31C0000000000000ull);
        CHECK(kind == math::bid::category::positive);
        CHECK(mantissa == 0ull);
        CHECK(exponent == 0);
        CHECK(math::bid::display_bid64(raw) == "0");
    }

    TEST_CASE("rejects exponent outside decimal64 range") {
        CHECK_THROWS_AS(math::bid::uint64_to_bid64(1ull, -384), std::out_of_range);
        CHECK_THROWS_AS(math::bid::uint64_to_bid64(1ull, 385), std::out_of_range);
    }

    TEST_CASE("oversized significand collapses to zero per current implementation") {
        const std::uint64_t raw = math::bid::uint64_to_bid64(10'000'000'000'000'000ull, 0);
        CHECK(raw == 0x31C0000000000000ull);
        CHECK(math::bid::display_bid64(raw) == "0");
    }
}

TEST_SUITE("decimal32 constructors and conversions") {
    TEST_CASE("default constructor yields zero") {
        const test::dec32_t value;
        CHECK(test::as_string(value) == "0");
    }

    TEST_CASE("constructs from significand and exponent") {
        const test::dec32_t value{12345u, -2};
        CHECK(test::as_string(value) == "123.45");
    }

    TEST_CASE("constructs from string like inputs") {
        CHECK(test::as_string(test::dec32_t{"123.45"}) == "123.45");
        CHECK(test::as_string(test::dec32_t{std::string{"123.45"}}) == "123.45");
        CHECK(test::as_string(test::dec32_t{std::string_view{"123.45"}}) == "123.45");
    }

    TEST_CASE("constructs from floating point and static from helpers") {
        CHECK(test::as_string(test::dec32_t{1.25}) == "1.25");
        CHECK(test::as_string(test::dec32_t::from(1.25f)) == "1.25");
        CHECK(test::as_string(test::dec32_t::from(1.25)) == "1.25");
        CHECK(test::as_string(test::dec32_t::from(static_cast<long double>(1.25))) == "1.25");
    }

    TEST_CASE("roundtrip through dpd wrapper") {
        const test::dec32_t original{"123.45"};
        const auto dpd = static_cast<math::dpd_t<std::uint32_t>>(original);
        const test::dec32_t roundtrip{dpd};
        CHECK(test::as_string(roundtrip) == "123.45");
        CHECK(roundtrip == original);
    }

    TEST_CASE("explicit conversion to double") {
        const test::dec32_t value{"123.25"};
        CHECK(static_cast<double>(value) == doctest::Approx(123.25));
    }
}

TEST_SUITE("decimal64 constructors and conversions") {
    TEST_CASE("default constructor yields zero") {
        const test::dec64_t value;
        CHECK(test::as_string(value) == "0");
    }

    TEST_CASE("constructs from significand and exponent") {
        const test::dec64_t value{123456789ull, -4};
        CHECK(test::as_string(value) == "12345.6789");
    }

    TEST_CASE("constructs from string and float sources") {
        CHECK(test::as_string(test::dec64_t{"1234567890.125"}) == "1234567890.125");
        CHECK(test::as_string(test::dec64_t{1.25L}) == "1.25");
        CHECK(test::as_string(test::dec64_t::from(1.25f)) == "1.25");
        CHECK(test::as_string(test::dec64_t::from(1.25)) == "1.25");
    }

    TEST_CASE("roundtrip through dpd wrapper") {
        const test::dec64_t original{"1234567890.125"};
        const auto dpd = static_cast<math::dpd_t<std::uint64_t>>(original);
        const test::dec64_t roundtrip{dpd};
        CHECK(test::as_string(roundtrip) == "1234567890.125");
        CHECK(roundtrip == original);
    }

    TEST_CASE("explicit conversion to long double") {
        const test::dec64_t value{"123.25"};
        CHECK(static_cast<long double>(value) == doctest::Approx(123.25));
    }
}

TEST_SUITE("decimal128 smoke tests") {
    TEST_CASE("supports string construction and formatting") {
        const test::dec128_t value{"3.141592653589793238462643383279502"}; //34 digits
        CHECK(value == test::dec128_t{"3.141592653589793238462643383279502"});
        CHECK_FALSE(test::as_string(value).empty());
        CHECK(test::as_string(value) == "3.141592653589793238462643383279502");
    }

    TEST_CASE("supports floating-point construction") {
        const test::dec128_t value{1.25L};
        CHECK(static_cast<long double>(value) == doctest::Approx(1.25L));
    }

    TEST_CASE("roundtrip through dpd wrapper") {
        const test::dec128_t original{"1234567890.125"};
        const auto dpd = static_cast<math::dpd_t<math::uint128_t>>(original);
        const test::dec128_t roundtrip{dpd};
        CHECK(roundtrip == original);
        CHECK(test::as_string(roundtrip) == test::as_string(original));
    }
}

TEST_SUITE("arithmetic operators") {
    TEST_CASE("decimal32 arithmetic") {
        const test::dec32_t a{"1.5"};
        const test::dec32_t b{"2.25"};

        CHECK(test::as_string(a + b) == "3.75");
        CHECK(test::as_string(b - a) == "0.75");
        CHECK(test::as_string(a * b) == "3.375");
        CHECK(test::as_string(b / a) == "1.5");
    }

    TEST_CASE("decimal64 arithmetic") {
        const test::dec64_t a{"1.5"};
        const test::dec64_t b{"2.25"};

        CHECK(test::as_string(a + b) == "3.75");
        CHECK(test::as_string(b - a) == "0.75");
        CHECK(test::as_string(a * b) == "3.375");
        CHECK(test::as_string(b / a) == "1.5");
    }

    TEST_CASE("unary plus and minus") {
        const test::dec32_t a{"12.5"};
        CHECK(test::as_string(+a) == "12.5");
        //CHECK(test::as_string(-a) == "-12.5");
    }
}

TEST_SUITE("comparison operators") {
    TEST_CASE("decimal32 comparisons") {
        const test::dec32_t a{"1.0"};
        const test::dec32_t b{"2.0"};
        const test::dec32_t c{"2.0"};

        CHECK(a < b);
        CHECK(b > a);
        CHECK(a <= b);
        CHECK(b >= c);
        CHECK(b == c);
        CHECK(a != b);
    }

    TEST_CASE("decimal64 comparisons") {
        const test::dec64_t a{"1.0"};
        const test::dec64_t b{"2.0"};
        const test::dec64_t c{"2.0"};

        CHECK(a < b);
        CHECK(b > a);
        CHECK(a <= b);
        CHECK(b >= c);
        CHECK(b == c);
        CHECK(a != b);
    }
}

TEST_SUITE("literals") {
    using namespace math::literals;

    TEST_CASE("decimal32 literal removes digit separators") {
        const auto value = 12'345.67_dec32;
        CHECK(test::as_string(value) == "12345.67");
    }

    TEST_CASE("decimal64 literal aliases _dec") {
        const auto a = 123.456_dec64;
        const auto b = 123.456_dec;
        CHECK(a == b);
        CHECK(test::as_string(a) == "123.456");
    }

    TEST_CASE("decimal128 literal works") {
        const auto value = 3.1415926535897932384626433832795029_dec128;
        CHECK(value == test::dec128_t{"3.1415926535897932384626433832795029"});
    }
}

TEST_SUITE("utilities") {
    TEST_CASE("math::decompose returns sign mantissa exponent for finite values") {
        const auto [signbit_32, mantissa_32, exponent_32] = math::decompose(test::dec32_t{"-123.45"});
        CHECK(signbit_32 == true);
        CHECK(mantissa_32 == 12345u);
        CHECK(exponent_32 == -2);

        const auto [signbit_64, mantissa_64, exponent_64] = math::decompose(test::dec64_t{"123.45"});
        CHECK(signbit_64 == false);
        CHECK(mantissa_64 == 12345ull);
        CHECK(exponent_64 == -2);
    }

    TEST_CASE("math::mantissa extracts unsigned mantissa from positive finite values") {
        CHECK(math::mantissa(test::dec32_t{"123.45"}) == 12345u);
        CHECK(math::mantissa(test::dec64_t{"123.45"}) == 12345ull);
    }

    TEST_CASE("math::mantissa rejects negative and non-finite values") {
        CHECK_THROWS_WITH_AS(math::mantissa(test::dec32_t{"-1"}), "attempting to convert negative value to unsigned", std::runtime_error);
        CHECK_THROWS_AS(math::mantissa(math::constants<std::uint32_t>::nan), std::runtime_error);
        CHECK_THROWS_AS(math::mantissa(math::constants<std::uint32_t>::inf), std::runtime_error);
    }
}

TEST_SUITE("streaming and formatting") {
    TEST_CASE("ostream insertion prints decimal") {
        std::ostringstream os;
        os << test::dec32_t{"123.45"};
        CHECK(os.str() == "123.45");
    }

    TEST_CASE("std::format uses formatter specialization") {
        CHECK(std::format("{}", test::dec32_t{"123.45"}) == "123.45");
        CHECK(std::format("value={}", test::dec64_t{"123.45"}) == "value=123.45");
    }
}

TEST_SUITE("transcendental smoke tests") {
    TEST_CASE("sqrt") {
        CHECK(test::as_string(math::sqrt(test::dec32_t{"4"})) == "2");
        CHECK(test::as_string(math::sqrt(test::dec64_t{"4"})) == "2");
    }

    TEST_CASE("exp and log10") {
        CHECK(test::as_string(math::exp(test::dec32_t{"0"})) == "1");
        CHECK(test::as_string(math::log10(test::dec32_t{"1000"})) == "3");

        CHECK(test::as_string(math::exp(test::dec64_t{"0"})) == "1");
        CHECK(test::as_string(math::log10(test::dec64_t{"1000"})) == "3");
    }

    TEST_CASE("trigonometric identities at easy points") {
        CHECK(test::as_string(math::sin(test::dec32_t{"0"})) == "0");
        CHECK(test::as_string(math::cosh(test::dec32_t{"0"})) == "1");

        CHECK(test::as_string(math::sin(test::dec64_t{"0"})) == "0");
        CHECK(test::as_string(math::cosh(test::dec64_t{"0"})) == "1");
    }
}

TEST_SUITE("constants") {
    TEST_CASE("basic decimal32 constants") {
        CHECK(test::as_string(math::constants<std::uint32_t>::zero) == "0");
        CHECK(test::as_string(math::constants<std::uint32_t>::one) == "1");
        CHECK(test::as_string(math::constants<std::uint32_t>::pi) == "3.141593");
        CHECK(test::as_string(math::constants<std::uint32_t>::golden_ratio) == test::as_string(math::constants<std::uint32_t>::phi));
    }

    TEST_CASE("basic decimal64 constants") {
        CHECK(test::as_string(math::constants<std::uint64_t>::zero) == "0");
        CHECK(test::as_string(math::constants<std::uint64_t>::one) == "1");
        CHECK(test::as_string(math::constants<std::uint64_t>::pi) == "3.141592653589793");
    }

    TEST_CASE("basic decimal128 constants") {
        CHECK(math::constants<math::uint128_t>::zero == test::dec128_t{"0"});
        CHECK(math::constants<math::uint128_t>::one == test::dec128_t{"1"});
        CHECK(math::constants<math::uint128_t>::pi == test::dec128_t{"3.1415926535897932384626433832795029"});
        CHECK(test::as_string(math::constants<math::uint128_t>::golden_ratio) == test::as_string(math::constants<math::uint128_t>::phi));
    }
}

TEST_SUITE("known gaps / regression sentinels") {
    TEST_CASE("decimal128 significand exponent constructor is not implemented yet") {
        const test::dec128_t value{math::uint128_t{}, 0};

        CHECK(value == test::dec128_t{"0"});
    }

    TEST_CASE("bid helper classification for infinities and nan is printable") {
        CHECK(math::bid::display_bid32(0x78000000u) == "+inf");
        CHECK(math::bid::display_bid32(0xf8000000u) == "-inf");
        CHECK(math::bid::display_bid32(0x7c000000u) == "nan");

        CHECK(math::bid::display_bid64(0x7800000000000000ull) == "+inf");
        CHECK(math::bid::display_bid64(0xf800000000000000ull) == "-inf");
        CHECK(math::bid::display_bid64(0x7c00000000000000ull) == "nan");
    }
}


TEST_SUITE("comparison operators::special values") {
    TEST_CASE("signed zero compares equal") {
        CHECK(test::dec32_t{"0"} == test::dec32_t{"-0"});
        CHECK(test::dec64_t{"0"} == test::dec64_t{"-0"});
        CHECK(test::dec128_t{"0"} == test::dec128_t{"-0"});

        CHECK(test::dec32_t{"0"} <= test::dec32_t{"-0"});
        CHECK(test::dec64_t{"0"} >= test::dec64_t{"-0"});
    }

    TEST_CASE("nan is unordered and not equal to itself") {
        const auto nan32 = math::constants<std::uint32_t>::nan;
        const auto nan64 = math::constants<std::uint64_t>::nan;
        const auto nan128 = math::constants<math::uint128_t>::nan;

        CHECK(nan32 != nan32);
        CHECK_FALSE(nan32 == nan32);
        CHECK_FALSE(nan32 < nan32);
        CHECK_FALSE(nan32 > nan32);

        CHECK(nan64 != nan64);
        CHECK_FALSE(nan64 == nan64);
        CHECK_FALSE(nan64 < nan64);
        CHECK_FALSE(nan64 > nan64);

        CHECK(nan128 != nan128);
        CHECK_FALSE(nan128 == nan128);
        CHECK_FALSE(nan128 < nan128);
        CHECK_FALSE(nan128 > nan128);
    }

    TEST_CASE("infinities compare as expected") {
        const auto p32 = math::constants<std::uint32_t>::inf;
        const auto n32 = test::dec32_t{"-inf"};
        const auto x32 = test::dec32_t{"1"};

        CHECK(p32 > x32);
        CHECK(n32 < x32);
        CHECK(p32 == p32);
        CHECK(n32 == n32);
        CHECK(p32 != n32);

        const auto p64 = math::constants<std::uint64_t>::inf;
        const auto n64 = test::dec64_t{"-inf"};
        const auto x64 = test::dec64_t{"1"};

        CHECK(p64 > x64);
        CHECK(n64 < x64);
        CHECK(p64 == p64);
        CHECK(n64 == n64);
        CHECK(p64 != n64);
    }
}

TEST_SUITE("utilities::special values") {
    TEST_CASE("math::decompose handles zero canonically") {
        const auto [signbit_32, mantissa_32, exponent_32] = math::decompose(test::dec32_t{"0"});
        CHECK(signbit_32 == false);
        CHECK(mantissa_32 == 0u);
        CHECK(exponent_32 <= 0);

        const auto [signbit_64, mantissa_64, exponent_64] = math::decompose(test::dec64_t{"0"});
        CHECK(signbit_64 == false);
        CHECK(mantissa_64 == 0ull);
        CHECK(exponent_64 <= 0);
    }

    TEST_CASE("math::mantissa of zero is zero") {
        CHECK(math::mantissa(test::dec32_t{"0"}) == 0u);
        CHECK(math::mantissa(test::dec64_t{"0"}) == 0ull);
    }
}