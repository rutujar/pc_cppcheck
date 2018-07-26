cmake_minimum_required(VERSION 3.6)
project(ci_helloworld C CXX)

include(ExternalProject)
find_package(Git REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------------------------------------
# Includes
# ------------------------------------------------------------------------------

include_directories(include)
include_directories(SYSTEM ${CMAKE_BINARY_DIR}/include)

# ------------------------------------------------------------------------------
# Catch
# ------------------------------------------------------------------------------

list(APPEND CATCH_CMAKE_ARGS
    "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}"
)

ExternalProject_Add(
    catch
    GIT_REPOSITORY      https://github.com/Bareflank/catch.git
    GIT_TAG             v1.2
    GIT_SHALLOW         1
    CMAKE_ARGS          ${CATCH_CMAKE_ARGS}
    PREFIX              ${CMAKE_BINARY_DIR}/external/catch/prefix
    TMP_DIR             ${CMAKE_BINARY_DIR}/external/catch/tmp
    STAMP_DIR           ${CMAKE_BINARY_DIR}/external/catch/stamp
    DOWNLOAD_DIR        ${CMAKE_BINARY_DIR}/external/catch/download
    SOURCE_DIR          ${CMAKE_BINARY_DIR}/external/catch/src
    BINARY_DIR          ${CMAKE_BINARY_DIR}/external/catch/build
)



# ------------------------------------------------------------------------------
# CppCheck
# ------------------------------------------------------------------------------

if(ENABLE_CPPCHECK)

    list(APPEND CPPCHECK_CMAKE_ARGS
        "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}"
    )

    ExternalProject_Add(
        cppcheck
        GIT_REPOSITORY      https://github.com/danmar/cppcheck.git
        GIT_TAG             1.79
        GIT_SHALLOW         1
        CMAKE_ARGS          ${CPPCHECK_CMAKE_ARGS}
        PREFIX              ${CMAKE_BINARY_DIR}/external/cppcheck/prefix
        TMP_DIR             ${CMAKE_BINARY_DIR}/external/cppcheck/tmp
        STAMP_DIR           ${CMAKE_BINARY_DIR}/external/cppcheck/stamp
        DOWNLOAD_DIR        ${CMAKE_BINARY_DIR}/external/cppcheck/download
        SOURCE_DIR          ${CMAKE_BINARY_DIR}/external/cppcheck/src
        BINARY_DIR          ${CMAKE_BINARY_DIR}/external/cppcheck/build
    )

    list(APPEND CPPCHECK_ARGS
        --enable=warning,style,performance,portability,unusedFunction
        --std=c++11
        --verbose
        --error-exitcode=1
        --language=c++
        -DMAIN=main
        -I ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/*.h
        ${CMAKE_SOURCE_DIR}/src/*.cpp
        ${CMAKE_SOURCE_DIR}/test/*.cpp
    )

    add_custom_target(
        check
        COMMAND ${CMAKE_BINARY_DIR}/bin/cppcheck ${CPPCHECK_ARGS}
        COMMENT "running cppcheck"
    )

endif()

# ------------------------------------------------------------------------------
# Coverage
# ------------------------------------------------------------------------------

if(ENABLE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

# ------------------------------------------------------------------------------
# Google Sanitizers
# ------------------------------------------------------------------------------

if(ENABLE_ASAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak")
endif()

if(ENABLE_USAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
endif()

if(ENABLE_TSAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
endif()

# ------------------------------------------------------------------------------
# Valgrind
# ------------------------------------------------------------------------------

set(MEMORYCHECK_COMMAND_OPTIONS "${MEMORYCHECK_COMMAND_OPTIONS} --leak-check=full")
set(MEMORYCHECK_COMMAND_OPTIONS "${MEMORYCHECK_COMMAND_OPTIONS} --track-fds=yes")
set(MEMORYCHECK_COMMAND_OPTIONS "${MEMORYCHECK_COMMAND_OPTIONS} --trace-children=yes")
set(MEMORYCHECK_COMMAND_OPTIONS "${MEMORYCHECK_COMMAND_OPTIONS} --error-exitcode=1")

# ------------------------------------------------------------------------------
# Main Executable
# ------------------------------------------------------------------------------

add_executable(ci_helloworld src/main.cpp)
add_dependencies(ci_helloworld gsl)
target_compile_definitions(ci_helloworld PRIVATE VIRTUAL= MAIN=main)

# ------------------------------------------------------------------------------
# Tests
# ------------------------------------------------------------------------------

include(CTest)

add_library(test_catch STATIC test/test.cpp)
add_dependencies(test_catch catch)

add_library(business_logic STATIC src/main.cpp)
add_dependencies(business_logic gsl)
target_compile_definitions(business_logic PRIVATE VIRTUAL=virtual MAIN=unittest_main)

macro(do_test str)
    add_executable(test_${str} test/test_${str}.cpp)
    add_test(test_${str} test_${str})
    target_link_libraries(test_${str} test_catch)
    target_link_libraries(test_${str} business_logic)
    add_dependencies(test_${str} gsl)
    add_dependencies(test_${str} catch)
    add_dependencies(test_${str} hippomocks)
    target_compile_definitions(test_${str} PRIVATE VIRTUAL=virtual)
endmacro(do_test)

do_test(main)
do_test(producer)
do_test(consumer)
