#ifndef _STDBIGOS_MATH_H
#define _STDBIGOS_MATH_H

#include "types.h"

#define ALIGN_UP(x, align)       (((x) + (align) - 1) / (align) * (align))
#define ALIGN_UP_POW2(x, pow2)   (((x) + (1ull << (pow2)) - 1) & ~((1ull << (pow2)) - 1))
#define ALIGN_DOWN(x, align)     (((x) / (align) * (align)))
#define ALIGN_DOWN_POW2(x, pow2) (((x) & ~((1ull << pow2) - 1)))

#define IS_ALIGNED(ptr, align)   ((reg_t)(x) == ALIGN_DOWN((reg_t)(x), (align))
#define IS_ALIGNED_POW2(ptr, po2)   ((reg_t)(x) == ALIGN_DOWN_POW2((reg_t)(x), (align))

#define POW2(x) (1 << (x))

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#endif
