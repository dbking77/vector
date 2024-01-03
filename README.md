# Vector

Partial implementation of std::vector

TODO
- [] How does copy and assignment operator deal with exceptions thrown from constructor of stored type.


## Memory Allocation

Data stored in a vector must be contiguous in memory.  That is what allows very fast O(1) lookup of data elements. 
However, when a data item is appeded to vector with push_back a bigger chunk of memory must be allocated and existing
elements copied to new location.
To avoid copying every element every time push_back is called, the vector has a capacity that can bigger than it size.
When a vector runs out of capacity, it will twice as much memory as before, instead of just enough memory for one more element.

### new T[capacity]
Some vector examples show allocating memory using array new ```new T[capacity]``` : https://www.geeksforgeeks.org/stdallocator-in-cpp-with-examples/

MyVec1 uses this memory allocation strategy.  However, this is not totally correct since new will also initialize the array values with a default constructor for the stored type.  If stored type has a complex constructor, this can introduce a lot of extra overhead.

```
new_capacity_ = size_ * 2
T* data_ = new data[capacity_]
```

Moreover, if a stored type has a protected or deleted default constructor, the vector cannot allocate it
```
struct MyStruct
{
    MyStruct() = delete;
    MyStruct(int input) { }
}

MyVec1<MyStruct> vec;
vec.push_back();  // <--- this will not compile because push_back will not be able to allocate memory with new 
```


### alligned_alloc or malloc
alligned_alloc and malloc will allocate memory but not initialize it.   This means unused extra capacity is not initialized.  It also storage of types that don't allow use of a default constructor. 
```
data_ = std::alligned_alloc(alignof(T), sizeof(T)*capacity);
```

Intializing allocated memory is a little unusual.  Typically ```new``` is used to be both allocate and initialize an object.  In this case, "placement new" is used to just initialize an object that has already been allocated.  Placement new will initialize an object at a memory address that is provided. 
```
new (address_to_storage) T(constructor_args);
```   

MyVec2 is an example vector that uses aligned_alloc to allocate memory and placement new to copy vector elements to different locations.

### std::allocator<T>
std::vector allows vector type to specify a allocator type as second template argument.
The default type for this allocator is std::allocator<T>.   std::allocator provides an allocate() and deallocate() function.

While std::allocator<T> is stateless (contains no instance member variables), not all implementations of std::allocator have to be stateless.
This means std::vector must store the allocator instance as well as the data pointer, size, and capacity:
```
class MyVec
{
    ALLOC_T allocator_;
    T* data_;
    size_t size_;
    size_t capacity_;
};
```

The with the previous code is the stateless allocator must take up at least one byte of space.  This is because all objects in c++ must have a UNIQUE address.  If allocator_ used no space, then the allocator_ and data_ would have same address in memory.
```
addressof(allocator_) != addressof(data_);
```

Because of allignement requirements on x86_64, the allocator size will be padded to 8 bytes.   So on x86_64 any empty vector will use 32bytes of storage instead of just 24.

One trick that can be used to have a stateless allocator use no extra memory is the empty-base-optimization (EBO).   https://en.cppreference.com/w/cpp/language/ebo


This optimization comes about when one class inherits from another with now state.

```
class Empty
{
    // no elements;
};

class Child1
{
    Empty e_;
    int x_;

    // Empty element "e" must be given at least 1 byte of memory space
    // Because of alignment requirements, padding must be added after element e_
    // so x_ is alligned.
    static_assert( sizeof(Child1) == 2*sizeof(int) );
};


class Child2 : public Empty
{
    // Even though there no elements class object can't have a zero size.
    static_assert( sizeof(Child2) == 1 );
};

class Child3 : public Empty
{
    int x_;

    // Because there is not any member variable for Empty in Child3, 
    // the address of Empty cannot be taken, and so it takes up no space
    static_assert( sizeof(Child3) == sizeof(int) );
};
```

Vec3 uses the EBO to allow a stateless allocator to use no space.  It does this by using an extra base class that inherits from the allocator type and
contains the data pointer.
```
template<typename T, typename ALLOC_T = std::allocator<T> >
struct MyVec3Base : public ALLOC_T
{
    // need a data member in the class so it is non-zero size
    T* data_ = nullptr;
};
```
