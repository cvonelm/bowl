// SPDX-License-Identifier: MIT

#pragma once

#include <bowl/exception.hpp>
#include <bowl/unexpected.hpp>

namespace bowl
{

/**
 *
 * Expected<T, E>: a container which can either contain a success object of type T or an
 * error object of type E.
 *
 * E _has_ to be derived from bowl::Error
 */
template <class T, class E>
class Expected
{
public:
    /**
     *
     * Constructs a Expected from Unexpected<E>, consuming it.
     *
     * This is used for returning the error case:
     *
     * Expected<OkCase, ErrorCase> foobar()
     * {
     *     return Unexpected(ErrorCase("I'm an error!");
     * }
     */
    Expected(Unexpected<E>&& e) : ok_(false), e_(std::move(e.unpack())), is_moved_(false)
    {
    }

    /**
     *
     * Construct an Expected from T, consuming (->moving) it.
     *
     * This is used for the success case.
     */
    Expected(T&& t) : ok_(true), t_(std::move(t)), is_moved_(false)
    {
    }

    /**
     * No copies, we don't know if the underlying T and E can be copied.
     */
    Expected(Expected<T, E>&) = delete;
    Expected<T, E>& operator=(Expected<T, E>&) = delete;

    Expected(Expected<T, E>&& other)
    {
        this->is_moved_ = other.is_moved_;

        if (!other.is_moved_)
        {
            if (other.ok_)
            {
                this->t_ = std::move(other.t_);
            }
            else
            {
                this->e_ = std::move(other.e_);
            }
        }

        this->ok_ = other.ok_;
        other.is_moved_ = true;
    }

    Expected<T, E>& operator=(Expected<T, E>&& other)
    {
        this->is_moved_ = other.is_moved_;

        if (!other.is_moved_)
        {
            if (other.ok_)
            {
                this->t_ = std::move(other.t_);
            }
            else
            {
                this->e_ = std::move(other.e_);
            }
        }

        this->ok_ = other.ok_;

        other.is_moved_ = true;
        return *this;
    }

    bool ok()
    {
        return ok_;
    }

    /**
     *
     * Return the success object if this Expected is ok()
     *
     * Throws MovedOutException if object has already been unpacked.
     * Throws FalseStateException if this Expected contains an error.
     */
    T&& unpack_ok()
    {
        check_if_moved();

        if (!ok_)
        {
            throw UnpackOkIfErrorException(e_);
        }

        is_moved_ = true;
        return std::move(t_);
    }

    /**
     *
     * Return the failure object if this Expected is !ok()
     *
     * Throws MovedOutException if object has already been unpacked.
     * Throws FalseStateException if this Expected contains a success.
     */
    E&& unpack_error()
    {
        check_if_moved();

        if (ok_)
        {
            throw UnpackErrorIfOkException();
        }

        is_moved_ = true;
        return std::move(e_);
    }

    /**
     *
     * If this Expected is !ok(), throw the contained Error
     * via its throw_as_exception() method.
     *
     * If this Expected is ok(), do nothing.
     *
     * Throws MovedOutException if this Expected has already succesfully
     * been unpacked.
     */
    void throw_if_error()
    {
        if (!ok_)
        {
            check_if_moved();

            is_moved_ = true;
            e_.throw_as_exception();
        }
    }

    ~Expected()
    {
        if (!ok_)
        {
            e_.~E();
        }
        else
        {
            t_.~T();
        }
    }

private:
    void check_if_moved()
    {

        if (is_moved_)
        {
            throw MovedOutException();
        }
    }

    bool ok_;

    union
    {
        T t_;
        E e_;
    };

    bool is_moved_;
};
} // namespace bowl
