cmake_minimum_required(VERSION 3.10)
project(DesignPatternsC)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find all .c files in the current directory
file(GLOB C_SOURCES "*.c")

# Create executables for each pattern
foreach(source ${C_SOURCES})
    get_filename_component(target_name ${source} NAME_WE)
    add_executable(${target_name} ${source})

    # Set output directory
    set_target_properties(${target_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endforeach()

# Optional: Create a custom target to build all patterns
add_custom_target(all_patterns DEPENDS ${C_SOURCES})

# Print available targets
message(STATUS "Available C pattern executables:")
foreach(source ${C_SOURCES})
    get_filename_component(target_name ${source} NAME_WE)
    message(STATUS "  - ${target_name}")
endforeach()