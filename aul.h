#pragma once

#include <stdint.h>

typedef struct {
	int ptrs_n, ptrs_s;
	void** ptrs;
} aul_list_t;
