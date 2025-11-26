// SPDX-License-Identifier: MIT

#pragma once

#include <string>

#include <cerrno>
#include <cstring>

namespace bowl
{

/**
 * Type-safe enum for all Linux errno #defines
 */
enum class Errno : int
{
    NOMEM = ENOMEM,
    PERM = EPERM,
    NOENT = ENOENT,
    SRCH = ESRCH,
    INTR = EINTR,
    IO = EIO,
    NXIO = ENXIO,
    TOOBIG = E2BIG,
    NOEXEC = ENOEXEC,
    BADF = EBADF,
    CHILD = ECHILD,
    AGAIN = EAGAIN,
    ACCES = EACCES,
    FAULT = EFAULT,
    NOTBLK = ENOTBLK,
    BUSY = EBUSY,
    EXIST = EEXIST,
    XDEV = EXDEV,
    NODEV = ENODEV,
    NOTDIR = ENOTDIR,
    ISDIR = EISDIR,
    INVAL = EINVAL,
    NFILE = ENFILE,
    MFILE = EMFILE,
    NOTTY = ENOTTY,
    TXTBSY = ETXTBSY,
    FBIG = EFBIG,
    NOSPC = ENOSPC,
    SPIPE = ESPIPE,
    ROFS = EROFS,
    MLINK = EMLINK,
    PIPE = EPIPE,
    DOM = EDOM,
    RANGE = ERANGE,
    DEADLK = EDEADLK,
    NAMETOOLONG = ENAMETOOLONG,
    NOLCK = ENOLCK,
    NOSYS = ENOSYS,
    NOTEMPTY = ENOTEMPTY,
    LOOP = ELOOP,
    WOULDBLOCK = EWOULDBLOCK,
    NOMSG = ENOMSG,
    IDRM = EIDRM,
    CHRNG = ECHRNG,
    L2NSYNC = EL2NSYNC,
    L3HLT = EL3HLT,
    L3RST = EL3RST,
    LNRNG = ELNRNG,
    UNATCH = EUNATCH,
    NOCSI = ENOCSI,
    L2HLT = EL2HLT,
    BADE = EBADE,
    BADR = EBADR,
    XFULL = EXFULL,
    NOANO = ENOANO,
    BADRQC = EBADRQC,
    BADSLT = EBADSLT,
    DEADLOCK = EDEADLOCK,
    BFONT = EBFONT,
    NOSTR = ENOSTR,
    NODATA = ENODATA,
    TIME = ETIME,
    NOSR = ENOSR,
    NONET = ENONET,
    NOPKG = ENOPKG,
    REMOTE = EREMOTE,
    NOLINK = ENOLINK,
    ADV = EADV,
    SRMNT = ESRMNT,
    COMM = ECOMM,
    PROTO = EPROTO,
    MULTIHOP = EMULTIHOP,
    DOTDOT = EDOTDOT,
    BADMSG = EBADMSG,
    OVERFLOW = EOVERFLOW,
    NOTUNIQ = ENOTUNIQ,
    BADFD = EBADFD,
    REMCHG = EREMCHG,
    LIBACC = ELIBACC,
    LIBBAD = ELIBBAD,
    LIBSCN = ELIBSCN,
    LIBMAX = ELIBMAX,
    LIBEXEC = ELIBEXEC,
    ILSEQ = EILSEQ,
    RESTART = ERESTART,
    STRPIPE = ESTRPIPE,
    USERS = EUSERS,
    NOTSOCK = ENOTSOCK,
    DESTADDRREQ = EDESTADDRREQ,
    MSGSIZE = EMSGSIZE,
    PROTOTYPE = EPROTOTYPE,
    NOPROTOOPT = ENOPROTOOPT,
    PROTONOSUPPORT = EPROTONOSUPPORT,
    SOCKTNOSUPPORT = ESOCKTNOSUPPORT,
    OPNOTSUPP = EOPNOTSUPP,
    PFNOSUPPORT = EPFNOSUPPORT,
    AFNOSUPPORT = EAFNOSUPPORT,
    ADDRINUSE = EADDRINUSE,
    ADDRNOTAVAIL = EADDRNOTAVAIL,
    NETDOWN = ENETDOWN,
    NETUNREACH = ENETUNREACH,
    NETRESET = ENETRESET,
    CONNABORTED = ECONNABORTED,
    CONNRESET = ECONNRESET,
    NOBUFS = ENOBUFS,
    ISCONN = EISCONN,
    NOTCONN = ENOTCONN,
    SHUTDOWN = ESHUTDOWN,
    TOOMANYREFS = ETOOMANYREFS,
    TIMEDOUT = ETIMEDOUT,
    CONNREFUSED = ECONNREFUSED,
    HOSTDOWN = EHOSTDOWN,
    HOSTUNREACH = EHOSTUNREACH,
    ALREADY = EALREADY,
    INPROGRESS = EINPROGRESS,
    STALE = ESTALE,
    UCLEAN = EUCLEAN,
    NOTNAM = ENOTNAM,
    NAVAIL = ENAVAIL,
    ISNAM = EISNAM,
    REMOTEIO = EREMOTEIO,
    DQUOT = EDQUOT,
    NOMEDIUM = ENOMEDIUM,
    MEDIUMTYPE = EMEDIUMTYPE,
    CANCELED = ECANCELED,
    NOKEY = ENOKEY,
    KEYEXPIRED = EKEYEXPIRED,
    KEYREVOKED = EKEYREVOKED,
    KEYREJECTED = EKEYREJECTED,
    OWNERDEAD = EOWNERDEAD,
    NOTRECOVERABLE = ENOTRECOVERABLE,
    RFKILL = ERFKILL,
    HWPOISON = EHWPOISON,
};

/**
 *
 * Base class for all Error types `E` in Expected<T, E>, MaybeError<E>,...
 */
class Error
{
public:
    /**
     * Give a human-readable representation of the error.
     */
    virtual std::string display() const = 0;
    /**
     *
     * Throw the given Error type as a corresponding exception.
     */
    virtual void throw_as_exception() const = 0;
};
class ErrnoError;

/**
 *
 * Corresponding exception to ErrnoError, thrown on ErrnoError::throw_as_exception()
 */
class ErrnoException : std::exception
{
public:
    ErrnoException(ErrnoError err);

    const char* what() const noexcept override
    {
        return strerror(static_cast<int>(errno_));
    }

protected:
    Errno errno_;
};

/**
 *
 * class Error wrapper around Unix `errno`s
 */
class ErrnoError : public Error
{
public:
    ErrnoError() : errno_(static_cast<Errno>(errno))
    {
    }

    std::string display() const override
    {
        return strerror(static_cast<int>(errno_));
    }

    enum Errno errnum()
    {
        return errno_;
    }

    [[noreturn]] void throw_as_exception() const override
    {
        throw ErrnoException(*this);
    }

private:
    enum Errno errno_;
};

class CustomError;

/**
 *
 * Corresponding exception type to CustomError
 */
class CustomException : public std::exception
{
public:
    CustomException(CustomError err);

    const char* what() const noexcept override
    {
        return err_.c_str();
    }

protected:
    std::string err_;
};

/**
 *
 * class Error specialization for just giving an error with a custom message
 */
class CustomError : public Error
{
public:
    CustomError(std::string str) : str_(str)
    {
    }

    std::string display() const override
    {
        return str_;
    }

    [[noreturn]] void throw_as_exception() const override
    {
        throw CustomException(*this);
    }

private:
    CustomError()
    {
    }

    std::string str_;
};

} // namespace bowl
