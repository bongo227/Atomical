cmake_minimum_required(VERSION 2.6)
 
set(CMAKE_C_COMPILER "C:/Program Files (x86)/LLVM/bin/clang.exe")
project(Furlang)


# Locate GTest
find_package(GTest REQUIRED)
include_directories(googletest/googletest/include)
 
# Link runTests with what we want to test and the GTest and pthread library
add_executable(build/fur_tests tests/test.cpp)
add_executable(build/fur src/main.c)
target_link_libraries(runTests ${GTEST_LIBRARIES} pthread)