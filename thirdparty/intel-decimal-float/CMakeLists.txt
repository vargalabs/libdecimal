# This file is part of the libdecimal project and is licensed under the MIT License.
#
# Copyright © 2025–2026 Varga Labs, Toronto, ON, Canada 🇨🇦
# Contact: info@vargalabs.com


set(name bid)
set(libname lib${name}_static)

project(${libname} LANGUAGES C CXX)

set(current_version "v2.2.0")
set(current_version_dir "${CMAKE_CURRENT_SOURCE_DIR}/${current_version}")
set(build_version_dir   "${CMAKE_CURRENT_BINARY_DIR}/${current_version}")
set(makefile_output     "${build_version_dir}/libbid.a")
set(libbid_include_dir  "${build_version_dir}/src")

file(COPY ${current_version_dir} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(
    OUTPUT ${makefile_output}
    COMMAND make -C ${build_version_dir}
            CC=${CMAKE_C_COMPILER}
            CALL_BY_REF=0
            GLOBAL_RND=0
            GLOBAL_FLAGS=0
            UNCHANGED_BINARY_FLAGS=0
    WORKING_DIRECTORY ${build_version_dir}
    COMMENT "Building Intel LIBBID static library"
    VERBATIM 
)

add_custom_target(${libname}_build ALL DEPENDS ${makefile_output} )
add_library(${libname} INTERFACE)
add_library(libdecimal::${libname} ALIAS ${libname})
add_dependencies(${libname} ${libname}_build)
target_include_directories(${libname} INTERFACE $<BUILD_INTERFACE:${libbid_include_dir}> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/libbid> )
target_link_libraries(${libname} INTERFACE $<BUILD_INTERFACE:${makefile_output}> $<INSTALL_INTERFACE:${CMAKE_INSTALL_LIBDIR}/libbid.a>)

# Export build artifacts to parent scope for installation
set(libbid_static_archive     "${makefile_output}"    PARENT_SCOPE)
set(libbid_static_include_dir "${libbid_include_dir}" PARENT_SCOPE)
