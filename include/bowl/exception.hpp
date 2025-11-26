// SPDX-License-Identifier: MIT

#pragma once

#include <exception>

namespace bowl
{
/**
 * Exception thrown if you unpack_(ok/error) a MaybeError or Expected
 * that has already been unpacked.
 */
class MovedOutException : public std::exception
{
public:
    const char* what() const noexcept override
    {
        return "Accessing already moved out type!";
    }
};

/**
 *
 * Exception thrown if you try to unpack_ok() an Expected that is !ok(), or
 * unpack_error() an Expected that is ok()
 */
class FalseStateException : public std::exception
{
public:
    const char* what() const noexcept override
    {
        return "Trying to unpack an error/ok-state that was not an error/ok-state";
    }
};
} // namespace bowl
