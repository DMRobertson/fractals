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

darray* copy_darray(const darray const* source){
	darray* output = malloc(sizeof(darray));
	output->length = source->length;
	output->points = malloc(sizeof(double) * output->length);
	memcpy(output->points, source->points, sizeof(double) * output->length);
	return output;
}

darray* open_and_load_darray(char* filename, char* fmt){
	FILE* file = fopen(filename, "r");
	if (file == NULL){
		return NULL;
	}
	darray* output = load_darray(file, fmt);
	fclose(file);
	return output;
}

darray* load_darray(FILE* file, char* fmt){
	size_t length;
	fscanf(file, "%zu", &length);
	// TODO: check length is even and probably > 0;
	printf("length: %zu\n", length);
	darray* output = new_darray(length); 
	for (size_t i = 0; i < length; i += 2){
		fscanf(file, fmt, output->points + i);
		fscanf(file, ",");
		fscanf(file, fmt, output->points + i+1);
	}
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