#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define PI 3.141592653589

typedef struct darray {
	size_t length;
	double* points;
} darray;

static double _koch_initial[4] = {0.0, 0.0, 1.0, 0.0};
static darray koch_initial = {4, _koch_initial};

//fourth entry is tan(pi/3)/6.0
static double _koch_rule[6] = {1/3.0, 0, 1/2.0, 0.28867513459, 2/3.0, 0};
static darray koch_rule = {6, _koch_rule};

double euclidean_length(double x1, double y1, double x2, double y2){
	//The Euclidean distance between (x1, y1) and (x2, y2)
	double x = x1 - x2;
	double y = y1 - y2;
	return sqrt(x*x + y*y);
}

void transform(double* target, size_t tindex, darray* replacement, double* dest, size_t destindex){
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

darray* iteration(darray* list, darray* rule){
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

darray* copy_darray(darray* source){
	darray* output = malloc(sizeof(darray));
	output->length = source->length;
	output->points = malloc(sizeof(double) * output->length);
	memcpy(output->points, source->points, sizeof(double) * output->length);
	return output;
}

int main(int argc, char** argv){
	darray* list = copy_darray(&koch_initial);
	darray* rule = copy_darray(&koch_rule);
	
	for (size_t i = 0; i < 2; i++){
		darray* newlist = iteration(list, rule);
		free(list->points);
		free(list);
		list = newlist;
		newlist = NULL;
		
		for (size_t j=0; j < list->length; j++){
			printf("%f ", list->points[j]);
		}
		printf("\npoints: %zu\n", list->length);
	}
	
	free(list->points);
	free(list);
	return 0;
}