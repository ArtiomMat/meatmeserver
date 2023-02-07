#pragma once

#include <stdint.h>

/*
	Machine Learning Engine

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
int mle_rng(void);

enum {
	MLE_COLORS_AUTO,
	// Nice eye friendly heatmap, after converting to grayscale ofc
	MLE_COLORS_HEATMAP,
	MLE_COLORS_RGB,
	MLE_COLORS_RGBA,
	MLE_COLORS_L,
	MLE_COLORS_LA,
};

/*
	The default value ranges of data are 0 to 1, this is how it is when
	maps are loaded from images, but the map is meant to be used beyond that,
	which is why it's not just char or uint8_t it is a fat ass float.
	Data is split into channels.
*/
typedef struct {
	mle_crd_t w, h;
	mle_val_t* data;
	// TODO: Unsused.
	// Used to determine how we export the image.
	uint8_t saved_colors;
	uint8_t channels_n;
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

typedef struct mle_brain_s mle_brain_t;

// hidden_layers_n does not include input layer, only hidden.
mle_brain_t* mle_init_brain(uint8_t hidden_layers_n);
void mle_free_brain(mle_brain_t* brain_p);
mle_brain_t* mle_open_brain(const char* fp);
void mle_save_brain(mle_brain_t* brain_p, const char* fp);

/*
	MAKER:
	Utility that helps make a brain structure from scratch.
	Using the makers you can add layers and automate hyperparameters.
*/

typedef enum {
	// Automatically determine hyperparameter
	MLE_HP_AUTO=-8,
	// Use the respective hyperparameter of the last layer
	MLE_HP_LAST,
} mle_hp_t;

typedef enum {
	// The OG input/output layer, simply values.
	MLE_LAYER_IO_VALS=0,
	// Notice the S, multiple maps are allowd as input/output.
	MLE_LAYER_IO_MAPS,

	// Hyperparameters: width, height, stride, number of kernels(-1=automatic calculation)
	MLE_LAYER_CONVO,
	// TODO
	// Hyperparameters: width, height, stride, number of kernels
	MLE_LAYER_UNCONVO,
	// Hyperparameters: number of neurons(-1=automatic)
	MLE_LAYER_NEURO,

	// Hyperparameters: factor(0.0f to 1.0f)
	MLE_LAYER_DROPOUT,

	// Hyperparameters: width AND height, stride
	MLE_LAYER_POOL_MAX,MLE_LAYER_POOL_MIN, MLE_LAYER_POOL_AVG,

	// TODO
	MLE_LAYER_UNPOOL_AVG,

	// Should be used for the output neurons, as it squishes the values given to it to be from 0 to 1
	MLE_LAYER_F_SIGMOID,
	// Should be used for the output neurons, as it squishes the values given to it to be from -1 to 1
	MLE_LAYER_F_TANH,
	// Should be used for hidden layers, ELU is preferred for speed of gradient descent.
	MLE_LAYER_F_LEAKY_RELU,
	// Use for hidden layers, preferred for speed of gradient descent.
	MLE_LAYER_F_ELU,
	// Use for hidden layers, Can cause the Dying ReLU problem, use ELU or Leaky ReLU, prefer ELU.
	MLE_LAYER_F_RELU,
} mle_layer_type_t;

typedef struct {
	int index;
	// Cached amount of "nodes" created only for the last layer."
	int last_nodes;
	// We cache the total amount of "nodes" created after each layer, so that
	// transitions between layer types work.
	int total_nodes;
	mle_brain_t* p;
} mle_maker_t;

typedef struct {
	mle_crd_t w, h;
	uint8_t channels_n;
} mle_map_cfg_t;

// Please call mle_finish_maker() after you finish adding all the layers.
void mle_init_maker_in_vals(mle_maker_t* maker_p, mle_brain_t* brain_p, int vals_n);
// Please call mle_finish_maker() after you finish adding all the layers.
// map_cfgs is copied.
void mle_init_maker_in_maps(mle_maker_t* maker_p, mle_brain_t* brain_p, mle_map_cfg_t* map_cfgs, int maps_n);
void mle_free_maker(mle_maker_t* maker_p);

void mle_add_function_layer(mle_maker_t* maker_p, mle_layer_type_t type);
/* 
Probability of training a unit(NN)/single pixel(CNN) in a layer.
prob=255 means 100% will be trained, 0 means Nope(pls dont). MLE_HP_* allowed
*/
void mle_add_dropout_layer(mle_maker_t* maker_p, mle_hp_t prob);
void mle_add_pool_layer(mle_maker_t* maker_p, mle_hp_t w, mle_hp_t h, mle_hp_t stride, mle_layer_type_t type);
/*
If you are transitioning from convolutional to neural, don't consider the transitional layer as the translation between convolutional to neural, this is done automatically when the layer is ran, in other words, this transitional layer is just another hidden layer that fully connects to the pervious convolutions, not a layer to which every pixel from to convolutions is copied to.

You can use MLE_HP_AUTO to automate the amount of neurons that will be used for this layer, it makes the layer size around half of the previous one, though it depends on future layers.

You can use MLE_HP_LAST to set the amount of neurons to that of the last layer, if it's a convolutional layer, the total amount of the convolutional pixels will be added up, don't worry about the calculation's cost, a cached value is used to make it O(1).
*/
void mle_add_neuro_layer(mle_maker_t* maker_p, mle_hp_t units_n);
/*
When stride is MLE_HP_AUTO, it will try to get the maximum stride allowed.
*/
void mle_add_convo_layer(mle_maker_t* maker_p, mle_hp_t units_n, mle_hp_t w, mle_hp_t h, mle_hp_t stride);

// 
void mle_end_maker(mle_maker_t* maker_p);

/*
	RUNNER:
	Utility that runs a brain for you, as fast as possible.
*/

typedef struct {
	void* outputs;
	int outputs_mem_size;
	mle_brain_t* p;
} mle_runner_t;

void mle_init_runner(mle_runner_t* runner_p);
void mle_free_runner(mle_runner_t* runner_p);
void mle_run(mle_runner_t* runner_p, void* input);

/*
	TRAINER:
	Utility that trains a brain for you, as fast as possible.
	The utility is interactive enough for almost any need.
*/

typedef struct {
	// Allocate it according to the output layer, for example if the output layer is
	// neurons allocate mle_val_t buffer with the same size.
	void* desired_output_p;
	// Allocate it according to the output layer, if the layer is convolution
	// for example, the input should be a map, so use mle_map_init().
	void* input_p;
} mle_train_sample_t;

typedef void (*mle_give_sample_cb_t) (mle_train_sample_t* input);
typedef void (*mle_on_epoch_end_cb_t) (float error);

typedef struct {
	mle_give_sample_cb_t give_sample;
	// Recommended to call mle_save_brain sometimes, backup lowest error ones.
	mle_on_epoch_end_cb_t on_epoch_end;
	// How many samples do you give before we run the back prop algorithm.
	// Smaller means less performance, bigger means more memory used
	uint8_t batch_size;

	mle_runner_t* r_p;
	mle_brain_t* b_p; // Pointer to the trained brain, don't touch.
} mle_trainer_t;

/*
runner_p can be NULL, one will be created automatically.

batch_size is how many samples do you give before we run the back prop algorithm, can be set to 1, which means it essentially isn't even a batch, back prop is ran every sample. Smaller means worse performance per batch but more accurate back prop and less memory used, bigger means faster per batch but more memory used and a drunk gradient descent.
*/
void mle_init_trainer(
	mle_trainer_t* trainer,
	mle_brain_t* brain_p, 
	mle_runner_t* runner_p, 
	mle_give_sample_cb_t give_sample, 
	mle_on_epoch_end_cb_t on_epoch_end,
	uint8_t batch_size);
void mle_free_trainer(mle_trainer_t* trainer);
/*
One isntinct is to run it on a thread, useless, it is meant to be ran in a dedicated program for training the AI, cause it's slow AF.
*/
void mle_train(mle_trainer_t* trainer);
