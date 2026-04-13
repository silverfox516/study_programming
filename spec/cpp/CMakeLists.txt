cmake_minimum_required(VERSION 3.20)
project(CppSpecExamples LANGUAGES CXX)

# ---------- helper: add all .cpp in a directory as separate executables ------
function(add_spec_targets DIR_PATH STANDARD)
    file(GLOB SOURCES "${DIR_PATH}/*.cpp")

    foreach(source ${SOURCES})
        get_filename_component(name ${source} NAME_WE)
        set(target_name "${STANDARD}_${name}")
        add_executable(${target_name} ${source})
        target_compile_features(${target_name} PRIVATE cxx_std_${STANDARD})
        set_target_properties(${target_name} PROPERTIES
            CXX_EXTENSIONS OFF
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${STANDARD}
        )
    endforeach()
endfunction()

# ---------- cpp98 (compile as C++11 minimum for headers) --------------------
file(GLOB CPP98_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/cpp98/*.cpp")
foreach(source ${CPP98_SOURCES})
    get_filename_component(name ${source} NAME_WE)
    set(target_name "cpp98_${name}")
    add_executable(${target_name} ${source})
    target_compile_features(${target_name} PRIVATE cxx_std_11)
    set_target_properties(${target_name} PROPERTIES
        CXX_EXTENSIONS OFF
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/cpp98
    )
endforeach()

# ---------- cpp11 -----------------------------------------------------------
add_spec_targets(${CMAKE_CURRENT_SOURCE_DIR}/cpp11 11)

# ---------- cpp14 -----------------------------------------------------------
add_spec_targets(${CMAKE_CURRENT_SOURCE_DIR}/cpp14 14)

# ---------- cpp17 -----------------------------------------------------------
add_spec_targets(${CMAKE_CURRENT_SOURCE_DIR}/cpp17 17)

# ---------- cpp20 (exclude modules_main — requires module precompilation) ---
file(GLOB CPP20_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/cpp20/*.cpp")
list(FILTER CPP20_SOURCES EXCLUDE REGEX ".*modules_main\\.cpp$")
foreach(source ${CPP20_SOURCES})
    get_filename_component(name ${source} NAME_WE)
    set(target_name "20_${name}")
    add_executable(${target_name} ${source})
    target_compile_features(${target_name} PRIVATE cxx_std_20)
    set_target_properties(${target_name} PROPERTIES
        CXX_EXTENSIONS OFF
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/cpp20
    )
endforeach()

# ---------- cpp23 -----------------------------------------------------------
file(GLOB CPP23_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/cpp23/*.cpp")
foreach(source ${CPP23_SOURCES})
    get_filename_component(name ${source} NAME_WE)
    set(target_name "23_${name}")
    add_executable(${target_name} ${source})
    target_compile_features(${target_name} PRIVATE cxx_std_23)
    set_target_properties(${target_name} PROPERTIES
        CXX_EXTENSIONS OFF
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/cpp23
    )
endforeach()

# ---------- threading -------------------------------------------------------
find_package(Threads REQUIRED)

# Targets that need pthread
set(THREADED_TARGETS
    11_threads 11_mutex_locks 11_futures 11_atomic 11_thread_local
    11_condition_variable
    17_shared_mutex 17_scoped_lock 17_parallel_algorithms
    20_jthread 20_sync_primitives 20_barrier 20_stop_token
)
foreach(t ${THREADED_TARGETS})
    if(TARGET ${t})
        target_link_libraries(${t} PRIVATE Threads::Threads)
    endif()
endforeach()

# ---------- summary ---------------------------------------------------------
message(STATUS "=== CppSpecExamples ===")
message(STATUS "  cpp98:  ${CMAKE_CURRENT_SOURCE_DIR}/cpp98")
message(STATUS "  cpp11:  ${CMAKE_CURRENT_SOURCE_DIR}/cpp11")
message(STATUS "  cpp14:  ${CMAKE_CURRENT_SOURCE_DIR}/cpp14")
message(STATUS "  cpp17:  ${CMAKE_CURRENT_SOURCE_DIR}/cpp17")
message(STATUS "  cpp20:  ${CMAKE_CURRENT_SOURCE_DIR}/cpp20  (modules_main excluded)")
message(STATUS "  cpp23:  ${CMAKE_CURRENT_SOURCE_DIR}/cpp23")
