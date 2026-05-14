#include <cstdint>
#include <vector>
#include <decimal/bid.hpp>
#include <decimal/fixed.hpp>
#include <decimal/scaled.hpp>

#include "utils.hpp"

#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.hpp>

// Wire-format encode/decode benchmark.
//
// Low-latency transports serialize decimals as (int64 significand, int16 exponent) pairs.
// This benchmark measures the full round-trip cost: value → wire pair → value.
//
// For scaled::decimal_t: encode is struct field access (near-free).
//                        decode is direct construction (near-free).
// For fixed::decimal_t:  encode is struct field access + compile-time exponent.
//                        decode is direct construction.
// For BID decimal_t:     encode is bid::decompose() — bit manipulation.
//                        decode is uint64_to_bid64() — bit packing.
//
// The question answered: can scaled replace custom fixed-width wire encoders?

namespace bench {

    struct wire_t {
        std::int64_t  significand;
        std::int16_t  exponent;
    };

    template<class significand_t>
    std::vector<significand_t> make_uint_input(std::size_t n) {
        std::vector<significand_t> v;
        v.reserve(n);
        std::uint64_t seed = 0x5678abcd1234ef90ULL;
        for (std::size_t i = 0; i < n; ++i) {
            seed ^= seed >> 12, seed ^= seed << 25, seed ^= seed >> 27;
            v.push_back(static_cast<significand_t>((seed * 2685821657736338717ULL) % 1'000'000'000ULL + 1ULL));
        }
        return v;
    }

} // namespace bench

int main() {
    using bid64   = math::decimal_t<std::uint64_t>;
    using fixed64 = math::fixed::decimal_t<std::uint64_t, -4>;
    using scaled  = math::scaled::decimal_t<std::int64_t>;
    using cat     = math::bid::category;
    constexpr std::size_t N = 10'000;

    const auto raw = bench::make_uint_input<std::uint64_t>(N);

    // Pre-build typed values
    std::vector<bid64>   bid_vals;
    std::vector<fixed64> fixed_vals;
    std::vector<scaled>  scaled_vals;
    bid_vals.reserve(N); fixed_vals.reserve(N); scaled_vals.reserve(N);
    for (std::size_t i = 0; i < N; ++i) {
        bid_vals.push_back(bid64(raw[i], -4));
        fixed_vals.push_back(fixed64{cat::positive, static_cast<std::uint64_t>(raw[i])});
        scaled_vals.push_back(scaled{static_cast<std::int64_t>(raw[i]), -4});
    }

    {
        ankerl::nanobench::Bench bench;
        bench.title("encode to wire pair (significand + exponent)").relative(true)
             .minEpochIterations(5'000).epochs(30);

        bench.run("scaled int64 (field access)", [&] {
            for (std::size_t i = 0; i < N; ++i) {
                bench::wire_t w{scaled_vals[i].significand, scaled_vals[i].exponent};
                ankerl::nanobench::doNotOptimizeAway(w);
            }
        });
        bench.run("fixed64 -4 (field + constexpr exp)", [&] {
            for (std::size_t i = 0; i < N; ++i) {
                bench::wire_t w{static_cast<std::int64_t>(fixed_vals[i].significand), std::int16_t{fixed64::exponent_v}};
                ankerl::nanobench::doNotOptimizeAway(w);
            }
        });
        bench.run("intel bid64 (decompose)", [&] {
            for (std::size_t i = 0; i < N; ++i) {
                auto [kind, sig, exp] = math::bid::decompose(bid_vals[i].value);
                bench::wire_t w{static_cast<std::int64_t>(sig), exp};
                ankerl::nanobench::doNotOptimizeAway(w);
            }
        });
    }
    {
        // Pre-build wire pairs
        std::vector<bench::wire_t> wire;
        wire.reserve(N);
        for (std::size_t i = 0; i < N; ++i)
            wire.push_back({static_cast<std::int64_t>(raw[i]), -4});

        ankerl::nanobench::Bench bench;
        bench.title("decode from wire pair").relative(true)
             .minEpochIterations(5'000).epochs(30);

        bench.run("scaled int64 (direct ctor)", [&] {
            for (std::size_t i = 0; i < N; ++i) {
                scaled v{wire[i].significand, wire[i].exponent};
                ankerl::nanobench::doNotOptimizeAway(v);
            }
        });
        bench.run("fixed64 -4 (direct ctor)", [&] {
            for (std::size_t i = 0; i < N; ++i) {
                fixed64 v{cat::positive, static_cast<std::uint64_t>(wire[i].significand)};
                ankerl::nanobench::doNotOptimizeAway(v);
            }
        });
        bench.run("intel bid64 (encode from pair)", [&] {
            for (std::size_t i = 0; i < N; ++i) {
                bid64 v{static_cast<std::uint64_t>(wire[i].significand), static_cast<int>(wire[i].exponent)};
                ankerl::nanobench::doNotOptimizeAway(v);
            }
        });
    }
}
