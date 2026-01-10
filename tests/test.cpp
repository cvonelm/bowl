// SPDX-License-Identifier: MIT

#include <bowl/error.hpp>
#include <bowl/exception.hpp>
#include <bowl/expected.hpp>
#include <bowl/macros.hpp>
#include <bowl/maybe_error.hpp>
#include <bowl/unexpected.hpp>

#include <catch2/catch_test_macros.hpp>

// Count how often both constructors of ErrorCase and OkCase have been called,
// so we can check that the move semantics work correctly.
uint64_t num_constructed = 0;
uint64_t num_copy_constructed = 0;

class CustomException : public std::exception
{
public:
    const char* what() const noexcept override
    {
        return "I am a little custom exception type";
    }
};

class ErrorCase : public bowl::Error
{
public:
    explicit ErrorCase()
    {
        num_constructed++;
    }

    ErrorCase(ErrorCase& ErrorCase)
    {
        num_copy_constructed++;
    }

    ErrorCase& operator=(ErrorCase&)
    {
        num_copy_constructed++;
        return *this;
    }

    [[noreturn]] void throw_as_exception() const override
    {
        throw CustomException();
    }

    std::string display() const override
    {
        return "I'm a little custom error case";
    }

    ErrorCase(ErrorCase&&) = default;
    ErrorCase& operator=(ErrorCase&&) = default;

    int errnum = 0;
};

class OkCase
{
public:
    explicit OkCase()
    {
        num_constructed++;
    }

    OkCase(OkCase& OkCase)
    {
        num_copy_constructed++;
    }

    OkCase& operator=(OkCase&)
    {
        num_copy_constructed++;
        return *this;
    }

    OkCase(OkCase&&) = default;
    OkCase& operator=(OkCase&&) = default;

    int payload = 0;
};

/* Unexpected */
TEST_CASE("Can unpack Unexpected", "[can_unpack_unexpected]")
{
    num_constructed = 0;
    num_copy_constructed = 0;

    ErrorCase ec;
    ec.errnum = 42;
    bowl::Unexpected<ErrorCase> unexp(std::move(ec));

    ErrorCase ec2;
    REQUIRE_NOTHROW(ec2 = unexp.unpack());

    REQUIRE(ec2.errnum == 42);

    REQUIRE_THROWS_AS(unexp.unpack(), bowl::MovedOutException);

    REQUIRE(num_constructed == 2);
    REQUIRE(num_copy_constructed == 0);
}

TEST_CASE("Can move unexpected", "[can_move_expected]")
{
    num_constructed = 0;
    num_copy_constructed = 0;

    ErrorCase ec;
    ec.errnum = 42;
    bowl::Unexpected<ErrorCase> unexp(std::move(ec));

    bowl::Unexpected<ErrorCase> unexp2 = std::move(unexp);

    // NOLINTBEGIN(clang-analyzer-cplusplus.Move)
    REQUIRE_THROWS_AS(unexp.unpack(), bowl::MovedOutException);
    // NOLINTEND(clang-analyzer-cplusplus.Move)

    ErrorCase ec2;

    REQUIRE_NOTHROW(ec2 = unexp2.unpack());

    REQUIRE_THROWS_AS(unexp2.unpack(), bowl::MovedOutException);

    REQUIRE(ec2.errnum == 42);

    REQUIRE(num_constructed == 2);
    REQUIRE(num_copy_constructed == 0);
}

/* MaybeError */
TEST_CASE("MaybeError works", "[maybe_error_works]")
{
    num_constructed = 0;
    num_copy_constructed = 0;

    ErrorCase ec;

    ec.errnum = 42;

    bowl::MaybeError<ErrorCase> e{ std::move(ec) };

    REQUIRE(!e.ok());
    ErrorCase err = e.unpack_error();

    REQUIRE(err.errnum == 42);
    REQUIRE(num_constructed == 1);
    REQUIRE(num_copy_constructed == 0);
}

