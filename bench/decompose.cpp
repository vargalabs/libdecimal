#include <cstdint>
#include <vector>
#include <boost/decimal.hpp>
#include <boost/decimal/cmath.hpp>
#include <decimal/bid.hpp>
#include <decimal/utils.hpp>
#include <decimal/scaled.hpp>

#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.hpp>

// Decompose benchmark: extract (significand, exponent) from each decimal type.
//
// Cost matters for serialization, logging, and any code path that must inspect
// the internal representation (e.g., risk checks, rounding, wire encoding).
//
// float/double:        math::utils::decompose — frexp-based decimal extraction
// intel BID32/64:      math::bid::decompose  — bit-manipulation on BID layout
// scaled int32/64:     v.as_pair()           — direct struct field access
// boost decimal32/64:  frexp10(v, &exp)      — normalized significand + exponent

namespace bench {

    template<class significand_t>
    std::vector<significand_t> make_uint_input(std::size_t n) {
        std::vector<significand_t> v;
        v.reserve(n);
        std::uint64_t seed = 0x9e3779b97f4a7c15ULL;
        for (std::size_t i = 0; i < n; ++i) {
            seed ^= seed >> 12, seed ^= seed << 25, seed ^= seed >> 27;
            v.push_back(static_cast<significand_t>((seed * 2685821657736338717ULL) % 1'000'000'000ULL + 1ULL));
        }
        return v;
    }

} // namespace bench

int main() {
    using bid32   = math::decimal_t<std::uint32_t>;
    using bid64   = math::decimal_t<std::uint64_t>;
    using scaled32 = math::scaled::decimal_t<std::uint32_t>;
    using scaled64 = math::scaled::decimal_t<std::int64_t>;
    using boost32 = boost::decimal::decimal32_t;
    using boost64 = boost::decimal::decimal64_t;
    constexpr std::size_t N = 10'000;

    const auto raw = bench::make_uint_input<std::uint64_t>(N);

    // Pre-build typed values
    std::vector<float>    float_vals;
    std::vector<double>   double_vals;
    std::vector<bid32>    bid32_vals;
    std::vector<bid64>    bid64_vals;
    std::vector<scaled32> sc32_vals;
    std::vector<scaled64> sc64_vals;
    std::vector<boost32>  boost32_vals;
    std::vector<boost64>  boost64_vals;
    for (std::size_t i = 0; i < N; ++i) {
        float_vals.push_back(static_cast<float>(raw[i]) * 1e-4f);
        double_vals.push_back(static_cast<double>(raw[i]) * 1e-4);
        bid32_vals.push_back(bid32(static_cast<std::uint32_t>(raw[i] % 1'000'000ULL), -4));
        bid64_vals.push_back(bid64(raw[i], -4));
        sc32_vals.push_back(scaled32{static_cast<std::uint32_t>(raw[i] % 1'000'000ULL), std::int16_t{-4}});
        sc64_vals.push_back(scaled64{static_cast<std::int64_t>(raw[i]), std::int16_t{-4}});
        boost32_vals.push_back(boost32(static_cast<std::uint32_t>(raw[i] % 1'000'000ULL), -4));
        boost64_vals.push_back(boost64(raw[i], -4));
    }

    // 32-bit decompose
    {
        ankerl::nanobench::Bench bench;
        bench.title("decompose to significand+exponent (32-bit)").relative(true)
             .minEpochIterations(40'000).epochs(30);

        bench.run("boost decimal32 (frexp10)", [&] {
            for (std::size_t i = 0; i < N; ++i) {
                int exp{};
                auto sig = boost::decimal::frexp10(boost32_vals[i], &exp);
                ankerl::nanobench::doNotOptimizeAway(sig);
                ankerl::nanobench::doNotOptimizeAway(exp);
            }
        });
        bench.run("float (utils::decompose)", [&] {
            for (std::size_t i = 0; i < N; ++i) {
                auto [kind, sig, exp] = math::utils::decompose<float, std::uint32_t, std::int16_t>(float_vals[i]);
                ankerl::nanobench::doNotOptimizeAway(sig);
                ankerl::nanobench::doNotOptimizeAway(exp);
            }
        });
        bench.run("intel bid32 (bid::decompose)", [&] {
            for (std::size_t i = 0; i < N; ++i) {
                auto [kind, sig, exp] = math::bid::decompose(bid32_vals[i].value);
                ankerl::nanobench::doNotOptimizeAway(sig);
                ankerl::nanobench::doNotOptimizeAway(exp);
            }
        });
        bench.run("scaled uint32 (as_pair)", [&] {
            for (std::size_t i = 0; i < N; ++i) {
                auto [sig, exp] = sc32_vals[i].as_pair();
                ankerl::nanobench::doNotOptimizeAway(sig);
                ankerl::nanobench::doNotOptimizeAway(exp);
            }
        });
    }

    // 64-bit decompose
    {
        ankerl::nanobench::Bench bench;
        bench.title("decompose to significand+exponent (64-bit)").relative(true)
             .minEpochIterations(40'000).epochs(30);

        bench.run("boost decimal64 (frexp10)", [&] {
            for (std::size_t i = 0; i < N; ++i) {
                int exp{};
                auto sig = boost::decimal::frexp10(boost64_vals[i], &exp);
                ankerl::nanobench::doNotOptimizeAway(sig);
                ankerl::nanobench::doNotOptimizeAway(exp);
            }
        });
        bench.run("double (utils::decompose)", [&] {
            for (std::size_t i = 0; i < N; ++i) {
                auto [kind, sig, exp] = math::utils::decompose<double, std::uint64_t, std::int16_t>(double_vals[i]);
                ankerl::nanobench::doNotOptimizeAway(sig);
                ankerl::nanobench::doNotOptimizeAway(exp);
            }
        });
        bench.run("intel bid64 (bid::decompose)", [&] {
            for (std::size_t i = 0; i < N; ++i) {
                auto [kind, sig, exp] = math::bid::decompose(bid64_vals[i].value);
                ankerl::nanobench::doNotOptimizeAway(sig);
                ankerl::nanobench::doNotOptimizeAway(exp);
            }
        });
        bench.run("scaled int64 (as_pair)", [&] {
            for (std::size_t i = 0; i < N; ++i) {
                auto [sig, exp] = sc64_vals[i].as_pair();
                ankerl::nanobench::doNotOptimizeAway(sig);
                ankerl::nanobench::doNotOptimizeAway(exp);
            }
        });
    }
}
