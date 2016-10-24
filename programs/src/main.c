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
	//name              key   arg           flags doc                                                           group
	{ "all"           , 'a',  NULL    , 0, "Print each iteration, not just the end result"                  , 0 },
	{ "format"        , 'f', "FORMAT" , 0, "The printf() conversion specification to use when printing"           
	                                       "or reading doubles (String, default '%lf')"                     , 0 },
	{ "include"       , 'I', "INCLUDE", 0, "Look for NAME.dat files in this directory  (default '.'). "           
	                                       "If used, it must occur before any --name=NAME option."          , 0 },
	{ "list"          , 'l',  NULL    , 0, "Print the names accepted by the INITIAL and RULE arguments and "      
	                                       "return. Looks in the directory specified by --include"          , 0 },
	{ "niter"         , 'n', "NITER"  , 0, "How many iterations to compute. (Nonegative integer, default 5)", 0 },
	{ "svg"           , 's',  NULL    , 0, "Output SVG markup instead of raw coordinates"                   , 0 },
	{ 0 }
};

extern options_t options;
extern darray* load_named_data(char* filename, char* ext, options_t* args);

static error_t parse_opt (int key, char *arg, struct argp_state *state){
	int result;
	int remaining_args;
	
	options_t* args = state->input;
	switch(key){
		case 'a':
			args->all = true;
			break;
		case 'f':
			args->format = arg;
			break;
		case 'I':
			args->include = arg;
			break;
		case 'l':
			args->list = true;
			break;
		case 'n':
			// result is the number of things scanned
			while ( isspace(*arg) ){
				arg++;
			}
			if (*arg == '-'){
				result = -1;
			} else {
				result = sscanf(arg , "%u", &args->niter);
			}
			if (result != 1){
				argp_failure(state, EINVAL, 0, "number of iterations should be a non-negative integer (received '%s')", arg);
			}
			break;
		case 's':
			args->svg = true;
			break;
		case ARGP_KEY_NO_ARGS:
			argp_usage(state);
			break;
		case ARGP_KEY_ARG:
			// We'll get here when reaching the 0th positional argument.
			// Check we have enough positional arguments.
			remaining_args = state->argc - state->next;
			if (remaining_args != 1){
				argp_usage(state);
			}
			args->initial = load_named_data(arg, "init", args);
			args->rule = load_named_data(state->argv[state->next], "rule", args);
			// Force argp to stop by informing it that it has consumed all args.
			state->next = state->argc;
 			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static const char args_doc[] = "INITIAL RULE";
static const char doc[] =
	"Compute iterations approximating various (topolgically) 1D fractals"
	"\v"
	"INITIAL and RULE are paths pointing at a file, which contains the textual description of a darray. "
	"This consists of an even unsigned integer, followed by pairs of the form '<float>,<float>'. "
	"The total number of floats must be the given integer."
	"\nAlternatively, one or both of these arguments can be given as '-', in which case we read the text description from stdin."
	"\v"
	"INITIAL looks for data in files INITIAL, INITIAL.init, and (if --include is present) INCLUDE/INITIAL and INCLUDE/INITIAL.init."
	"If INITIAL already ends in '.init' then the second and fourth possibilities are skipped."
	"RULE does the same using the .rule extension instead of .init."
;

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
	
	for (size_t i = 0; i < options.niter; i++){
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