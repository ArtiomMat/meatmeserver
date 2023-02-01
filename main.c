#include <stdio.h>
#include "mle.h"

int main(int args_n, char **args) {
	mle_map_t map;
	if (!mle_load_map(&map, "0.jpg")) {
		puts("Failed to load map.");
		return 1;
	}
	/*
	mle_brain_trainer_t trainer = {
		{
			MLE_LAYER_CONV, 3, 3, 1, -1,
			MLE_LAYER_MAX_POOL, 3, 3, 1,
			MLE_LAYER_LEAKY_RELU,
			
			MLE_LAYER_CONV, 3, 3, 1, 16,
			MLE_LAYER_MAX_POOL, 3, 3, 1,
			MLE_LAYER_LEAKY_RELU,

			MLE_LAYER_NEURONS, -1,
			MLE_LAYER_DROPOUT,
			MLE_LAYER_LEAKY_RELU,

			MLE_LAYER_NEURONS, 1,
			MLE_LAYER_SIGMOID,
		},
	};
*/
	mle_rotate_map(&map, 45, map.w/2, map.h/2);

	//mle_resize_map(&map, 64, 64);
	// mle_lighten_map(&map, 0.9f);
	// mle_noise_map(&map, 20);
	// mle_contrast_map(&map, 1.4f);
	// mle_limit_map(&map, 0, 1);
	mle_save_map_png(&map, "p_resized_smooth.png");
	return 0;
}
