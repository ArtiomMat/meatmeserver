#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

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

void mle_init_map(mle_map_t* map_p, mle_crd_t w, mle_crd_t h, int channels_n) {
	map_p->w = w;
	map_p->h = h;
	map_p->channels_n = channels_n;
	map_p->data = malloc(w*h*channels_n*sizeof(float));
}

void mle_free_map(mle_map_t* map_p) {
	free(map_p->data);
}

void mle_copy_map(mle_map_t* from, mle_map_t* to) {
	mle_init_map(to, from->w, from->h, from->channels_n);
	memcpy(to->data, from->data, to->w*to->h*to->channels_n);
}

void mle_get_map_pixel(mle_map_t* map_p, mle_val_t* pixel, mle_crd_t x, mle_crd_t y) {
	for (int c = 0; c < map_p->channels_n; c++)
		pixel[c] = map_p->data[(map_p->w*y+x)*map_p->channels_n+c];
}
void mle_set_map_pixel(mle_map_t* map_p, mle_val_t* pixel, mle_crd_t x, mle_crd_t y) {
	for (int c = 0; c < map_p->channels_n; c++)
		map_p->data[(map_p->w*y+x)*map_p->channels_n+c] = pixel[c];
}

static void ycbcr_to_rgb(float ycbcr[3], float rgb[3]) {
	rgb[0] = ycbcr[2] + 1.40200 * (ycbcr[0] - 0x80);
	rgb[1] = ycbcr[2] - 0.34414 * (ycbcr[1] - 0x80) - 0.71414 * (ycbcr[0] - 0x80);
	rgb[2] = ycbcr[2] + 1.77200 * (ycbcr[1] - 0x80);
}

static char load_jpeg_map(mle_map_t* map_p, FILE* f) {
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

	mle_crd_t w = jpeg.output_width, h = jpeg.output_height;
	int channels_n = jpeg.num_components;

	mle_init_map(map_p, w, h, channels_n);

	unsigned char* row = malloc(w*channels_n);
	while (jpeg.output_scanline < jpeg.output_height) {
		jpeg_read_scanlines(&jpeg, &row, 1);

		int y = jpeg.output_scanline-1; // Because it is incremented
		
		for (int i = 0; i < w*channels_n; i++)
			map_p->data[(w*y)*channels_n+i] = row[i]/255.0f;
	}

	jpeg_finish_decompress(&jpeg);
	jpeg_destroy_decompress(&jpeg);
	free(row);

	return 1;
}
static char load_png_map(mle_map_t* map_p, FILE* f) {
	fseek(f, 8, SEEK_SET); // Skip first 8 bytes of header

	png_structp png_p = png_create_read_struct(
		PNG_LIBPNG_VER_STRING, 
		NULL,
		NULL,
		NULL
	);
	if (!png_p)
		return 0;
	
	png_infop info_p = png_create_info_struct(png_p);
	if (!info_p) {
		png_destroy_read_struct(&png_p, NULL, NULL);
		return 0;
	}
	
	png_init_io(png_p, f);
	png_set_sig_bytes(png_p, 8);
	png_read_info(png_p, info_p);

	int color_type = png_get_color_type(png_p, info_p);
	if (color_type & PNG_COLOR_MASK_PALETTE)
		return 0;

	mle_crd_t w = png_get_image_width(png_p, info_p);
	mle_crd_t h = png_get_image_height(png_p, info_p);
	int channels_n = 1;

	if (color_type & PNG_COLOR_MASK_COLOR)
		channels_n += 2;
	if (color_type & PNG_COLOR_MASK_ALPHA)
		channels_n += 1;


	mle_init_map(map_p, w, h, channels_n);

	unsigned char row[w*channels_n];
	
	for (int y = 0; y < h; y++) {
		png_read_row(png_p, row, NULL);
		
		for (int i = 0; i < w*channels_n; i++)
			map_p->data[(w*y)*channels_n+i] = row[i]/255.0f;
	}

	png_destroy_info_struct(png_p, &info_p);
	png_destroy_read_struct(&png_p, &info_p, NULL);

	return 1;
}

