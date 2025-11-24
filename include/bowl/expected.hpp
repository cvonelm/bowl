#pragma once

#include "bowl/exception.hpp"
#include <bowl/unexpected.hpp>

namespace bowl
{
template <class T, class E>
class Expected
{
public:
    Expected(Unexpected<E>&& e) : ok_(false), e_(std::move(e.unpack())), is_moved_(false)
    {
    }
    
    Expected(T&& t) : ok_(true), t_(std::move(t)), is_moved_(false)
    {
    }


    Expected(Expected<T, E>&) = delete;
    Expected<T, E>& operator=(Expected<T, E>&) = delete;

    Expected(Expected<T, E>&& other)
    {
        this->is_moved_ = other.is_moved_;
        if(!other.is_moved_)
        {
        if(other.ok_)
        {
            this->t_ = other.t_;
        }
        else {
            this->e_ = other.e_;
        }
        }

        this->ok_ = other.ok_;
    }

    Expected<T, E> & operator=(Expected<T, E>&& other)
    {
        this->is_moved_ = other.is_moved_;

        if(!other.is_moved_)
        {
        if(other.ok_)
        {
            this->t_ = std::move(other.t_);
        }
        else {
            this->e_ = std::move(other.e_);
        }
        }

        this->ok_ = other.ok_;

        return *this;
    }

    bool ok()
    {
        return ok_;
    }

    T&& unpack_ok()
    {
        if (is_moved_)
        {
            throw MovedOutException();
        }

        if (!ok_)
        {
            throw FalseStateException();
        }

        is_moved_ = true;
        return t_;
    }

    E&& unpack_error()
    {
        if (is_moved_)
        {
            throw MovedOutException();
        }

        if (ok_)
        {
            throw FalseStateException();
        }

        is_moved_ = true;
        return e_;
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
    bool ok_;

    union
    {
        T t_;
        E e_;
    };

    bool is_moved_;
};
}
