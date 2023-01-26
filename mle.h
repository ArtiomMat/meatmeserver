#pragma once

/*
	Machine Learning EASY!

	A library mainly designed for image processing, with CNN, DNN, FCN, RNN,
	support, with modularity.

	You can save and load models.

	All of that with state of the art algorithms implemented by your's truly.

	Not added yet: RNN and DNN(Deconvolution).
*/

/*
	The default value ranges of data are 0 to 1, this is how it is when
	maps are loaded from images, but the map is meant to be used beyond that,
	which is why it's not just char or uint8_t it is a fat ass float.
	Data is split into channels.
*/
typedef struct {
	short w, h;
	float* data;
	int channels_n;
} mle_map_t;

void mle_load_map(mle_map_t* map, const char* fp);
// Returns 0 on failure, 1 on success.
char mle_save_map(mle_map_t* map, const char* fp);
// Copies the data aswell, useful.
void mle_copy_map(mle_map_t* from, mle_map_t* to);
// Allocate pixel to map->channels_n.
// x and y must be within map->w/h respectively.
void mle_get_map_pixel(mle_map_t* map, float* pixel, short x, short y);
// Allocate pixel to map->channels_n.
// x and y must be within map->w/h respectively.
void mle_set_map_pixel(mle_map_t* map, float* pixel, short x, short y);
// Nearest neighbor, mle_resize_map_bi() for bilinear
void mle_resize_map(mle_map_t* map, short w, short h);
// Bilinear
void mle_resize_map_bi(mle_map_t* map, short w, short h);
// left right top bottom. if cropped out of bounds 0 is written to the new
// data.
void mle_crop_map(mle_map_t* map, short l, short t, short r, short b);
// Can cause loss of data, since image can go out of bounds, so crop the map
// to make it bigger to keep the data.
void mle_rotate_map(mle_map_t* map, float deg);
// Random noise across all channels, 0 means no loss 1 means total loss of data.
void mle_noise_map(mle_map_t* map, float strength);
// Pepper only noise, 0 means no loss 1 means total loss of data.
void mle_noise_map_pepper(mle_map_t* map, float strength);
// Salt only noise, 0 means no loss 1 means total loss of data..
void mle_noise_map_salt(mle_map_t* map, float strength);
// Salt and pepper noise, faster than the two functions called individually.
// 0 means no loss 1 means total loss of data.
void mle_noise_map_snp(mle_map_t* map, float strength);

/*typedef struct {
	mle_map_t map;
	float bias;
} mle_kernel_t;*/
