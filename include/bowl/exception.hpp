#pragma once

#include <exception>

namespace bowl
{
class MovedOutException : public std::exception
    {
        public:
            const char *what() const noexcept override
            {
                return "Accessing already moved out type!";
            }
    };

class FalseStateException : public std::exception
    {
        public:
            const char *what() const noexcept override
            {
                return "Trying to unpack an error/ok-state that was not an error/ok-state";
            }
    };

}
