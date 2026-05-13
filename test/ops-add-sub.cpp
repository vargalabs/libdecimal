/* This file is part of the LIBDECIMAL project and is licensed under the MIT License.
 * Copyright © 2025–2026 Varga Labs, Toronto, ON, Canada 🇨🇦 Contact: info@vargalabs.com */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

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
}

TEST_SUITE("linear operations::decimal32") {
    TEST_CASE("unary plus preserves value") {
        const test::dec32_t x{"12.5"};
        CHECK(+x == x);
        CHECK(test::as_string(+x) == "12.5");
    }

    TEST_CASE("unary minus produces additive inverse") {
        const test::dec32_t x{"12.5"};
        CHECK(test::as_string(-x) == "-12.5");
        CHECK(x + (-x) == test::dec32_t{"0"});
    }

    TEST_CASE("addition") {
        CHECK(test::as_string(test::dec32_t{"1.25"} + test::dec32_t{"2.5"}) == "3.75");
        CHECK(test::as_string(test::dec32_t{"0"} + test::dec32_t{"7.125"}) == "7.125");
        CHECK(test::as_string(test::dec32_t{"-1.5"} + test::dec32_t{"2.0"}) == "0.5");
    }

    TEST_CASE("subtraction") {
        CHECK(test::as_string(test::dec32_t{"2.5"} - test::dec32_t{"1.25"}) == "1.25");
        CHECK(test::as_string(test::dec32_t{"1.25"} - test::dec32_t{"2.5"}) == "-1.25");
        CHECK(test::as_string(test::dec32_t{"7"} - test::dec32_t{"7"}) == "0");
    }

    TEST_CASE("additive identity") {
        const test::dec32_t x{"123.456"};
        CHECK(x + test::dec32_t{"0"} == x);
        CHECK(test::dec32_t{"0"} + x == x);
    }

    TEST_CASE("additive inverse") {
        const test::dec32_t x{"123.456"};
        CHECK(x + (-x) == test::dec32_t{"0"});
        CHECK((-x) + x == test::dec32_t{"0"});
    }

    TEST_CASE("commutativity of addition for exact values") {
        const test::dec32_t a{"1.25"};
        const test::dec32_t b{"2.75"};
        CHECK(a + b == b + a);
    }

    TEST_CASE("associativity of addition for simple exact values") {
        const test::dec32_t a{"1.25"};
        const test::dec32_t b{"2.5"};
        const test::dec32_t c{"3.75"};
        CHECK((a + b) + c == a + (b + c));
    }

    TEST_CASE("compound addition assignment") {
        test::dec32_t x{"1.25"};
        x += test::dec32_t{"2.5"};
        CHECK(test::as_string(x) == "3.75");
    }

    TEST_CASE("compound subtraction assignment") {
        test::dec32_t x{"3.75"};
        x -= test::dec32_t{"2.5"};
        CHECK(test::as_string(x) == "1.25");
    }
}

TEST_SUITE("linear operations::decimal64") {
    TEST_CASE("unary plus preserves value") {
        const test::dec64_t x{"12.5"};
        CHECK(+x == x);
        CHECK(test::as_string(+x) == "12.5");
    }

    TEST_CASE("unary minus produces additive inverse") {
        const test::dec64_t x{"12.5"};
        CHECK(test::as_string(-x) == "-12.5");
        CHECK(x + (-x) == test::dec64_t{"0"});
    }

    TEST_CASE("addition") {
        CHECK(test::as_string(test::dec64_t{"1.25"} + test::dec64_t{"2.5"}) == "3.75");
        CHECK(test::as_string(test::dec64_t{"0"} + test::dec64_t{"7.125"}) == "7.125");
        CHECK(test::as_string(test::dec64_t{"-1.5"} + test::dec64_t{"2.0"}) == "0.5");
    }

    TEST_CASE("subtraction") {
        CHECK(test::as_string(test::dec64_t{"2.5"} - test::dec64_t{"1.25"}) == "1.25");
        CHECK(test::as_string(test::dec64_t{"1.25"} - test::dec64_t{"2.5"}) == "-1.25");
        CHECK(test::as_string(test::dec64_t{"7"} - test::dec64_t{"7"}) == "0");
    }

    TEST_CASE("additive identity") {
        const test::dec64_t x{"123.456"};
        CHECK(x + test::dec64_t{"0"} == x);
        CHECK(test::dec64_t{"0"} + x == x);
    }

    TEST_CASE("additive inverse") {
        const test::dec64_t x{"123.456"};
        CHECK(x + (-x) == test::dec64_t{"0"});
        CHECK((-x) + x == test::dec64_t{"0"});
    }

    TEST_CASE("commutativity of addition for exact values") {
        const test::dec64_t a{"1.25"};
        const test::dec64_t b{"2.75"};
        CHECK(a + b == b + a);
    }

    TEST_CASE("associativity of addition for simple exact values") {
        const test::dec64_t a{"1.25"};
        const test::dec64_t b{"2.5"};
        const test::dec64_t c{"3.75"};
        CHECK((a + b) + c == a + (b + c));
    }

    TEST_CASE("compound addition assignment") {
        test::dec64_t x{"1.25"};
        x += test::dec64_t{"2.5"};
        CHECK(test::as_string(x) == "3.75");
    }

    TEST_CASE("compound subtraction assignment") {
        test::dec64_t x{"3.75"};
        x -= test::dec64_t{"2.5"};
        CHECK(test::as_string(x) == "1.25");
    }
}

