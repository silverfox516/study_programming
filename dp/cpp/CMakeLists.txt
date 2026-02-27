cmake_minimum_required(VERSION 3.10)
project(DesignPatternsCPP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find all .cpp files in the current directory
file(GLOB CPP_SOURCES "*.cpp")

# Exclude implementation-only files (no main function)
list(FILTER CPP_SOURCES EXCLUDE REGEX ".*factory_impl\\.cpp$")

# Create executables for each pattern
foreach(source ${CPP_SOURCES})
    get_filename_component(target_name ${source} NAME_WE)

    # Special case for factory_improved which needs factory_impl.cpp
    if(target_name STREQUAL "factory_improved")
        add_executable(${target_name} ${source} factory_impl.cpp)
    else()
        add_executable(${target_name} ${source})
    endif()

    # Set output directory
    set_target_properties(${target_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endforeach()

# Optional: Create a custom target to build all patterns
add_custom_target(all_patterns DEPENDS ${CPP_SOURCES})

# Print available targets
message(STATUS "Available C++ pattern executables:")
foreach(source ${CPP_SOURCES})
    get_filename_component(target_name ${source} NAME_WE)
    message(STATUS "  - ${target_name}")
endforeach()