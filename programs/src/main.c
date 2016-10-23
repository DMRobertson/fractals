#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

// glibc specific
#include <argp.h>
#include <unistd.h>
// #define __USE_GNU
// #include <fenv.h>

#define PI 3.141592653589

#include "darray.h"
#include "iteration.h"
#include "options.h"

static struct argp_option options_definition[] = {
	//name          key  arg      flags doc                                                           group
	{ "all"           , 'a',  NULL  , 0, "Print each iteration, not just the end result"                  , 0 },
	{ "iterations"    , 'i', "NITER", 0, "How many iterations to compute. (Nonegative integer, default 5)", 0 },
	{ "include"       , 'I', "INC"  , 0, "Look for NAME.dat files in this directory  (default '.')."           
	                                     "If used, it must occur before any --name=NAME option."          , 0 },
	{ "list-names"    , 'l',  NULL  , 0, "Print the list of NAMEs accepted by --name and return"          , 0 },
	{ "name"          , 'n', "NAME" , 0, "Construct a pre-built fractal called NAME"                      , 0 },
	{ "printf"        , 'p',  "FMT" , 0, "The printf() conversion specification to use when printing"           
	                                     "doubles. (String, default '%lf')"                                , 0 },
	{ "svg"           , 's',  NULL  , 0, "Output SVG markup instead of raw coordinates"                   , 0 },
	{ 0 }
};

extern options_t options;

extern const char * const named_fractal_names[];
extern darray* named_fractal_data[];
extern const size_t named_fractal_count;

static error_t parse_opt (int key, char *arg, struct argp_state *state){
	int result;
	char* filename;
	
	options_t* args = state->input;
	switch(key){
		case 'a':
			args->all = true;
			break;
		case 'i':
			// result is the number of things scanned
			while ( isspace(*arg) ){
				arg++;
			}
			if (*arg == '-'){
				result = -1;
			} else {
				result = sscanf(arg , "%u", &args->iterations);
			}
			if (result != 1){
				argp_failure(state, EINVAL, 0, "number of iterations should be a non-negative integer (received '%s')", arg);
			}
			break;
		case 'I':
			args->names_include = arg;
			break;
		case 'l':
			args->list_names = true;
			//TODO: print out list of names
			exit(EXIT_SUCCESS);
			break;
		case 'n':
		 	//"/",  ".dat" and '\0' contribute 1, 4 and 1 char respectively, for a total of 6
			filename = malloc(sizeof(char) * (strlen(arg) + strlen(args->names_include) + 6) ); 
			sprintf(filename, "%s/%s.dat", args->names_include, arg);
			result = access(filename, F_OK | R_OK);
			if (result != 0){
				argp_failure(state, EXIT_FAILURE, errno, "named fractal data %s", filename);
			}
			//TODO: load initial and rule here
			result = load_named_fractal(filename, &options);
			if (result != 0){
				argp_failure(state, EXIT_FAILURE, errno, "Data file error %s", filename);
			}
			free(filename);
			break;
		case 'p':
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

static const char args_doc[] = "";
static const char doc[] = "Compute iterations approximating various (topolgically) 1D fractals";

struct argp parser = {
	options_definition, parse_opt, args_doc, doc, NULL, NULL, NULL
};

int main(int argc, char** argv){	
	error_t result = argp_parse(&parser, argc, argv, 0, NULL, &options);
	if (result){
		fprintf(stderr, "%s: Problem parsing arguments: %s\n", argv[0], strerror(result));
		exit(EXIT_FAILURE);
	}
	
	if (options.initial == NULL || options.rule == NULL){
		fprintf(stderr, "%s: Unspecified initial setup or replacement rule\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	darray* list = options.initial;
	double initial_length = euclidean_length(list->points[0], list->points[1], list->points[2], list->points[3]);
	
	if (options.svg){
		printf("<svg version='1.1' width='2' height='1' viewbox='-0.5 -0.5 2 1' xmlns='http://www.w3.org/2000/svg'>\n");
	}
	
	for (size_t i = 0; i < options.iterations; i++){
		if (options.all){
			print_darray(list, options.format, options.svg);
		}
		darray* newlist = iteration(list, initial_length, options.rule);
		free_darray(list);
		list = newlist;
		newlist = NULL;
	}
	
	print_darray(list, options.format, options.svg);
	if (options.svg){
		printf("</svg>");
	}
	
	free_darray(list);
	free_darray(options.rule);
	exit(EXIT_SUCCESS);
}