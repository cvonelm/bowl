#include <bowl/error.hpp>

namespace bowl
{
ErrnoException::ErrnoException(ErrnoError err) : errno_(err.errnum())
{
}

CustomException::CustomException(CustomError err) : err_(err.display())
{
}
} // namespace bowl
