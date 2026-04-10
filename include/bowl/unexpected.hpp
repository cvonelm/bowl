// SPDX-FileCopyrightText: 2026 Christian von Elm <christian.von_elm@tu-dresden.de>
//
// SPDX-License-Identifier: MIT

#pragma once

#include <bowl/exception.hpp>

#include <utility>

namespace bowl
{

/**
 *
 * Unexpected<E>: A convenience class for construction Error states.
 *
 * A !ok() MaybeError<E>  or !ok() Expected can be constructed from
 * the Unexpected<E>.
 */
template <class E>
class Unexpected
{
public:
    Unexpected(E&& e) noexcept : e_(std::move(e)), is_moved_(false)
    {
    }

    Unexpected() = delete;

    Unexpected(Unexpected<E>& other) = delete;
    Unexpected<E>& operator=(Unexpected<E>&) = delete;

    Unexpected<E>& operator=(Unexpected<E>&& other) noexcept
    {
        this->e_ = std::move(other.e_);
        this->is_moved_ = other.is_moved_;

        other.is_moved_ = true;

        return *this;
    }

    Unexpected(Unexpected<E>&& other) noexcept : is_moved_(other.is_moved_), e_(other.e_)

    {
        other.is_moved_ = true;
    }

    /**
     *
     * Unpacks the Unexpected, consuming it.
     *
     * Throws MovedOutException, if this Unexpected has already been consumed.
     */
    E&& unpack()
    {
        if (is_moved_ == true)
        {
            throw MovedOutException();
        }

        is_moved_ = true;
        return std::move(e_);
    }

    ~Unexpected()
    {
        e_.~E();
    }

private:
    union
    {
        E e_;
        char placeholder;
    };

    bool is_moved_;
};
} // namespace bowl
