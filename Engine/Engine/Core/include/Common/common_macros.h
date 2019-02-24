#pragma once

#include "Math/Math.h"

// Makes casts easier to grep
// Yes, we could use c++ style casts, but this codebase is trying to stay very c like
#define cast(type) (type)


#define KILOBYTES(VALUE) ((VALUE) * 1024)
#define MEGABYTES(VALUE) (KILOBYTES(VALUE) * 1024)
#define GIGABYTES(VALUE) (MEGABYTES(VALUE) * 1024)


#define ALIGN_DOWN(n, align) ((n) & ~((align) - 1))
#define ALIGN_UP(n, align) ALIGN_DOWN((n) + (align) - 1, (align))
#define ALIGN_DOWN_PTR(p, align) (cast(void *)ALIGN_DOWN((uintptr_t)(p), (align)))
#define ALIGN_UP_PTR(p, align) (cast(void *)ALIGN_UP((uintptr_t)(p), (align)))

#define CLAMP_MAX(x, max) MIN(x, max)
#define CLAMP_MIN(x, min) MAX(x, min)