char mle_load_map(mle_map_t* map_p, const char* fp) {
	FILE* f = fopen(fp, "rb");
	if (!f)
		return 0;
	{
		char jpeg_magic[] = {0xff, 0xd8, 0xff};
		char png_magic[] = {0x89, 'P', 'N', 'G'};

		char test_magic[4];
		fread(test_magic, 4, 1, f);
		rewind(f);
		if (!memcmp(test_magic, jpeg_magic, 3))
			return load_jpeg_map(map_p, f);
		else if (!memcmp(test_magic, png_magic, 4))
			return load_png_map(map_p, f);
	}
	// Loading MAP(An MLE file).
	mle_crd_t size[2];
	fread(size, 2, 2, f);
	int channels_n;
	fread(&channels_n, 4, 1, f);

	mle_init_map(map_p, size[0], size[1], channels_n);

	fread(map_p->data, size[0]*size[1]*channels_n, 1, f);
	
	fclose(f);
	return 0;
}

char mle_save_map_png(mle_map_t* map_p, const char* fp) {
	FILE* f = fopen(fp, "wb");
	if (!f)
		return 0;
	
	png_structp png_p = png_create_write_struct(
		PNG_LIBPNG_VER_STRING, 
		NULL,
		NULL,
		NULL
	);
	if (!png_p)
		return 0;
	
	png_infop info_p = png_create_info_struct(png_p);
	if (!info_p) {
		png_destroy_read_struct(&png_p, NULL, NULL);
		return 0;
	}
	
	png_init_io(png_p, f);

	// Determine color type
	int color_type = 0;
	int c = map_p->channels_n;
	if (c == 1)
		color_type = PNG_COLOR_TYPE_GRAY;
	else if (c == 2)
		color_type = PNG_COLOR_TYPE_GRAY_ALPHA;
	else if (c == 3)
		color_type = PNG_COLOR_TYPE_RGB;
	else if (c == 4)
		color_type = PNG_COLOR_TYPE_RGB_ALPHA;
		
	png_set_IHDR(
		png_p,
		info_p,
		map_p->w, map_p->h,
		8, color_type,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE,
		PNG_FILTER_TYPE_BASE
	);
	png_write_info(png_p, info_p);

	unsigned char row[map_p->w*c];
	
	for (int y = 0; y < map_p->h; y++) {
		for (int i = 0; i < map_p->w*c; i++)
			row[i] = map_p->data[(map_p->w*y)*c+i]*255.0f;
		png_write_row(png_p, row);
	}

	png_destroy_info_struct(png_p, &info_p);
	png_destroy_write_struct(&png_p, &info_p);

	return 1;
}

char mle_save_map_jpeg(mle_map_t* map_p, const char* fp, int quality) {
	FILE* f = fopen(fp, "wb");
	if (!f)
		return 0;
	
	struct jpeg_compress_struct jpeg;
	struct jpeg_error_mgr jerr;

	jpeg.err = jpeg_std_error(&jerr);

	jpeg_create_compress(&jpeg);

	jpeg_stdio_dest(&jpeg, f);

	jpeg.image_width = map_p->w;
	jpeg.image_height = map_p->h;
	jpeg.input_components = map_p->channels_n;
	if (map_p->channels_n == 3)
		jpeg.in_color_space = JCS_RGB;
	else if (map_p->channels_n == 1)
		jpeg.in_color_space = JCS_GRAYSCALE;

	jpeg_set_defaults(&jpeg);

	if (quality != 100)
		jpeg_set_quality(&jpeg, quality, FALSE);
	
	jpeg_start_compress(&jpeg, TRUE);

	unsigned char* row = malloc(map_p->channels_n * map_p->w);
	while (jpeg.next_scanline < jpeg.image_height) {
		int y = jpeg.next_scanline;
		for (int i = 0; i < map_p->w*map_p->channels_n; i++) {
			row[i] = map_p->data[(map_p->w*y)*map_p->channels_n+i]*255.0f;
		}
		jpeg_write_scanlines(&jpeg, &row, 1);
	}
	
	jpeg_finish_compress(&jpeg);
	jpeg_destroy_compress(&jpeg);
	free(row);
	fclose(f);

	return 1;
}

// ======================
// RESIZE
// ======================

void mle_resize_map(mle_map_t* map_p, mle_crd_t w, mle_crd_t h) {
	mle_map_t resized;
	mle_init_map(&resized, w, h, map_p->channels_n);
	
	mle_val_t h_ratio = (mle_val_t)map_p->h/h;
	mle_val_t w_ratio = (mle_val_t)map_p->h/w;

	mle_val_t pixel[map_p->channels_n];
	for (int y = 0; y < h; y++) {
		int picked_y = y*h_ratio;
		for (int x = 0; x < w; x++) {
			int picked_x = x*w_ratio;
			mle_get_map_pixel(map_p, pixel, picked_x, picked_y);
			mle_set_map_pixel(&resized, pixel, x, y);
		}
	}

	mle_free_map(map_p);
	*map_p = resized;
}

