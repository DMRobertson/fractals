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
	//name              key   arg       flags                doc                                                              group
	{ "all"           , 'a',  NULL    , 0,                   "Print each iteration, not just the end result"                  , 0 },
	{ "draw"          , 'd', "WIDTH"  , OPTION_ARG_OPTIONAL, "Output SVG and draw the path as a line of length WIDTH pixels "       
	                                                         "(double, default 1)"                                            , 0 },
	{ "fill"          , 'f',  NULL    , 0,                   "Output SVG and fill the area bounded by the path."              , 0 },
	{ "format"        , 'F', "FORMAT" , 0,                   "The printf() conversion specification to use when printing"           
	                                                         "or reading doubles (default '%lf')"                             , 0 },
	{ "include"       , 'I', "INCLUDE", 0,                   "Look for NAME.dat files in this directory  (default '.'). "           
	                                                         "If used, it must occur before any --name=NAME option."          , 0 },
	{ "list"          , 'l',  NULL    , 0,                   "Print the names accepted by the INITIAL and RULE arguments and "      
	                                                         "return. Looks in the directory specified by --include"          , 0 },
	{ "niter"         , 'n', "NITER"  , 0,                   "How many iterations to compute (nonegative integer, default 5)" , 0 },
	{ 0 }
};

extern options_t options;
extern darray* load_named_data(char* filename, char* ext, options_t* args);

static error_t parse_opt (int key, char *arg, struct argp_state *state){
	int result;
	int num_args;
	
	options_t* args = state->input;
	switch(key){
		case 'a':
			args->all = true;
			break;
		case 'd':
			args->svg = true;
			if (arg == NULL){
				args->draw = 1.0;
			} else {
				sscanf(arg, "%lf", &args->draw);
			}
			break;
		case 'f':
			args->fill = true;
			args->svg = true;
			break;
		case 'F':
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
		case ARGP_KEY_NO_ARGS:
			argp_usage(state);
			break;
		case ARGP_KEY_ARG:
			// We'll get here when reaching the 0th positional argument.
			// Check we have enough positional arguments.
			num_args = 1 + state->argc - state->next;
			if (num_args < 2 || num_args > 3){
				argp_usage(state);
			}
			char** arg_ptr = state->argv + state->next - 1;
			args->initial = load_named_data(arg_ptr[0], "init", args);
			args->rule = load_named_data(arg_ptr[1], "rule", args);
			if (num_args == 3){
				args->flip = arg_ptr[2];
				for (char* ptr = args->flip; *ptr != '\0'; ptr++){
					//convert to ASCII uppercase
					*ptr &= 0b11011111;
				}
				fprintf(stderr, "%s\n", args->flip);
			}
			// Force argp to stop by informing it that it has consumed all args.
			state->next = state->argc;
 			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static const char args_doc[] = "INITIAL RULE [FLIP]";
static const char doc[] =
	"Compute iterations approximating various (topolgically) 1D fractals"
	"\v"
	"INITIAL and RULE are paths pointing at a file, which contains the textual description of a darray. "
	"This consists of an even unsigned integer, followed by pairs of the form '<float>,<float>'. "
	"The total number of floats must be the given integer."
	"Alternatively, one or both of these arguments can be given as '-', in which case we read the text description from stdin."
	"\n\n"
	"INITIAL looks for data in files INITIAL, INITIAL.init, and (if --include is present) INCLUDE/INITIAL and INCLUDE/INITIAL.init. "
	"If INITIAL already ends in '.init' then the second and fourth possibilities are skipped."
	"RULE does the same using the .rule extension instead of .init."
	"\n\n"
	"The RULE is a list of points p1, ..., pn and represents a deformation of the line segment (0, 0) to (1, 0). "
	"Every line segment in INITIAL is replaced by a scaled, rotated and possibly reflected copy of the RULE. "
	"Reflection is controlled by the FLIP argument."
	//TODO describe FLIP
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
	
	darray** iterations = malloc(sizeof(darray*) * (options.niter + 1));
	iterations[0] = options.initial;
	
	if (options.niter > 0){
		size_t i = 0;
		//Don't apply any flips on the first application if we're beginning with a line segment
		if (options.initial->length == 4){
			iterations[i+1] = iteration(iterations[i], options.rule, "N");
			i++;
		}
		for (; i < options.niter; i++){
			iterations[i+1] = iteration(iterations[i], options.rule, options.flip);
		}
	}
	darray* last = iterations[options.niter];
	
	if (options.svg){
		double xmin = +INFINITY;
		double xmax = -INFINITY;
		double ymin = +INFINITY;
		double ymax = -INFINITY;
		
		for (size_t j = 0; j < last->length; j += 2){
			xmin = fmin(xmin, last->points[j]);
			xmax = fmax(xmax, last->points[j]);
			ymin = fmin(ymin, last->points[j+1]);
			ymax = fmax(ymax, last->points[j+1]);
		}
		double width  = xmax - xmin;
		double height = ymax - ymin;
		printf("<svg version='1.1' width='%f' height='%f' viewbox='0 0 %f %f' xmlns='http://www.w3.org/2000/svg'>\n",
			width, height, width, height);
		printf("\t<g transform='translate(%f,%f)'>\n", -xmin, -ymin);
	}
	
	if (options.all){
		for (size_t i = 0; i < options.niter; i++){
			printf("\t\t");
			print_darray(iterations[i], &options);
		}
	}
	print_darray(iterations[options.niter], &options);
	if (options.svg){
		printf("\t</g>\n");
		printf("</svg>");
	}
	
	for (size_t i = 0; i <= options.niter; i++){
		free_darray(iterations[i]);
	}
	free(iterations);
	free_darray(options.rule);
	exit(EXIT_SUCCESS);
}