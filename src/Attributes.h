#pragma once

/// Include file for portable usage of __attribute__

#ifdef __MINGW32__
#define ATTRIBUTE_FORMAT_PRINTF(stringIndex, firstToCheck) __attribute__((format(__MINGW_PRINTF_FORMAT, stringIndex, firstToCheck)))
#elif defined(__GNUC__)
#define ATTRIBUTE_FORMAT_PRINTF(stringIndex, firstToCheck) __attribute__((format(printf, stringIndex, firstToCheck)))
#else
#define ATTRIBUTE_FORMAT_PRINTF(stringIndex, firstToCheck)
#endif

#ifdef __GNUC__

#define ATTRIBUTE_HOT __attribute__((hot))
#define LIKELY(condition) __builtin_expect((condition), 1)
#define UNLIKELY(condition) __builtin_expect((condition), 0)
#define PREFETCH(address, isWrite, locality) __builtin_prefetch((address), (isWrite), (locality))

#else

#define ATTRIBUTE_HOT
#define LIKELY(condition) condition
#define UNLIKELY(condition) condition
#define PREFETCH(address, isWrite, locality)

#endif