TEST_CASE("MaybeError::ok() work", "[maybe_error_ok_works]")
{
    num_constructed = 0;
    num_copy_constructed = 0;

    bowl::MaybeError<ErrorCase> e{};

    REQUIRE(e.ok());

    REQUIRE_THROWS_AS(e.unpack_error(), bowl::UnpackErrorIfOkException);

    REQUIRE(num_constructed == 0);
    REQUIRE(num_copy_constructed == 0);
}

TEST_CASE("Can't unpack twice"
          "[maybe_error cant unpack twice]")
{
    num_constructed = 0;
    num_copy_constructed = 0;

    bowl::MaybeError<ErrorCase> e{ ErrorCase() };

    REQUIRE_NOTHROW(e.unpack_error());
    REQUIRE_THROWS_AS(e.unpack_error(), bowl::MovedOutException);

    REQUIRE(num_constructed == 1);
    REQUIRE(num_copy_constructed == 0);
}

TEST_CASE("Can construct MaybeError from Unexpected", "[maybe_error from Unexpected]")
{
    num_constructed = 0;
    num_copy_constructed = 0;

    bowl::MaybeError<ErrorCase> err{ bowl::Unexpected<ErrorCase>(ErrorCase()) };

    REQUIRE(!err.ok());

    REQUIRE(num_constructed == 1);
    REQUIRE(num_copy_constructed == 0);
}

TEST_CASE("MaybeError::throw_if_error works", "[maybe_error_throw_if_error() works]")
{
    num_constructed = 0;
    num_copy_constructed = 0;

    bowl::MaybeError<ErrorCase> err{ ErrorCase() };

    REQUIRE(!err.ok());
    REQUIRE_THROWS_AS(err.throw_if_error(), CustomException);

    bowl::MaybeError<ErrorCase> err2{};

    REQUIRE(err2.ok());
    REQUIRE_NOTHROW(err2.throw_if_error());

    REQUIRE(num_constructed == 1);
    REQUIRE(num_copy_constructed == 0);
}

TEST_CASE("Can't call MaybeError::throw_if_error twice", "[maybe_error_throw_if_error() works]")
{
    num_constructed = 0;
    num_copy_constructed = 0;

    bowl::MaybeError<ErrorCase> err{ ErrorCase() };

    REQUIRE(!err.ok());
    REQUIRE_THROWS_AS(err.throw_if_error(), CustomException);
    REQUIRE_THROWS_AS(err.throw_if_error(), bowl::MovedOutException);

    REQUIRE(num_constructed == 1);
    REQUIRE(num_copy_constructed == 0);
}

TEST_CASE("Move on MaybeError works", "[maybe_error_move]")
{

    num_constructed = 0;
    num_copy_constructed = 0;

    ErrorCase ec;
    ec.errnum = 42;

    bowl::MaybeError<ErrorCase> err{ std::move(ec) };
    bowl::MaybeError<ErrorCase> err2{ std::move(err) };

    // NOLINTBEGIN(clang-analyzer-cplusplus.Move)
    REQUIRE_THROWS_AS(err.unpack_error(), bowl::MovedOutException);
    // NOLINTEND(clang-analyzer-cplusplus.Move)

    REQUIRE_THROWS_AS(err.throw_if_error(), bowl::MovedOutException);

    ErrorCase ec2;
    REQUIRE_NOTHROW(ec2 = err2.unpack_error());

    REQUIRE(ec2.errnum == 42);

    REQUIRE(num_constructed == 2);
    REQUIRE(num_copy_constructed == 0);
}

/* Expected */
TEST_CASE("Can create ok Expected", "[can_create_ok_expected]")
{
    num_constructed = 0;
    num_copy_constructed = 0;

    OkCase ok;
    ok.payload = 52;

    bowl::Expected<OkCase, ErrorCase> ok_expected(std::move(ok));

    REQUIRE(ok_expected.ok());

    REQUIRE_THROWS_AS(ok_expected.unpack_error(), bowl::UnpackErrorIfOkException);

    REQUIRE_NOTHROW(ok_expected.throw_if_error());

    OkCase ok2;
    REQUIRE_NOTHROW(ok2 = ok_expected.unpack_ok());

    REQUIRE(ok.payload == 52);

    REQUIRE_THROWS_AS(ok_expected.unpack_error(), bowl::MovedOutException);
    REQUIRE_THROWS_AS(ok_expected.unpack_ok(), bowl::MovedOutException);

    REQUIRE(num_constructed == 2);
    REQUIRE(num_copy_constructed == 0);
}

