#pragma once

#ifdef __GNUC__

#define ATTRIBUTE_HOT __attribute__((hot))
#define ATTRIBUTE_OPTIMIZE(optString) __attribute__((optimize(optString)))
#define LIKELY(condition) __builtin_expect((condition), 1)
#define UNLIKELY(condition) __builtin_expect((condition), 0)
#define PREFETCH(address, isWrite, locality) __builtin_prefetch((address), (isWrite), (locality))

#else

#define ATTRIBUTE_HOT
#define ATTRIBUTE_OPTIMIZE(optString)
#define LIKELY(condition) condition
#define UNLIKELY(condition) condition
#define PREFETCH(address, isWrite, locality)

#endif
