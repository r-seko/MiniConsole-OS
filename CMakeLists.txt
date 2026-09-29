cmake_minimum_required(VERSION 3.20)

if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

project(MiniConsoleOS VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/CompilerFlags.cmake)

include_directories(include)

add_executable(MiniConsoleOS
    src/main.cpp
)

set_project_warnings(MiniConsoleOS)
enable_sanitizers(MiniConsoleOS)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(unit_tests
    tests/test_main.cpp
)
target_link_libraries(unit_tests PRIVATE gtest_main)
set_project_warnings(unit_tests)
enable_sanitizers(unit_tests)

include(GoogleTest)
gtest_discover_tests(unit_tests)