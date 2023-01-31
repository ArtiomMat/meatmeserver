#include <stdio.h>
#include "mle.h"

int main(int args_n, char **args) {
	mle_map_t map;
	if (!mle_load_map(&map, "0.jpg")) {
		puts("Failed to load map.");
		return 1;
	}
	// mle_crop_map(&map, -100, -100, map.w+100, map.h+100);
	// mle_noise_map_limited(&map, 10, 0, 1);
	mle_rotate_map(&map, 1, map.w/2, map.h/2);
	// mle_resize_map(&map, 64, 64);
//	mle_resize_map_s(&map, 128, 128);
	mle_save_map_png(&map, "p_resized_smooth.png");
	return 0;
}
