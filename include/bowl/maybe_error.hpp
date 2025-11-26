// SPDX-License-Identifier: MIT

#pragma once

#include <bowl/exception.hpp>
#include <bowl/unexpected.hpp>

namespace bowl
{

/**
 *
 * MaybeError<E>: either indicates ok() with no further information or !ok(),
 * and contains an Error object of type E for more information.
 */
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

    MaybeError(MaybeError<E>&) = delete;
    MaybeError<E>& operator=(MaybeError<E>&) = delete;

    MaybeError(MaybeError<E>&& other)
    {
        this->ok_ = other.ok_;

        this->is_moved_ = other.is_moved_;

        if (!other.is_moved_)
        {
            this->e_ = std::move(other.e_);
        }

        other.is_moved_ = true;
    }

    MaybeError& operator=(MaybeError<E>&& other)
    {
        this->ok_ = other.ok_;

        this->is_moved_ = other.is_moved_;

        if (!other.is_moved_)
        {
            this->e_ = std::move(other.e_);
        }
        other.is_moved_ = true;

        return *this;
    }

    /**
     *
     * Checks if this MaybeError<E> is ok()
     */
    bool ok()
    {
        return ok_;
    }

    /**
     *
     * If this MaybeError is !ok(), consume it and return the
     * contained error.
     *
     * Throws FalseStateException if unpack_error() is called
     * on a ok() MaybeError.
     *
     * Throws MovedOutException if this MaybeError has already been
     * consumed.
     */
    E&& unpack_error()
    {
        check_is_moved();

        if (ok_)
        {
            throw FalseStateException();
        }
        is_moved_ = true;

        return std::move(e_);
    }

    /**
     *
     * If this MaybeError is !ok(), throw the contained
     * error via its throw_as_exception() method.
     *
     * Does nothing if MaybeError is ok().
     *
     * Throws MovedOutException if this MaybeError has
     * already been consumed.
     */
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
        if (is_moved_)
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
} // namespace bowl
