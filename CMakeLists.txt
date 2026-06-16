# This file is part of the libdecimal project and is licensed under the MIT License.
#
# Copyright © 2025–2026 Varga Labs, Toronto, ON, Canada 🇨🇦
# Contact: info@vargalabs.com

cmake_minimum_required(VERSION 3.22)
project(libdecimal VERSION 0.1.0 DESCRIPTION "Deterministic Decimal Arithmetic Built on Intel LIBBID"  LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(CTest)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(WARNING "In-source builds are discouraged. Skipping compile_commands.json symlink.")
else()
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink  ${CMAKE_BINARY_DIR}/compile_commands.json ${CMAKE_SOURCE_DIR}/compile_commands.json
        RESULT_VARIABLE symlink_result OUTPUT_QUIET ERROR_QUIET)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(libdecimal_BUILD_TESTS "Build tests" ${BUILD_TESTING})
option(libdecimal_BUILD_DOCS  "Enable MkDocs documentation targets" ON)

set(git_commit_hash "unknown")
set(git_tag "")

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
    execute_process( COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_VARIABLE git_commit_hash OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
    execute_process( COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=0 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_VARIABLE git_tag OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET )
endif()

if(git_tag)
    set(libdecimal_version_string "${git_tag}")
else()
    set(libdecimal_version_string "${PROJECT_VERSION}")
endif()

# Third-party dependencies
    add_subdirectory(thirdparty EXCLUDE_FROM_ALL)
# Public library target

add_library(libdecimal INTERFACE)
add_library(libdecimal::libdecimal ALIAS libdecimal)
target_compile_features(libdecimal INTERFACE cxx_std_23)
target_include_directories(libdecimal INTERFACE $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_definitions(libdecimal INTERFACE libdecimal_SOFTWARE_VERSION="${libdecimal_version_string}" libdecimal_SOFTWARE_COMMIT_HASH="${git_commit_hash}" )
target_link_libraries(libdecimal INTERFACE libdecimal::libbid_static )
# ------------------------------------------------------------------------------
# Installation
# ------------------------------------------------------------------------------
install( FILES ${CMAKE_SOURCE_DIR}/include/decimal/* DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install( TARGETS libdecimal libbid_static EXPORT libdecimal_targets)
install( FILES ${libbid_static_archive} DESTINATION ${CMAKE_INSTALL_LIBDIR} )
install( DIRECTORY ${libbid_static_include_dir}/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libbid FILES_MATCHING PATTERN "*.h")
install( EXPORT libdecimal_targets FILE libdecimal_targets.cmake NAMESPACE libdecimal:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libdecimal)

configure_package_config_file( ${CMAKE_SOURCE_DIR}/cmake/libdecimal_config.cmake.in ${CMAKE_BINARY_DIR}/libdecimal_config.cmake INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libdecimal )
write_basic_package_version_file( ${CMAKE_BINARY_DIR}/libdecimal_config_version.cmake VERSION ${PROJECT_VERSION} COMPATIBILITY SameMajorVersion )
install( FILES ${CMAKE_BINARY_DIR}/libdecimal_config.cmake ${CMAKE_BINARY_DIR}/libdecimal_config_version.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libdecimal )
# ------------------------------------------------------------------------------
# Tests
# ------------------------------------------------------------------------------
if(libdecimal_BUILD_TESTS)
    enable_testing()
    function(add_test_case file_name)
        add_executable(test-${file_name} test/${file_name}.cpp)
        set_target_properties(test-${file_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
        target_link_libraries(test-${file_name} PRIVATE libdecimal::libdecimal libdoctest)
        if(CMAKE_BUILD_TYPE MATCHES "Debug")
            if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
                target_compile_options(test-${file_name} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer )
                target_link_options(test-${file_name} PRIVATE -fsanitize=address,undefined )
                target_compile_definitions(test-${file_name} PRIVATE DEBUG)
            endif()
        endif()
        add_test(NAME test-${file_name} COMMAND test-${file_name})
    endfunction()

    add_test_case(example)
    add_test_case(uint128)
    add_test_case(decimal)
    add_test_case(decimal-bid-spec)
    add_test_case(decimal-display)
    add_test_case(float-conversion)
    add_test_case(ops-add-sub)
    add_test_case(ops-mul-div)
    add_test_case(ops-transcendental)
    add_test_case(import-export)
    add_test_case(constants)
    add_test_case(algebraic-identities)
endif()
# ------------------------------------------------------------------------------
# Benchmarks
# ------------------------------------------------------------------------------
option(libdecimal_BUILD_BENCHMARKS "Build benchmarks" OFF)

if(libdecimal_BUILD_BENCHMARKS)
    set(BENCH_OUTPUT_DIR ${CMAKE_BINARY_DIR})
    function(add_benchmark name)
        add_executable(bench-${name} bench/${name}.cpp)
        set_target_properties(bench-${name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BENCH_OUTPUT_DIR})
        target_link_libraries(bench-${name} PRIVATE libdecimal::libdecimal libboostdecimal libnanobench)
        target_compile_options(bench-${name} PRIVATE -O3 -march=native)
    endfunction()

    add_benchmark(construct-from-pair)
    add_benchmark(decompose)
    add_benchmark(compare)
    add_benchmark(arithmetic)
    add_benchmark(string)
    add_benchmark(ops-fee)
    add_benchmark(encode-decode)
    add_benchmark(mixed-ops)
endif()
# ------------------------------------------------------------------------------
# MkDocs Documentation Targets
# ------------------------------------------------------------------------------
if(libdecimal_BUILD_DOCS)
    find_program(PYTHON_EXECUTABLE python3 REQUIRED)
    set(DOCS_VENV         ${CMAKE_SOURCE_DIR}/.venv)
    set(DOCS_ACTIVATE     ${DOCS_VENV}/bin/activate)
    set(DOCS_REQUIREMENTS ${CMAKE_SOURCE_DIR}/requirements.txt)
    set(DOCS_BASE_URL     http://127.0.0.1:9000/blog)

    add_custom_target(docs_venv
        COMMAND ${CMAKE_COMMAND} -E echo "Setting up Python virtual environment"
        COMMAND ${PYTHON_EXECUTABLE} -m venv ${DOCS_VENV}
        COMMAND ${DOCS_VENV}/bin/pip install --upgrade pip
        COMMAND ${DOCS_VENV}/bin/pip install -r ${DOCS_REQUIREMENTS}
        BYPRODUCTS ${DOCS_ACTIVATE}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Creating .venv and installing MkDocs dependencies"
        VERBATIM
    )

    add_custom_target(docs_serve
        COMMAND ${CMAKE_COMMAND} -E echo "Starting MkDocs server at ${DOCS_BASE_URL}"
        COMMAND ${DOCS_VENV}/bin/mkdocs serve --livereload --dev-addr=127.0.0.1:9000
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        DEPENDS docs_venv
        COMMENT "Serving MkDocs site"
        USES_TERMINAL
    )

    add_custom_target(docs_build
        COMMAND ${DOCS_VENV}/bin/mkdocs build
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        DEPENDS docs_venv
        COMMENT "Building MkDocs site"
        VERBATIM
    )

    add_custom_target(docs_clean
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${DOCS_VENV}
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_SOURCE_DIR}/site
        COMMAND ${CMAKE_COMMAND} -E rm -f ${CMAKE_SOURCE_DIR}/docs/assets/social/*.png
        COMMENT "Cleaning documentation artifacts"
    )
endif()