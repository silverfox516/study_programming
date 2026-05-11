cmake_minimum_required(VERSION 3.20)
project(AsioStudy LANGUAGES CXX)

# ---------- standalone asio via FetchContent --------------------------------
include(FetchContent)
FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG        asio-1-30-2
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(asio)

add_library(asio_standalone INTERFACE)
target_include_directories(asio_standalone INTERFACE
    ${asio_SOURCE_DIR}/asio/include
)
target_compile_definitions(asio_standalone INTERFACE
    ASIO_STANDALONE
    ASIO_NO_DEPRECATED
)

find_package(Threads REQUIRED)
target_link_libraries(asio_standalone INTERFACE Threads::Threads)

if(WIN32)
    # Windows 10/11 헤더 필요 (asio가 GetTickCount64 등 사용)
    target_compile_definitions(asio_standalone INTERFACE
        _WIN32_WINNT=0x0A00
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
    target_link_libraries(asio_standalone INTERFACE ws2_32 mswsock)
endif()

# ---------- helper: per-source executable -----------------------------------
function(add_asio_target SUBDIR)
    file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR}/*.cpp")
    foreach(source ${SOURCES})
        get_filename_component(name ${source} NAME_WE)
        set(target_name "asio_${SUBDIR}_${name}")
        add_executable(${target_name} ${source})
        target_compile_features(${target_name} PRIVATE cxx_std_17)
        target_link_libraries(${target_name} PRIVATE asio_standalone)
        set_target_properties(${target_name} PROPERTIES
            CXX_EXTENSIONS OFF
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${SUBDIR}
        )
    endforeach()
endfunction()

# ---------- modules ---------------------------------------------------------
add_asio_target(core)

# ---------- summary ---------------------------------------------------------
message(STATUS "=== AsioStudy ===")
message(STATUS "  asio source: ${asio_SOURCE_DIR}")
message(STATUS "  core:        ${CMAKE_CURRENT_SOURCE_DIR}/core")
