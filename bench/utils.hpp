#pragma once

#include <cstddef>
#include <tuple>
#include <utility>

namespace bench {

    template<class tuple_t, class fn_t, std::size_t... index>
    constexpr void static_for_impl(fn_t&& fn, std::index_sequence<index...>) {
        (fn.template operator()<std::tuple_element_t<index, tuple_t>>(), ...);
    }

    template<class tuple_t, class fn_t>
    constexpr void static_for(fn_t&& fn) {
        static_for_impl<tuple_t>(
            std::forward<fn_t>(fn), std::make_index_sequence<std::tuple_size_v<tuple_t>>{}
        );
    }
} // namespace bench
