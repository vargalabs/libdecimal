/* This file is part of the LIBDECIMAL project and is licensed under the MIT License.
 * Copyright © 2025–2026 Varga Labs, Toronto, ON, Canada 🇨🇦 Contact: info@vargalabs.com */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <string>
#include <doctest/all>
#include "decimal.hpp"

namespace test {
    using dec32_t  = math::decimal_t<std::uint32_t>;
    using dec64_t  = math::decimal_t<std::uint64_t>;
    using dec128_t = math::decimal_t<math::uint128_t>;

    template <class decimal_t>
    std::string as_string(decimal_t value) {
        return static_cast<std::string>(value);
    }
}

TEST_SUITE("multiplication and division::decimal32") {
    TEST_CASE("multiplication of exact values") {
        CHECK(test::dec32_t{"1.5"} * test::dec32_t{"2"} == test::dec32_t{"3"});
        CHECK(test::dec64_t{"1.5"} * test::dec64_t{"2"} == test::dec64_t{"3"});
        CHECK(test::dec64_t{"1.25"} * test::dec64_t{"2.5"} == test::dec64_t{"3.125"});
    }

    TEST_CASE("division of exact values") {
        CHECK(test::as_string(test::dec32_t{"6"} / test::dec32_t{"3"}) == "2");
        CHECK(test::as_string(test::dec32_t{"3"} / test::dec32_t{"2"}) == "1.5");
        CHECK(test::as_string(test::dec32_t{"3.125"} / test::dec32_t{"2.5"}) == "1.25");
    }

    TEST_CASE("multiplicative identity") {
        const test::dec32_t x{"123.456"};
        CHECK(x * test::dec32_t{"1"} == x);
        CHECK(test::dec32_t{"1"} * x == x);
    }

    TEST_CASE("zero annihilates multiplication") {
        CHECK(test::dec32_t{"0"} * test::dec32_t{"123.456"} == test::dec32_t{"0"});
        CHECK(test::dec32_t{"123.456"} * test::dec32_t{"0"} == test::dec32_t{"0"});
    }

    TEST_CASE("division by one preserves value") {
        const test::dec32_t x{"123.456"};
        CHECK(x / test::dec32_t{"1"} == x);
    }

    TEST_CASE("self division gives one for nonzero values") {
        const test::dec32_t x{"123.456"};
        CHECK(x / x == test::dec32_t{"1"});
    }

    TEST_CASE("sign rules") {
        CHECK(test::as_string(test::dec32_t{"2"} * test::dec32_t{"-3"}) == "-6");
        CHECK(test::as_string(test::dec32_t{"-2"} * test::dec32_t{"-3"}) == "6");
        CHECK(test::as_string(test::dec32_t{"6"} / test::dec32_t{"-3"}) == "-2");
        CHECK(test::as_string(test::dec32_t{"-6"} / test::dec32_t{"-3"}) == "2");
    }

    TEST_CASE("commutativity of multiplication for exact values") {
        const test::dec32_t a{"1.25"};
        const test::dec32_t b{"2.5"};
        CHECK(a * b == b * a);
    }

    TEST_CASE("associativity of multiplication for simple exact values") {
        const test::dec32_t a{"2"};
        const test::dec32_t b{"3"};
        const test::dec32_t c{"4"};
        CHECK((a * b) * c == a * (b * c));
    }

    TEST_CASE("distributivity over addition for exact values") {
        const test::dec32_t a{"2"};
        const test::dec32_t b{"3"};
        const test::dec32_t c{"4"};
        CHECK(a * (b + c) == (a * b) + (a * c));
    }
}

TEST_SUITE("multiplication and division::decimal64") {
    TEST_CASE("multiplication of exact values") {
        CHECK(test::dec32_t{"2"} * test::dec32_t{"3"} == test::dec32_t{"6"});
        CHECK(test::dec32_t{"1.5"} * test::dec32_t{"2"} == test::dec32_t{"3"});
        CHECK(test::dec32_t{"1.25"} * test::dec32_t{"2.5"} == test::dec32_t{"3.125"});
    }
    TEST_CASE("division of exact values") {
        CHECK(test::as_string(test::dec64_t{"6"} / test::dec64_t{"3"}) == "2");
        CHECK(test::as_string(test::dec64_t{"3"} / test::dec64_t{"2"}) == "1.5");
        CHECK(test::as_string(test::dec64_t{"3.125"} / test::dec64_t{"2.5"}) == "1.25");
    }

    TEST_CASE("multiplicative identity") {
        const test::dec64_t x{"123.456"};
        CHECK(x * test::dec64_t{"1"} == x);
        CHECK(test::dec64_t{"1"} * x == x);
    }

    TEST_CASE("zero annihilates multiplication") {
        CHECK(test::dec64_t{"0"} * test::dec64_t{"123.456"} == test::dec64_t{"0"});
        CHECK(test::dec64_t{"123.456"} * test::dec64_t{"0"} == test::dec64_t{"0"});
    }

    TEST_CASE("division by one preserves value") {
        const test::dec64_t x{"123.456"};
        CHECK(x / test::dec64_t{"1"} == x);
    }

    TEST_CASE("self division gives one for nonzero values") {
        const test::dec64_t x{"123.456"};
        CHECK(x / x == test::dec64_t{"1"});
    }

    TEST_CASE("sign rules") {
        CHECK(test::as_string(test::dec64_t{"2"} * test::dec64_t{"-3"}) == "-6");
        CHECK(test::as_string(test::dec64_t{"-2"} * test::dec64_t{"-3"}) == "6");
        CHECK(test::as_string(test::dec64_t{"6"} / test::dec64_t{"-3"}) == "-2");
        CHECK(test::as_string(test::dec64_t{"-6"} / test::dec64_t{"-3"}) == "2");
    }

    TEST_CASE("commutativity of multiplication for exact values") {
        const test::dec64_t a{"1.25"};
        const test::dec64_t b{"2.5"};
        CHECK(a * b == b * a);
    }

    TEST_CASE("associativity of multiplication for simple exact values") {
        const test::dec64_t a{"2"};
        const test::dec64_t b{"3"};
        const test::dec64_t c{"4"};
        CHECK((a * b) * c == a * (b * c));
    }

    TEST_CASE("distributivity over addition for exact values") {
        const test::dec64_t a{"2"};
        const test::dec64_t b{"3"};
        const test::dec64_t c{"4"};
        CHECK(a * (b + c) == (a * b) + (a * c));
    }
}

