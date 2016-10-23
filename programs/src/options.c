#include <stdio.h>
#include <errno.h>
#include "options.h"

options_t options = {
	false, //all
	5,     //iterations
	false, //list-names
	NULL,  //names
	".",   //names-include
	"%lf",  //printf
	false, //svg
	//internal only
	NULL,  //initial. freed in the main loop.
	NULL   //rule     freed at the end of the main loop.
};

int load_named_fractal(char* filename, options_t* args){
	FILE* file = fopen(filename, "r");
	if (file == NULL){
		return EXIT_FAILURE;
	}
	args->initial = load_darray(file, args->format);
	args->rule = load_darray(file, args->format);
	if (errno != 0){
		return EXIT_FAILURE;
	}
	fclose(file);
	return EXIT_SUCCESS;
}