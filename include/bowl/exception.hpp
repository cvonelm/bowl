// SPDX-License-Identifier: MIT

#pragma once

#include <exception>
#include <string>

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
 * Exception thrown if you try to unpack_error() an Expected/MaybeError
 * that is actually ok()
 */
class UnpackErrorIfOkException : public std::exception
{
public:
    const char* what() const noexcept override
    {
        return "Trying to access unpack_error(), but object was in ok() state!";
    }
};

/**
 *
 * Exception thrown if you try to unpack_ok() an Expected/MaybeError
 * that is actually !ok()
 */
template <class E>
class UnpackOkIfErrorException : public std::exception
{
public:
    UnpackOkIfErrorException(const E& err)
    {
        what_ =
            "Trying to access unpack_ok() but object was in !ok() state, error: " + err.display();
    }

    const char* what() const noexcept override
    {
        return what_.c_str();
    }

private:
    std::string what_;
};

} // namespace bowl
