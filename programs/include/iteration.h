#ifndef ITERATION_H_
#define ITERATION_H_

#include "darray.h"

double euclidean_length(const double x1, const double y1, const double x2, const double y2);

void transform(const double * const target, const size_t tindex, const double initial_length, const darray * const replacement, double * const dest, const size_t destindex);

darray* iteration(const darray * const list, const double initial_length, const darray * const rule);

#endif //ITERATION_H_