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

#ifndef MY_VEC2_HPP_GUARD
#define MY_VEC2_HPP_GUARD

#include <cstdint>
#include <cstdlib>

template<typename T>
class MyVec2
{
public:
    MyVec2() = default;

    MyVec2(const MyVec2& other)
    {
        data_ = allocate(other.size_);
        for (std::size_t ii = 0; ii < other.size_; ++ii) // std::copy?
        {
            data_[ii] = other.data_[ii];
        }
        size_ = other.size_;
        capacity_ = other.size_; 
    }

    ~MyVec2()
    {
        for (std::size_t ii = 0; ii < size_; ++ii)
        {
            data_[ii].~T();
        }
        deallocate(data_);
    }

    static T* allocate(std::size_t capacity)
    {
        return reinterpret_cast<T*>(aligned_alloc(alignof(T), sizeof(T) * capacity));
    }

    static void deallocate(T* ptr)
    {
        std::free(ptr);
    }

    void reserve(std::size_t capacity)
    {
        if (capacity > capacity_)
        {
            capacity_ = capacity;
            T* tmp = allocate(capacity);
            for (std::size_t ii = 0; ii < size_; ++ii)
            {
                // use placement new to invoke T's copy constructor 
                new (&tmp[ii]) T(data_[ii]);
            }
            deallocate(data_);
            data_ = tmp;
        }
    }

    void push_back(const T& val)
    {
        if (size_ >= capacity_)
        {
            std::size_t new_capacity = size_ > 0 ? size_ * 2 : 1;
            reserve(new_capacity);
        }
        data_[size_] = val;
        ++size_;
    }

    const T& operator[](std::size_t idx) const
    {
        return data_[idx]; 
    }

    T& operator[](std::size_t idx)
    {
        return data_[idx];
    }

    T* data()
    {
        return data_;
    }

    const T* data() const
    {
        return data_;
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
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
    T* data_ = nullptr;
};

#endif  // MY_VEC2_HPP_GUARD