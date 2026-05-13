#include <boost/decimal.hpp>
#include <decimal/bid.hpp>
#include <decimal/utils.hpp>

#include "traits.hpp"
#include "utils.hpp"

#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.hpp>

namespace bench {
    using import_32_t = std::tuple<
        traits_t<boost::decimal::decimal32_t>,
        bench::ieee754<float, bench::kind_t::naive>,
        bench::ieee754<float, bench::kind_t::optimised>,
        traits_t<math::decimal_t<std::uint32_t>>,
        traits_t<math::scaled::decimal_t<std::uint32_t>>,
        traits_t<math::bcd::decimal_t<std::uint32_t>>,
        bench::fixed<math::fixed::decimal_t<std::uint32_t, -4>, bench::kind_t::naive>,
        bench::fixed<math::fixed::decimal_t<std::uint32_t, -4>, bench::kind_t::optimised>
    >;

    using import_64_t = std::tuple<
        traits_t<boost::decimal::decimal64_t>,
        bench::ieee754<double, bench::kind_t::naive>,
        bench::ieee754<double, bench::kind_t::optimised>,
        traits_t<math::decimal_t<std::uint64_t>>,
        traits_t<math::scaled::decimal_t<std::uint64_t>>,
        traits_t<math::bcd::decimal_t<std::uint64_t>>,
        bench::fixed<math::fixed::decimal_t<std::uint64_t, -4>, bench::kind_t::naive>,
        bench::fixed<math::fixed::decimal_t<std::uint64_t, -4>, bench::kind_t::optimised>
    >;

    using type_list = std::tuple<import_32_t, import_64_t>;


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

    template<class trait_t>
    void run_import_case(ankerl::nanobench::Bench& bench, std::size_t n) {
        using significand_t = typename trait_t::significand_t;
        const auto input = make_input<significand_t>(n);
        bench.run(trait_t::name().data(), [&] {
            for (std::size_t i = 0; i < n; ++i) {
                auto x = trait_t::make(input[i].significand, input[i].exponent);
                ankerl::nanobench::doNotOptimizeAway(x);
            }
        });
    }

    template<class group> void run_import_group(std::string_view title, std::size_t n) {
        ankerl::nanobench::Bench bench;
        bench.title(title.data()).relative(true)
             .minEpochIterations(50'000).epochs(30);
        static_for<group>([&]<class element_t>() {
            run_import_case<element_t>(bench, n);
        });
    }

} // namespace bench

int main() {
    constexpr std::size_t n = 1'000;

    bench::run_import_group<bench::import_32_t>("construct from significand+exponent (32-bit)", n);
    bench::run_import_group<bench::import_64_t>("construct from significand+exponent (64-bit)", n);
}
