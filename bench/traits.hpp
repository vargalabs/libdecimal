#pragma once

#include <cstdint>
#include <cmath>
#include <string_view>
#include <cstdint>
#include <decimal/bid.hpp>
#include <decimal/utils.hpp>
#include <decimal/scaled.hpp>
#include <decimal/bcd.hpp>
#include <decimal/fixed.hpp>
#include <boost/decimal.hpp>

namespace bench {
    enum class kind_t { naive, optimised };
    template <class value_t, kind_t kind> struct ieee754;
    template<class value_t, kind_t kind> struct fixed;

    using category = math::bid::category;
    template<class value_t> constexpr value_t pow10(int exponent) {
        value_t value = static_cast<value_t>(1);
        if (exponent >= 0)
            for (int i = 0; i < exponent; ++i) value *= static_cast<value_t>(10);
        else for (int i = 0; i < -exponent; ++i) value /= static_cast<value_t>(10);
        return value;
    }

    template<class element_t> struct traits_t;
    template<> struct traits_t<math::decimal_t<std::uint32_t>> {
        using element_t = math::decimal_t<std::uint32_t>;
        using significand_t = std::uint32_t;
        using exponent_t = std::int16_t;
        static constexpr std::string_view name() { return "intel decimal32"; }
        static element_t make(significand_t significand, int exponent) {
            return element_t(significand, exponent);
        }
    };
    template<> struct traits_t<math::decimal_t<std::uint64_t>> {
        using element_t = math::decimal_t<std::uint64_t>;
        using significand_t = std::uint64_t;
        using exponent_t = std::uint16_t;
        static constexpr std::string_view name() { return "intel decimal64"; }
        static element_t make(significand_t significand, int exponent) {
            return element_t(significand, exponent);
        }
    };
    template<> struct traits_t<math::decimal_t<math::uint128_t>> {
        using element_t = math::decimal_t<math::uint128_t>;
        using significand_t = math::uint128_t;
        using exponent_t = std::int16_t;
        static constexpr std::string_view name() { return "intel decimal128"; }
        static element_t make(significand_t significand, int exponent) {
            return element_t(significand, exponent);
        }
    };
    template<> struct traits_t<boost::decimal::decimal32_t> {
        using element_t = boost::decimal::decimal32_t;
        using significand_t = std::uint32_t;
        using exponent_t = std::int16_t;
        static constexpr std::string_view name() { return "boost decimal32"; }
        static element_t make(significand_t significand, int exponent) {
            return element_t(significand, exponent);
        }
    };
    template<> struct traits_t<boost::decimal::decimal64_t> {
        using element_t = boost::decimal::decimal64_t;
        using significand_t = std::uint64_t;
        using exponent_t = std::int16_t;
        static constexpr std::string_view name() { return "boost decimal64"; }
        static element_t make(significand_t significand, int exponent) {
            return element_t(significand, exponent);
        }
    };
    template<>  struct traits_t<boost::decimal::decimal128_t> {
        using element_t = boost::decimal::decimal128_t;
        using significand_t = __uint128_t;
        using exponent_t = std::int16_t;
        static constexpr std::string_view name() { return "boost decimal128"; }
        static element_t make(significand_t significand, int exponent) {
            return element_t(significand, exponent);
        }
    };
    template<> struct traits_t<float> {
        using element_t = float;
        using significand_t = std::uint32_t;
        using exponent_t = std::int16_t;
        static constexpr std::string_view name() { return "float"; }
        static element_t make(significand_t significand, int exponent) {
            return static_cast<element_t>(significand) * std::pow(static_cast<element_t>(10), static_cast<element_t>(exponent));
        }
        inline std::tuple<category, significand_t, exponent_t> decompose(element_t value) {
            return math::utils::decompose<element_t,significand_t, exponent_t>(value);
        }
    };
    template<> struct traits_t<double> {
        using element_t = double;
        using significand_t = std::uint64_t;
        using exponent_t = std::int16_t;
        static constexpr std::string_view name() { return "double"; }
        static element_t make(significand_t significand, int exponent) {
            return math::utils::make<element_t>(significand, exponent);
        }
        inline std::tuple<category, significand_t, exponent_t> decompose(element_t value) {
            return math::utils::decompose<element_t,significand_t, exponent_t>(value);
        }
    };
    template <> struct ieee754<float, kind_t::naive> {
        using element_t = float;
        using category_t = math::bid::category;
        using significand_t = std::uint32_t;
        using exponent_t = std::int16_t;
        static constexpr std::string_view name() { return "float naive"; }
        static element_t make(significand_t significand, int exponent) {
            return static_cast<element_t>(significand) *
                   std::pow(static_cast<element_t>(10), static_cast<element_t>(exponent));
        }
        static std::tuple<category_t, significand_t, exponent_t> decompose(element_t value) {
            return math::utils::decompose<element_t, significand_t, exponent_t>(value);
        }
    };
    template <> struct ieee754<float, kind_t::optimised> {
        using element_t = float;
        using category_t = math::bid::category;
        using significand_t = std::uint32_t;
        using exponent_t = std::int16_t;
        static constexpr std::string_view name() { return "float optimised"; }
        static element_t make(significand_t significand, int exponent) {
            return math::utils::make<element_t>(significand, exponent);
        }
        static std::tuple<category_t, significand_t, exponent_t> decompose(element_t value) {
            return math::utils::decompose<element_t, significand_t, exponent_t>(value);
        }
    };
    template <> struct ieee754<double, kind_t::naive> {
        using element_t = double;
        using category_t = math::bid::category;
        using significand_t = std::uint64_t;
        using exponent_t = std::int16_t;
        static constexpr std::string_view name() { return "double naive"; }
        static element_t make(significand_t significand, int exponent) {
            return static_cast<element_t>(significand) *
                   std::pow(static_cast<element_t>(10), static_cast<element_t>(exponent));
        }
        static std::tuple<category_t, significand_t, exponent_t> decompose(element_t value) {
            return math::utils::decompose<element_t, significand_t, exponent_t>(value);
        }
    };
    template <> struct ieee754<double, kind_t::optimised> {
        using element_t = double;
        using category_t = math::bid::category;
        using significand_t = std::uint64_t;
        using exponent_t = std::int16_t;
        static constexpr std::string_view name() { return "double optimised"; }
        static element_t make(significand_t significand, int exponent) {
            return math::utils::make<element_t>(significand, exponent);
        }
        static std::tuple<category_t, significand_t, exponent_t> decompose(element_t value) {
            return math::utils::decompose<element_t, significand_t, exponent_t>(value);
        }
    };
    template<> struct traits_t<math::scaled::decimal_t<std::uint32_t>> {
        using element_t = math::scaled::decimal_t<std::uint32_t>;
        using significand_t = std::uint32_t;
        using exponent_t = std::int16_t;
        static constexpr std::string_view name() { return "scaled pair32"; }
        static element_t make(significand_t significand, int exponent) {
            return element_t{significand, static_cast<typename element_t::exponent_type_t>(exponent)};
        }
    };
    template<> struct traits_t<math::scaled::decimal_t<std::uint64_t>> {
        using element_t = math::scaled::decimal_t<std::uint64_t>;
        using significand_t = std::uint64_t;
        using exponent_t = std::int16_t;
        static constexpr std::string_view name() { return "scaled pair64"; }
        static element_t make(significand_t significand, int exponent) {
            return element_t{significand, static_cast<typename element_t::exponent_type_t>(exponent)};
        }
        static std::pair<significand_t, exponent_t> decompose(element_t const& value) {
            return {value.significand, value.exponent};
        }
    };

