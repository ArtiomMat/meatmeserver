#include <stdio.h>
#include <stdlib.h>

#include "mle_local.h"

#define INIT_MAKER(T) \
	maker_p->index = 1/*Skip over input layer*/; \
	maker_p->p = brain_p; \
	brain_p->layers[0].t = T

void mle_init_maker_in_vals(mle_maker_t* maker_p, mle_brain_t* brain_p, int vals_n) {
	INIT_MAKER(MLE_LAYER_IO_VALS);
	maker_p->channels_n = 1; // Just an array.

	brain_p->layers[0].l_io_vals.n = vals_n;

}
void mle_init_maker_in_map(mle_maker_t* maker_p, mle_brain_t* brain_p, mle_map_cfg_t map_cfg) {
	INIT_MAKER(MLE_LAYER_IO_MAP);
	maker_p->channels_n = map_cfg.channels_n;
	brain_p->layers[0].l_io_map.cfg = map_cfg;
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
	if (check_hp_last(&prob)) {
		layer_t* f = find_last(maker_p, MLE_LAYER_DROPOUT);
		if (f) prob = f->l_dropout.prob;
	}
	
	if (prob == MLE_HP_AUTO)
		prob = 220;
	
	l->l_dropout.prob = prob;
}

void mle_add_pool_layer(mle_maker_t* maker_p, mle_hp_t w, mle_hp_t h, mle_hp_t stride, mle_layer_type_t type) {
	layer_t* l = get(maker_p, type);
	
	// Evaluate width
	if (check_hp_last(&w)) {
		layer_t* f = find_last(maker_p, MLE_LAYER_DROPOUT);
		if (f) w = f->l_pool.width;
	}
	if (w == MLE_HP_AUTO)
		w = 3;
	// Evaluate height
	if (check_hp_last(&h)) {
		layer_t* f = find_last(maker_p, MLE_LAYER_DROPOUT);
		if (f) w = f->l_pool.height;
	}
	if (h == MLE_HP_AUTO)
		h = 3;
	// Evaluate stride
	if (check_hp_last(&stride)) {
		layer_t* f = find_last(maker_p, MLE_LAYER_DROPOUT);
		if (f) w = f->l_pool.stride;
	}
	if (stride == MLE_HP_AUTO)
		stride = 1;
	
	l->l_pool.width = w;
	l->l_pool.height = h;
	l->l_pool.stride = stride;
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
	
	l->l_neuro_p = malloc(sizeof (*l->l_neuro_p));
	l->l_neuro_p->units = malloc(units_n * sizeof (*l->l_neuro_p->units));
	l->l_neuro_p->units->weights = malloc(maker_p->last_nodes * sizeof (mle_val_t));
}

void mle_add_convo_layer(mle_maker_t* maker_p, mle_hp_t units_n, mle_hp_t w, mle_hp_t h, mle_hp_t stride) {
	layer_t* l = get(maker_p, MLE_LAYER_NEURO);
	
	// Evaluate units_n
	if (check_hp_last(&stride)) {
		layer_t* f = find_last(maker_p, MLE_LAYER_DROPOUT);
		if (f) stride = f->l_convo_p->stride;
	}
	if (stride == MLE_HP_AUTO) {
		stride = 
	}
	
	l->l_convo_p = malloc(sizeof (*l->l_convo_p));
	l->l_convo_p->units_n = units_n;
	l->l_convo_p->units = malloc(units_n*sizeof (*l->l_convo_p->units));
	

	for (int i = 0; i < units_n; i++) {
		mle_init_map(&l->l_convo_p->units->weights, w, h, maker_p->channels_n);
	}
	
	if (maker_p->index == 2)
		maker_p->last_nodes += units_n*(1);
}