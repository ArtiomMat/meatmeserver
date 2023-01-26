#include <string.h>
#include <stdlib.h>

#include "mle.h"

void mle_copy_map(mle_map_t* from, mle_map_t* to) {
	int size = from->channels_n*from->w*from->h;
	to->data = malloc(size);
	memcpy(to->data, from->data, size);

	to->w = from->w;

	to->h = from->h;
	
	to->channels_n = from->channels_n;
}
void mle_get_map_pixel(mle_map_t* map, float* pixel, short x, short y) {
	for (int i = 0; i < map->channels_n; i++) {
		int index = i*map->channels_n + map->h*y + x;
		pixel[i] = map->data[index];
	}
}
void mle_set_map_pixel(mle_map_t* map, float* pixel, short x, short y) {
	for (int i = 0; i < map->channels_n; i++) {
		int index = i*map->channels_n + map->h*y + x;
		map->data[i] = pixel[index];
	}
}