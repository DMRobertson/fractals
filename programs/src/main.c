#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <argp.h>

#define PI 3.141592653589

#include "darray.h"
#include "data.h"
#include "iteration.h"

static struct argp_option options_definition[] = {
	//name          key  arg      flags doc                                                           group
	{ "all"       , 'a',  NULL  , 0, "Print each iteration, not just the end result"                  , 0 },
	{ "format"    , 'f',  NULL  , 0, "The printf() conversion specification to use when printing "
	                                 "doubles. (String, default %f)"                                  , 0 },
	{ "iterations", 'i', "NITER", 0, "How many iterations to compute. (Positive integer, default 5)"  , 0 },
	{ "list-names", 'l',  NULL  , 0, "Print the list of NAMEs accepted by --name and return"          , 0 },
	{ "name"      , 'n', "NAME" , 0, "Construct a pre-built fractal called NAME."                     , 0 },
	{ "svg"       , 's',  NULL  , 0, "Output SVG markup instead of pure coordinates"                  , 0 },
	{ 0 }
};

typedef struct options {
	bool all;
	char* format;
	unsigned iterations;
	bool list_names;
	char* name;
	bool svg;
	// internal-only: the parse_opt function is responsible for forming these out of the arguments.
	darray* initial;
	darray* rule;
} options_t;

options_t options = {
	false, //all
	"%f",  //format
	5,     //iterations
	false, //list-names
	NULL,  //names
	false, //svg
	//internal only
	NULL,  //initial
	NULL   //rule
};

extern const char * const named_fractal_names[];
extern darray* named_fractal_data[];
extern const size_t named_fractal_count;

int bsearch_strcmp(const void* a, const void* b){
	return strcmp( (char*) a, (char*) b);
}

static error_t parse_opt (int key, char *arg, struct argp_state *state){
	int failure;
	
	options_t* args = state->input;
	switch(key){
		case 'a':
			args->all = true;
			break;
		case 'f':
			args->format = arg;
			break;
		case 'i':
			failure = sscanf(arg , "%u", &args->iterations);
			if (failure != 1){
				argp_error(state, "number of iterations should be a non-negative integer");
			}
			break;
		case 'l':
			args->list_names = true;
			break;
		case 'n':
			for (size_t index = 0; index < named_fractal_count; index++){
				if (strcmp(named_fractal_names[index], arg) == 0){
					args->name = arg;
					args->initial = named_fractal_data[2*index];
					args->rule = named_fractal_data[2*index + 1];
					break;
				}
			}
			if (args->name == NULL){
				argp_error(state, "Unknown fractal %s. Use --list-names to print the names of known fractals", arg);
			}
			break;
		case 's':
			args->svg = true;
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static const char args_doc[] = "";
static const char doc[] = "Compute iterations approximating various (topolgically) 1D fractals";

struct argp parser = {
	// second entry is the argp_parser_t parser
	options_definition, parse_opt, args_doc, doc, NULL, NULL, NULL
};

int main(int argc, char** argv){	
	error_t result = argp_parse(&parser, argc, argv, 0, NULL, &options);
	//TODO: use argp_error
	if (result){
		fprintf(stderr, "%s argument parser: %s\n", argv[0], strerror(result));
		exit(EXIT_FAILURE);
	}
	
	darray* list = copy_darray(options.initial);
	double initial_length = euclidean_length(list->points[0], list->points[1], list->points[2], list->points[3]);
	
	if (options.svg){
		printf("<svg version='1.1' width='2' height='1' viewbox='-0.5 -0.5 2 1' xmlns='http://www.w3.org/2000/svg'>\n");
	}
	
	for (size_t i = 0; i < options.iterations; i++){
		if (options.all){
			print_darray(list, options.format, options.svg);
		}
		darray* newlist = iteration(list, initial_length, options.rule);
		free(list->points);
		free(list);
		list = newlist;
		newlist = NULL;
	}
	
	print_darray(list, options.format, options.svg);
	if (options.svg){
		printf("</svg>");
	}
	
	free(list->points);
	free(list);
	exit(EXIT_SUCCESS);
}