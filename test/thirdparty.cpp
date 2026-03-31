#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/all>

#include <string>
#include <string_view>
#include <cstdint>

#include "decimal.hpp"

namespace test {
    inline std::uint32_t str2bid32_raw(std::string_view str, unsigned int* flags = nullptr) {
        unsigned int local_flags = 0;
        std::string tmp{str};
        auto raw = __bid32_from_string(tmp.data(), 0, &local_flags);
        if (flags) *flags = local_flags;
        return raw;
    }

    inline std::uint64_t str2bid64_raw(std::string_view str, unsigned int* flags = nullptr) {
        unsigned int local_flags = 0;
        std::string tmp{str};
        auto raw = __bid64_from_string(tmp.data(), 0, &local_flags);
        if (flags) *flags = local_flags;
        return raw;
    }

    inline std::string bid32_as_string(std::string_view str) {
        const auto raw = str2bid32_raw(str);
        return math::bid::display_bid32(raw);
    }

    inline std::string bid64_as_string(std::string_view str) {
        const auto raw = str2bid64_raw(str);
        return math::bid::display_bid64(raw);
    }
}

TEST_SUITE("intel bid parser::decimal32") {
    TEST_CASE("parses ordinary decimal strings") {
        CHECK(test::bid32_as_string("0") == "0");
        CHECK(test::bid32_as_string("1") == "1");
        CHECK(test::bid32_as_string("10") == "10");
        CHECK(test::bid32_as_string("123.45") == "123.45");
        CHECK(test::bid32_as_string("-123.45") == "-123.45");
    }

    TEST_CASE("parses scientific notation positive exponents") {
        CHECK(test::bid32_as_string("1e0") == "1");
        CHECK(test::bid32_as_string("1e1") == "10");
        CHECK(test::bid32_as_string("1e2") == "100");
        CHECK(test::bid32_as_string("1e3") == "1000");
        CHECK(test::bid32_as_string("1e4") == "10000");

        CHECK(test::bid32_as_string("1E0") == "1");
        CHECK(test::bid32_as_string("1E1") == "10");
        CHECK(test::bid32_as_string("1E2") == "100");
        CHECK(test::bid32_as_string("1E3") == "1000");
        CHECK(test::bid32_as_string("1E4") == "10000");
    }

    TEST_CASE("parses scientific notation negative exponents") {
        CHECK(test::bid32_as_string("1e-0") == "1");
        CHECK(test::bid32_as_string("1e-1") == "0.1");
        CHECK(test::bid32_as_string("1e-2") == "0.01");
        CHECK(test::bid32_as_string("1e-3") == "0.001");
        CHECK(test::bid32_as_string("1e-4") == "0.0001");

        CHECK(test::bid32_as_string("1E-0") == "1");
        CHECK(test::bid32_as_string("1E-1") == "0.1");
        CHECK(test::bid32_as_string("1E-2") == "0.01");
        CHECK(test::bid32_as_string("1E-3") == "0.001");
        CHECK(test::bid32_as_string("1E-4") == "0.0001");
    }

    TEST_CASE("parses fractional scientific notation") {
        CHECK(test::bid32_as_string("1.2e0") == "1.2");
        CHECK(test::bid32_as_string("1.2e1") == "12");
        CHECK(test::bid32_as_string("1.2e2") == "120");
        CHECK(test::bid32_as_string("1.2e-1") == "0.12");
        CHECK(test::bid32_as_string("1.2e-2") == "0.012");

        CHECK(test::bid32_as_string("1.23e2") == "123");
        CHECK(test::bid32_as_string("1.23e-2") == "0.0123");
    }

    TEST_CASE("parses signed scientific notation") {
        CHECK(test::bid32_as_string("-1e0") == "-1");
        CHECK(test::bid32_as_string("-1e1") == "-10");
        CHECK(test::bid32_as_string("-1e-1") == "-0.1");
        CHECK(test::bid32_as_string("-1.2e2") == "-120");
    }

    TEST_CASE("records flags for malformed input") {
        unsigned int flags = 0;

        const auto raw_empty = test::str2bid32_raw("", &flags);
        CHECK(flags != 0);
        CHECK(math::bid::display_bid32(raw_empty) == "nan");

        flags = 0;
        const auto raw_alpha = test::str2bid32_raw("abc", &flags);
        CHECK(flags != 0);
        CHECK(math::bid::display_bid32(raw_alpha) == "nan");

        flags = 0;
        const auto raw_bad = test::str2bid32_raw("1..2", &flags);
        CHECK(flags != 0);
        CHECK(math::bid::display_bid32(raw_bad) == "nan");
    }
}

TEST_SUITE("intel bid parser::decimal64") {
    TEST_CASE("parses scientific notation positive exponents") {
        CHECK(test::bid64_as_string("1e0") == "1");
        CHECK(test::bid64_as_string("1e1") == "10");
        CHECK(test::bid64_as_string("1e2") == "100");
        CHECK(test::bid64_as_string("1e3") == "1000");
        CHECK(test::bid64_as_string("1e4") == "10000");
    }

    TEST_CASE("parses scientific notation negative exponents") {
        CHECK(test::bid64_as_string("1e-0") == "1");
        CHECK(test::bid64_as_string("1e-1") == "0.1");
        CHECK(test::bid64_as_string("1e-2") == "0.01");
        CHECK(test::bid64_as_string("1e-3") == "0.001");
        CHECK(test::bid64_as_string("1e-4") == "0.0001");
    }

    TEST_CASE("parses fractional scientific notation") {
        CHECK(test::bid64_as_string("1.2e0") == "1.2");
        CHECK(test::bid64_as_string("1.2e1") == "12");
        CHECK(test::bid64_as_string("1.2e2") == "120");
        CHECK(test::bid64_as_string("1.2e-1") == "0.12");
        CHECK(test::bid64_as_string("1.2e-2") == "0.012");
    }
}