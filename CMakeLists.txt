cmake_minimum_required(VERSION 3.16)
project(mini_stl LANGUAGES C CXX VERSION 1.0.0)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# (Necessary) Add the right definitions and flags for compiler,
# providing debugging feature for executable
set(CMAKE_BUILD_TYPE Debug)

# add_compile_options(-Wall -Wextra -Werror)

include(cmake/mini_build_library.cmake)

set(PROJECT_TARGET_SRCS "")

include_directories(
    code/include
    test/include
)
add_subdirectory(code)

add_library(mini-lib SHARED ${PROJECT_TARGET_SRCS})

find_package(GTest REQUIRED)
enable_testing()
add_subdirectory(test)
