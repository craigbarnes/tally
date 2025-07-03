#ifndef MACROS_H
#define MACROS_H

#if !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 199901L)
#error C99 compiler required
#endif

#define DO_PRAGMA(x) _Pragma(#x)
#define VERSION_GE(L, l, R, r) ((L > R) || ((L == R) && (l >= r)))

#if defined(__GNUC__) && defined(__GNUC_MINOR__)
    #define GNUC_AT_LEAST(x, y) VERSION_GE(__GNUC__, __GNUC_MINOR__, x, y)
#else
    #define GNUC_AT_LEAST(x, y) 0
#endif

#ifdef __has_extension
    #define HAS_EXTENSION(x) __has_extension(x)
#else
    #define HAS_EXTENSION(x) 0
#endif

#ifdef __has_attribute
    #define HAS_ATTRIBUTE(x) __has_attribute(x)
#else
    #define HAS_ATTRIBUTE(x) 0
#endif

#ifdef __has_builtin
    #define HAS_BUILTIN(x) __has_builtin(x)
#else
    #define HAS_BUILTIN(x) 0
#endif

#if GNUC_AT_LEAST(3, 0) || HAS_ATTRIBUTE(unused) || defined(__TINYC__)
    #define UNUSED __attribute__((__unused__))
#else
    #define UNUSED
#endif

#if GNUC_AT_LEAST(3, 0) || HAS_ATTRIBUTE(format)
    #define PRINTF(x) __attribute__((__format__(__printf__, (x), (x + 1))))
    #define VPRINTF(x) __attribute__((__format__(__printf__, (x), 0)))
#else
    #define PRINTF(x)
    #define VPRINTF(x)
#endif

#if (GNUC_AT_LEAST(3, 0) || HAS_BUILTIN(__builtin_expect)) && defined(__OPTIMIZE__)
    #define likely(x) __builtin_expect(!!(x), 1)
    #define unlikely(x) __builtin_expect(!!(x), 0)
#else
    #define likely(x) (x)
    #define unlikely(x) (x)
#endif

#if GNUC_AT_LEAST(4, 3) || HAS_ATTRIBUTE(hot)
    #define HOT __attribute__((__hot__))
#else
    #define HOT
#endif

#if GNUC_AT_LEAST(4, 3) || HAS_ATTRIBUTE(cold)
    #define COLD __attribute__((__cold__))
#else
    #define COLD
#endif

#if GNUC_AT_LEAST(4, 5) || HAS_BUILTIN(__builtin_unreachable)
    #define UNREACHABLE() __builtin_unreachable()
#else
    #define UNREACHABLE()
#endif

#if __STDC_VERSION__ >= 201112L
    #define noreturn _Noreturn
#elif GNUC_AT_LEAST(3, 0)
    #define noreturn __attribute__((__noreturn__))
#else
    #define noreturn
#endif

#if __STDC_VERSION__ >= 202311L
    // static_assert is a standard keyword in ISO C23 (§6.4.1) and the
    // second argument is optional (§6.7.11)
#elif __STDC_VERSION__ >= 201112L
    // ISO C11 provides _Static_assert (§6.4.1), with a mandatory second
    // argument (§6.7.10)
    #define static_assert(x) _Static_assert((x), #x)
#elif GNUC_AT_LEAST(4, 6) || HAS_EXTENSION(c_static_assert)
    // GCC 4.6+ and Clang allow _Static_assert (as a language extension),
    // even in pre-C11 standards modes:
    // https://clang.llvm.org/docs/LanguageExtensions.html#c11-static-assert
    #define static_assert(x) __extension__ _Static_assert((x), #x)
#else
    #define static_assert(x)
#endif

#if GNUC_AT_LEAST(4, 2) || defined(__clang__)
    #define DISABLE_WARNING(wflag) DO_PRAGMA(GCC diagnostic ignored wflag)
#else
    #define DISABLE_WARNING(wflag)
#endif

#endif
