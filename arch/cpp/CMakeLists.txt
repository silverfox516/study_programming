cmake_minimum_required(VERSION 3.10)
project(ArchitecturePatterns LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB CPP_SOURCES "*.cpp")

foreach(source ${CPP_SOURCES})
    get_filename_component(target_name ${source} NAME_WE)
    add_executable(${target_name} ${source})
    set_target_properties(${target_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endforeach()
