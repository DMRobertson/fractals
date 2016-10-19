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

static struct argp_option options[] = {
	//name          key  arg      flags doc                                          group
	{ "iterations", 'i', "niter", 0, "How many iterations to compute. (Positive integer, default 5)"  , 0 },
	{ "all"       , 'a', NULL   , 0, "Print each iteration, not just the end result"                  , 0 },
	{ "svg"       , 's', NULL   , 0, "Output SVG markup instead of pure coordinates"                  , 0 },
	{ "format"    , 'f', NULL   , 0, "The printf() conversion specification to use when printing "
	                                 "doubles. (String, default %f)"                                  , 0 },
	{ 0 }
};

struct arguments {
	unsigned iterations;
	bool all;
	char* format;
	bool svg;
};

static error_t parse_opt (int key, char *arg, struct argp_state *state){
	struct arguments * args = state->input;
	int failure;
	switch(key){
		case 'i':
			failure = sscanf(arg , "%u", &args->iterations);
			if (failure != 1){
				argp_error(state, "number of iterations should be a non-negative integer");
			}
			break;
		case 'a':
			args->all = true;
			break;
		case 'f':
			args->format = arg;
			break;
		case 's':
			args->svg = true;
			break;
		default:
			return ARGP_ERR_UNKNOWN;	
	}
	return 0;
}

// static char args_doc[] = "";
static char args_doc[] = "";
static char doc[] = "Compute iterations approximating various (topolgically) 1D fractals";

struct argp parser = {
	// second entry is the argp_parser_t parser
	options, parse_opt, args_doc, doc, NULL, NULL, NULL
};

int main(int argc, char** argv){
	struct arguments arguments;
	arguments.iterations = 5;
	arguments.all = false;
	arguments.format = "%f";
	arguments.svg = false;
	
	error_t result = argp_parse(&parser, argc, argv, 0, NULL, &arguments);
	if (result){
		fprintf(stderr, "%s argument parser: %s\n", argv[0], strerror(result));
		exit(EXIT_FAILURE);
	}
	
	darray* list = copy_darray(&koch_initial);
	// darray* rule = &koch_rule;
	darray* rule = &test_rule;
	
	if (arguments.svg){
		printf("<svg version='1.1' width='2' height='1' viewbox='-0.5 -0.5 2 1' xmlns='http://www.w3.org/2000/svg'>\n");
	}
	
	for (size_t i = 0; i < arguments.iterations; i++){
		if (arguments.all){
			print_darray(list, arguments.format, arguments.svg);
		}
		darray* newlist = iteration(list, rule);
		free(list->points);
		free(list);
		list = newlist;
		newlist = NULL;
	}
	
	print_darray(list, arguments.format, arguments.svg);
	if (arguments.svg){
		printf("</svg>");
	}
	
	free(list->points);
	free(list);
	exit(EXIT_SUCCESS);
}