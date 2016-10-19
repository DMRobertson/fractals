#include <math.h>
#include "iteration.h"

double euclidean_length(const double x1, const double y1, const double x2, const double y2){
	//The Euclidean distance between (x1, y1) and (x2, y2)
	double x = x1 - x2;
	double y = y1 - y2;
	return sqrt(x*x + y*y);
}

void transform(const double * const target, const size_t tindex, const darray * const replacement, double * const dest, const size_t destindex){
	double hypotenuse = euclidean_length(target[tindex], target[tindex+1], target[tindex+2], target[tindex+3]);
	double scale = 1 / hypotenuse;
	double sine   = (target[tindex+3] - target[tindex+1]) / hypotenuse;
	double cosine = (target[tindex+2] - target[tindex+0]) / hypotenuse;
	
	for (size_t i = 0; i < replacement->length; i += 2){
		double x = replacement->points[i];
		double y = replacement->points[i+1];
		
		x -= target[tindex];
		y -= target[tindex+1];
		x /= scale;
		y /= scale;
		dest[destindex+i]   = x * cosine - y * sine;
		dest[destindex+i+1] = x * sine   + y * cosine;
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

