cmake_minimum_required(VERSION 3.20)

project(MiniConsoleOS VERSION 0.1.0 LANGUAGES CXX)

if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/CompilerFlags.cmake)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(miniconsole_core
    src/core/memory/fixed_block_allocator.cpp
)
target_include_directories(miniconsole_core PUBLIC include)
set_project_warnings(miniconsole_core)
enable_sanitizers(miniconsole_core)

add_executable(MiniConsoleOS src/main.cpp)
target_link_libraries(MiniConsoleOS PRIVATE miniconsole_core)
set_project_warnings(MiniConsoleOS)
enable_sanitizers(MiniConsoleOS)

enable_testing()
add_executable(unit_tests
    tests/test_main.cpp
    tests/test_fixed_block_allocator.cpp
)
target_link_libraries(unit_tests PRIVATE miniconsole_core gtest_main)
set_project_warnings(unit_tests)
enable_sanitizers(unit_tests)

include(GoogleTest)
gtest_discover_tests(unit_tests)