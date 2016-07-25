cmake_minimum_required(VERSION 3.3)
project(hashmap)

enable_testing()
add_subdirectory(third_party/gtest)
set(CXX clang++)
include_directories(third_party/gtest/googletest/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g3 -Wall -Wextra -O3 -DNDEBUG")

add_executable(hashmap_test hashmap.cpp)
add_executable(hashbench hashbench.cpp)

target_link_libraries(hashmap_test gtest_main)
link_directories(third_party/gtest/googlemock/gtest/)
