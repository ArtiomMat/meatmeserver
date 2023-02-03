#include "ptr.h"
#include <stdlib.h>

static char* mem_p;

void* ptr_sys(ptr32_t ptr) {
	return (uint64_t)mem_p+(uint64_t)ptr;
}

int ptr_init_mem(ptr32_t initial_memory) {
	mem_p = malloc(initial_memory);
	if (!mem_p)
		return 0;
	
	for (int i = 0; i < initial_memory; i++)
		mem_p[i] = 0;
	
	return 1;
}
void ptr_free_mem(void) {
	free(mem_p);
}

// Give memory
ptr32_t ptr_give(ptr16_t size);
// Give zeroed memory
ptr32_t ptr_zero(ptr32_t ptr, ptr16_t new_size);
// Resize memory given
ptr32_t ptr_resize(ptr32_t ptr, ptr16_t new_size);
// Free memory given
void ptr_free(ptr32_t ptr) {
	
}