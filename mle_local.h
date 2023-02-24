#pragma once

#include "mle.h"

// Width and height are stored in weights
typedef struct {
	struct {
		// Both allocated to be the size of the first layer's map_cfgs_n
		mle_map_t weights;
		mle_val_t bias;
	}* units; // 8

	struct {
		uint16_t mode	: 2; // 0=disabled, 1=min, 2=avg, 3=max
		uint16_t stride	: 4;
		uint16_t width	: 5;
		uint16_t height	: 5;
	} pool;
	struct {
		uint8_t prob; // 255 for full training.
	} dropout_prob; // 1
	uint8_t stride;
	uint8_t dilation;
	uint8_t padding;

	uint8_t units_n; // 1

	uint8_t func; // Activation function
} layer_c_t;

typedef struct {
	struct {
		mle_val_t* weights; // The size of the previous layer's units_n.
		mle_val_t bias;
	}* units;

	struct {
		uint8_t prob; // 255 for full training.
	} dropout;

	uint16_t units_n;
	
	uint8_t func; // Activation function
} layer_n_t;

typedef struct mle_lobe_s {
	struct lobe_s** inputs; // The lobes that are inputted into this lobe.
	struct lobe_s** outputs; // The lobes we output this lobe's outputs.
	
	union {
		void* pl; // The pointer to any layer
		layer_n_t* nl;
		layer_c_t* cl;
	};

	mle_map_cfg_t map_cfg; // Only used if this is a convolutional lobe.

	uint8_t inputs_n;
	uint8_t outputs_n;

	uint8_t layers_n;

	uint8_t t;
} mle_lobe_t, lobe_t;

x() {
	sizeof(mle_lobe_t);
}

// At head of each layer struct to identify it.
// We cast each layer to this 
typedef struct layer_s {
	char t;
	union {
		// Pointers for more complex structs that cannot fit in 8 bytes.
		struct {
			uint16_t units_n;
			struct {
				// The size of the previous layer's units_n.
				mle_val_t* weights;
				mle_val_t bias;
			}* units;
		}* l_neuro_p;
		struct {
			struct {
				// Both allocated to be the size of the first layer's map_cfgs_n
				mle_map_t weights;
				mle_val_t bias;
			}* units;
			uint8_t stride;
			uint8_t units_n;
		}* l_convo_p;

		// Structures and members that fit in 8 bytes and don't require pointers:

		struct {
			int n;
		} l_io_vals;
		struct {
			mle_map_cfg_t cfg;
		} l_io_map;
		struct {
			uint8_t prob;
		} l_dropout;
		struct {
			uint16_t width, height;
			uint8_t stride;
		} l_pool;
	};
} layer_t;
