/* This file is part of the LIBDECIMAL project and is licensed under the MIT License.
 * Copyright © 2025–2026 Varga Labs, Toronto, ON, Canada 🇨🇦 Contact: info@vargalabs.com */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <stdint.h>
#include <doctest/all>
#include <decimal/bid.hpp>

#define TRACE std::cerr
#define uint128_t __uint128_t

struct value_t{ std::string a,b,c; };
using decimal_t = typename math::decimal_t<uint64_t>;

TEST_CASE("pi") {
    using uint128_t = BID_UINT128;
    TRACE << math::sin(math::constants<uint32_t>::pi) << " " << math::constants<uint32_t>::pi << std::endl;
    TRACE << math::sin(math::constants<uint64_t>::pi) << " " << math::constants<uint64_t>::pi << std::endl;
    TRACE << math::sin(math::constants<uint128_t>::pi) << " " << math::constants<uint128_t>::pi << std::endl;
    TRACE << math::exp(math::constants<uint64_t>::zero) << std::endl;
    TRACE << math::exp(math::constants<uint32_t>::zero) << std::endl;
}

TEST_CASE("sin-32") {
    using namespace math::literals;
    using decimal_t =  math::decimal_t<uint64_t>;
    std::pair<std::string,std::string> values[] = {{"0", "0"},{"0.5235988", "0.5000000"},{"0.7853982", "0.7071068"},
    {"1.0471975", "0.8660254"},{"1.5707963", "1.0000000"},{"3.1415927", "0.0000000"},{"4.7123890", "-1.0000000"},
    {"6.2831853", "0.0000000"},{"-0.5235988", "-0.5000000"},{"-0.7853982", "-0.7071068"},{"-1.0471975", "-0.8660254"},
    {"-1.5707963", "-1.0000000"},{"-3.1415927", "0.0000000"},{"0.3926991", "0.3826834"},{"2.6179939", "0.5000000"},
    {"3.6651914", "-0.5000000"},{"3.9269908", "-0.7071068"},{"2.3561945", "0.7071068"},{"5.7595865", "-0.5000000"},{"5.4977871", "-0.7071068"} };        
    for(auto& test : values){
        std::cerr << test.second << " " << math::sin( decimal_t(test.first)) << std::endl; 
        //CHECK(test.first == test.second);
    }
}

TEST_CASE("testing") {
    CHECK( static_cast<std::string>(math::decimal_t<uint64_t>("nan")) == "nan");
    CHECK( static_cast<std::string>(math::decimal_t<uint64_t>("-inf")) == "-inf");
    CHECK( static_cast<std::string>(math::decimal_t<uint64_t>("+inf")) == "+inf");
}

TEST_CASE("bid64-to-string") {
    std::string test_values[] = 
        {"1000000000000001","1.000000000000001","100.0000000000001","1000.000000000001", "10000.00000000001","100000.0000000001",
        "1000000.000000001","10000000.00000001","100000000.0000001", "1000000000.000001","10000000000.00001","100000000000.0001",
        "1000000000000.001","10000000000000.01","100000000000000.1", "nan", "-inf", "+inf"};
    for(std::string value : test_values)
        CHECK( static_cast<std::string>(math::decimal_t<uint64_t>(value)) == value);
}
TEST_CASE("double to decimal") {
    using namespace math::literals;
    double test_values[] = {10.01, 1.001, 0.0001};
    for(double value : test_values){
        math::decimal_t<uint64_t> bid = value;
        TRACE << value << " " << bid.value  << " " << bid << " " << static_cast<long double>(bid) << " " << static_cast<std::string>(0.1_dec + 0.3_dec) <<  std::endl;
    }
}
TEST_CASE("decimal_t<uint64_t> : a + b = c") {
    value_t test_values[] = {
        {"0.01","0.03","0.04"},
        {"0.1","0.2","0.3"}}; 
    for(value_t value : test_values){
        math::decimal_t<uint64_t> A(value.a), B(value.b), C(value.c);
        TRACE << A << "+" << B << "=" << C << " " << (A+B) <<   std::endl;
        CHECK( static_cast<std::string>(A+B) == value.c);
        CHECK( A+B == C);
    }
}
TEST_CASE("decimal_t<T> literals") {
    using namespace math::literals;
    CHECK(static_cast<std::string>(0.1_dec + 0.3_dec) == "0.4");
    CHECK(static_cast<std::string>(0.1_dec32 + 0.3_dec32) == "0.4");
    CHECK(static_cast<std::string>(0.1_dec64 + 0.3_dec64) == "0.4");
    //CHECK(static_cast<std::string>(0.1_dec128 + 0.3_dec128) == "0.4");
}
TEST_CASE("decimal_t<T> operator") {
    using namespace math::literals;
    CHECK(0.1_dec < 0.2_dec);
    CHECK(0.2_dec > 0.01_dec);
    CHECK(0.2_dec >= 0.01_dec);    
    CHECK(0.1_dec >= 0.1_dec);    
}


TEST_CASE("bid64-python-testcases") {
    using namespace math::literals;
    using decimal_t =  math::decimal_t<uint64_t>;

    decimal_t test_values[] = {
        1.23456_dec64, 345677.1234_dec64, -12.2345678_dec64, -9999999999999999_dec64, 9999999999999999_dec64,
        decimal_t("NaN"), decimal_t("Inf"), decimal_t("-Inf") };
    for (auto & decimal: test_values)
        TRACE <<  static_cast<std::string>(decimal) << " " << decimal.value << std::endl;  
}
TEST_CASE("zeroes") {

    using namespace math::literals;
    TRACE << "ZERO: " << std::hex
    << (0.0_dec32).value << " " << (0.0_dec64).value << "(0.0_dec128).value " <<  std::endl;
}

TEST_CASE("from-integer") {
    using namespace math::literals;
    CHECK(static_cast<std::string>(0.0_dec32) == static_cast<std::string>(math::decimal_t<uint32_t>(0, -3)) );
    CHECK(static_cast<std::string>(4.001_dec32) == static_cast<std::string>(math::decimal_t<uint32_t>(4001UL, -3)) );
    CHECK(static_cast<std::string>(1.111111_dec32) == static_cast<std::string>(math::decimal_t<uint32_t>(1111111UL, -6)) );
    CHECK(static_cast<std::string>(9.99999_dec32) == static_cast<std::string>(math::decimal_t<uint32_t>(999999UL, -5)) );
    CHECK(static_cast<std::string>(8.999999_dec32) == static_cast<std::string>(math::decimal_t<uint32_t>(8999999UL, -6)) );
    CHECK(static_cast<std::string>(9.999999_dec32) == static_cast<std::string>(math::decimal_t<uint32_t>(9999999UL, -6)) );

    CHECK(static_cast<std::string>(0_dec64) == static_cast<std::string>(math::decimal_t<uint64_t>(0, -3)) );
    CHECK(static_cast<std::string>(4.001_dec64) == static_cast<std::string>(math::decimal_t<uint64_t>(4001ULL, -3)) );
    CHECK(static_cast<std::string>(9.99999999999999_dec64) == static_cast<std::string>(math::decimal_t<uint64_t>(999999999999999ULL, -14)) );
    CHECK(static_cast<std::string>(9.999999999999999_dec64) == static_cast<std::string>(math::decimal_t<uint64_t>(9999999999999999ULL, -15)) );

}
