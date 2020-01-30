#pragma once

#ifdef __GNUC__

#define ATTRIBUTE_HOT __attribute__((hot))
#define ATTRIBUTE_OPTIMIZE(optString) __attribute__((optimize(optString)))

#else

#define ATTRIBUTE_HOT
#define ATTRIBUTE_OPTIMIZE(optString)

#endif
