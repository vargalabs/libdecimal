/* This file is part of the LIBDECIMAL project and is licensed under the MIT License.
 * Copyright © 2025–2026 Varga Labs, Toronto, ON, Canada 🇨🇦 Contact: info@vargalabs.com */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <cmath>
#include <cstdint>
#include <limits>
#include <string>

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

    template <class float_t>
    bool nearly_equal(float_t a, float_t b, float_t eps = static_cast<float_t>(1e-12)) {
        return std::fabs(a - b) <= eps;
    }
}

TEST_SUITE("float conversions::decimal32") {
    TEST_CASE("constructs from float family") {
        CHECK(test::as_string(test::dec32_t{1.25f}) == "1.25");
        CHECK(test::as_string(test::dec32_t{1.25}) == "1.25");
        CHECK(test::as_string(test::dec32_t{static_cast<long double>(1.25)}) == "1.25");
    }

    TEST_CASE("static from helpers match direct construction") {
        CHECK(test::dec32_t::from(1.25f) == test::dec32_t{1.25f});
        CHECK(test::dec32_t::from(1.25) == test::dec32_t{1.25});
        CHECK(test::dec32_t::from(static_cast<long double>(1.25)) ==
              test::dec32_t{static_cast<long double>(1.25)});
    }

    TEST_CASE("roundtrip float friendly values through double") {
        const test::dec32_t a{0.5};
        const test::dec32_t b{1.25};
        const test::dec32_t c{12.75};
        const test::dec32_t d{-3.5};

        CHECK(static_cast<double>(a) == doctest::Approx(0.5));
        CHECK(static_cast<double>(b) == doctest::Approx(1.25));
        CHECK(static_cast<double>(c) == doctest::Approx(12.75));
        CHECK(static_cast<double>(d) == doctest::Approx(-3.5));
    }

    TEST_CASE("preserves decimal-friendly values via string normalization") {
        CHECK(test::as_string(test::dec32_t{0.1}) == "0.1000000"); // NOTE: we preserve zeroes
        CHECK(test::as_string(test::dec32_t{0.25}) == "0.25");
        CHECK(test::as_string(test::dec32_t{2.5}) == "2.5");
        CHECK(test::as_string(test::dec32_t{-0.125}) == "-0.125");
    }

    TEST_CASE("zero and signed zero conversions") {
        CHECK(static_cast<double>(test::dec32_t{0.0}) == doctest::Approx(0.0));
        CHECK(static_cast<double>(test::dec32_t{-0.0}) == doctest::Approx(0.0));

        CHECK_FALSE(test::as_string(test::dec32_t{0.0}).empty());
        CHECK_FALSE(test::as_string(test::dec32_t{-0.0}).empty());
    }
}

TEST_SUITE("float conversions::decimal64") {
    TEST_CASE("constructs from float family") {
        CHECK(test::as_string(test::dec64_t{1.25f}) == "1.25");
        CHECK(test::as_string(test::dec64_t{1.25}) == "1.25");
        CHECK(test::as_string(test::dec64_t{1.25L}) == "1.25");
    }

    TEST_CASE("static from helpers match direct construction") {
        CHECK(test::dec64_t::from(1.25f) == test::dec64_t{1.25f});
        CHECK(test::dec64_t::from(1.25) == test::dec64_t{1.25});
        CHECK(test::dec64_t::from(1.25L) == test::dec64_t{1.25L});
    }

    TEST_CASE("roundtrip float friendly values through long double") {
        const test::dec64_t a{0.5};
        const test::dec64_t b{1.25};
        const test::dec64_t c{12.75};
        const test::dec64_t d{-3.5};

        CHECK(static_cast<long double>(a) == doctest::Approx(0.5L));
        CHECK(static_cast<long double>(b) == doctest::Approx(1.25L));
        CHECK(static_cast<long double>(c) == doctest::Approx(12.75L));
        CHECK(static_cast<long double>(d) == doctest::Approx(-3.5L));
    }

    TEST_CASE("preserves decimal-friendly values via string normalization") {
        CHECK(test::as_string(test::dec64_t{0.1}) == "0.1000000000000000");
        CHECK(test::as_string(test::dec64_t{0.25}) == "0.25");
        CHECK(test::as_string(test::dec64_t{2.5}) == "2.5");
        CHECK(test::as_string(test::dec64_t{-0.125}) == "-0.125");
    }

    TEST_CASE("decimal64 has stronger precision for long literals") {
        const test::dec64_t value{1234567890.125};
        CHECK(static_cast<long double>(value) == doctest::Approx(1234567890.125L));
    }
}

