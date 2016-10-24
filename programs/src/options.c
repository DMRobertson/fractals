#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include "options.h"

options_t options = {
	false, //all
	"%lf", //format
	"",   //include
	false, //list
	5,     //niter
	false, //svg
	//internal only
	NULL,  //initial. freed in the main loop.
	NULL   //rule     freed at the end of the main loop.
};

bool has_extension(char* filename, char* ext){
	char* near_end = strrchr(filename, '.');
	return (near_end != NULL) && (strcmp(near_end, ext) == 0);
} 

darray* load_named_data(char* filename, char* ext, options_t* args){
	if (strcmp(filename, "-") == 0){
		return load_darray(stdin, args->format);
	}
	
	size_t include_length = strlen(args->include);
	size_t length = include_length + strlen(ext) + strlen(filename) + 1; //exta 1 for the null
	char* candidate = malloc(sizeof(char) * length);
	bool correct_extension = has_extension(filename, ext);
	
	darray* result;
	result = open_and_load_darray(filename, args->format);
	if (result != NULL){ goto end; }
	if (!correct_extension){
		sprintf(candidate, "%s.%s", filename, ext);
		result = open_and_load_darray(candidate, args->format);
		if (result != NULL){ goto end; }
	}
	if (include_length > 0){
		sprintf(candidate, "%s/%s", args->include, filename);
		result = open_and_load_darray(candidate, args->format);
		if (result != NULL){ goto end; }
		if (!correct_extension){
			sprintf(candidate, "%s/%s.%s", args->include, filename, ext);
			result = open_and_load_darray(candidate, args->format);
			if (result != NULL){ goto end; }
		}
	}
	end:
	free(candidate);
	return result;
}


