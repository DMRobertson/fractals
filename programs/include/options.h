#ifndef OPTIONS_H_
#define OPTIONS_H_

#include "darray.h"

typedef struct options {
	bool all;
	unsigned iterations;
	bool list_names;
	char* name;
	char* names_include;
	char* format;
	bool svg;
	// internal-only: the parse_opt function is responsible for forming these out of the arguments.
	darray* initial;
	darray* rule;
} options_t;

int load_named_fractal(char* filename, options_t* args);

#endif //OPTIONS_H