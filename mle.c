#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <jpeglib.h>
#include <png.h>

#include "mle.h"

static long _rng_seed = 1;
static void rng_seed(long seed)
{
	_rng_seed = seed;
}
// Good luck getting repeating values out of this
static int rng(void)
{
	_rng_seed = (_rng_seed * 0x2051064DE3) >> 32;
	return (int)_rng_seed;
}

void mle_init_map(mle_map_t* map, short w, short h, int channels_n) {
	map->w = w;
	map->h = h;
	map->channels_n = channels_n;
	map->data = malloc(w*h*channels_n*sizeof(float));
}

void mle_free_map(mle_map_t* map) {
	free(map->data);
}

void mle_copy_map(mle_map_t* from, mle_map_t* to) {
	mle_init_map(to, from->w, from->h, from->channels_n);
	memcpy(to->data, from->data, to->w*to->h*to->channels_n);
}

void mle_get_map_pixel(mle_map_t* map, float* pixel, short x, short y) {
	for (int c = 0; c < map->channels_n; c++)
		pixel[c] = map->data[(map->w*y+x)*map->channels_n+c];
}
void mle_set_map_pixel(mle_map_t* map, float* pixel, short x, short y) {
	for (int c = 0; c < map->channels_n; c++)
		map->data[(map->w*y+x)*map->channels_n+c] = pixel[c];
}

static void ycbcr_to_rgb(float ycbcr[3], float rgb[3]) {
	rgb[0] = ycbcr[2] + 1.40200 * (ycbcr[0] - 0x80);
	rgb[1] = ycbcr[2] - 0.34414 * (ycbcr[1] - 0x80) - 0.71414 * (ycbcr[0] - 0x80);
	rgb[2] = ycbcr[2] + 1.77200 * (ycbcr[1] - 0x80);
}

static char load_jpeg_map(mle_map_t* map, FILE* f) {
	struct jpeg_decompress_struct jpeg;
   
	struct jpeg_error_mgr jerr;
	jpeg.err = jpeg_std_error(&jerr);

	jpeg_create_decompress(&jpeg);

	jpeg_stdio_src(&jpeg, f);

	jpeg_read_header(&jpeg, TRUE);

	// Set output color space to RGB if image is in YCbCr, because fuck YCbCr.
	if (jpeg.jpeg_color_space == JCS_YCbCr)
		jpeg.out_color_space = JCS_RGB;
	jpeg_calc_output_dimensions(&jpeg);

	jpeg_start_decompress(&jpeg);

	int w = jpeg.output_width, h = jpeg.output_height;
	int channels_n = jpeg.num_components;

	printf("%d\n", channels_n);

	mle_init_map(map, w, h, channels_n);

	unsigned char* row = malloc(w*channels_n);
	while (jpeg.output_scanline < jpeg.output_height) {
		jpeg_read_scanlines(&jpeg, &row, 1);
		
		int y = jpeg.output_scanline-1; // Because it is incremented
		
		for (int i = 0; i < w*channels_n; i++)
			map->data[(w*y)*channels_n+i] = row[i]/255.0f;
	}

	jpeg_finish_decompress(&jpeg);
	jpeg_destroy_decompress(&jpeg);
	free(row);

	return 1;
}
static char load_png_map(mle_map_t* map, FILE* f) {

}
char mle_load_map(mle_map_t* map, const char* fp) {
	FILE* f = fopen(fp, "rb");
	if (!f)
		return 0;

	{
	uint32_t jpeg_magic = 0xffd8ff;
	uint32_t png_magic = 0x89504e47;

	uint32_t test_magic;
	fread(&test_magic, 4, 1, f);
	rewind(f);
	if ((test_magic & 0xffffff) == jpeg_magic)
		return load_jpeg_map(map, f);
	else if (test_magic == png_magic)
		return load_png_map(map, f);
	}
	// Loading MAP(An MLE file).

	fclose(f);
	return 1;
}

// If quality is 100 no compression is applied, 0 is just death.
char mle_save_map_jpeg(mle_map_t* map, const char* fp, int quality) {
	FILE* f = fopen(fp, "wb");
	if (!f)
		return 0;
	
	struct jpeg_compress_struct jpeg;
	struct jpeg_error_mgr jerr;

	jpeg.err = jpeg_std_error(&jerr);

	jpeg_create_compress(&jpeg);

	jpeg_stdio_dest(&jpeg, f);

	jpeg.image_width = map->w;
	jpeg.image_height = map->h;
	jpeg.input_components = map->channels_n;
	jpeg.in_color_space = JCS_RGB;

	jpeg_set_defaults(&jpeg);

	if (quality != 100)
		jpeg_set_quality(&jpeg, quality, FALSE);
	
	jpeg_start_compress(&jpeg, TRUE);

	unsigned char* row = malloc(map->channels_n * map->w);
	while (jpeg.next_scanline < jpeg.image_height) {
		int y = jpeg.next_scanline;
		for (int i = 0; i < map->w*map->channels_n; i++) {
			row[i] = map->data[(map->w*y)*map->channels_n+i]*255.0f;
		}
		jpeg_write_scanlines(&jpeg, &row, 1);
	}
	
	jpeg_finish_compress(&jpeg);
	jpeg_destroy_compress(&jpeg);
	free(row);
	fclose(f);

	return 1;
}

void mle_limit_map(mle_map_t* map, float min, float max) {
	for (int i = 0; i < map->channels_n*map->w*map->h; i++) {
		if (map->data[i] > max)
			map->data[i] = 255;
		else if (map->data[i] < min)
			map->data[i] = 0;
	}
}


// ======================
// NOISES
// ======================

static void apply_noise(float* v, uint8_t strength) {
	float r = ((rng() % strength)/255.0f);
	if (rng()%2) // Subtract or add? IDK.
		*v+=r;
	else
		*v-=r;
}

void mle_noise_map(mle_map_t* map, uint8_t strength) {
	for (int i = 0; i < map->channels_n*map->w*map->h; i++)
		apply_noise(map->data+i, strength);
}

void mle_noise_map_limited(mle_map_t* map, uint8_t strength, float min, float max) {
	for (int i = 0; i < map->channels_n*map->w*map->h; i++) {
		apply_noise(map->data+i, strength);
		if (map->data[i] > max)
			map->data[i] = max;
		else if (map->data[i] < min)
			map->data[i] = min;
	}
}
