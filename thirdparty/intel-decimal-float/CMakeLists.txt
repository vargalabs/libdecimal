# This file is part of the libdecimal project and is licensed under the MIT License.
#
# Copyright © 2025–2026 Varga Labs, Toronto, ON, Canada 🇨🇦
# Contact: info@vargalabs.com

set(name bid)
set(libname lib${name}_static)

project(${libname} LANGUAGES C)

set(bid_root "${CMAKE_CURRENT_SOURCE_DIR}/v2.2.0")
set(src_dir   "${bid_root}/src")
set(f128_dir  "${bid_root}/float128")

file(GLOB bid_core_sources CONFIGURE_DEPENDS
    "${src_dir}/*.c"
)

set(bid_f128_sources
    "${f128_dir}/dpml_ux_bid.c"
    "${f128_dir}/dpml_ux_bessel.c"
    "${f128_dir}/dpml_ux_cbrt.c"
    "${f128_dir}/dpml_ux_erf.c"
    "${f128_dir}/dpml_ux_exp.c"
    "${f128_dir}/dpml_ux_int.c"
    "${f128_dir}/dpml_ux_inv_hyper.c"
    "${f128_dir}/dpml_ux_inv_trig.c"
    "${f128_dir}/dpml_ux_lgamma.c"
    "${f128_dir}/dpml_ux_log.c"
    "${f128_dir}/dpml_ux_mod.c"
    "${f128_dir}/dpml_ux_powi.c"
    "${f128_dir}/dpml_ux_pow.c"
    "${f128_dir}/dpml_ux_sqrt.c"
    "${f128_dir}/dpml_ux_trig.c"
    "${f128_dir}/dpml_ux_ops.c"
    "${f128_dir}/dpml_ux_ops_64.c"
    "${f128_dir}/dpml_four_over_pi.c"
    "${f128_dir}/dpml_exception.c"
    "${f128_dir}/sqrt_tab_t.c"
)

add_library(libbid_core_obj OBJECT ${bid_core_sources})
target_include_directories(libbid_core_obj PRIVATE
    "${src_dir}"
)
target_compile_definitions(libbid_core_obj PRIVATE
    USE_COMPILER_F128_TYPE=0
    USE_COMPILER_F80_TYPE=0
    LINUX
    efi2
    DECIMAL_CALL_BY_REFERENCE=0
    DECIMAL_GLOBAL_ROUNDING=0
    DECIMAL_GLOBAL_EXCEPTION_FLAGS=0
)
target_compile_options(libbid_core_obj PRIVATE -w)
set_target_properties(libbid_core_obj PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
)

add_library(libbid_f128_obj OBJECT ${bid_f128_sources})
target_include_directories(libbid_f128_obj PRIVATE
    "${src_dir}"
    "${f128_dir}"
)
target_compile_definitions(libbid_f128_obj PRIVATE
    USE_NATIVE_QUAD_TYPE=0
    efi2
    linux
    gcc
)
target_compile_options(libbid_f128_obj PRIVATE -w)
set_target_properties(libbid_f128_obj PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
)

# Object libraries must be PIC when they are linked into a shared library.
if(libdecimal_BUILD_SHARED_LIBS)
    set_target_properties(libbid_core_obj libbid_f128_obj PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Target name stays libbid_static for backward compatibility; the on-disk name
# changes based on the build mode (libbid_static.a vs libbid.so).
if(libdecimal_BUILD_SHARED_LIBS)
    add_library(${libname} SHARED
        $<TARGET_OBJECTS:libbid_core_obj>
        $<TARGET_OBJECTS:libbid_f128_obj>
    )
    set_target_properties(${libname} PROPERTIES OUTPUT_NAME bid)
else()
    add_library(${libname} STATIC
        $<TARGET_OBJECTS:libbid_core_obj>
        $<TARGET_OBJECTS:libbid_f128_obj>
    )
    set_target_properties(${libname} PROPERTIES OUTPUT_NAME bid_static)
endif()
add_library(libdecimal::${libname} ALIAS ${libname})

# libbid is a real install artifact (libdecimal's INTERFACE target links it), so
# it must build under the default "all" target even though the thirdparty
# subdirectory is added EXCLUDE_FROM_ALL (which exists to keep doctest / nanobench
# / boost-decimal — used only by tests and benchmarks — out of the default build).
# Without this, `cmake --build` followed by `cmake --install` fails to find the
# library whenever tests are disabled (e.g. the packaging workflow).
set_target_properties(${libname} PROPERTIES EXCLUDE_FROM_ALL OFF)

target_include_directories(${libname}
    PUBLIC
        $<BUILD_INTERFACE:${src_dir}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/libbid>
)

set(libbid_static_include_dir "${src_dir}" CACHE PATH "Intel LIBBID public header source directory" FORCE)
