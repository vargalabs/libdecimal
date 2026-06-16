# This file is part of the libdecimal project and is licensed under the MIT License.
#
# Copyright © 2025–2026 Varga Labs, Toronto, ON, Canada 🇨🇦
# Contact: info@vargalabs.com

set(examples
    bid_example
    fixed_example
    bcd_example
    scaled_example
)

foreach(name IN LISTS examples)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE libdecimal::libdecimal)
    set_target_properties(${name} PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
    )
endforeach()
