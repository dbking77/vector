// only include vector so preprocessor to allow inspection of vector header
// after preprocessing is run.   
// This makes it easier to read through stl vector implementation because
// makes heavy use of preprocessor macros for different compiler versions
// becauese vector also pulls in lots of other headers, use grep to only include
// contents

// Include other headers before start so they don't get re-included in vector
#include <cstdint>
#include <memory>

// g++ -std=c++17 -P -E -CC stl_vector.cpp -o /dev/stdout | grep START_AFTER_HERE -A100000 > stl_vector_pp.hpp
#include <vector>

int main()
{
    return 0;
}