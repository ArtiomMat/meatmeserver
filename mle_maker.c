#include <stdio.h>
#include <stdlib.h>

#include "mle_local.h"

#define INIT_MAKER \
	maker_p->index = 1/*Skip over input layer*/; \
	maker_p->p = brain_p; \
	maker_p->total_nodes = maker_p->last_nodes = 0;

void mle_init_maker_in_vals(mle_maker_t* maker_p, mle_brain_t* brain_p, int vals_n) {
	INIT_MAKER

	brain_p->layers[0].t = MLE_LAYER_IO_VALS;

	brain_p->layers[0].io_vals_n = vals_n;
}
void mle_init_maker_in_maps(mle_maker_t* maker_p, mle_brain_t* brain_p, mle_map_cfg_t* map_cfgs, int map_cfgs_n) {
	INIT_MAKER

	brain_p->layers[0].t = MLE_LAYER_IO_MAPS;

	// Allocating
	layer_io_maps_t* l = brain_p->layers[0].p = malloc(sizeof(*l));
	l->map_cfgs_n = map_cfgs_n;
	l->map_cfgs = malloc(sizeof(map_cfgs_n));
	// Copying
	for (int i = 0; i < map_cfgs_n; i++)
		l->map_cfgs[i] = map_cfgs[i];
}

// Gets variable while also setting it's type cause why not.
static layer_t* get(mle_maker_t* maker_p, mle_layer_type_t type) {
	layer_t* l = &maker_p->p->layers[maker_p->index++];
	l->t = type;
	return l;
}

static int check_hp_last(mle_hp_t* hp) {
	if (*hp == MLE_HP_LAST) {
		*hp = MLE_HP_AUTO;
		return 1;
	}
	return 0;
}
static layer_t* find_last(mle_maker_t* maker_p, mle_layer_type_t type) {
	// -2 because we did get() so -1, and -1 to skip current index.
	// i >= 1 to not consider first layer.
	for (int i = maker_p->index-2; i >= 1; i--)
		if (maker_p->p->layers[i].t == type)
			return &maker_p->p->layers[i];
	return NULL;
}

void mle_add_function_layer(mle_maker_t* maker_p, mle_layer_type_t type) {
	layer_t* l = get(maker_p, type);
}

void mle_add_dropout_layer(mle_maker_t* maker_p, mle_hp_t prob) {
	layer_t* l = get(maker_p, MLE_LAYER_DROPOUT);
	
	// Evaluate factor
	if (check_hp_last(&prob))
		prob = find_last(maker_p, MLE_LAYER_DROPOUT)->dropout_prob;
	
	if (prob == MLE_HP_AUTO)
		prob = 220;
	
	l->dropout_prob = prob;
}

void mle_add_pool_layer(mle_maker_t* maker_p, mle_hp_t w, mle_hp_t h, mle_hp_t stride, mle_layer_type_t type) {
	layer_t* l = get(maker_p, type);
	
	// Evaluate width
	if (check_hp_last(&w)) {
		layer_t* f = find_last(maker_p, MLE_LAYER_DROPOUT);
		if (f) w = f->pool.width;
	}
	if (w == MLE_HP_AUTO)
		w = 3;
	// Evaluate height
	if (check_hp_last(&h)) {
		layer_t* f = find_last(maker_p, MLE_LAYER_DROPOUT);
		if (f) w = f->pool.height;
	}
	if (h == MLE_HP_AUTO)
		h = 3;
	// Evaluate stride
	if (check_hp_last(&stride)) {
		layer_t* f = find_last(maker_p, MLE_LAYER_DROPOUT);
		if (f) w = f->pool.stride;
	}
	if (stride == MLE_HP_AUTO)
		stride = 1;
	
	l->pool.width = w;
	l->pool.height = h;
	l->pool.stride = stride;
}
void mle_add_neuro_layer(mle_maker_t* maker_p, mle_hp_t units_n) {
	layer_t* l = get(maker_p, MLE_LAYER_NEURO);
	
	// Evaluate units_n
	if (check_hp_last(&units_n)) {
		layer_neuro_t* ln = find_last(maker_p, MLE_LAYER_DROPOUT)->p;
		if (ln) units_n = ln->units_n;
	}
	if (units_n == MLE_HP_AUTO) {
		int prev_i = maker_p->index-2;
		if (prev_i >= 0) {
			// If previous layer is a convolutional layer, it's a lil complex,
			// what we do is calculate the total number of pixels that are created
			// until this layer.
			if (maker_p->p->layers[prev_i].t == MLE_LAYER_CONVO) {
				
			}
		}
	}

	layer_neuro_t* ln = l->p = malloc(sizeof(layer_neuro_t));
	ln->units_n = units_n;
	ln->units = malloc(sizeof (*ln->units)*units_n);
	// Allocate weights to previous layer's nodes number.
	ln->units->weights = malloc(
		sizeof(*ln->units->weights) * maker_p->last_nodes
	);
}

void mle_add_convo_layer(mle_maker_t* maker_p, mle_hp_t units_n, mle_hp_t w, mle_hp_t h, mle_hp_t stride) {
	layer_t* l = get(maker_p, MLE_LAYER_NEURO);
	
	// Evaluate units_n
	if (check_hp_last(&stride)) {
		layer_t* fi = find_last(maker_p, MLE_LAYER_DROPOUT);
		layer_convo_t* f = fi->p;
		if (f) stride = f->stride;
	}
	if (stride == MLE_HP_AUTO) {
		
	}

	layer_neuro_t* ln = l->p = malloc(sizeof(layer_neuro_t));
	ln->units_n = units_n;
	ln->units = malloc(sizeof (*ln->units)*units_n);
	// Allocate weights to previous layer's nodes number.
	ln->units->weights = malloc(
		sizeof(*ln->units->weights) * maker_p->last_nodes
	);
}