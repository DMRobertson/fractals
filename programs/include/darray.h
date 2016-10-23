#ifndef DARRAY_H_
#define DARRAY_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct darray {
	size_t length;
	double* points;
} darray;

darray* copy_darray(darray const * const source);
darray* new_darray(size_t length);
darray* load_darray(FILE* file, char* fmt);
void free_darray(darray* source);
void print_darray(darray const * const source, const char * const format, const bool svg);

#endif // DARRAY_H_