#include <stdlib.h>

#include "mle_local.h"

// TODO: Free functions for all the shit.

mle_brain_t* mle_init_brain(uint8_t hlayers_n) {
	hlayers_n++; // Input layer
	mle_brain_t* brain_p = malloc(sizeof(mle_brain_t));
	brain_p->layers_n = hlayers_n;
	brain_p->layers = malloc(hlayers_n*sizeof(layer_t));

	return brain_p;
}
void mle_free_brain(mle_brain_t* brain_p) {
	free(brain_p->layers);
	free(brain_p);
}

mle_brain_t* mle_open_brain(const char* fp);
void mle_save_brain(mle_brain_t* brain_p, const char* fp);
