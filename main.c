#include <stdio.h>
#include "mle.h"

int main(int args_n, char **args) {
	mle_map_t map;
	mle_load_map(&map, "0.jpg");
	mle_noise_map_limited(&map, 100, 0, 1);
	mle_save_map_jpeg(&map, "1.jpg", 100);
	float p[3] = {1,2,3};
	mle_get_map_pixel(&map, p, 316, 7);
	printf("%f %f %f\n", p[0], p[1], p[2]);
	return 0;
}
