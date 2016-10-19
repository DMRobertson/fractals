#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "darray.h"

darray* copy_darray(const darray const* source){
	darray* output = malloc(sizeof(darray));
	output->length = source->length;
	output->points = malloc(sizeof(double) * output->length);
	memcpy(output->points, source->points, sizeof(double) * output->length);
	return output;
}

void print_darray(const darray const* source, const char * const format, const bool svg){
	if (svg){
		printf("<polyline style='fill:none; stroke: black; stroke-width: 0.02px' points=\"");
	}
	for (size_t j=0; j < source->length; j += 2){
		printf(format, source->points[j]);
		printf(",");
		printf(format, source->points[j+1]);
		printf(" ");
	}
	if (svg){
		printf("\" />");
	}
	printf("\n");
}