#ifndef CONSTRUCTOR_COUNTER_HPP_GUARD
#define CONSTRUCTOR_COUNTER_HPP_GUARD

#include <cstdint>
#include <iostream>

/**
 * Keeps a global counter of each time default or non-default constructor is called
*/
struct ConstructorCounter
{
    static constexpr bool debug = false;
    ConstructorCounter() {
        if (debug)
        {
            std::cerr << "  default_constructor " << this << std::endl;
        }
        ++default_constructor_count;
    }
    ConstructorCounter(int unused) {
        if (debug)
        {
            std::cerr << "  other_constructor " << this << std::endl;
        }
        ++other_constructor_count;
    }
    ConstructorCounter(const ConstructorCounter& other) {
        if (debug)
        {
            std::cerr << "  copy_constructor " << this << std::endl;
        }
        ++copy_constructor_count;
    }
    ConstructorCounter(ConstructorCounter&& other) {
        if (debug)
        {
            std::cerr << "  move_constructor " << this << std::endl;
        }
        ++move_constructor_count;}
    ~ConstructorCounter() {
        if (debug)
        {
            std::cerr << "  destructor " << this << std::endl;
        }
        ++destructor_count;
    }

    const ConstructorCounter& operator=(const ConstructorCounter& other)
    {
        ++copy_assignment_count;
        return *this;
    }

    ConstructorCounter& operator=(ConstructorCounter&& other)
    {
        ++move_assignment_count;
        return *this;
    }

    friend void swap(ConstructorCounter& a, ConstructorCounter& b)
    {
        ++swap_count;
    }

    static void reset()
    {
        default_constructor_count = 0;
        other_constructor_count = 0;;
        copy_constructor_count = 0;
        move_constructor_count = 0;
        destructor_count = 0;
        swap_count = 0;
        copy_assignment_count = 0;
        move_assignment_count = 0;
    }

    static std::size_t any_constructor_count()
    {
        return default_constructor_count + other_constructor_count + copy_constructor_count + move_constructor_count;
    }

    static std::size_t default_constructor_count;
    static std::size_t other_constructor_count;
    static std::size_t copy_constructor_count;
    static std::size_t move_constructor_count;
    static std::size_t destructor_count;
    static std::size_t swap_count;
    static std::size_t copy_assignment_count;
    static std::size_t move_assignment_count;
};

#endif  // CONSTRUCTOR_COUNTER_HPP_GUARD