TEST_CASE("Can create error Expected", "[can_create_error_expected]")
{
    num_constructed = 0;
    num_copy_constructed = 0;

    ErrorCase ec;
    ec.errnum = 52;

    bowl::Expected<OkCase, ErrorCase> err_expected(std::move(ec));

    REQUIRE(!err_expected.ok());

    REQUIRE_THROWS_AS(err_expected.unpack_ok(), bowl::UnpackOkIfErrorException<ErrorCase>);

    ErrorCase ec2;
    REQUIRE_NOTHROW(ec2 = err_expected.unpack_error());

    REQUIRE(ec2.errnum == 52);

    REQUIRE_THROWS_AS(err_expected.unpack_error(), bowl::MovedOutException);
    REQUIRE_THROWS_AS(err_expected.unpack_ok(), bowl::MovedOutException);

    REQUIRE(num_constructed == 2);
    REQUIRE(num_copy_constructed == 0);
}

TEST_CASE("Expected::throw_if_error()", "[expected_throw_if_error_works]")
{
    num_constructed = 0;
    num_copy_constructed = 0;

    ErrorCase ec;
    ec.errnum = 52;

    bowl::Expected<OkCase, ErrorCase> err_expected(std::move(ec));

    REQUIRE_THROWS_AS(err_expected.throw_if_error(), CustomException);
    REQUIRE_THROWS_AS(err_expected.throw_if_error(), bowl::MovedOutException);
    REQUIRE_THROWS_AS(err_expected.unpack_error(), bowl::MovedOutException);
    REQUIRE_THROWS_AS(err_expected.unpack_ok(), bowl::MovedOutException);
}

TEST_CASE("ErrnoError works", "[errno_error_works]")
{
    errno = ENOMEM;
    bowl::ErrnoError errno_error{};

    REQUIRE(errno_error.errnum() == bowl::Errno::NOMEM);
    REQUIRE(static_cast<int>(errno_error.errnum()) == ENOMEM);
    REQUIRE(errno_error.display() == "Cannot allocate memory");

    REQUIRE_THROWS_AS(errno_error.throw_as_exception(), bowl::ErrnoException);
}

TEST_CASE("CustomError works", "[custom_error_works]")
{
    bowl::Expected<OkCase, bowl::CustomError> exp{ bowl::Unexpected(bowl::CustomError("foobar")) };

    REQUIRE(!exp.ok());

    REQUIRE_THROWS_AS(exp.unpack_ok(), bowl::UnpackOkIfErrorException<bowl::CustomError>);

    bowl::CustomError err2 = exp.unpack_error();
    REQUIRE(err2.display() == "foobar");

    REQUIRE_THROWS_AS(err2.throw_as_exception(), bowl::CustomException);
}

bowl::Expected<int, bowl::CustomError> returning_error()
{
    return bowl::Unexpected(bowl::CustomError("I'm an error!"));
}

bowl::Expected<int, bowl::CustomError> forwarding_error()
{
    CHECK_ASSIGN(foo, returning_error());

    return foo;
}

bowl::Expected<int, bowl::CustomError> returning_value()
{
    return 42;
}

bowl::Expected<int, bowl::CustomError> forwarding_value()
{
    CHECK_ASSIGN(foo, returning_value());

    return foo;
}

TEST_CASE("CHECK_ASSIGN works")
{
    auto res = forwarding_error();

    REQUIRE(!res.ok());
    REQUIRE(res.unpack_error().display() == "I'm an error!");

    auto res2 = forwarding_value();

    REQUIRE(res2.ok());
    REQUIRE(res2.unpack_ok() == 42);
}
