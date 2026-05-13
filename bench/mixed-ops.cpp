#include <vector>
#include <boost/decimal.hpp>
#include <decimal/bid.hpp>
#include <decimal/fixed.hpp>
#include <decimal/scaled.hpp>

#include "utils.hpp"

#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.hpp>

// Risk limit check: interleaved compare + conditional add + assign.
//
// Simulates accumulating fills against an exposure limit:
//   accumulator += value
//   if accumulator > limit: accumulator = limit
//   result = accumulator
//
// Branch-heavy with data dependencies between iterations. Tests whether
// clean-loop microbench numbers hold under realistic control flow.
// Mixed positive/negative values exercise all sign branches.

namespace bench {

    template<class decimal_t, class make_fn>
    void bench_risk_limit(ankerl::nanobench::Bench& bench, std::string_view name, std::size_t n,
                          make_fn make, decimal_t zero, decimal_t limit) {
        std::vector<decimal_t> values;
        values.reserve(n);
        std::uint64_t seed = 0xcafebabe12345678ULL;
        for (std::size_t i = 0; i < n; ++i) {
            seed ^= seed >> 12, seed ^= seed << 25, seed ^= seed >> 27;
            std::int64_t raw = static_cast<std::int64_t>((seed * 2685821657736338717ULL) % 20'000ULL) - 10'000LL;
            values.push_back(make(raw));
        }
        bench.run(name.data(), [&] {
            decimal_t acc = zero;
            for (std::size_t i = 0; i < n; ++i) {
                acc += values[i];
                if (acc > limit) acc = limit;
            }
            ankerl::nanobench::doNotOptimizeAway(acc);
        });
    }

} // namespace bench

int main() {
    using bid64   = math::decimal_t<std::uint64_t>;
    using fixed64 = math::fixed::decimal_t<std::uint64_t, -4>;
    using scaled  = math::scaled::decimal_t<std::int64_t>;
    using boost64 = boost::decimal::decimal64_t;
    using cat     = math::bid::category;
    constexpr std::size_t N = 10'000;
    constexpr std::uint64_t LIMIT_SIG = 10'000'000'000ULL;  // 1,000,000.0000

    ankerl::nanobench::Bench bench;
    bench.title("risk limit check: accumulate + clamp (10k, mixed pos/neg)").relative(true)
         .minEpochIterations(500).epochs(30);

    bench::bench_risk_limit<boost64>(bench, "boost decimal64", N,
        [](std::int64_t v){
            return v >= 0 ? boost64(static_cast<std::uint64_t>(v), -4)
                          : -boost64(static_cast<std::uint64_t>(-v), -4);
        },
        boost64(0, 0), boost64(LIMIT_SIG, -4));

    bench::bench_risk_limit<double>(bench, "double", N,
        [](std::int64_t v){ return static_cast<double>(v) * 1e-4; },
        0.0, static_cast<double>(LIMIT_SIG) * 1e-4);

    bench::bench_risk_limit<bid64>(bench, "intel bid64", N,
        [](std::int64_t v){
            return v >= 0 ? bid64(static_cast<std::uint64_t>(v), -4)
                          : -bid64(static_cast<std::uint64_t>(-v), -4);
        },
        bid64(0, 0), bid64(LIMIT_SIG, -4));

    bench::bench_risk_limit<fixed64>(bench, "fixed64 -4", N,
        [](std::int64_t v){
            return v >= 0 ? fixed64{cat::positive, static_cast<std::uint64_t>(v)}
                          : fixed64{cat::negative, static_cast<std::uint64_t>(-v)};
        },
        fixed64{cat::zero, 0u}, fixed64{cat::positive, LIMIT_SIG});

    bench::bench_risk_limit<scaled>(bench, "scaled int64", N,
        [](std::int64_t v){ return scaled{v, -4}; },
        scaled{0, 0}, scaled{static_cast<std::int64_t>(LIMIT_SIG), -4});
}
