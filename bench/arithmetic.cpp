#include <vector>
#include <boost/decimal.hpp>
#include <decimal/bid.hpp>
#include <decimal/bcd.hpp>
#include <decimal/fixed.hpp>
#include <decimal/scaled.hpp>

#include "utils.hpp"

#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.hpp>

namespace bench {

    template<class significand_t>
    std::vector<significand_t> make_int_input(std::size_t n) {
        std::vector<significand_t> v;
        v.reserve(n);
        std::uint64_t seed = 0x9e3779b97f4a7c15ULL;
        for (std::size_t i = 0; i < n; ++i) {
            seed ^= seed >> 12, seed ^= seed << 25, seed ^= seed >> 27;
            v.push_back(static_cast<significand_t>((seed * 2685821657736338717ULL) % 1'000'000ULL + 1ULL));
        }
        return v;
    }

    // Accumulator: sum all N values
    template<class decimal_t, class make_fn>
    void bench_accumulate(ankerl::nanobench::Bench& bench, std::string_view name, std::size_t n, make_fn make, decimal_t zero) {
        const auto raw = make_int_input<std::uint64_t>(n);
        std::vector<decimal_t> values;
        values.reserve(n);
        for (std::size_t i = 0; i < n; ++i) values.push_back(make(raw[i]));
        bench.run(name.data(), [&] {
            decimal_t acc = zero;
            for (std::size_t i = 0; i < n; ++i) acc += values[i];
            ankerl::nanobench::doNotOptimizeAway(acc);
        });
    }

    // Dot product: sum of price * quantity pairs (N pairs)
    template<class decimal_t, class make_fn>
    void bench_dot_product(ankerl::nanobench::Bench& bench, std::string_view name, std::size_t n, make_fn make_price, make_fn make_qty, decimal_t zero) {
        const auto prices = make_int_input<std::uint64_t>(n);
        const auto qtys   = make_int_input<std::uint64_t>(n);
        std::vector<decimal_t> pv, qv;
        pv.reserve(n); qv.reserve(n);
        for (std::size_t i = 0; i < n; ++i) {
            pv.push_back(make_price(prices[i]));
            qv.push_back(make_qty(qtys[i]));
        }
        bench.run(name.data(), [&] {
            decimal_t acc = zero;
            for (std::size_t i = 0; i < n; ++i) acc += pv[i] * qv[i];
            ankerl::nanobench::doNotOptimizeAway(acc);
        });
    }

} // namespace bench

int main() {
    using bid64   = math::decimal_t<std::uint64_t>;
    using bcd64   = math::bcd::decimal_t<std::uint64_t>;
    using fixed64 = math::fixed::decimal_t<std::uint64_t, -4>;
    using scaled  = math::scaled::decimal_t<std::int64_t>;
    using boost64 = boost::decimal::decimal64_t;
    using cat     = math::bid::category;

    auto make_bid64   = [](std::uint64_t v){ return bid64(v, -4); };
    auto make_bcd64   = [](std::uint64_t v){ return bcd64{cat::positive, v, std::int16_t{-4}}; };
    auto make_fixed64 = [](std::uint64_t v){ return fixed64{cat::positive, v}; };
    auto make_scaled  = [](std::uint64_t v){ return scaled{static_cast<std::int64_t>(v), -4}; };
    auto make_boost64 = [](std::uint64_t v){ return boost64(v, -4); };
    auto make_double  = [](std::uint64_t v){ return static_cast<double>(v) * 1e-4; };

    constexpr std::size_t N_acc  = 10'000;
    constexpr std::size_t N_dot  = 256;

    {
        ankerl::nanobench::Bench bench;
        bench.title("accumulate sum (10k values)").relative(true).minEpochIterations(500).epochs(30);
        bench::bench_accumulate<boost64>(bench, "boost decimal64", N_acc, make_boost64, boost64(0, 0));
        bench::bench_accumulate<double> (bench, "double",          N_acc, make_double,  0.0);
        bench::bench_accumulate<bid64>  (bench, "intel bid64",     N_acc, make_bid64,   bid64(0, 0));
        bench::bench_accumulate<bcd64>  (bench, "bcd64",           N_acc, make_bcd64,   bcd64{0ull});
        bench::bench_accumulate<fixed64>(bench, "fixed64 -4",      N_acc, make_fixed64, fixed64{cat::zero, 0u});
        bench::bench_accumulate<scaled> (bench, "scaled int64",    N_acc, make_scaled,  scaled{0, 0});
    }
    {
        ankerl::nanobench::Bench bench;
        bench.title("dot product price*qty (256 pairs)").relative(true).minEpochIterations(2'000).epochs(30);
        bench::bench_dot_product<boost64>(bench, "boost decimal64", N_dot, make_boost64, make_boost64, boost64(0, 0));
        bench::bench_dot_product<double> (bench, "double",          N_dot, make_double,  make_double,  0.0);
        bench::bench_dot_product<bid64>  (bench, "intel bid64",     N_dot, make_bid64,   make_bid64,   bid64(0, 0));
        bench::bench_dot_product<bcd64>  (bench, "bcd64",           N_dot, make_bcd64,   make_bcd64,   bcd64{0ull});
        bench::bench_dot_product<fixed64>(bench, "fixed64 -4",      N_dot, make_fixed64, make_fixed64, fixed64{cat::zero, 0u});
        bench::bench_dot_product<scaled> (bench, "scaled int64",    N_dot, make_scaled,  make_scaled,  scaled{0, 0});
    }
}
