cmake_minimum_required(VERSION 2.6)
project(Atomical)

# Use gcc
set(CMAKE_CXX_COMPILER /usr/bin/g++-7)

# Enable debug symbols
# set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14") # use c++14
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0") # debug, no optimisation
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage") # enabling coverage

# Compiler library
add_library(atomical ../src/lib.cpp)

# Compiler executable
# add_executable(atomical-bin ../src/main.cpp)
# target_link_libraries(atomical-bin ${LLVM_LIBS} atomical)
# # target_compile_options(atomical-bin PRIVATE "-Werror")
# # target_compile_options(atomical-bin PRIVATE "-std=c++14")
# set_target_properties(atomical-bin PROPERTIES OUTPUT_NAME atomical)

# Test executable 
add_executable(atomical-test ../tests/test.cpp)
target_link_libraries(atomical-test atomical stdc++fs)