#include "./array.hpp"

#include <iostream>

int
main()
{
    pel::array<int, 5> myArr{5, 4, 3, 2, 1};
    myArr.replace_back(10);
    std::size_t var = myArr.length();

    for(auto& data : myArr)
    {
        std::cout << data << "\n";
    }

    return static_cast<int>(var);
}
