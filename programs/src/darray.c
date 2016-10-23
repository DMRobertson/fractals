#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "darray.h"

darray* new_darray(size_t length){
	darray* output = malloc(sizeof(darray));
	output->length = length;
	output->points = malloc(length * sizeof(double));
	return output;
}

darray* load_darray(FILE* file, char* fmt){
	int result, result2;
	char c;
	size_t length;
	fscanf(file, "%zu", &length);
	printf("length: %zu\n", length);
	darray* output = new_darray(length); 
	for (size_t i = 0; i < length; i += 2){
		c = getc(file); ungetc(c, file);
		result = fscanf(file, fmt, output->points + i);
		c = getc(file); ungetc(c, file);
		result2 = fscanf(file, ",");
		c = getc(file); ungetc(c, file);
		result = fscanf(file, fmt, output->points + i+1);
	}
	return output;
}

darray* copy_darray(const darray const* source){
	darray* output = malloc(sizeof(darray));
	output->length = source->length;
	output->points = malloc(sizeof(double) * output->length);
	memcpy(output->points, source->points, sizeof(double) * output->length);
	return output;
}

void free_darray(darray* source){
	free(source->points);
	free(source);
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