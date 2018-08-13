#pragma once


#define KILOBYTES(VALUE) ((VALUE) * 1024)
#define MEGABYTES(VALUE) (KILOBYTES(VALUE) * 1024)
#define GIGABYTES(VALUE) (MEGABYTES(VALUE) * 1024)


#define ALIGN_DOWN(n, align) ((n) & ~((align) - 1))
#define ALIGN_UP(n, align) ALIGN_DOWN((n) + (align) - 1, (align))
#define ALIGN_DOWN_PTR(p, align) ((void *)ALIGN_DOWN((uintptr_t)(p), (align)))//cast
#define ALIGN_UP_PTR(p, align) ((void *)ALIGN_UP((uintptr_t)(p), (align)))//cast