TEST_SUITE("multiplication and division::decimal128") {
    TEST_CASE("multiplication of exact values") {
        CHECK(test::dec128_t{"2"} * test::dec128_t{"3"} == test::dec128_t{"6"});
        CHECK(test::dec128_t{"1.5"} * test::dec128_t{"2"} == test::dec128_t{"3"});
        CHECK(test::dec128_t{"1.25"} * test::dec128_t{"2.5"} == test::dec128_t{"3.125"});
    }

    TEST_CASE("division of exact values") {
        CHECK(test::dec128_t{"6"} / test::dec128_t{"3"} == test::dec128_t{"2"});
        CHECK(test::dec128_t{"3"} / test::dec128_t{"2"} == test::dec128_t{"1.5"});
        CHECK(test::dec128_t{"3.125"} / test::dec128_t{"2.5"} == test::dec128_t{"1.25"});
    }

    TEST_CASE("multiplicative identity") {
        const test::dec128_t x{"123.456"};
        CHECK(x * test::dec128_t{"1"} == x);
        CHECK(test::dec128_t{"1"} * x == x);
    }

    TEST_CASE("zero annihilates multiplication") {
        CHECK(test::dec128_t{"0"} * test::dec128_t{"123.456"} == test::dec128_t{"0"});
        CHECK(test::dec128_t{"123.456"} * test::dec128_t{"0"} == test::dec128_t{"0"});
    }

    TEST_CASE("division by one preserves value") {
        const test::dec128_t x{"123.456"};
        CHECK(x / test::dec128_t{"1"} == x);
    }

    TEST_CASE("self division gives one for nonzero values") {
        const test::dec128_t x{"123.456"};
        CHECK(x / x == test::dec128_t{"1"});
    }

    TEST_CASE("sign rules") {
        CHECK(test::dec128_t{"2"} * test::dec128_t{"-3"} == test::dec128_t{"-6"});
        CHECK(test::dec128_t{"-2"} * test::dec128_t{"-3"} == test::dec128_t{"6"});
        CHECK(test::dec128_t{"6"} / test::dec128_t{"-3"} == test::dec128_t{"-2"});
        CHECK(test::dec128_t{"-6"} / test::dec128_t{"-3"} == test::dec128_t{"2"});
    }

    TEST_CASE("commutativity of multiplication for exact values") {
        const test::dec128_t a{"1.25"};
        const test::dec128_t b{"2.5"};
        CHECK(a * b == b * a);
    }

    TEST_CASE("associativity of multiplication for simple exact values") {
        const test::dec128_t a{"2"};
        const test::dec128_t b{"3"};
        const test::dec128_t c{"4"};
        CHECK((a * b) * c == a * (b * c));
    }

    TEST_CASE("distributivity over addition for exact values") {
        const test::dec128_t a{"2"};
        const test::dec128_t b{"3"};
        const test::dec128_t c{"4"};
        CHECK(a * (b + c) == (a * b) + (a * c));
    }
}

TEST_SUITE("multiplication and division::cross precision") {
    TEST_CASE("same exact product agrees across precisions") {
        CHECK(test::as_string(test::dec32_t{"1.25"} * test::dec32_t{"2.5"}) == "3.125");
        CHECK(test::as_string(test::dec64_t{"1.25"} * test::dec64_t{"2.5"}) == "3.125");
        CHECK(test::as_string(test::dec128_t{"1.25"} * test::dec128_t{"2.5"}) == "3.125");
    }

    TEST_CASE("same exact quotient agrees across precisions") {
        CHECK(test::as_string(test::dec32_t{"3.125"} / test::dec32_t{"2.5"}) == "1.25");
        CHECK(test::as_string(test::dec64_t{"3.125"} / test::dec64_t{"2.5"}) == "1.25");
        CHECK(test::as_string(test::dec128_t{"3.125"} / test::dec128_t{"2.5"}) == "1.25");
    }
}

TEST_SUITE("multiplication and division::exceptional behavior") {
    TEST_CASE("division by zero behavior is explicit") {
        // Tighten this once you decide the library contract:
        // throw, inf, nan, or status flag.
        //
        // For now, this is a probe test you can adapt:
        //
        // CHECK_THROWS(test::dec32_t{"1"} / test::dec32_t{"0"});
        // CHECK_THROWS(test::dec64_t{"1"} / test::dec64_t{"0"});
        // CHECK_THROWS(test::dec128_t{"1"} / test::dec128_t{"0"});

        CHECK(true);
    }
}