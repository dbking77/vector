/*
Copyright (c) 2024 Derek King

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <iostream>
#include <string>
#include <vector>

#include "my_vec1.hpp"
#include "my_vec2.hpp"
#include "my_vec3.hpp"

#include "constructor_counter.hpp"

#include <gtest/gtest.h>    

template<typename VEC_T>
void print_vec(const VEC_T& vec, const std::string& name)
{
    std::cerr << name << " : " << vec.size() << " of " << vec.capacity() << std::endl;
    for (size_t ii = 0; ii < vec.size(); ++ii)
    {
        std::cerr << vec[ii] << std::endl;
    }
}

template <class VEC_T>
class BasicVectorTests : public testing::Test
{
    using VecType = VEC_T;
};

// The list of types we want to test.
using VectorIntTypes = ::testing::Types<
    std::vector<int>,
    MyVec1<int>,
    MyVec2<int>,
    MyVec3<int>
>;

TYPED_TEST_SUITE(BasicVectorTests, VectorIntTypes);

TYPED_TEST(BasicVectorTests, capacity_doubling)
{
    using VectorType = TypeParam;
    VectorType vec;
    EXPECT_EQ(vec.capacity(), 0);
    vec.push_back(1);
    EXPECT_EQ(vec.capacity(), 1);
    vec.push_back(2);
    EXPECT_EQ(vec.capacity(), 2);
    vec.push_back(3);
    EXPECT_EQ(vec.capacity(), 4);
}

TYPED_TEST(BasicVectorTests, empty_vector_data)
{
    using VectorType = TypeParam;
    VectorType vec;
    EXPECT_EQ(vec.data(), nullptr);
}

TYPED_TEST(BasicVectorTests, empty_vector_capacity)
{
    using VectorType = TypeParam;
    VectorType vec;
    EXPECT_EQ(vec.capacity(), 0ul);
}

/**
 * When copying from another vector, the new vector's capcity is equal to the other
 * vectors size, not the other vectors capacity.
 * This aspect is probably up to the implementation, not set by a standard
*/
TYPED_TEST(BasicVectorTests, vector_copy_capacity)
{
    using VectorType = TypeParam;
    VectorType vec1;
    vec1.reserve(3);
    vec1.push_back(1);
    VectorType vec2(vec1);
    EXPECT_EQ(vec2.capacity(), 1ul);
}

/**
 * We kind of expect vector to require 3 64bit (on 64bit arch) values.  
 * Either pointers or sizes in order to stored pointer to data, size, and capacity
*/
TYPED_TEST(BasicVectorTests, sizeof_vector)
{
    using VectorType = TypeParam;
    EXPECT_EQ(sizeof(VectorType), 3*sizeof(std::size_t));
}

TEST(ConstructorCounter, adl_swap)
{
    ConstructorCounter a;
    ConstructorCounter b;
    ConstructorCounter::reset();
    // using naked swap should trigger ADL (argument dependent lookup) to use ConstructorCounter::swap instead of std::swap
    using std::swap;
    swap(a, b);  
    EXPECT_EQ(ConstructorCounter::swap_count, 1);
    EXPECT_EQ(ConstructorCounter::destructor_count, 0);
    EXPECT_EQ(ConstructorCounter::any_constructor_count(), 0);
}

TEST(ConstructorCounter, std_swap)
{
    ConstructorCounter a;
    ConstructorCounter b;
    ConstructorCounter::reset();
    // std::swap will first make a temporary copy and use the assigment operator
    // swap(T& a, T& b) {
    //   T tmp(std::move(a);
    //   a = std::move(b);
    //   b = std::move(tmp);
    // } 
    std::swap(a, b);
    EXPECT_EQ(ConstructorCounter::swap_count, 0);
    EXPECT_EQ(ConstructorCounter::destructor_count, 1);
    EXPECT_EQ(ConstructorCounter::move_assignment_count, 2);
    EXPECT_EQ(ConstructorCounter::move_constructor_count, 1);
}

TEST(ConstructorCounter, equal_construct_and_destruct)
{
    ConstructorCounter::reset();
    {
        ConstructorCounter a;
        ConstructorCounter b{0};
        ConstructorCounter c{a};
    }
    EXPECT_EQ(ConstructorCounter::any_constructor_count(), 3);
    EXPECT_EQ(ConstructorCounter::destructor_count, 3);
}

// The list of types we want to test.
using VectorConstructorCounterTypes = ::testing::Types<
    std::vector<ConstructorCounter>,
    MyVec1<ConstructorCounter>,
    MyVec2<ConstructorCounter>,
    MyVec3<ConstructorCounter>  
>;

template <class VEC_T>
class VectorConstructorCounterTests : public testing::Test { };

TYPED_TEST_SUITE(VectorConstructorCounterTests, VectorConstructorCounterTypes);

TYPED_TEST(VectorConstructorCounterTests, reserving_does_not_construct)
{
    using VectorType = TypeParam;
    ConstructorCounter::reset(); 
    VectorType vec1;
    vec1.reserve(3);
    ASSERT_EQ(ConstructorCounter::any_constructor_count(), 0);
}


TYPED_TEST(VectorConstructorCounterTests, equal_construct_and_destruct)
{
    using VectorType = TypeParam;
    ConstructorCounter::reset();
    { 
        VectorType vec;
        vec.push_back(ConstructorCounter{1});
        vec.push_back(ConstructorCounter{2});
        vec.push_back(ConstructorCounter{3});
        // this forces vec out of scope and should force desstruction of internal objects
    }
    //EXPECT_EQ(ConstructorCounter::destructor_count, 6);
    EXPECT_EQ(ConstructorCounter::any_constructor_count(), ConstructorCounter::destructor_count);
}