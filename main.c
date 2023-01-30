#include <stdio.h>
#include "mle.h"

int main(int args_n, char **args) {
	mle_map_t map;
	if (!mle_load_map(&map, "p.png")) {
		puts("Failed to load map.");
		return 1;
	}
	// mle_noise_map_limited(&map, 10, 0, 1);
	mle_rotate_map(&map, -0.1);
	mle_resize_map(&map, 64, 64);
//	mle_resize_map_s(&map, 128, 128);
	mle_save_map_png(&map, "p_resized_smooth.png");
	return 0;
}
