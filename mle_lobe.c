#include "mle_local.h"

void mle_init_lobe(mle_lobe_t* l, mle_lobe_type_t t, int layers_n, int inputs_n, int outputs_n) {
	l->t = t;

	l->inputs_n = inputs_n;
	l->inputs = malloc(inputs_n*sizeof(*l->inputs));
	l->outputs_n = outputs_n;
	l->inputs = malloc(inputs_n*sizeof(*l->outputs));
	
	l->layers_n = layers_n;

	switch(t) {
		case MLE_LOBE_N:
		l->nl = malloc(layers_n*sizeof(*l->nl));
		break;
		case MLE_LOBE_C:
		l->cl = malloc(layers_n*sizeof(*l->cl));
		break;
	}
}
void mle_free_lobe(mle_lobe_t* l) {
	free(l->pl);
	free(l->inputs);
	free(l->outputs);
}

void mle_set_lobe_input(mle_lobe_t* l, int i, mle_lobe_t* input) {
	l->inputs[i] = input;
}
void mle_set_lobe_output(mle_lobe_t* l, int i, mle_lobe_t* output) {
	l->outputs[i] = output;
}

void mle_set_lobe_n_layer(mle_lobe_t* l, int i, uint8_t units_n) {
	if (l->t != MLE_LOBE_C) return;
	l->nl[i].units_n = units_n;
	l->nl[i].units = malloc(units_n*sizeof(*l->nl[i].units));

	// Getting the weights number
	int weights_n;
	if (i > 0) // In the case that this is just another hidden layer we good
		weights_n = l->nl[i-1].units_n;
	else { // This is where SHIT HITS THE FAN.
		// Loop through each input appending a calculated sum of units.
		for (int i = 0; i < l->inputs_n; i++) {
			mle_lobe_t* input = l->inputs[i];
			// Convolutional Lobe.
			if (input->t == MLE_LOBE_C) {
				int n = input->map_cfg.;
				for (int i = 0; i < input->layers_n; i++) {
					
				}
			}
			// Neural Lobe.
			else if (input->t == MLE_LOBE_N) {
				weights_n = input->nl[input->layers_n-1].units_n;
			}
		}
	}

	for (int i = 0; i < units_n; i++)
		l->nl[i].units[i].weights = malloc()
}
void mle_set_lobe_c_layer(mle_lobe_t* l, int i, uint8_t units_n, uint8_t w, uint8_t h, uint8_t stride, uint8_t dilation, uint8_t padding) {
	if (l->t != MLE_LOBE_C) return;
}

/*
i is the index of the affected REAL layer, don't increment to the next layer!

Valid only for CN.
*/
void mle_set_lobe_layer_pool(mle_lobe_t* l, int i, uint8_t w, uint8_t h, uint8_t stride, uint8_t mode);
/*
i is the index of the affected REAL layer, don't increment to the next layer!

1 for 100% training probability, 0 for 0% probability of training the node.
For CNN a node is a pixel in the filter map.
*/
void mle_set_lobe_layer_dropout(mle_lobe_t* l, int i, float prob);
/*
i is the index of the affected REAL layer, don't increment to the next layer!
Use MLE_FUNC_* for func.
*/
void mle_set_lobe_layer_func(mle_lobe_t* l, int i, uint8_t func);