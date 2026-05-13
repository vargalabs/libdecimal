#include <boost/decimal.hpp>
#include <decimal/bid.hpp>

#include "traits.hpp"
#include "utils.hpp"

#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.hpp>

namespace bench {

    using decompose_32_t = std::tuple<float,  math::decimal_t<std::uint32_t>,  math::scaled::decimal_t<std::uint32_t>, boost::decimal::decimal32_t>;
    using decompose_64_t = std::tuple<double, math::decimal_t<std::uint64_t>, math::scaled::decimal_t<std::uint64_t>, boost::decimal::decimal64_t>;
    using type_list = std::tuple<decompose_32_t, decompose_64_t>;


    template<class significand_t> struct input_t {
        significand_t significand;
        int exponent;
    };

    template<class significand_t> std::vector<input_t<significand_t>> make_input(std::size_t n, int exponent = -4) {
        std::vector<input_t<significand_t>> v;
        v.reserve(n);
        std::uint64_t seed = 0x9e3779b97f4a7c15ULL;
        for (std::size_t i = 0; i < n; ++i) {
            seed ^= seed >> 12, seed ^= seed << 25, seed ^= seed >> 27;
            std::uint64_t raw = (seed * 2685821657736338717ULL) % 1'000'000'000ULL + 1ULL;
            v.push_back({static_cast<significand_t>(raw), exponent });
        }
        return v;
    }

    template<class element_t>
    void run_decompose_case(ankerl::nanobench::Bench& bench, std::size_t n) {
        using traits = traits_t<element_t>;
        using significand = typename traits::significand_t;
        const auto input = make_input<significand>(n);

        std::vector<element_t> values;
        values.reserve(n);
        for (std::size_t i = 0; i < n; ++i)
            values.push_back(traits::make(input[i].significand, input[i].exponent));

        bench.run(traits::name().data(), [&] {
            for (std::size_t i = 0; i < n; ++i) {
                auto x = traits::decompose(values[i]);
                ankerl::nanobench::doNotOptimizeAway(x.significand);
                ankerl::nanobench::doNotOptimizeAway(x.exponent);
            }
        });
    }

    template<class group> void run_decompose_group(std::string_view title, std::size_t n) {
        ankerl::nanobench::Bench bench;
        bench.title(title.data()).relative(true)
             .minEpochIterations(40'000).epochs(30);
        static_for<group>([&]<class element_t>() {
            run_decompose_case<element_t>(bench, n);
        });
    }

} // namespace bench

int main() {
    constexpr std::size_t n = 1'000;

    bench::run_decompose_group<bench::decompose_32_t>("decompose to significand+exponent (32-bit)", n);
    bench::run_decompose_group<bench::decompose_64_t>("decompose to significand+exponent (64-bit)", n);
}
