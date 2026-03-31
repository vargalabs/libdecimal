/* This file is part of the LIBDECIMAL project and is licensed under the MIT License.
 * Copyright © 2025–2026 Varga Labs, Toronto, ON, Canada 🇨🇦 Contact: info@vargalabs.com */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <array>
#include <sstream>
#include <string>
#include <format>

#include <doctest/all>
#include "decimal.hpp"

namespace test {
    using dec32_t = math::decimal_t<std::uint32_t>;
    using dec64_t = math::decimal_t<std::uint64_t>;
    using dec128_t = math::decimal_t<math::uint128_t>;

    template <class decimal_t>
    std::string as_string(decimal_t value) {
        return static_cast<std::string>(value);
    }
}

TEST_SUITE("print::bid::print") {
    TEST_CASE("prints unsigned integral coefficients") {
        std::array<char, BID32_MAX_PRINTED_DIGITS> buffer{};

        CHECK(math::bid::print(false, 0u, 0, buffer.data(), buffer.size()) == "0");
        CHECK(math::bid::print(false, 1u, 0, buffer.data(), buffer.size()) == "1");
        CHECK(math::bid::print(false, 42u, 0, buffer.data(), buffer.size()) == "42");
        CHECK(math::bid::print(false, 9999999u, 0, buffer.data(), buffer.size()) == "9999999");
    }

    TEST_CASE("prints signed integral coefficients") {
        std::array<char, BID32_MAX_PRINTED_DIGITS> buffer{};

        CHECK(math::bid::print(true, 0u, 0, buffer.data(), buffer.size()) == "0");
        CHECK(math::bid::print(true, 1u, 0, buffer.data(), buffer.size()) == "-1");
        CHECK(math::bid::print(true, 42u, 0, buffer.data(), buffer.size()) == "-42");
    }

    TEST_CASE("prints fractional coefficients with negative exponent") {
        std::array<char, BID32_MAX_PRINTED_DIGITS> buffer{};

        CHECK(math::bid::print(false, 1u, -1, buffer.data(), buffer.size()) == "0.1");
        CHECK(math::bid::print(false, 12u, -1, buffer.data(), buffer.size()) == "1.2");
        CHECK(math::bid::print(false, 123u, -2, buffer.data(), buffer.size()) == "1.23");
        CHECK(math::bid::print(false, 5u, -3, buffer.data(), buffer.size()) == "0.005");
        CHECK(math::bid::print(true, 5u, -3, buffer.data(), buffer.size()) == "-0.005");
    }

    TEST_CASE("prints positive exponent as zero padding or unchanged text") {
        std::array<char, BID32_MAX_PRINTED_DIGITS> buffer{};

        CHECK(math::bid::print(false, 7u, 1, buffer.data(), buffer.size()) == "7");
        CHECK(math::bid::print(false, 42u, 2, buffer.data(), buffer.size()) == "420");
        CHECK(math::bid::print(false, 42u, 3, buffer.data(), buffer.size()) == "4200");
        CHECK(math::bid::print(true, 42u, 3, buffer.data(), buffer.size()) == "-4200");
    }

    TEST_CASE("preserves already aligned decimal point placement") {
        std::array<char, BID32_MAX_PRINTED_DIGITS> buffer{};

        CHECK(math::bid::print(false, 100u, -2, buffer.data(), buffer.size()) == "1.00");
        CHECK(math::bid::print(false, 1000u, -3, buffer.data(), buffer.size()) == "1.000");
        CHECK(math::bid::print(false, 1000u, -2, buffer.data(), buffer.size()) == "10.00");
    }
}

TEST_SUITE("print::bid::display_bid32") {
    TEST_CASE("prints finite positive values") {
        CHECK(math::bid::display_bid32(math::bid::uint32_to_bid32(12345u, -2)) == "123.45");
        CHECK(math::bid::display_bid32(math::bid::uint32_to_bid32(1u, 0)) == "1");
        CHECK(math::bid::display_bid32(math::bid::uint32_to_bid32(5u, -3)) == "0.005");
    }

    TEST_CASE("prints decimal32 zero in canonical current form") {
        CHECK(math::bid::display_bid32(math::bid::uint32_to_bid32(0u, 0)) == "0");
        CHECK(math::bid::display_bid32(math::bid::uint32_to_bid32(0u, 42)) == "0");
    }

    TEST_CASE("prints non-finite decimal32 values") {
        CHECK(math::bid::display_bid32(0x78000000u) == "+inf");
        CHECK(math::bid::display_bid32(0xf8000000u) == "-inf");
        CHECK(math::bid::display_bid32(0x7c000000u) == "nan");
    }

    TEST_CASE("oversized coefficient collapses to current zero representation") {
        const auto raw = math::bid::uint32_to_bid32(10'000'000u, 0);
        CHECK(math::bid::display_bid32(raw) == "0");
    }
}