    template<> struct traits_t<math::bcd::decimal_t<std::uint32_t>> {
        using element_t = math::bcd::decimal_t<std::uint32_t>;
        using category_t = math::bid::category;
        using significand_t = std::uint32_t;
        using exponent_t = std::int16_t;
        static constexpr std::string_view name() { return "bcd pair32"; }
        static element_t make(significand_t significand, int exponent) {
            return element_t{category_t::positive, significand, static_cast<typename element_t::exponent_t>(exponent)};
        }
        static std::tuple<category_t, significand_t, exponent_t> decompose(element_t const& value) {
            return {value.kind, value.significand, value.exponent};
        }
        static std::uint64_t checksum(element_t const& x) {
            return math::bcd::checksum(x);
        }
    };

    template<> struct traits_t<math::bcd::decimal_t<std::uint64_t>> {
        using element_t = math::bcd::decimal_t<std::uint64_t>;
        using category_t = math::bid::category;
        using significand_t = std::uint64_t;
        using exponent_t = std::int16_t;
        static constexpr std::string_view name() { return "bcd pair64"; }
        static element_t make(significand_t significand, int exponent) {
            return element_t{category_t::positive, significand, static_cast<typename element_t::exponent_t>(exponent)};
        }
        static std::tuple<category_t, significand_t, exponent_t> decompose(element_t const& value) {
            return {value.kind, value.significand, value.exponent};
        }
        static std::uint64_t checksum(element_t const& x) {
            return math::bcd::checksum(x);
        }
    };

