/* This file is part of the LIBDECIMAL project and is licensed under the MIT License.
 * Copyright © 2025–2026 Varga Labs, Toronto, ON, Canada 🇨🇦 Contact: info@vargalabs.com */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/all>

#include "decimal.hpp"

namespace {
    template <typename T>
    auto get_constant(const char* name) -> const math::decimal_t<T>& {
        using constants_t = math::constants<T>;

        if      (std::string_view{name} == "nan")          return constants_t::nan;
        else if (std::string_view{name} == "inf")          return constants_t::inf;
        else if (std::string_view{name} == "zero")         return constants_t::zero;
        else if (std::string_view{name} == "one")          return constants_t::one;
        else if (std::string_view{name} == "pi")           return constants_t::pi;
        else if (std::string_view{name} == "tau")          return constants_t::tau;
        else if (std::string_view{name} == "e")            return constants_t::e;
        else if (std::string_view{name} == "phi")          return constants_t::phi;
        else if (std::string_view{name} == "golden_ratio") return constants_t::golden_ratio;
        else if (std::string_view{name} == "silver_ratio") return constants_t::silver_ratio;
        else if (std::string_view{name} == "sqrt2")        return constants_t::sqrt2;
        else if (std::string_view{name} == "sqrt3")        return constants_t::sqrt3;
        else if (std::string_view{name} == "sqrt5")        return constants_t::sqrt5;
        else if (std::string_view{name} == "ln2")          return constants_t::ln2;
        else if (std::string_view{name} == "ln10")         return constants_t::ln10;
        else if (std::string_view{name} == "log2e")        return constants_t::log2e;
        else if (std::string_view{name} == "log10e")       return constants_t::log10e;
        else if (std::string_view{name} == "deg_to_rad")   return constants_t::deg_to_rad;
        else if (std::string_view{name} == "rad_to_deg")   return constants_t::rad_to_deg;
        else if (std::string_view{name} == "inv_pi")       return constants_t::inv_pi;
        else if (std::string_view{name} == "two_pi")       return constants_t::two_pi;
        else if (std::string_view{name} == "half_pi")      return constants_t::half_pi;
        else if (std::string_view{name} == "quarter_pi")   return constants_t::quarter_pi;
        else if (std::string_view{name} == "cube_root_2")  return constants_t::cube_root_2;
        else if (std::string_view{name} == "cube_root_3")  return constants_t::cube_root_3;

        throw std::runtime_error("unknown constant name");
    }
    template <typename T> void check_constant_eq(const char* name, std::string_view expected) {
        using decimal_t = math::decimal_t<T>;
        const decimal_t actual = decimal_t{expected};

        CAPTURE(name);
        CHECK(math::constants<T>{}.zero == math::constants<T>::zero); // keeps doctest happy with template instantiation context
        CHECK((decltype(math::constants<T>::zero){} + math::constants<T>::zero, true)); // cheap compile-time-ish smoke
        CHECK(get_constant<T>(name) == actual);
    }

    template <typename T> void check_special_values() {
        using decimal_t   = math::decimal_t<T>;
        using constants_t = math::constants<T>;

        CHECK(constants_t::nan != constants_t::nan);
        CHECK(constants_t::inf == decimal_t{"inf"});
        CHECK(constants_t::zero == decimal_t{"0"});
        CHECK(constants_t::one == decimal_t{"1"});
        CHECK(constants_t::inf > constants_t::one);
    }

    template <typename T> void check_common_aliases() {
        using constants_t = math::constants<T>;
        CHECK(constants_t::tau == constants_t::two_pi);
    }

    template <typename integral_t>
    void check_golden_ratio_alias() {
        using constants_t = math::constants<integral_t>;
        CHECK(constants_t::golden_ratio == constants_t::phi);
    }
}

TEST_CASE("math::constants<uint32_t>") {
    check_special_values<uint32_t>();
    check_common_aliases<uint32_t>();
    check_golden_ratio_alias<uint32_t>();

    check_constant_eq<uint32_t>("pi",           "3.141593");
    check_constant_eq<uint32_t>("tau",          "6.283185");
    check_constant_eq<uint32_t>("e",            "2.718282");
    check_constant_eq<uint32_t>("phi",          "1.618034");
    check_constant_eq<uint32_t>("golden_ratio", "1.618034");
    check_constant_eq<uint32_t>("silver_ratio", "2.414214");
    check_constant_eq<uint32_t>("sqrt2",        "1.414214");
    check_constant_eq<uint32_t>("sqrt3",        "1.732051");
    check_constant_eq<uint32_t>("sqrt5",        "2.236068");
    check_constant_eq<uint32_t>("ln2",          "0.693147");
    check_constant_eq<uint32_t>("ln10",         "2.302586");
    check_constant_eq<uint32_t>("log2e",        "1.442696");
    check_constant_eq<uint32_t>("log10e",       "0.4342945");
    check_constant_eq<uint32_t>("deg_to_rad",   "0.01745329");
    check_constant_eq<uint32_t>("rad_to_deg",   "57.29578");
    check_constant_eq<uint32_t>("inv_pi",       "0.3183099");
    check_constant_eq<uint32_t>("two_pi",       "6.283185");
    check_constant_eq<uint32_t>("half_pi",      "1.570796");
    check_constant_eq<uint32_t>("quarter_pi",   "0.7853982");
    check_constant_eq<uint32_t>("cube_root_2",  "1.259921");
    check_constant_eq<uint32_t>("cube_root_3",  "1.4422496");
}