TEST_SUITE("print::bid::display_bid64") {
    TEST_CASE("prints finite positive values") {
        CHECK(math::bid::display_bid64(math::bid::uint64_to_bid64(123456789ull, -4)) == "12345.6789");
        CHECK(math::bid::display_bid64(math::bid::uint64_to_bid64(1ull, 0)) == "1");
        CHECK(math::bid::display_bid64(math::bid::uint64_to_bid64(5ull, -3)) == "0.005");
    }

    TEST_CASE("prints decimal64 zero in canonical current form") {
        CHECK(math::bid::display_bid64(math::bid::uint64_to_bid64(0ull, 0)) == "0");
        CHECK(math::bid::display_bid64(math::bid::uint64_to_bid64(0ull, 123)) == "0");
    }

    TEST_CASE("prints non-finite decimal64 values") {
        CHECK(math::bid::display_bid64(0x7800000000000000ull) == "+inf");
        CHECK(math::bid::display_bid64(0xf800000000000000ull) == "-inf");
        CHECK(math::bid::display_bid64(0x7c00000000000000ull) == "nan");
    }

    TEST_CASE("oversized coefficient collapses to current zero representation") {
        const auto raw = math::bid::uint64_to_bid64(10'000'000'000'000'000ull, 0);
        CHECK(math::bid::display_bid64(raw) == "0");
    }
}

TEST_SUITE("print::bid::display_bid128") {
    TEST_CASE("prints finite decimal128 in normalized fixed notation") {
        CHECK(test::as_string(test::dec128_t{"1.25"}) == "1.25");
        CHECK(test::as_string(test::dec128_t{"1234567890.125"}) == "1234567890.125");
        CHECK(test::as_string(test::dec128_t{"3.141592653589793238462643383279502"}) ==
              "3.141592653589793238462643383279502");
    }

    TEST_CASE("rounds inputs beyond 34 significant digits for display") {
        const test::dec128_t value{"3.1415926535897932384626433832795029"};
        CHECK(test::as_string(value) == "3.141592653589793238462643383279503");
    }

    TEST_CASE("prints decimal128 zero in normalized form") {
        CHECK(test::as_string(test::dec128_t{"0"}) == "0");
        CHECK(test::as_string(test::dec128_t{math::uint128_t{}, 0}) == "0");
    }

    TEST_CASE("does not emit scientific notation for normalized decimal128 display") {
        CHECK(test::as_string(test::dec128_t{"1.25"}).find('E') == std::string::npos);
        CHECK(test::as_string(test::dec128_t{"1234567890.125"}).find('E') == std::string::npos);
        CHECK(test::as_string(test::dec128_t{"3.141592653589793238462643383279502"}).find('E') == std::string::npos);
    }
}

TEST_SUITE("print::decimal_string_conversion") {
    TEST_CASE("decimal32 string conversion") {
        CHECK(test::as_string(test::dec32_t{}) == "0");
        CHECK(test::as_string(test::dec32_t{"123.45"}) == "123.45");
        CHECK(test::as_string(test::dec32_t{"0.005"}) == "0.005");
    }

    TEST_CASE("decimal64 string conversion") {
        CHECK(test::as_string(test::dec64_t{}) == "0");
        CHECK(test::as_string(test::dec64_t{"123.45"}) == "123.45");
        CHECK(test::as_string(test::dec64_t{"0.005"}) == "0.005");
    }

    TEST_CASE("decimal128 string conversion") {
        CHECK(test::as_string(test::dec128_t{"0"}) == "0");
        CHECK(test::as_string(test::dec128_t{"123.45"}) == "123.45");
        CHECK(test::as_string(test::dec128_t{"0.005"}) == "0.005");
    }
}

TEST_SUITE("print::ostream_insertion") {
    TEST_CASE("streams decimal32") {
        std::ostringstream os;
        os << test::dec32_t{"123.45"};
        CHECK(os.str() == "123.45");
    }

    TEST_CASE("streams decimal64") {
        std::ostringstream os;
        os << test::dec64_t{"123.45"};
        CHECK(os.str() == "123.45");
    }

    TEST_CASE("streams decimal128") {
        std::ostringstream os;
        os << test::dec128_t{"123.45"};
        CHECK(os.str() == "123.45");
    }

    TEST_CASE("streams special display forms when representable through constants") {
        std::ostringstream os_inf;
        os_inf << math::constants<std::uint32_t>::inf;
        CHECK(os_inf.str() == "+inf");

        std::ostringstream os_nan;
        os_nan << math::constants<std::uint32_t>::nan;
        CHECK(os_nan.str() == "nan");
    }
}

TEST_SUITE("print::std_format") {
    TEST_CASE("formats decimal32") {
        CHECK(std::format("{}", test::dec32_t{"123.45"}) == "123.45");
        CHECK(std::format("value={}", test::dec32_t{"123.45"}) == "value=123.45");
    }

    TEST_CASE("formats decimal64") {
        CHECK(std::format("{}", test::dec64_t{"123.45"}) == "123.45");
        CHECK(std::format("value={}", test::dec64_t{"123.45"}) == "value=123.45");
    }

    TEST_CASE("formats decimal128") {
        CHECK(std::format("{}", test::dec128_t{"123.45"}) == "123.45");
        CHECK(std::format("value={}", test::dec128_t{"123.45"}) == "value=123.45");
    }
}

TEST_SUITE("print::regression_sentinels") {
    TEST_CASE("decimal32 and decimal64 zero formatting differ currently") {
        CHECK(test::as_string(test::dec32_t{}) == "0");
        CHECK(test::as_string(test::dec64_t{}) == "0");
    }

    TEST_CASE("decimal128 display is normalized rather than raw Intel scientific form") {
        const auto s = test::as_string(test::dec128_t{"1.25"});
        CHECK(s == "1.25");
        CHECK(s.find('E') == std::string::npos);
    }
}

TEST_SUITE("print::extended formatting") {
    TEST_CASE("formats negative finite values") {
        CHECK(test::as_string(test::dec32_t{"-123.45"}) == "-123.45");
        CHECK(test::as_string(test::dec64_t{"-123.45"}) == "-123.45");
        CHECK(test::as_string(test::dec128_t{"-123.45"}) == "-123.45");
    }

    TEST_CASE("formats very small finite values without scientific notation") {
        CHECK(test::as_string(test::dec32_t{"0.005"}) == "0.005");
        CHECK(test::as_string(test::dec64_t{"0.0000000000000001"}) == "0.0000000000000001");
        CHECK(test::as_string(test::dec128_t{"0.0000000000000000000000000000000001"}) ==
              "0.0000000000000000000000000000000001");
    }

    TEST_CASE("std::format basic formatting") {
        CHECK(std::format("{}", test::dec32_t{"12.5"}) == "12.5");
        CHECK(std::format("{}", test::dec64_t{"12.5"}) == "12.5");
        CHECK(std::format("{}", test::dec128_t{"12.5"}) == "12.5");
    }

    TEST_CASE("std::format width and alignment") {
        CHECK(std::format("{:>10}", test::dec32_t{"12.5"}) == "      12.5");
        CHECK(std::format("{:<10}", test::dec64_t{"12.5"}) == "12.5      ");
        CHECK(std::format("{:*^10}", test::dec128_t{"12.5"}) == "***12.5***");
    }

    TEST_CASE("format string roundtrip is stable") {
        const auto a = std::format("{}", test::dec32_t{"123.45"});
        const auto b = std::format("{}", test::dec64_t{"123.45"});
        const auto c = std::format("{}", test::dec128_t{"123.45"});

        CHECK(test::as_string(test::dec32_t{a}) == a);
        CHECK(test::as_string(test::dec64_t{b}) == b);
        CHECK(test::as_string(test::dec128_t{c}) == c);
    }
    TEST_CASE("printout") {
        using namespace math::literals;
        std::cout << std::format("{:>10}", 12.345_dec ) << std::endl;
        std::cout << std::format("{:<5}", 12.345_dec32 ) << std::endl;
        std::cout << std::format("{}", 12.345_dec64 ) << std::endl;
        std::cout << std::format("{}", 12.345_dec128 ) << std::endl;
    }
}