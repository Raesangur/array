#include "./array.hpp"

#include <iostream>

// constexpr std::size_t
// foo()
//{
//    pel::array<int, 5> myArr{5, 4, 3, 2, 1};
//    myArr.replace_back(10);
//    constexpr std::size_t var = myArr.length();
//
//    return var;
//}

template<typename T>
struct AAAA
{
    constexpr AAAA(T* ptr) : pointer{ptr}
    {
    }
    T* pointer;

    constexpr virtual std::ptrdiff_t
    operator-(const AAAA& other)
    {
        return pointer - other.pointer;
    }
};

template<typename T>
struct ptrType : public AAAA<T>
{
    constexpr ptrType(T* ptr) : AAAA{ptr}
    {
    }

    
};

constexpr std::size_t
foo()
{
    int array[5] = {0};

    AAAA<int> ptr1{&array[0]};
    AAAA<int> ptr2{&array[5]};
    // pel::iterator_base<int> it1{array};
    // pel::iterator_base<int> it2{array + (sizeof(array) / sizeof(int))};

    return ptr2 - ptr1;
}

int
main()
{
    constexpr std::size_t var = foo();
    ;

    return static_cast<int>(var);
}
