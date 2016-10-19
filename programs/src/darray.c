#include <string.h>
#include <stdio.h>
#include "darray.h"

darray* copy_darray(const darray const* source){
	darray* output = malloc(sizeof(darray));
	output->length = source->length;
	output->points = malloc(sizeof(double) * output->length);
	memcpy(output->points, source->points, sizeof(double) * output->length);
	return output;
}

void print_darray(const darray const* source){
	for (size_t j=0; j < source->length; j++){
		printf("%f ", source->points[j]);
	}
	printf("\n");
}