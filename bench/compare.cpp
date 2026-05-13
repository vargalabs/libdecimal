#include <algorithm>
#include <vector>
#include <boost/decimal.hpp>
#include <decimal/bid.hpp>
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
        std::uint64_t seed = 0xdeadbeefcafe1234ULL;
        for (std::size_t i = 0; i < n; ++i) {
            seed ^= seed >> 12, seed ^= seed << 25, seed ^= seed >> 27;
            v.push_back(static_cast<significand_t>((seed * 2685821657736338717ULL) % 1'000'000'000ULL + 1ULL));
        }
        return v;
    }

    template<class decimal_t, class make_fn>
    void bench_compare_micro(ankerl::nanobench::Bench& bench, std::string_view name, std::size_t n, make_fn make) {
        const auto raw = make_int_input<std::uint64_t>(n);
        std::vector<decimal_t> values;
        values.reserve(n);
        for (std::size_t i = 0; i < n; ++i) values.push_back(make(raw[i]));
        std::size_t count = 0;
        bench.run(name.data(), [&] {
            for (std::size_t i = 0; i + 1 < n; ++i)
                if (values[i] < values[i + 1]) ++count;
            ankerl::nanobench::doNotOptimizeAway(count);
        });
    }

    template<class decimal_t, class make_fn>
    void bench_sort(ankerl::nanobench::Bench& bench, std::string_view name, std::size_t n, make_fn make) {
        const auto raw = make_int_input<std::uint64_t>(n);
        std::vector<decimal_t> values;
        values.reserve(n);
        for (std::size_t i = 0; i < n; ++i) values.push_back(make(raw[i]));
        bench.run(name.data(), [&] {
            std::vector<decimal_t> tmp = values;
            std::sort(tmp.begin(), tmp.end());
            ankerl::nanobench::doNotOptimizeAway(tmp.front());
        });
    }

} // namespace bench

int main() {
    using bid64   = math::decimal_t<std::uint64_t>;
    using fixed64 = math::fixed::decimal_t<std::uint64_t, -4>;
    using scaled  = math::scaled::decimal_t<std::int64_t>;
    using boost64 = boost::decimal::decimal64_t;
    constexpr std::size_t N_micro = 10'000;
    constexpr std::size_t N_sort  = 1'000;

    {
        ankerl::nanobench::Bench bench;
        bench.title("compare <  (micro, 10k pairs)").relative(true).minEpochIterations(5'000).epochs(30);
        bench::bench_compare_micro<double> (bench, "double",         N_micro, [](std::uint64_t v){ return static_cast<double>(v) * 1e-4; });
        bench::bench_compare_micro<bid64>  (bench, "intel bid64",    N_micro, [](std::uint64_t v){ return bid64(v, -4); });
        bench::bench_compare_micro<fixed64>(bench, "fixed64 -4",     N_micro, [](std::uint64_t v){ return fixed64{math::bid::category::positive, static_cast<std::uint64_t>(v)}; });
        bench::bench_compare_micro<scaled> (bench, "scaled int64",   N_micro, [](std::uint64_t v){ return scaled{static_cast<std::int64_t>(v), -4}; });
        bench::bench_compare_micro<boost64>(bench, "boost decimal64",N_micro, [](std::uint64_t v){ return boost64(v, -4); });
    }
    {
        ankerl::nanobench::Bench bench;
        bench.title("sort 1000 values").relative(true).minEpochIterations(500).epochs(30);
        bench::bench_sort<double> (bench, "double",         N_sort, [](std::uint64_t v){ return static_cast<double>(v) * 1e-4; });
        bench::bench_sort<bid64>  (bench, "intel bid64",    N_sort, [](std::uint64_t v){ return bid64(v, -4); });
        bench::bench_sort<fixed64>(bench, "fixed64 -4",     N_sort, [](std::uint64_t v){ return fixed64{math::bid::category::positive, static_cast<std::uint64_t>(v)}; });
        bench::bench_sort<scaled> (bench, "scaled int64",   N_sort, [](std::uint64_t v){ return scaled{static_cast<std::int64_t>(v), -4}; });
        bench::bench_sort<boost64>(bench, "boost decimal64",N_sort, [](std::uint64_t v){ return boost64(v, -4); });
    }
}
