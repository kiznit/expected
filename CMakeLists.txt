# Version 3.22 is likely not a strict requirement.
# cmake 3.22 is happy if we specify 3.16, but cmake 3.16 doesn't work.
cmake_minimum_required(VERSION 3.22)

project(std_expected
    HOMEPAGE_URL https://github.com/kiznit/expected
    DESCRIPTION "C++ implementation of std::expected as proposed in P0323R12"
    VERSION 0.12.0
    LANGUAGES CXX)

option(expected_BUILD_TESTS "Build tests" ON)

add_library(expected INTERFACE)
add_library(kiznit::expected ALIAS expected)

target_include_directories(
    expected
    INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include/expected>
)

set_target_properties(
    expected
    PROPERTIES
        CXX_STANDARD 20
        CMAKE_CXX_STANDARD_REQUIRED True
)


if (expected_BUILD_TESTS)
    include (CTest)
    add_subdirectory(test)
endif()