    template<> struct traits_t<math::bcd::decimal_t<math::bid::uint128_t>> {
        using element_t = math::bcd::decimal_t<math::bid::uint128_t>;
        using category_t = math::bid::category;
        using significand_t = math::bid::uint128_t;
        using exponent_t = std::int16_t;
        static constexpr std::string_view name() { return "bcd pair128"; }
        static element_t make(significand_t significand, int exponent) {
            return element_t{category_t::positive, significand, static_cast<typename element_t::exponent_t>(exponent)};
        }
        static std::tuple<category_t, significand_t, exponent_t> decompose(element_t const& value) {
            return {value.kind, value.significand, value.exponent};
        }
        static std::uint64_t checksum(element_t const& x) {
            return math::bcd::checksum(x);
        }
    };
    template<> struct fixed<math::fixed::decimal_t<std::uint32_t, -4>, kind_t::naive> {
        using element_t = math::fixed::decimal_t<std::uint32_t, -4>;
        using category_t = math::bid::category;
        using significand_t = std::uint32_t;
        using exponent_t = int;
        static constexpr std::string_view name() { return "fixed32 checked"; }
        static element_t make(significand_t significand, int exponent) {
            if (exponent != element_t::exponent_v) throw std::domain_error("fixed32: exponent mismatch");
            return element_t{category_t::positive, significand};
        }
        static std::tuple<category_t, significand_t, exponent_t> decompose(element_t const& value) {
            return value.decompose();
        }
        static std::uint64_t checksum(element_t const& x) {
            return math::fixed::checksum(x);
        }
    };
    template<> struct fixed<math::fixed::decimal_t<std::uint32_t, -4>, kind_t::optimised> {
        using element_t = math::fixed::decimal_t<std::uint32_t, -4>;
        using category_t = math::bid::category;
        using significand_t = std::uint32_t;
        using exponent_t = int;
        static constexpr std::string_view name() { return "fixed32 unchecked"; }
        static element_t make(significand_t significand, int) {
            return element_t{category_t::positive, significand};
        }
        static std::tuple<category_t, significand_t, exponent_t> decompose(element_t const& value) {
            return value.decompose();
        }
        static std::uint64_t checksum(element_t const& x) {
            return math::fixed::checksum(x);
        }
    };
    template<> struct fixed<math::fixed::decimal_t<std::uint64_t, -4>, kind_t::naive> {
        using element_t = math::fixed::decimal_t<std::uint64_t, -4>;
        using category_t = math::bid::category;
        using significand_t = std::uint64_t;
        using exponent_t = int;
        static constexpr std::string_view name() { return "fixed64 checked"; }
        static element_t make(significand_t significand, int exponent) {
            if (exponent != element_t::exponent_v) throw std::domain_error("fixed64: exponent mismatch");
            return element_t{category_t::positive, significand};
        }
        static std::tuple<category_t, significand_t, exponent_t> decompose(element_t const& value) {
            return value.decompose();
        }
        static std::uint64_t checksum(element_t const& x) {
            return math::fixed::checksum(x);
        }
    };

    template<> struct fixed<math::fixed::decimal_t<std::uint64_t, -4>, kind_t::optimised> {
        using element_t = math::fixed::decimal_t<std::uint64_t, -4>;
        using category_t = math::bid::category;
        using significand_t = std::uint64_t;
        using exponent_t = int;
        static constexpr std::string_view name() { return "fixed64 unchecked"; }
        static element_t make(significand_t significand, int) {
            return element_t{category_t::positive, significand};
        }
        static std::tuple<category_t, significand_t, exponent_t> decompose(element_t const& value) {
            return value.decompose();
        }
        static std::uint64_t checksum(element_t const& x) {
            return math::fixed::checksum(x);
        }
    };
} // namespace bench
