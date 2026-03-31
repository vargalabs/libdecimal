/* This file is part of the LIBDECIMAL project and is licensed under the MIT License.
 * Copyright © 2025–2026 Varga Labs, Toronto, ON, Canada 🇨🇦 Contact: info@vargalabs.com */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <cmath>
#include <cstdint>
#include <doctest/all>
#include "decimal.hpp"

namespace test {
    using dec32_t  = math::decimal_t<std::uint32_t>;
    using dec64_t  = math::decimal_t<std::uint64_t>;
    using dec128_t = math::decimal_t<math::uint128_t>;

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

TEST_SUITE("transcendental functions::decimal32") {
    TEST_CASE("sqrt at exact points") {
        CHECK(test::as_double(math::sqrt(test::dec32_t{"0"})) == doctest::Approx(0.0));
        CHECK(test::as_double(math::sqrt(test::dec32_t{"1"})) == doctest::Approx(1.0));
        CHECK(test::as_double(math::sqrt(test::dec32_t{"4"})) == doctest::Approx(2.0));
        CHECK(test::as_double(math::sqrt(test::dec32_t{"9"})) == doctest::Approx(3.0));
    }

    TEST_CASE("exp at easy points") {
        CHECK(test::as_double(math::exp(test::dec32_t{"0"})) == doctest::Approx(1.0));
        CHECK(test::as_double(math::exp(test::dec32_t{"1"})) == doctest::Approx(std::exp(1.0)));
    }

    TEST_CASE("log at easy points") {
        CHECK(test::as_double(math::log(test::dec32_t{"1"})) == doctest::Approx(0.0));
        CHECK(test::as_double(math::log10(test::dec32_t{"1"})) == doctest::Approx(0.0));
        CHECK(test::as_double(math::log10(test::dec32_t{"10"})) == doctest::Approx(1.0));
        CHECK(test::as_double(math::log10(test::dec32_t{"1000"})) == doctest::Approx(3.0));
    }

    TEST_CASE("trigonometric easy points") {
        CHECK(test::as_double(math::sin(test::dec32_t{"0"})) == doctest::Approx(0.0));
        CHECK(test::as_double(math::cos(test::dec32_t{"0"})) == doctest::Approx(1.0));
        CHECK(test::as_double(math::tan(test::dec32_t{"0"})) == doctest::Approx(0.0));
    }

    TEST_CASE("hyperbolic easy points") {
        CHECK(test::as_double(math::sinh(test::dec32_t{"0"})) == doctest::Approx(0.0));
        CHECK(test::as_double(math::cosh(test::dec32_t{"0"})) == doctest::Approx(1.0));
        CHECK(test::as_double(math::tanh(test::dec32_t{"0"})) == doctest::Approx(0.0));
    }

    TEST_CASE("inverse trig easy points") {
        CHECK(test::as_double(math::asin(test::dec32_t{"0"})) == doctest::Approx(0.0));
        CHECK(test::as_double(math::acos(test::dec32_t{"1"})) == doctest::Approx(0.0));
        CHECK(test::as_double(math::atan(test::dec32_t{"0"})) == doctest::Approx(0.0));
    }

    TEST_CASE("inverse hyperbolic easy points") {
        CHECK(test::as_double(math::asinh(test::dec32_t{"0"})) == doctest::Approx(0.0));
        CHECK(test::as_double(math::atanh(test::dec32_t{"0"})) == doctest::Approx(0.0));
        CHECK(test::as_double(math::acosh(test::dec32_t{"1"})) == doctest::Approx(0.0));
    }

    TEST_CASE("identity style checks") {
        const auto x = test::dec32_t{"0.5"};
        CHECK(test::as_double(math::log(math::exp(x))) == doctest::Approx(0.5).epsilon(1e-5));
        CHECK(test::as_double(math::sin(x) * math::sin(x) + math::cos(x) * math::cos(x))
              == doctest::Approx(1.0).epsilon(1e-5));
    }
}

TEST_SUITE("transcendental functions::decimal64") {
    TEST_CASE("sqrt at exact points") {
        CHECK(test::as_long_double(math::sqrt(test::dec64_t{"0"})) == doctest::Approx(0.0L));
        CHECK(test::as_long_double(math::sqrt(test::dec64_t{"1"})) == doctest::Approx(1.0L));
        CHECK(test::as_long_double(math::sqrt(test::dec64_t{"4"})) == doctest::Approx(2.0L));
        CHECK(test::as_long_double(math::sqrt(test::dec64_t{"9"})) == doctest::Approx(3.0L));
    }

    TEST_CASE("exp at easy points") {
        CHECK(test::as_long_double(math::exp(test::dec64_t{"0"})) == doctest::Approx(1.0L));
        CHECK(test::as_long_double(math::exp(test::dec64_t{"1"})) == doctest::Approx(std::expl(1.0L)));
    }

    TEST_CASE("log at easy points") {
        CHECK(test::as_long_double(math::log(test::dec64_t{"1"})) == doctest::Approx(0.0L));
        CHECK(test::as_long_double(math::log10(test::dec64_t{"1"})) == doctest::Approx(0.0L));
        CHECK(test::as_long_double(math::log10(test::dec64_t{"10"})) == doctest::Approx(1.0L));
        CHECK(test::as_long_double(math::log10(test::dec64_t{"1000"})) == doctest::Approx(3.0L));
    }

    TEST_CASE("trigonometric easy points") {
        CHECK(test::as_long_double(math::sin(test::dec64_t{"0"})) == doctest::Approx(0.0L));
        CHECK(test::as_long_double(math::cos(test::dec64_t{"0"})) == doctest::Approx(1.0L));
        CHECK(test::as_long_double(math::tan(test::dec64_t{"0"})) == doctest::Approx(0.0L));
    }

    TEST_CASE("hyperbolic easy points") {
        CHECK(test::as_long_double(math::sinh(test::dec64_t{"0"})) == doctest::Approx(0.0L));
        CHECK(test::as_long_double(math::cosh(test::dec64_t{"0"})) == doctest::Approx(1.0L));
        CHECK(test::as_long_double(math::tanh(test::dec64_t{"0"})) == doctest::Approx(0.0L));
    }

    TEST_CASE("inverse trig easy points") {
        CHECK(test::as_long_double(math::asin(test::dec64_t{"0"})) == doctest::Approx(0.0L));
        CHECK(test::as_long_double(math::acos(test::dec64_t{"1"})) == doctest::Approx(0.0L));
        CHECK(test::as_long_double(math::atan(test::dec64_t{"0"})) == doctest::Approx(0.0L));
    }

    TEST_CASE("inverse hyperbolic easy points") {
        CHECK(test::as_long_double(math::asinh(test::dec64_t{"0"})) == doctest::Approx(0.0L));
        CHECK(test::as_long_double(math::atanh(test::dec64_t{"0"})) == doctest::Approx(0.0L));
        CHECK(test::as_long_double(math::acosh(test::dec64_t{"1"})) == doctest::Approx(0.0L));
    }

    TEST_CASE("identity style checks") {
        const auto x = test::dec64_t{"0.5"};
        CHECK(test::as_long_double(math::log(math::exp(x))) == doctest::Approx(0.5L).epsilon(1e-10));
        CHECK(test::as_long_double(math::sin(x) * math::sin(x) + math::cos(x) * math::cos(x))
              == doctest::Approx(1.0L).epsilon(1e-10));
    }
}

TEST_SUITE("transcendental functions::decimal128") {
    TEST_CASE("sqrt at exact points") {
        CHECK(test::as_long_double(math::sqrt(test::dec128_t{"0"})) == doctest::Approx(0.0L));
        CHECK(test::as_long_double(math::sqrt(test::dec128_t{"1"})) == doctest::Approx(1.0L));
        CHECK(test::as_long_double(math::sqrt(test::dec128_t{"4"})) == doctest::Approx(2.0L));
        CHECK(test::as_long_double(math::sqrt(test::dec128_t{"9"})) == doctest::Approx(3.0L));
    }

    TEST_CASE("exp at easy points") {
        CHECK(test::as_long_double(math::exp(test::dec128_t{"0"})) == doctest::Approx(1.0L));
        CHECK(test::as_long_double(math::exp(test::dec128_t{"1"})) == doctest::Approx(std::expl(1.0L)));
    }

    TEST_CASE("log at easy points") {
        CHECK(test::as_long_double(math::log(test::dec128_t{"1"})) == doctest::Approx(0.0L));
        CHECK(test::as_long_double(math::log10(test::dec128_t{"1"})) == doctest::Approx(0.0L));
        CHECK(test::as_long_double(math::log10(test::dec128_t{"10"})) == doctest::Approx(1.0L));
        CHECK(test::as_long_double(math::log10(test::dec128_t{"1000"})) == doctest::Approx(3.0L));
    }

    TEST_CASE("trigonometric easy points") {
        CHECK(test::as_long_double(math::sin(test::dec128_t{"0"})) == doctest::Approx(0.0L));
        CHECK(test::as_long_double(math::cos(test::dec128_t{"0"})) == doctest::Approx(1.0L));
        CHECK(test::as_long_double(math::tan(test::dec128_t{"0"})) == doctest::Approx(0.0L));
    }

    TEST_CASE("hyperbolic easy points") {
        CHECK(test::as_long_double(math::sinh(test::dec128_t{"0"})) == doctest::Approx(0.0L));
        CHECK(test::as_long_double(math::cosh(test::dec128_t{"0"})) == doctest::Approx(1.0L));
        CHECK(test::as_long_double(math::tanh(test::dec128_t{"0"})) == doctest::Approx(0.0L));
    }

    TEST_CASE("inverse trig easy points") {
        CHECK(test::as_long_double(math::asin(test::dec128_t{"0"})) == doctest::Approx(0.0L));
        CHECK(test::as_long_double(math::acos(test::dec128_t{"1"})) == doctest::Approx(0.0L));
        CHECK(test::as_long_double(math::atan(test::dec128_t{"0"})) == doctest::Approx(0.0L));
    }

    TEST_CASE("inverse hyperbolic easy points") {
        CHECK(test::as_long_double(math::asinh(test::dec128_t{"0"})) == doctest::Approx(0.0L));
        CHECK(test::as_long_double(math::atanh(test::dec128_t{"0"})) == doctest::Approx(0.0L));
        CHECK(test::as_long_double(math::acosh(test::dec128_t{"1"})) == doctest::Approx(0.0L));
    }

    TEST_CASE("identity style checks") {
        const auto x = test::dec128_t{"0.5"};
        CHECK(test::as_long_double(math::log(math::exp(x))) == doctest::Approx(0.5L).epsilon(1e-12));
        CHECK(test::as_long_double(math::sin(x) * math::sin(x) + math::cos(x) * math::cos(x))
              == doctest::Approx(1.0L).epsilon(1e-12));
    }
}

TEST_SUITE("transcendental functions::cross precision agreement") {
    TEST_CASE("sqrt agrees at easy point") {
        CHECK(test::as_double(math::sqrt(test::dec32_t{"2"}))
              == doctest::Approx(static_cast<double>(test::as_long_double(math::sqrt(test::dec64_t{"2"})))).epsilon(1e-5));
    }

    TEST_CASE("exp agrees at zero") {
        CHECK(test::as_double(math::exp(test::dec32_t{"0"})) == doctest::Approx(1.0));
        CHECK(test::as_long_double(math::exp(test::dec64_t{"0"})) == doctest::Approx(1.0L));
        CHECK(test::as_long_double(math::exp(test::dec128_t{"0"})) == doctest::Approx(1.0L));
    }
}

TEST_CASE("registered unary math functions compile for decimal32") {
    const test::dec32_t x{"0.5"};
    (void)math::tgamma(x);
    (void)math::lgamma(x);
    (void)math::sin(x);
    (void)math::cos(x);
    (void)math::tan(x);
    (void)math::sinh(x);
    (void)math::cosh(x);
    (void)math::tanh(x);
    (void)math::asin(x);
    (void)math::acos(test::dec32_t{"0.5"});
    (void)math::atan(x);
    (void)math::asinh(x);
    (void)math::acosh(test::dec32_t{"1.5"});
    (void)math::atanh(test::dec32_t{"0.25"});
    (void)math::log(test::dec32_t{"2"});
    (void)math::log2(test::dec32_t{"2"});
    (void)math::log10(test::dec32_t{"10"});
    (void)math::log1p(x);
    (void)math::exp(x);
    (void)math::expm1(x);
    (void)math::exp10(x);
    (void)math::erf(x);
    (void)math::erfc(x);
    (void)math::cbrt(test::dec32_t{"8"});
    (void)math::sqrt(test::dec32_t{"4"});
    (void)math::abs(test::dec32_t{"-1"});
    CHECK(true);
}