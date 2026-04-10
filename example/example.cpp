// SPDX-FileCopyrightText: 2026 Christian von Elm <christian.von_elm@tu-dresden.de>
//
// SPDX-License-Identifier: MIT

#include <bowl/error.hpp>
#include <bowl/expected.hpp>

#include <cmath>
#include <iostream>

class NegativeNumberError : bowl::Error
{
public:
    std::string display() const override
    {
        return "Can not take root of negative number!";
    }
};

bowl::Expected<int, NegativeNumberError> root(int num)
{
    if (num < 0)
    {
        return bowl::Unexpected(NegativeNumberError{});
    }
    return sqrt(num);
}

int main(void)
{
    int num;
    std::cout << "Give a number to take a root of: ";
    std::cin >> num;

    auto res = root(num);

    if (res.ok())
    {
        std::cout << "The root of your number is: " << res.unpack_ok() << std::endl;
        return 0;
    }
    else
    {
        std::cout << "Failed to take root of your number: " << res.unpack_error().display();
        return -1;
    }
}
