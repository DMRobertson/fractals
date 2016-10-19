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
	{ 0 }
};

struct arguments {
	unsigned iterations;
	bool all;
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
	arguments.all = 0; // false
	
	error_t result = argp_parse(&parser, argc, argv, 0, NULL, &arguments);
	if (result){
		fprintf(stderr, "%s argument parser: %s\n", argv[0], strerror(result));
		exit(EXIT_FAILURE);
	}
	
	printf("all: %d niter: %d\n", arguments.all, arguments.iterations);
	
	darray* list = copy_darray(&koch_initial);
	darray* rule = copy_darray(&koch_rule);
	
	for (size_t i = 0; i < 2; i++){
		darray* newlist = iteration(list, rule);
		free(list->points);
		free(list);
		list = newlist;
		newlist = NULL;
	}
	
	print_darray(list);	
	free(list->points);
	free(list);
	exit(EXIT_SUCCESS);
}