static float pow2(float x) {
	return x*x;
}

void mle_resize_map_s(mle_map_t* map_p, mle_crd_t w, mle_crd_t h) {
	mle_map_t resized;
	mle_init_map(&resized, w, h, map_p->channels_n);
	
	float h_ratio = (float)map_p->h/h;
	float w_ratio = (float)map_p->w/w;

	float pixel[map_p->channels_n];
	float picked_pixel[map_p->channels_n];
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			// Reset pixel to write
			for (int c = 0; c < map_p->channels_n; c++)
				pixel[c] = 0;
			
			// Coordinates of middle pixel we are picking
			float m_x=x*w_ratio, m_y=y*h_ratio;
			// Coordinates of surrounding pixels of the middle pixel
			short lt_x = (short)m_x, lt_y = (short)m_y;
			short lb_x = lt_x, lb_y = lt_y+1;
			short rt_x = lt_x+1, rt_y = lt_y;
			short rb_x = lt_x+1, rb_y = lt_y+1;

			// Distances of surrounding pixels
			float d_lt = (abs(m_x-lt_x)+abs(m_y-lt_y))/2;
			float d_lb = (abs(m_x-lb_x)+abs(m_y-lb_y))/2;
			float d_rt = (pow2(m_x-rt_x)+abs(m_y-rt_y))/2;
			float d_rb = (abs(m_x-rb_x)+abs(m_y-rb_y))/2;

			// The combination of the pixels
			mle_get_map_pixel(map_p, picked_pixel, lt_x, lt_y);
			for (int c = 0; c < map_p->channels_n; c++)
				pixel[c] += d_lt*picked_pixel[c]/4;
			mle_get_map_pixel(map_p, picked_pixel, lb_x, lb_y);
			for (int c = 0; c < map_p->channels_n; c++)
				pixel[c] += d_lb*picked_pixel[c]/4;
			mle_get_map_pixel(map_p, picked_pixel, rt_x, rt_y);
			for (int c = 0; c < map_p->channels_n; c++)
				pixel[c] += d_rt*picked_pixel[c]/4;
			mle_get_map_pixel(map_p, picked_pixel, rb_x, rb_y);
			for (int c = 0; c < map_p->channels_n; c++)
				pixel[c] += d_rb*picked_pixel[c]/4;

			mle_set_map_pixel(&resized, pixel, x, y);
		}
	}

	mle_free_map(map_p);
	*map_p = resized;
}

// ======================
// NOISES
// ======================

static void apply_noise(mle_val_t* v, uint8_t strength) {
	mle_val_t r = ((rng() % (strength+1))/255.0f);
	if (rng()%2) // Subtract or add? IDK.
		*v+=r;
	else
		*v-=r;
}

void mle_noise_map(mle_map_t* map_p, uint8_t strength) {
	for (int i = 0; i < map_p->channels_n*map_p->w*map_p->h; i++)
		apply_noise(map_p->data+i, strength);
}

void mle_noise_map_limited(mle_map_t* map_p, uint8_t strength, mle_val_t min, mle_val_t max) {
	for (int i = 0; i < map_p->channels_n*map_p->w*map_p->h; i++) {
		apply_noise(map_p->data+i, strength);
		if (map_p->data[i] > max)
			map_p->data[i] = max;
		else if (map_p->data[i] < min)
			map_p->data[i] = min;
	}
}

void mle_noise_map_killer(mle_map_t* map_p, uint8_t strength, mle_val_t value) {
	for (int i = 0; i < map_p->w*map_p->h; i+=map_p->channels_n) {
		if ((unsigned char)rng() <= strength)
			for (int c = 0; c < map_p->channels_n; c++)
				map_p->data[i+c] = value;
	}
}

// MISC


void mle_limit_map(mle_map_t* map_p, mle_val_t min, mle_val_t max) {
	for (int i = 0; i < map_p->channels_n*map_p->w*map_p->h; i++) {
		if (map_p->data[i] > max)
			map_p->data[i] = 255;
		else if (map_p->data[i] < min)
			map_p->data[i] = 0;
	}
}
