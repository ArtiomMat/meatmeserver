#pragma once

#include <stdint.h>

// Library that allows you to use 16,32,64 bit pointers!
// In memory intensive programs where memory is important.

typedef uint16_t ptr16_t;
typedef uint32_t ptr32_t;

// Initial memory is measured in bytes.
// Recommended to estimate initial memory as close as possible,
// average case scenario, as it automatically exapnds if needed.
// Returns 0 if succeeds to get initial memory from system.
int ptr_init_mem(ptr32_t initial_memory);
void ptr_free_mem(void);

// Give memory
ptr32_t ptr_give(ptr16_t size);
// Give zeroed memory
ptr32_t ptr_zero(ptr32_t ptr, ptr16_t new_size);
// Resize memory given
ptr32_t ptr_resize(ptr32_t ptr, ptr16_t new_size);
void* ptr_sys(ptr32_t ptr);
// Free memory given
void ptr_free(ptr32_t ptr);

