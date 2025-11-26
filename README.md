# 🎳 bowl -- When you do not want to throw

This is a lightweight set of classes for programming C++ without throwing exceptions for those of us still living in the C++17 world.


## Example

Here is a complete, albeit a tad synthetic example:

```cpp
#include <bowl/expected.hpp>
#include <bowl/error.hpp>

#include <iostream>
#include <cmath>

class NegativeNumberError : bowl::Error
{
public:
    std::string display() const override
    {
        return "Can not take root of negative number!";
    }

    [[noreturn]] void throw_as_exception() const override
    {
        throw std::runtime_error(display());
    }
};


bowl::Expected<int, NegativeNumberError> root(int num)
{
    if(num < 0)
    {
        return bowl::Unexpected(NegativeNumberError{});
    }
    return sqrt(num);
}

int main(void)
{
    int num;
    std::cout << "Give a number to take a root of: ";
    std::cin >> num;

    auto res = root(num);

    if(res.ok())
    {
    std::cout << "The root of your number is: " << res.unpack_ok() << std::endl;
    return 0;
    }
    else
    {
       std::cout << "Failed to take root of your number: " << res.unpack_error().display();
       return -1;
    }

}
```

## Building

This library is build using CMake.

Currently, the only option to use it, is to `add_subdirectory()` this repository to your `CMakeLists.txt` and
then link to the `bowl` library.

If you want to run the tests or the example application:

```sh
mkdir build
cd build
cmake ..
make

#Tests
make test

# Example application
./example
```
## Documentation
This package offers two classes for returning errors without throwing: `Expected<T, E>` and `MaybeError<E>`.
`Expected<T, E>` is the one to use when you either want to return a value `T` or an error `E`.
`MaybeError<E>` is the correct choice when you want to indicate success or a specific error.

`Unexpected<E>` is a helper class for easier creaton of the error case with `Expected<T, E>`.

This allows you to write code as:


```cpp

Expected<FoobarReturnClass, FoobarError> foobar()
{
    [...]
    if(err)
    {
        return Unexpected(FoobarError("Error errored!"));
    }

    return ok_object;
}
```

`Expected<T, E>`, `Unexpected<E>` and `MaybeError<E>` consume the objects of `T` and `E` given to them and
in turn if they are unpacked with `unpack_(ok/error)()` they are consumed.

All objects are moved in and outside of these clases with rvalue-references.
Most noticeably, this means that you can not unpack the same Expected twice.

`bowl` expects that you use a derived class of `bowl::Error` for the `E` template argument.

The derived class needs to implement to virtual functions:
- `std::string display() const` Create a human-readable string for displaying the error
- `[[noreturn]] void throw_as_exception() const` Create an exception from this error and throw it.

`bowl` contains two predefined Error Types:
- `ErrnoError` creates a `bowl::Error` from the current value of `errno`
- `CustomError` creates a `bowl::Error` from a given string.
