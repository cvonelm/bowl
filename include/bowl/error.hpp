#pragma once

#include <system_error>
#include <string>

#include <cerrno>
#include <cstring>

namespace bowl
{
enum class Errno : int
{
    NOMEM = ENOMEM
};
    class Error
    {
        public:
        virtual std::string display() = 0;
        virtual void throw_as_exception() = 0;
    };
class ErrnoError;
class ErrnoException : std::exception
{
    public:
    ErrnoException(ErrnoError err);

    const char * what() const noexcept override
    {
        return strerror(static_cast<int>(errno_));
    }
    protected:
    Errno errno_;

};
    class ErrnoError : public Error
    {
        public:
        ErrnoError() : errno_(static_cast<Errno>(errno))
        {
        }

        std::string display()
        {
            return strerror(static_cast<int>(errno_));
        }

        enum Errno errnum()
        {
            return errno_;
        }

        [[noreturn]] void throw_as_exception()
        {
            throw ErrnoError(*this);
        }

        private:
        enum Errno errno_;

    };
class CustomError : public Error
{
public:

    CustomError(std::string str) : str_(str)
    {
    }

    std::string display() override
    {
        return str_;
    }

    [[noreturn]] void throw_as_exception() override
    {
        throw std::system_error(errno, std::system_category());
    }

private:
    CustomError() {}
    std::string str_;
};

} // namespace lo2s
