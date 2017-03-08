cmake_minimum_required(VERSION 2.6)

# Use Clang
# set(CMAKE_C_COMPILER /usr/bin/clang-3.9)
set(CMAKE_C_COMPILER /usr/bin/clang++-3.9)
add_definitions("-fpermissive")

project(Furlang)

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Test executable 
add_executable(fur_tests ../tests/test.cpp)
target_link_libraries(fur_tests ${GTEST_LIBRARIES} pthread)

# Compiler executable
add_executable(fur ../src/main.c)