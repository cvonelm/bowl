#pragma once

#include <bowl/unexpected.hpp>
#include <bowl/exception.hpp>

#include <stdexcept>

namespace bowl
{

#define CHECK_ERROR(op, fmtstring)                                                                 \
    {                                                                                              \
        auto res = op;                                                                             \
        if (!res.ok())                                                                             \
        {                                                                                          \
            return Unexpected(Error::contextualize(fmtstring, op.unpack_error()));                 \
        }                                                                                          \
    }

template <class E>
class MaybeError
{
public:
    MaybeError(E&& e) : ok_(false), is_moved_(false), e_(std::move(e))
    {
    }

    MaybeError(Unexpected<E>&& e) : ok_(false), is_moved_(false), e_(std::move(e.unpack()))
    {
    }

    MaybeError() : ok_(true), is_moved_(false)
    {
    }

    bool ok()
    {
        return ok_;
    }

    E&& unpack_error()
    {
        check_is_moved();

        if (ok_)
        {
            throw std::runtime_error("Can not get Error from MaybeError if status is ok!");
        }
        is_moved_ = true;

        return std::move(e_);
    }

    void throw_if_error()
    {
        if (!ok_)
        {
            check_is_moved();
            is_moved_ = true;
            e_.throw_as_exception();
        }
    }

    ~MaybeError()
    {
        if (!ok_)
        {
            e_.~E();
        }
    }

private:
    void check_is_moved()
    {
        if(is_moved_)
        {
            throw MovedOutException();
        }
    }

    bool ok_;
    bool is_moved_;

    union
    {
        E e_;
        char placeholder_;
    };
};
} // namespace lo2s
