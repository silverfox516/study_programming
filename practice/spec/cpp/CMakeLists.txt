cmake_minimum_required(VERSION 3.20)
project(CppSpecPractice LANGUAGES CXX)

# -----------------------------------------------------------------------------
# Practice 실습용 CMakeLists.
#
# 디렉터리 컨벤션:
#   practice/spec/cpp/<standard>/<topic>/stepNN.cpp
#     예) cpp11/lambda_expressions/step01.cpp
#
# 각 stepNN.cpp 는 독립 main() 을 가지므로 step 별 실행 파일을 만든다.
# target 이름: practice_<standard>_<topic>_<step>
# 출력 위치:   build/bin/<standard>/<topic>/stepNN(.exe)
#
# C++ 표준:
#   폴더명(cpp11, cpp14, ...)은 "주제가 처음 도입된 표준" 을 의미하며,
#   실습 파일은 그 이후 표준 기능을 함께 다룰 수 있다.
#   (예: cpp11/lambda_expressions/step08,09 는 C++14 generic lambda / init capture)
#   따라서 실제 빌드 표준은 README 와 동일하게 C++17 을 기본으로 한다.
#   특정 토픽이 더 높은 표준이 필요해지면 그 토픽 폴더에
#   `cmake_std.txt` (단일 정수만 기재) 파일을 두면 그 값으로 올라간다.
# -----------------------------------------------------------------------------

set(PRACTICE_DEFAULT_STD 17)

function(add_practice_topic STANDARD_DIR TOPIC_DIR)
    # STANDARD_DIR: "cpp11", "cpp14", ...
    # TOPIC_DIR:    절대 경로 (해당 토픽 폴더)

    get_filename_component(topic ${TOPIC_DIR} NAME)

    # 토픽별 표준 override
    set(topic_std ${PRACTICE_DEFAULT_STD})
    if(EXISTS "${TOPIC_DIR}/cmake_std.txt")
        file(READ "${TOPIC_DIR}/cmake_std.txt" override_std)
        string(STRIP "${override_std}" override_std)
        set(topic_std ${override_std})
    endif()

    file(GLOB STEP_SOURCES "${TOPIC_DIR}/step*.cpp")
    foreach(source ${STEP_SOURCES})
        get_filename_component(name ${source} NAME_WE)
        set(target_name "practice_${STANDARD_DIR}_${topic}_${name}")
        add_executable(${target_name} ${source})
        target_compile_features(${target_name} PRIVATE cxx_std_${topic_std})
        set_target_properties(${target_name} PROPERTIES
            CXX_EXTENSIONS OFF
            RUNTIME_OUTPUT_DIRECTORY
                ${CMAKE_BINARY_DIR}/bin/${STANDARD_DIR}/${topic}
        )
    endforeach()
endfunction()

# 각 표준 폴더 하위의 모든 토픽 자동 등록
set(PRACTICE_STANDARDS cpp98 cpp11 cpp14 cpp17 cpp20 cpp23)

foreach(std ${PRACTICE_STANDARDS})
    set(std_root "${CMAKE_CURRENT_SOURCE_DIR}/${std}")
    if(NOT IS_DIRECTORY ${std_root})
        continue()
    endif()

    file(GLOB topic_entries RELATIVE ${std_root} ${std_root}/*)
    foreach(entry ${topic_entries})
        set(topic_path "${std_root}/${entry}")
        if(IS_DIRECTORY ${topic_path})
            add_practice_topic(${std} ${topic_path})
        endif()
    endforeach()
endforeach()

# ---------- summary ---------------------------------------------------------
get_property(_targets DIRECTORY PROPERTY BUILDSYSTEM_TARGETS)
list(LENGTH _targets _count)
message(STATUS "=== CppSpecPractice ===")
message(STATUS "  default std: c++${PRACTICE_DEFAULT_STD}")
message(STATUS "  targets:     ${_count}")
