#include "constructor_counter.hpp"

std::size_t ConstructorCounter::default_constructor_count = 0;
std::size_t ConstructorCounter::other_constructor_count = 0;
std::size_t ConstructorCounter::copy_constructor_count = 0;
std::size_t ConstructorCounter::move_constructor_count = 0;
std::size_t ConstructorCounter::destructor_count = 0;
std::size_t ConstructorCounter::swap_count = 0;
std::size_t ConstructorCounter::copy_assignment_count = 0;
std::size_t ConstructorCounter::move_assignment_count = 0;
