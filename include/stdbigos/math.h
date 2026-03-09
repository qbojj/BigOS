#ifndef _STDBIGOS_MATH_H
#define _STDBIGOS_MATH_H

#include "types.h"

#define POW2(x) (1ull << (x))

#define ALIGN_DOWN(x, align)     ((x) / (align) * (align))
#define ALIGN_DOWN_POW2(x, pow2) ALIGN_DOWN(x, POW2(pow2))
#define ALIGN_UP(x, align)       ALIGN_DOWN((x) + (align) - 1, align)
#define ALIGN_UP_POW2(x, pow2)   ALIGN_UP(x, POW2(pow2))

#define IS_ALIGNED(ptr, align)     ((reg_t)(x) == ALIGN_DOWN((reg_t)(x), align))
#define IS_ALIGNED_POW2(ptr, pow2) ((reg_t)(x) == ALIGN_DOWN_POW2((reg_t)(x), pow2))

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#endif
