# This file is part of the libdecimal project and is licensed under the MIT License.
#
# Copyright © 2025–2026 Varga Labs, Toronto, ON, Canada 🇨🇦
# Contact: info@vargalabs.com

set(name boostdecimal)

set(libname lib${name})
project(${libname} LANGUAGES CXX)

set(current_version_dir "v6.0.1")
add_library(${libname} INTERFACE)
target_include_directories(${libname} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/${current_version_dir}/include)

