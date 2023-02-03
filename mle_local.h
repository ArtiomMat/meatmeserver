#pragma once

#include "mle.h"

// At head of each layer struct to identify it.
// We cast each layer to this 
typedef struct layer_s {
	char t;
	struct layer_s* prev;
	union {
		void* p;
		unsigned char dropout_factor;
	};
} layer_t;

typedef struct {
	// TODO: Can probably be removed, units[0].map.w/h already dictate it.
	uint16_t width, height;
	uint8_t stride;
	uint8_t units_num;
	struct {
		mle_map_t map;
		mle_val_t bias;
	}* units;
} layer_convo_t;

typedef struct {
	uint16_t units_num;
	struct {
		mle_val_t map;
		mle_val_t bias;
	}* units;
} layer_neuro_t;

typedef struct mle_brain_s {
	layer_t* layers;
	uint8_t layers_num;
} mle_brain_t;
