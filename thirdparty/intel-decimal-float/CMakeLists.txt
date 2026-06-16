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

# ---- Per-platform Intel BID build macros ------------------------------------
# Intel's BID sources select OS / architecture / compiler from preprocessor
# macros.  The x86-64 architecture is forced with `efi2` (-> _M_AMD64 -> amd64
# in float128/architecture.h) on every platform, and the compiler family is
# auto-detected from _MSC_VER / __GNUC__ (float128/compiler.h), so only the OS
# token has to be supplied per platform:
#   * Linux  : LINUX / linux   (unchanged from the original build)
#   * macOS  : nothing -- op_system.h / bid_conf.h key off __APPLE__ -> darwin
#   * Windows: WINDOWS (core) / WNT (DPML float128, -> OP_SYSTEM wnt)
# NOTE: the float128 (DPML) sources only ship an amd64 code path, so an Apple
# runner must be Intel (macos-13); Apple Silicon would need an aarch64 port.
if(WIN32)
    set(bid_core_os WINDOWS)
    set(bid_f128_os WNT)
    set(bid_f128_cc "")          # _MSC_VER auto-selects msc_cc
elseif(APPLE)
    set(bid_core_os "")          # __APPLE__ auto-detected
    set(bid_f128_os "")          # __APPLE__ -> darwin
    set(bid_f128_cc "")          # __GNUC__ (clang) auto-selects gnu_cc
else()
    set(bid_core_os LINUX)
    set(bid_f128_os linux)
    set(bid_f128_cc gcc)
endif()

# Intel's sources are intentionally warning-noisy; silence them per toolchain.
if(MSVC)
    set(bid_warn_flag /w)
else()
    set(bid_warn_flag -w)
endif()

add_library(libbid_core_obj OBJECT ${bid_core_sources})
target_include_directories(libbid_core_obj PRIVATE
    "${src_dir}"
)
target_compile_definitions(libbid_core_obj PRIVATE
    USE_COMPILER_F128_TYPE=0
    USE_COMPILER_F80_TYPE=0
    efi2
    DECIMAL_CALL_BY_REFERENCE=0
    DECIMAL_GLOBAL_ROUNDING=0
    DECIMAL_GLOBAL_EXCEPTION_FLAGS=0
)
if(bid_core_os)
    target_compile_definitions(libbid_core_obj PRIVATE ${bid_core_os})
endif()
target_compile_options(libbid_core_obj PRIVATE ${bid_warn_flag})
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
)
if(bid_f128_os)
    target_compile_definitions(libbid_f128_obj PRIVATE ${bid_f128_os})
endif()
if(bid_f128_cc)
    target_compile_definitions(libbid_f128_obj PRIVATE ${bid_f128_cc})
endif()
target_compile_options(libbid_f128_obj PRIVATE ${bid_warn_flag})
set_target_properties(libbid_f128_obj PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
)

add_library(${libname} STATIC
    $<TARGET_OBJECTS:libbid_core_obj>
    $<TARGET_OBJECTS:libbid_f128_obj>
)
add_library(libdecimal::${libname} ALIAS ${libname})

target_include_directories(${libname}
    PUBLIC
        $<BUILD_INTERFACE:${src_dir}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/libbid>
)

set(libbid_static_archive     $<TARGET_FILE:${libname}> PARENT_SCOPE)
set(libbid_static_include_dir "${src_dir}"              PARENT_SCOPE)
