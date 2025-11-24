#include "bowl/error.hpp"

namespace bowl
{
    ErrnoException::ErrnoException(ErrnoError err) : errno_(err.errnum())
    {
    }
}
