#ifndef OPTIONS_H_
#define OPTIONS_H_

#include "darray.h"

#define EPSILON 1e-10

typedef struct options {
	bool all;
	bool draw;
	bool fill;
	char* format;
	char* include;
	bool list;
	unsigned niter;
	bool svg;
	// internal-only: the parse_opt function is responsible for forming these out of the arguments.
	darray* initial;
	darray* rule;
	char* flip;
} options_t;

darray* load_named_data(char* filename, char* ext, options_t* args);
void print_darray(darray const * const source, const options_t* options);

#endif //OPTIONS_H