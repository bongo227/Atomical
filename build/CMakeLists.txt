cmake_minimum_required(VERSION 2.6)

# Use Clang
set(CMAKE_C_COMPILER /usr/bin/clang-3.9)
# set(CMAKE_C_COMPILER /usr/bin/clang++-3.9)
add_definitions("-fpermissive")
add_definitions("-fPIC")
# add_definitions("-Werror")
# add_definitions("-Wno-everything")

project(Furlang)

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Locate LLVM
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
link_directories(${LLVM_LIBRARY_DIRS})
llvm_map_components_to_libnames(llvm_libs support core irreader)

# Enable debug symbols
set(CMAKE_BUILD_TYPE Debug)

# Test executable 
add_executable(fur_tests ../tests/test.cpp)
target_link_libraries(fur_tests ${GTEST_LIBRARIES} pthread ${llvm_libs})

# Compiler executable
add_executable(fur ../src/main.c)
target_link_libraries(fur ${llvm_libs})