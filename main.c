#include <stdio.h>
#include "mle.h"

int main(int args_n, char **args) {
	mle_map_t map;
	if (!mle_load_map(&map, "emo.png")) {
		puts("Failed to load map.");
		return 1;
	}

	mle_lighten_map(&map, 0.9f);
	mle_noise_map(&map, 70);
	mle_contrast_map(&map, 2.0f);
	mle_limit_map(&map, 0, 1);
	mle_save_map_png(&map, "emo2.png");
	return 0;
}
