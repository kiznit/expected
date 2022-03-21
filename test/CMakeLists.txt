cmake_minimum_required(VERSION 3.14)

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v2.13.7
)

FetchContent_MakeAvailable(Catch2)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CXX_FLAGS -Wall -Wextra -pedantic -Werror)
    set(CXX_FLAGS_NO_EXCEPTIONS -fno-exceptions)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CXX_FLAGS /W4 /WX)
    # Turning off exceptions on MSVC causes problems with system headers that are
    # not expecting it. This breaks compiling the unit tests.
    #set(CXX_FLAGS_NO_EXCEPTIONS)
endif()

# Enable C++ concepts for older versions of GCC
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
set(CXX_FLAGS ${CXX_FLAGS} -fconcepts)
endif()

set(SRC
    catch2main.cpp
    expected.test.cpp
    unexpected.test.cpp
    old.expected.test.cpp
)

# Unit tests with exception handling
add_executable(expected-test ${SRC})

target_link_libraries(
    expected-test
    PRIVATE
        expected
        Catch2::Catch2
)

set_target_properties(
    expected-test
    PROPERTIES
        CXX_STANDARD 20
        CMAKE_CXX_STANDARD_REQUIRED True
)

target_compile_options(expected-test PRIVATE ${CXX_FLAGS})

add_test(NAME expected COMMAND expected-test)

# Unit tests without exception handling
add_executable(expected-test-no-exceptions ${SRC})

target_link_libraries(
    expected-test-no-exceptions
    PRIVATE
        expected
        Catch2::Catch2
)

set_target_properties(
    expected-test-no-exceptions
    PROPERTIES
        CXX_STANDARD 20
        CMAKE_CXX_STANDARD_REQUIRED True
)

target_compile_options(expected-test-no-exceptions PRIVATE ${CXX_FLAGS} ${CXX_FLAGS_NO_EXCEPTIONS} -DKZ_EXCEPTIONS=0)

add_test(NAME expected-no-exceptions COMMAND expected-test-no-exceptions)