TEST_SUITE("linear operations::decimal128") {
    TEST_CASE("unary plus preserves value") {
        const test::dec128_t x{"12.5"};
        CHECK(+x == x);
    }

    TEST_CASE("unary minus produces additive inverse") {
        const test::dec128_t x{"12.5"};
        CHECK((-x) + x == test::dec128_t{"0"});
        CHECK(x + (-x) == test::dec128_t{"0"});
    }

    TEST_CASE("addition") {
        CHECK(test::dec128_t{"1.25"} + test::dec128_t{"2.5"} == test::dec128_t{"3.75"});
        CHECK(test::dec128_t{"0"} + test::dec128_t{"7.125"} == test::dec128_t{"7.125"});
        CHECK(test::dec128_t{"-1.5"} + test::dec128_t{"2.0"} == test::dec128_t{"0.5"});
    }

    TEST_CASE("subtraction") {
        CHECK(test::dec128_t{"2.5"} - test::dec128_t{"1.25"} == test::dec128_t{"1.25"});
        CHECK(test::dec128_t{"1.25"} - test::dec128_t{"2.5"} == test::dec128_t{"-1.25"});
        CHECK(test::dec128_t{"7"} - test::dec128_t{"7"} == test::dec128_t{"0"});
    }

    TEST_CASE("additive identity") {
        const test::dec128_t x{"123.456"};
        CHECK(x + test::dec128_t{"0"} == x);
        CHECK(test::dec128_t{"0"} + x == x);
    }

    TEST_CASE("additive inverse") {
        const test::dec128_t x{"123.456"};
        CHECK(x + (-x) == test::dec128_t{"0"});
        CHECK((-x) + x == test::dec128_t{"0"});
    }

    TEST_CASE("commutativity of addition for exact values") {
        const test::dec128_t a{"1.25"};
        const test::dec128_t b{"2.75"};
        CHECK(a + b == b + a);
    }

    TEST_CASE("associativity of addition for simple exact values") {
        const test::dec128_t a{"1.25"};
        const test::dec128_t b{"2.5"};
        const test::dec128_t c{"3.75"};
        CHECK((a + b) + c == a + (b + c));
    }

    TEST_CASE("compound addition assignment") {
        test::dec128_t x{"1.25"};
        x += test::dec128_t{"2.5"};
        CHECK(x == test::dec128_t{"3.75"});
    }

    TEST_CASE("compound subtraction assignment") {
        test::dec128_t x{"3.75"};
        x -= test::dec128_t{"2.5"};
        CHECK(x == test::dec128_t{"1.25"});
    }
}

TEST_SUITE("linear operations::cross type invariants") {
    TEST_CASE("same exact decimal expression agrees across precisions") {
        const auto s32  = test::as_string(test::dec32_t{"1.25"} + test::dec32_t{"2.5"});
        const auto s64  = test::as_string(test::dec64_t{"1.25"} + test::dec64_t{"2.5"});
        const auto s128 = test::as_string(test::dec128_t{"1.25"} + test::dec128_t{"2.5"});

        CHECK(s32 == "3.75");
        CHECK(s64 == "3.75");
        CHECK(s128 == "3.75");
    }

    TEST_CASE("x - x = 0 across precisions") {
        CHECK(test::dec32_t{"123.456"} - test::dec32_t{"123.456"} == test::dec32_t{"0"});
        CHECK(test::dec64_t{"123.456"} - test::dec64_t{"123.456"} == test::dec64_t{"0"});
        CHECK(test::dec128_t{"123.456"} - test::dec128_t{"123.456"} == test::dec128_t{"0"});
    }
}