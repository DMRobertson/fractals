#include <math.h>
#include <string.h>
#include "iteration.h"

double euclidean_length(const double x1, const double y1, const double x2, const double y2){
	//The Euclidean distance between (x1, y1) and (x2, y2)
	double x = x1 - x2;
	double y = y1 - y2;
	return sqrt(x*x + y*y);
}

void transform(
	const double* const target, const size_t tindex,
	const darray* const replacement, double* const dest, const size_t destindex, const char flip
){
	double hypotenuse = euclidean_length(target[tindex], target[tindex+1], target[tindex+2], target[tindex+3]);
	double sine   = (target[tindex+3] - target[tindex+1]) / hypotenuse;
	double cosine = (target[tindex+2] - target[tindex+0]) / hypotenuse;
	
	bool hflip = false;
	bool vflip  = false;
	switch (flip){
		case 'R': //random
			vflip = rand() & 1;
			break;
		case 'V': //vertical
			vflip = true;
			break;
		case 'H': //horizontal
			hflip = true;
			break;
		case 'B': //both
			hflip = true;
			vflip  = true;
			break;
		default: //e.g. n for normal
			break;
	}
	
	size_t writeindex;
	for (size_t k = 0; k < replacement->length; k += 2){
		double x = replacement->points[k];
		double y = replacement->points[k+1];
		
		if (vflip){ y *= -1; }
		if (hflip){
			x = 1 - x;
			// need to flip order of points in memory too
			writeindex = destindex + replacement->length - 2 - k;
		} else {
			writeindex = destindex + k;
		}
		
		x *= hypotenuse;
		y *= hypotenuse;
		
		dest[writeindex]     = x * cosine - y * sine;
		dest[writeindex + 1] = x * sine   + y * cosine;
		
		dest[writeindex]     += target[tindex];
		dest[writeindex + 1] += target[tindex+1];
	}
}

darray* iteration(const darray * const list, const darray * const rule, const char* const flip_pattern){
	size_t points = (list->length/2 - 1) * (rule->length/2 + 1) + 1;
	darray* newlist = malloc(sizeof(darray));
	newlist->length = points*2;
	newlist->points = malloc(sizeof(double) * newlist->length );
	newlist->points[0] = list->points[0];
	newlist->points[1] = list->points[1];
	
	size_t modulus = strlen(flip_pattern);
	size_t k = 0;
	for (size_t i=0, j=2; i < list->length - 2; i += 2){
		transform(list->points, i, rule, newlist->points, j, flip_pattern[k]);
		j += rule->length;
		newlist->points[j+0] = list->points[i+2];
		newlist->points[j+1] = list->points[i+3];
		j += 2;
		k++;
		k %= modulus;
	}
	return newlist;
}

