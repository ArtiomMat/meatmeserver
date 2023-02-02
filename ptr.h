#pragma once

#include <stdint.h>

// Library that allows you to use 16,32,64 bit pointers!
// In memory intensive programs where memory is important.

typedef uint16_t ptr16_t;
typedef uint32_t ptr32_t;
#if __x86_64__
typedef uint64_t ptr64_t;
#endif
typedef uintmax_t ptrmax_t;

// Initial memory is measured in bytes.
// Recommended to estimate initial memory as close as possible,
// average case scenario, as it automatically exapnds if needed.
// Returns what malloc(initial_memory) returns.
int ptr_init_mem(ptrmax_t initial_memory, char ptr_size);
void ptr_free_mem(void);

// Give memory
ptrmax_t ptr_gmem(int size);
// Resize memory given
ptrmax_t ptr_rmem(ptrmax_t ptr, int new_size);

void ptr_fmem(ptrmax_t ptr);

