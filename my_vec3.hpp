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

#ifndef MY_VEC3_HPP_GUARD
#define MY_VEC3_HPP_GUARD

#include <cstdint>
#include <memory>


/**
 * Even a stateless allocator (no member variables) as part of a 
 * class will take up 1 byte of data because every object must 
 * have a unqiue address
 * This extra byte would increase memory usage of vector from 
 * 24 to 32 bytes on x86_64 because of memory alignment requirements
 *  
 * It is possible to avoid this by abusing the EBO optimization, and
 * making the allocator a base class of the vector.
 * 
 * However, we don't want the allocator to be the base class of Vec.
 * Instead we make 
 * https://en.cppreference.com/w/cpp/language/ebo
*/
template<typename T, typename ALLOC_T = std::allocator<T> >
struct MyVec3Base : public ALLOC_T
{
    // need a data member in the class so it is non-zero size
    T* data_ = nullptr;
};

template<typename T, typename ALLOC_T = std::allocator<T> >
class MyVec3
{
public:
    MyVec3() = default;

    MyVec3(const MyVec3& other)
    {
        base_.data_ = allocate(other.size_);
        for (std::size_t ii = 0; ii < other.size_; ++ii) // std::copy?
        {
            base_.data_[ii] = other.base_.data_[ii];
        }
        size_ = other.size_;
        capacity_ = other.size_; 
    }

    ~MyVec3()
    {
        for (std::size_t ii = 0; ii < size_; ++ii)
        {
            base_.data_[ii].~T();
        }
        deallocate(base_.data_, capacity_);
    }

    void deallocate(T* ptr, std::size_t capacity)
    {
       //allocator_.deallocate(ptr, capacity);
       base_.deallocate(ptr, capacity);
    }

    T* allocate(std::size_t capacity)
    {
        return base_.allocate(capacity);
    }

    void reserve(std::size_t capacity)
    {
        if (capacity > capacity_)
        {
            T* tmp = allocate(capacity);
            for (std::size_t ii = 0; ii < size_; ++ii)
            {
                // use placement new to invoke T's copy constructor 
                new (&tmp[ii]) T(base_.data_[ii]);
            }
            deallocate(base_.data_, capacity);
            capacity_ = capacity;
            base_.data_ = tmp;
        }
    }

    void push_back(const T& val)
    {
        if (size_ >= capacity_)
        {
            std::size_t new_capacity = size_ > 0 ? size_ * 2 : 1;
            reserve(new_capacity);
        }
        base_.data_[size_] = val;
        ++size_;
    }

    const T& operator[](std::size_t idx) const
    {
        return base_.data_[idx]; 
    }

    T& operator[](std::size_t idx)
    {
        return base_.data_[idx];
    }

    T* data()
    {
        return base_.data_;
    }

    const T* data() const
    {
        return base_.data_;
    }

    std::size_t size() const
    {
        return size_;
    }

    std::size_t capacity() const
    {
        return capacity_;
    }

protected:
    MyVec3Base<T, ALLOC_T> base_;
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
    //T* data_ = nullptr;
};


#endif  // MY_VEC3_HPP_GUARD