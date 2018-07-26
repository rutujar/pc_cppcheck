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
# Hippomocks
# ------------------------------------------------------------------------------

list(APPEND HIPPOMOCKS_CMAKE_ARGS
    "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}"
)

ExternalProject_Add(
    hippomocks
    GIT_REPOSITORY      https://github.com/Bareflank/hippomocks.git
    GIT_TAG             v1.2
    GIT_SHALLOW         1
    CMAKE_ARGS          ${HIPPOMOCKS_CMAKE_ARGS}
    PREFIX              ${CMAKE_BINARY_DIR}/external/hippomocks/prefix
    TMP_DIR             ${CMAKE_BINARY_DIR}/external/hippomocks/tmp
    STAMP_DIR           ${CMAKE_BINARY_DIR}/external/hippomocks/stamp
    DOWNLOAD_DIR        ${CMAKE_BINARY_DIR}/external/hippomocks/download
    SOURCE_DIR          ${CMAKE_BINARY_DIR}/external/hippomocks/src
    BINARY_DIR          ${CMAKE_BINARY_DIR}/external/hippomocks/build
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
