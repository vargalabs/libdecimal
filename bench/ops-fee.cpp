#include <vector>
#include <boost/decimal.hpp>
#include <decimal/bid.hpp>
#include <decimal/fixed.hpp>
#include <decimal/scaled.hpp>

#include "utils.hpp"

#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.hpp>

// Fee calculation benchmark: fee = notional * rate
//
// In trading: fee = fill_quantity * fill_price * commission_rate
// All values are at 4 decimal places.
// The key question: which type is fastest for the multiply hot path?
//
// Note on rounding: BID uses Intel LIBBID round-to-nearest-even by default.
// fixed::decimal_t truncates to integer when mul overflows the exponent range.
// scaled::decimal_t truncates via integer arithmetic.

namespace bench {

    template<class significand_t>
    std::vector<significand_t> make_uint_input(std::size_t n, std::uint64_t mod = 1'000'000ULL) {
        std::vector<significand_t> v;
        v.reserve(n);
        std::uint64_t seed = 0x1234567890abcdefULL;
        for (std::size_t i = 0; i < n; ++i) {
            seed ^= seed >> 12, seed ^= seed << 25, seed ^= seed >> 27;
            v.push_back(static_cast<significand_t>((seed * 2685821657736338717ULL) % mod + 1ULL));
        }
        return v;
    }

    template<class decimal_t, class make_fn>
    void bench_fee_calc(ankerl::nanobench::Bench& bench, std::string_view name, std::size_t n,
                        make_fn make_notional, make_fn make_rate) {
        const auto notional_raw = make_uint_input<std::uint64_t>(n, 10'000'000ULL);
        const auto rate_raw     = make_uint_input<std::uint64_t>(n, 1'000ULL);
        std::vector<decimal_t> notionals, rates;
        notionals.reserve(n); rates.reserve(n);
        for (std::size_t i = 0; i < n; ++i) {
            notionals.push_back(make_notional(notional_raw[i]));
            rates.push_back(make_rate(rate_raw[i]));
        }
        bench.run(name.data(), [&] {
            for (std::size_t i = 0; i < n; ++i) {
                auto fee = notionals[i] * rates[i];
                ankerl::nanobench::doNotOptimizeAway(fee);
            }
        });
    }

} // namespace bench

int main() {
    using bid64   = math::decimal_t<std::uint64_t>;
    using fixed64 = math::fixed::decimal_t<std::uint64_t, -4>;
    using scaled  = math::scaled::decimal_t<std::int64_t>;
    using boost64 = boost::decimal::decimal64_t;
    using cat     = math::bid::category;
    constexpr std::size_t N = 1'000;

    auto make_bid64   = [](std::uint64_t v){ return bid64(v, -4); };
    auto make_fixed64 = [](std::uint64_t v){ return fixed64{cat::positive, v}; };
    auto make_scaled  = [](std::uint64_t v){ return scaled{static_cast<std::int64_t>(v), -4}; };
    auto make_boost64 = [](std::uint64_t v){ return boost64(v, -4); };
    auto make_double  = [](std::uint64_t v){ return static_cast<double>(v) * 1e-4; };

    {
        ankerl::nanobench::Bench bench;
        bench.title("fee = notional * rate (multiply 1k pairs)").relative(true)
             .minEpochIterations(10'000).epochs(30);
        bench::bench_fee_calc<double> (bench, "double",          N, make_double,  make_double);
        bench::bench_fee_calc<bid64>  (bench, "intel bid64",     N, make_bid64,   make_bid64);
        bench::bench_fee_calc<fixed64>(bench, "fixed64 -4",      N, make_fixed64, make_fixed64);
        bench::bench_fee_calc<scaled> (bench, "scaled int64",    N, make_scaled,  make_scaled);
        bench::bench_fee_calc<boost64>(bench, "boost decimal64", N, make_boost64, make_boost64);
    }
}
