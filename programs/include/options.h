#ifndef OPTIONS_H_
#define OPTIONS_H_

#include "darray.h"

typedef struct options {
	bool all;
	char* format;
	char* include;
	bool list;
	unsigned niter;
	bool svg;
	// internal-only: the parse_opt function is responsible for forming these out of the arguments.
	darray* initial;
	darray* rule;
} options_t;

darray* load_named_data(char* filename, char* ext, options_t* args);

#endif //OPTIONS_H