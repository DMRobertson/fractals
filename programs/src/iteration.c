#include <math.h>
#include "iteration.h"
#include "stdio.h"

double euclidean_length(const double x1, const double y1, const double x2, const double y2){
	//The Euclidean distance between (x1, y1) and (x2, y2)
	double x = x1 - x2;
	double y = y1 - y2;
	return sqrt(x*x + y*y);
}

void transform(const double * const target, const size_t tindex, const darray * const replacement, double * const dest, const size_t destindex){
	double hypotenuse = euclidean_length(target[tindex], target[tindex+1], target[tindex+2], target[tindex+3]);
	double sine   = (target[tindex+3] - target[tindex+1]) / hypotenuse;
	double cosine = (target[tindex+2] - target[tindex+0]) / hypotenuse;
	
	for (size_t k = 0; k < replacement->length; k += 2){
		double x = replacement->points[k];
		double y = replacement->points[k+1];
		
		x *= hypotenuse;
		y *= hypotenuse;
		
		dest[destindex+k]  = x * cosine - y * sine;
		dest[destindex+k+1]= x * sine   + y * cosine;
		
		dest[destindex+k]   += target[tindex];
		dest[destindex+k+1] += target[tindex+1];
	}
}

darray* iteration(const darray * const list, const darray * const rule){
	size_t points = (list->length/2 - 1) * (rule->length/2 + 1) + 1;
	darray* newlist = malloc(sizeof(darray));
	newlist->length = points*2;
	newlist->points = malloc(sizeof(double) * newlist->length );
	newlist->points[0] = list->points[0];
	newlist->points[1] = list->points[1];
	
	for (size_t i=0, j=2; i < list->length - 2; i += 2){
		transform(list->points, i, rule, newlist->points, j);
		j += rule->length;
		newlist->points[j+0] = list->points[i+2];
		newlist->points[j+1] = list->points[i+3];
		j += 2;
	}
	return newlist;
}

