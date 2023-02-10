#include <stdio.h>
#include "mle.h"


int main(int args_n, char **args) {
	mle_brain_t* brain_p = mle_init_brain(10);
	mle_maker_t maker;
	mle_init_maker(&maker, brain_p, MLE_LAYER_IO_MAP, MLE_LAYER_IO_VALS);
	mle_add_dropout_layer(&maker, MLE_HP_LAST);
	mle_add_dropout_layer(&maker, 69);
	mle_add_dropout_layer(&maker, MLE_HP_LAST);
	mle_add_dropout_layer(&maker, MLE_HP_AUTO);
	mle_add_dropout_layer(&maker, MLE_HP_LAST);
	return 0;
}
