#pragma once

#include <bowl/exception.hpp>
#include <utility>

namespace bowl
{
template <class E>
class Unexpected
{
public:
    Unexpected(E&& e) : e_(std::move(e)), is_moved_(false)
    {
    }
    
    Unexpected() = delete;
    
    Unexpected(Unexpected<E>& other) = delete;
    Unexpected<E> & operator=(Unexpected<E>&) = delete;

    Unexpected<E> & operator=(Unexpected<E>&&) = delete;
    Unexpected(Unexpected<E>&& other) = delete;
    
    E&& unpack()
    {
        if (is_moved_ == true)
        {
            throw MovedOutException();
        }
        is_moved_ = true;
        return std::move(e_);
    }

private:
    E e_;
    bool is_moved_;
};
}