TEST_CASE("math::constants<uint64_t>") {
    check_special_values<uint64_t>();
    check_common_aliases<uint64_t>();

    check_constant_eq<uint64_t>("pi",           "3.141592653589793");
    check_constant_eq<uint64_t>("tau",          "6.283185307179586");
    check_constant_eq<uint64_t>("e",            "2.718281828459045");
    check_constant_eq<uint64_t>("phi",          "1.618033988749895");
    check_constant_eq<uint64_t>("silver_ratio", "2.414213562373095");
    check_constant_eq<uint64_t>("sqrt2",        "1.414213562373095");
    check_constant_eq<uint64_t>("sqrt3",        "1.732050807568877");
    check_constant_eq<uint64_t>("sqrt5",        "2.236067977499790");
    check_constant_eq<uint64_t>("ln2",          "0.6931471805599453");
    check_constant_eq<uint64_t>("ln10",         "2.302585092994046");
    check_constant_eq<uint64_t>("log2e",        "1.442695040888963");
    check_constant_eq<uint64_t>("log10e",       "0.4342944819032518");
    check_constant_eq<uint64_t>("deg_to_rad",   "0.01745329251994330");
    check_constant_eq<uint64_t>("rad_to_deg",   "57.29577951308232");
    check_constant_eq<uint64_t>("inv_pi",       "0.3183098861837907");
    check_constant_eq<uint64_t>("two_pi",       "6.283185307179586");
    check_constant_eq<uint64_t>("half_pi",      "1.570796326794897");
    check_constant_eq<uint64_t>("quarter_pi",   "0.785398163397448");
    check_constant_eq<uint64_t>("cube_root_2",  "1.259921049894873");
    check_constant_eq<uint64_t>("cube_root_3",  "1.442249570307408");
}

TEST_CASE("math::constants<BID_UINT128>") {
    check_special_values<BID_UINT128>();
    check_common_aliases<BID_UINT128>();
    check_golden_ratio_alias<BID_UINT128>();

    check_constant_eq<BID_UINT128>("pi",           "3.1415926535897932384626433832795029");
    check_constant_eq<BID_UINT128>("tau",          "6.2831853071795864769252867665590058");
    check_constant_eq<BID_UINT128>("e",            "2.7182818284590452353602874713526625");
    check_constant_eq<BID_UINT128>("phi",          "1.6180339887498948482045868343656381");
    check_constant_eq<BID_UINT128>("golden_ratio", "1.6180339887498948482045868343656381");
    check_constant_eq<BID_UINT128>("silver_ratio", "2.4142135623730950488016887242096981");
    check_constant_eq<BID_UINT128>("sqrt2",        "1.4142135623730950488016887242096981");
    check_constant_eq<BID_UINT128>("sqrt3",        "1.7320508075688772935274463415058724");
    check_constant_eq<BID_UINT128>("sqrt5",        "2.2360679774997896964091736687312762");
    check_constant_eq<BID_UINT128>("ln2",          "0.6931471805599453094172321214581766");
    check_constant_eq<BID_UINT128>("ln10",         "2.3025850929940456840179914546843642");
    check_constant_eq<BID_UINT128>("log2e",        "1.4426950408889634073599246810018921");
    check_constant_eq<BID_UINT128>("log10e",       "0.4342944819032518276511289189166051");
    check_constant_eq<BID_UINT128>("deg_to_rad",   "0.0174532925199432957692369076848861");
    check_constant_eq<BID_UINT128>("rad_to_deg",   "57.2957795130823208767981548141051703");
    check_constant_eq<BID_UINT128>("inv_pi",       "0.3183098861837906715377675267450287");
    check_constant_eq<BID_UINT128>("two_pi",       "6.2831853071795864769252867665590058");
    check_constant_eq<BID_UINT128>("half_pi",      "1.5707963267948966192313216916397514");
    check_constant_eq<BID_UINT128>("quarter_pi",   "0.7853981633974483096156608458198757");
    check_constant_eq<BID_UINT128>("cube_root_2",  "1.2599210498948731647672106072782284");
    check_constant_eq<BID_UINT128>("cube_root_3",  "1.4422495703074083823216383107801096");
}