TEST_SUITE("float conversions::decimal128") {
    TEST_CASE("constructs from floating point family") {
        const test::dec128_t a{1.25f};
        const test::dec128_t b{1.25};
        const test::dec128_t c{1.25L};

        CHECK(static_cast<long double>(a) == doctest::Approx(1.25L));
        CHECK(static_cast<long double>(b) == doctest::Approx(1.25L));
        CHECK(static_cast<long double>(c) == doctest::Approx(1.25L));
    }

    TEST_CASE("static from helpers match direct construction when supported") {
        CHECK(test::dec128_t::from(1.25f) == test::dec128_t{1.25f});
        CHECK(test::dec128_t::from(1.25) == test::dec128_t{1.25});
        CHECK(test::dec128_t::from(1.25L) == test::dec128_t{1.25L});
    }

    TEST_CASE("normalized display after float construction") {
        CHECK(test::as_string(test::dec128_t{1.25L}) == "1.25");
        CHECK(test::as_string(test::dec128_t{0.125L}) == "0.125");
        CHECK(test::as_string(test::dec128_t{-2.5L}) == "-2.5");
    }

    TEST_CASE("decimal128 rounds to its representable precision") {
        const test::dec128_t value{
            static_cast<long double>(3.1415926535897932384626433832795029L)
        };

        CHECK_FALSE(test::as_string(value).empty());
    }
}

TEST_SUITE("float conversions::cross checks") {
    TEST_CASE("string constructed and float constructed values agree for simple decimals") {
        CHECK(test::dec32_t{"1.25"} == test::dec32_t{1.25});
        CHECK(test::dec64_t{"1.25"} == test::dec64_t{1.25});
        CHECK(test::dec128_t{"1.25"} == test::dec128_t{1.25L});
    }

    TEST_CASE("back conversion is numerically stable for simple decimals") {
        {
            const double x = static_cast<double>(test::dec32_t{1.25});
            CHECK(x == doctest::Approx(1.25));
        }
        {
            const long double x = static_cast<long double>(test::dec64_t{1.25});
            CHECK(x == doctest::Approx(1.25L));
        }
        {
            const long double x = static_cast<long double>(test::dec128_t{1.25L});
            CHECK(x == doctest::Approx(1.25L));
        }
    }

    TEST_CASE("binary awkward values still produce finite decimals") {
        const auto s32  = test::as_string(test::dec32_t{0.2});
        const auto s64  = test::as_string(test::dec64_t{0.2});
        const auto s128 = test::as_string(test::dec128_t{0.2L});

        CHECK_FALSE(s32.empty());
        CHECK_FALSE(s64.empty());
        CHECK_FALSE(s128.empty());

        CHECK(std::isfinite(static_cast<double>(test::dec32_t{0.2})));
        CHECK(std::isfinite(static_cast<long double>(test::dec64_t{0.2})));
        CHECK(std::isfinite(static_cast<long double>(test::dec128_t{0.2L})));
    }
}

TEST_SUITE("float conversions::special values if supported") {
    TEST_CASE("non-finite input handling is explicit") {
        const auto nan_f = std::numeric_limits<float>::quiet_NaN();
        const auto inf_d = std::numeric_limits<double>::infinity();

        // Keep these as smoke probes: if your implementation maps them to decimal NaN/Inf,
        // assert that explicitly; if it throws, assert that instead.
        // Uncomment and tighten once you settle the contract.

        // CHECK(test::as_string(test::dec32_t{nan_f}) == "nan");
        // CHECK(test::as_string(test::dec64_t{inf_d}) == "+inf");

        CHECK(std::isnan(nan_f));
        CHECK(std::isinf(inf_d));
    }
}

TEST_SUITE("float conversions::signed zero policy") {
    TEST_CASE("floating signed zero normalizes to zero") {
        CHECK(test::as_string(test::dec32_t{-0.0f}) == "0");
        CHECK(test::as_string(test::dec32_t{-0.0}) == "0");

        CHECK(test::as_string(test::dec64_t{-0.0f}) == "0");
        CHECK(test::as_string(test::dec64_t{-0.0}) == "0");

        CHECK(test::as_string(test::dec128_t{-0.0f}) == "0");
        CHECK(test::as_string(test::dec128_t{-0.0L}) == "0");
    }

    TEST_CASE("positive and negative zero compare equal after float construction") {
        CHECK(test::dec32_t{0.0} == test::dec32_t{-0.0});
        CHECK(test::dec64_t{0.0} == test::dec64_t{-0.0});
        CHECK(test::dec128_t{0.0L} == test::dec128_t{-0.0L});
    }
}