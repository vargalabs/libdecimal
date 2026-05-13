#include <format>
#include <sstream>
#include <vector>
#include <boost/decimal.hpp>
#include <boost/decimal/cstdlib.hpp>
#include <decimal/bid.hpp>
#include <decimal/bcd.hpp>
#include <decimal/scaled.hpp>

#include "utils.hpp"

#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.hpp>

// Note: fixed::decimal_t has no string constructor (compile-time exponent requires no runtime parsing).
// scaled::decimal_t has no decimal-point string constructor; it accepts (significand, exponent) only.
// Parse benchmarks cover: BID, BCD, Boost.
// Format benchmarks cover: BID (std::formatter), BCD (.str()), scaled (cast), Boost.

namespace bench {

    std::vector<std::string> make_decimal_strings(std::size_t n) {
        std::vector<std::string> v;
        v.reserve(n);
        std::uint64_t seed = 0xabcdef1234567890ULL;
        for (std::size_t i = 0; i < n; ++i) {
            seed ^= seed >> 12, seed ^= seed << 25, seed ^= seed >> 27;
            std::uint64_t integer_part = (seed * 2685821657736338717ULL) % 100'000ULL + 1ULL;
            std::uint64_t frac_part   = (seed >> 13) % 10'000ULL;
            v.push_back(std::to_string(integer_part) + "." + std::string(4 - std::to_string(frac_part).size(), '0') + std::to_string(frac_part));
        }
        return v;
    }

    // BCD parse takes (digit-string, exponent) — strip the decimal point
    std::vector<std::pair<std::string, int>> make_bcd_inputs(std::size_t n) {
        std::vector<std::pair<std::string, int>> v;
        v.reserve(n);
        std::uint64_t seed = 0xabcdef1234567890ULL;
        for (std::size_t i = 0; i < n; ++i) {
            seed ^= seed >> 12, seed ^= seed << 25, seed ^= seed >> 27;
            std::uint64_t integer_part = (seed * 2685821657736338717ULL) % 100'000ULL + 1ULL;
            std::uint64_t frac_part   = (seed >> 13) % 10'000ULL;
            v.push_back({std::to_string(integer_part) + std::string(4 - std::to_string(frac_part).size(), '0') + std::to_string(frac_part), -4});
        }
        return v;
    }

} // namespace bench

int main() {
    using bid64   = math::decimal_t<std::uint64_t>;
    using bcd64   = math::bcd::decimal_t<std::uint64_t>;
    using scaled  = math::scaled::decimal_t<std::int64_t>;
    using boost64 = boost::decimal::decimal64_t;
    constexpr std::size_t N = 1'000;

    const auto str_inputs  = bench::make_decimal_strings(N);
    const auto bcd_inputs  = bench::make_bcd_inputs(N);

    // --- Parse ---
    {
        ankerl::nanobench::Bench bench;
        bench.title("parse decimal string").relative(true).minEpochIterations(2'000).epochs(30);

        bench.run("intel bid64", [&] {
            for (std::size_t i = 0; i < N; ++i) {
                auto v = bid64(str_inputs[i].c_str());
                ankerl::nanobench::doNotOptimizeAway(v);
            }
        });
        bench.run("bcd64 (digit-string + exp)", [&] {
            for (std::size_t i = 0; i < N; ++i) {
                auto v = bcd64(std::string_view(bcd_inputs[i].first), static_cast<std::int16_t>(bcd_inputs[i].second));
                ankerl::nanobench::doNotOptimizeAway(v);
            }
        });
        bench.run("boost decimal64", [&] {
            for (std::size_t i = 0; i < N; ++i) {
                auto v = boost::decimal::strtod64(str_inputs[i].c_str(), nullptr);
                ankerl::nanobench::doNotOptimizeAway(v);
            }
        });
        bench.run("stod (double reference)", [&] {
            for (std::size_t i = 0; i < N; ++i) {
                auto v = std::stod(str_inputs[i]);
                ankerl::nanobench::doNotOptimizeAway(v);
            }
        });
    }

    // --- Format ---
    std::uint64_t seed = 0x9e3779b97f4a7c15ULL;
    std::vector<bid64>   bid_vals;
    std::vector<bcd64>   bcd_vals;
    std::vector<scaled>  scaled_vals;
    std::vector<boost64> boost_vals;
    std::vector<double>  double_vals;
    bid_vals.reserve(N); bcd_vals.reserve(N); scaled_vals.reserve(N); boost_vals.reserve(N); double_vals.reserve(N);
    for (std::size_t i = 0; i < N; ++i) {
        seed ^= seed >> 12, seed ^= seed << 25, seed ^= seed >> 27;
        std::uint64_t v = (seed * 2685821657736338717ULL) % 1'000'000'000ULL + 1ULL;
        bid_vals.push_back(bid64(v, -4));
        bcd_vals.push_back(bcd64{math::bid::category::positive, v, std::int16_t{-4}});
        scaled_vals.push_back(scaled{static_cast<std::int64_t>(v), -4});
        boost_vals.push_back(boost64(v, -4));
        double_vals.push_back(static_cast<double>(v) * 1e-4);
    }

    {
        ankerl::nanobench::Bench bench;
        bench.title("format to string").relative(true).minEpochIterations(2'000).epochs(30);

        bench.run("intel bid64 (std::format)", [&] {
            for (std::size_t i = 0; i < N; ++i) {
                auto s = std::format("{}", bid_vals[i]);
                ankerl::nanobench::doNotOptimizeAway(s);
            }
        });
        bench.run("bcd64 (.str())", [&] {
            for (std::size_t i = 0; i < N; ++i) {
                auto s = bcd_vals[i].str();
                ankerl::nanobench::doNotOptimizeAway(s);
            }
        });
        bench.run("scaled (cast to string)", [&] {
            for (std::size_t i = 0; i < N; ++i) {
                auto s = static_cast<std::string>(scaled_vals[i]);
                ankerl::nanobench::doNotOptimizeAway(s);
            }
        });
        bench.run("boost decimal64", [&] {
            for (std::size_t i = 0; i < N; ++i) {
                std::ostringstream oss;
                oss << boost_vals[i];
                auto s = oss.str();
                ankerl::nanobench::doNotOptimizeAway(s);
            }
        });
        bench.run("double (to_string reference)", [&] {
            for (std::size_t i = 0; i < N; ++i) {
                auto s = std::to_string(double_vals[i]);
                ankerl::nanobench::doNotOptimizeAway(s);
            }
        });
    }
}
