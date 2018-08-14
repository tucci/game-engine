#pragma once


#define DEBUG 1


#if DEBUG
#define debug_print(...) printf(__VA_ARGS__)

#else
#define NDEBUG 
#define debug_print(...) do {} while (0)
#endif

#include <assert.h>