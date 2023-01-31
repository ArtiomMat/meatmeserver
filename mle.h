#pragma once

#include <stdint.h>

/*
	Machine Learning EASY!

	A library mainly designed for image processing, with CNN, DNN, FCN, RNN,
	support, with modularity.

	Float based.

	You can save and load models.

	All of that with state of the art algorithms implemented by your's truly.

	Not added yet: RNN and DNN(Deconvolution).
*/

typedef unsigned short mle_crd_t;
typedef float mle_val_t;

void mle_rng_seed(long seed);

/*
	The default value ranges of data are 0 to 1, this is how it is when
	maps are loaded from images, but the map is meant to be used beyond that,
	which is why it's not just char or uint8_t it is a fat ass float.
	Data is split into channels.
*/
typedef struct {
	mle_crd_t w, h;
	mle_val_t* data;
	int channels_n;
} mle_map_t;

// Must be freed with mle_free_map()
void mle_init_map(mle_map_t* map_p, mle_crd_t w, mle_crd_t h, int channels_n);
void mle_free_map(mle_map_t* map_p);
// Supports: png, jpeg, map(custom file)
// Palettized PNGs are not supported, they are unsuitable for AI.
// Returns 0 on failure, 1 on success.
// Must be freed with mle_free_map()
char mle_load_map(mle_map_t* map_p, const char* fp);
// Supports only channels_n=3 for RGB and channels_n=1 for Grayscale.
// If quality is 100 no compression is applied, 0 is just death.
// Before saving map_p should be limited to avoid visual glitches. OVERRIDES EXISTING FILES! Returns 0 on failure, 1 on success.
char mle_save_map_jpeg(mle_map_t* map_p, const char* fp, int quality);
// Before saving map_p should be limited to avoid visual glitches. OVERRIDES EXISTING FILES! Returns 0 on failure, 1 on success.
char mle_save_map_png(mle_map_t* map_p, const char* fp);
// Before saving map_p should be limited to avoid visual glitches. OVERRIDES EXISTING FILES! Returns 0 on failure, 1 on success.
char mle_save_map(mle_map_t* map_p, const char* fp);
// Copies the data aswell, useful.
void mle_copy_map(mle_map_t* from, mle_map_t* to);
// Allocate pixel to map_p->channels_n.
// x and y must be within map_p->w/h respectively.
void mle_get_map_pixel(mle_map_t* map_p, mle_val_t* pixel, mle_crd_t x, mle_crd_t y);
// Allocate pixel to map_p->channels_n.
// x and y must be within map_p->w/h respectively.
void mle_set_map_pixel(mle_map_t* map_p, mle_val_t* pixel, mle_crd_t x, mle_crd_t y);
// Nearest neighbor, mle_resize_map_s() for smooth resizing
void mle_resize_map(mle_map_t* map_p, mle_crd_t w, mle_crd_t h);
// Smooth resizing
// void mle_resize_map_s(mle_map_t* map_p, mle_crd_t w, mle_crd_t h);
// left right top bottom. if cropped out of bounds(<0 OR >w/h-1) 0 is written to
// the new data.
void mle_crop_map(mle_map_t* map_p, int l, int t, int r, int b);
// Can cause loss of data, since image can go out of bounds, so crop the map_p
// to make it bigger to keep the data.
void mle_rotate_map(mle_map_t* map_p, float rad, mle_crd_t around_x, mle_crd_t around_y);
// 1 means no change, called lighten but you can also darken it(factor < 1).
void mle_lighten_map(mle_map_t* map_p, float factor);
// 1 means no change.
void mle_contrast_map(mle_map_t* map_p, float factor);
// Random noise across all channels, 0 means no loss 255 means total loss of data.
void mle_noise_map(mle_map_t* map_p, uint8_t strength);
// The noise is applied but the pixel values are limited to min and max.
// Random noise across all channels, 0 means no loss 255 means total loss of data.
void mle_noise_map_limited(mle_map_t* map_p, uint8_t strength, mle_val_t min, mle_val_t max);
// value is what we set the pixel if it is chosen to be killed.
// Pepper only noise, 0 means no loss 255 means total loss of data.
void mle_noise_map_killer(mle_map_t* map_p, uint8_t strength, mle_val_t value);
// Pixels above of below min or max are set to min or max respectively.
void mle_limit_map(mle_map_t* map_p, mle_val_t min, mle_val_t max);

enum mle_layer_type_e {
	MLE_LAYER_NULL,
	// Hyperparameters: width, height, stride, number of kernels(-1=automatic calculation)
	MLE_LAYER_CONV,
	// TODO
	// Hyperparameters: width, height, stride, number of kernels
	MLE_LAYER_DECONV,
	// Hyperparameters: number of neurons(-1=automatic)
	MLE_LAYER_NEURONS,

	MLE_LAYER_DROPOUT,

	// Exclusive to kernel brains.
	// Hyperparameters: width AND height, stride
	MLE_LAYER_MAX_POOL, MLE_LAYER_MIN_POOL, MLE_LAYER_AVG_POOL,

	// TODO
	MLE_LAYER_AVG_UNPOOL,

	// Should be used for the output neurons, as it squishes the values given to it to be from 0 to 1
	MLE_LAYER_SIGMOID,
	// Should be used for the output neurons, as it squishes the values given to it to be from -1 to 1
	MLE_LAYER_TANH,
	// Should be used for hidden layers, ELU is preferred for speed of gradient descent.
	MLE_LAYER_LEAKY_RELU,
	// Use for hidden layers, preferred for speed of gradient descent.
	MLE_LAYER_ELU,
	// Use for hidden layers, Can cause the Dying ReLU problem, use ELU or Leaky ReLU, prefer ELU.
	MLE_LAYER_RELU,
};

typedef struct {
	// Allocate it according to the output layer, for example if the output layer is
	// neurons allocate mle_val_t buffer with the same size.
	void* desired_output_p;
	// Allocate it according to the output layer, if the layer is convolution
	// for example, the input should be a map, so use mle_map_init().
	void* input_p;
} mle_training_sample_t;

typedef void* mle_brain_p_t;

typedef struct {
	/*
Terminated with MLE_LAYER_NULL.

Structure goes: <layer type 0>, <hyperparameters 0>, <layer type 1> ... MLE_LAYER_NULL
See MLE_LAYER_* for information on hyperparameters.

Example: {MLE_LAYER_CONV, width, height, stride, num, MLE_LAYER_ELU, MLE_LAYER_NEURONS, num, MLE_LAYER_NULL}
In the example, we have a single kernels layer, that transforms into array that is fed into num output neurons.

If the 
	*/
	int* layer_cfgs;
	// How many samples do you give before we run the back prop algorithm.
	// Bigger means faster 
	int batch_size;
	void (*init_sample) (mle_training_sample_t* input);

	mle_brain_p_t p; // Pointer to the trained brain, don't touch.
} mle_brain_trainer_t;

/*
	create
	for batches
		

*/
