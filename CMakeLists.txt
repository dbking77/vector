cmake_minimum_required(VERSION 3.0.0)
project(vector VERSION 0.1.0 LANGUAGES C CXX)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(vector my_vec1.hpp my_vec2.hpp vector.cpp)

enable_testing()
find_package(GTest REQUIRED)

add_executable(vector_test vector_test.cpp constructor_counter.cpp)
target_link_libraries(vector_test vector)
target_link_libraries(vector_test GTest::gtest GTest::gtest_main gtest)
gtest_discover_tests(vector_test)