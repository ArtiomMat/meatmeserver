#pragma once

#include "mle.h"

// At head of each layer struct to identify it.
// We cast each layer to this 
typedef struct layer_s {
	char t;
	union {
		// Pointer for more complex structs that cannot fit in 8 bytes.
		void* p;

		// Structures and members that fit in 8 bytes and don't require p:

		uint16_t io_vals_n;
		uint8_t dropout_prob;
		struct {
			uint16_t width, height;
			uint8_t stride;
		} pool;
	};
} layer_t;

typedef struct mle_brain_s {
	// Includes input layer.
	layer_t* layers;
	uint8_t layers_n;
} mle_brain_t;

typedef struct {
	// TODO: Can probably be removed, units[0].map.w/h already dictate it.
	uint16_t width, height;
	uint8_t stride;
	uint8_t units_n;
	struct {
		mle_map_t map;
		mle_val_t bias;
	}* units;
} layer_convo_t;

typedef struct {
	uint16_t units_n;
	struct {
		// The size of the previous layer's units_n.
		mle_val_t* weights;
		mle_val_t bias;
	}* units;
} layer_neuro_t;

typedef struct {
	mle_map_cfg_t* map_cfgs;
	uint8_t map_cfgs_n;
} layer_io_maps_t;
