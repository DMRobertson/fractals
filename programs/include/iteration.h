#ifndef ITERATION_H_
#define ITERATION_H_

#include "darray.h"

double euclidean_length(const double x1, const double y1, const double x2, const double y2);

void transform(
	const double* const target, const size_t tindex,
	const darray* const replacement, double* const dest, const size_t destindex, const char flip
);

darray* iteration(const darray * const list, const darray * const rule, const char* const flip_pattern);

#endif //ITERATION_H_