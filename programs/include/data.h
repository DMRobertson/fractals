#ifndef DATA_H_
#define DATA_H_

#include "darray.h"

double _koch_initial[4] = {0.0, 0.0, 1.0, 0.0};
darray koch_initial = {4, _koch_initial};

//fourth entry is tan(pi/3)/6.0
double _koch_rule[6] = {1/3.0, 0, 1/2.0, 0.28867513459, 2/3.0, 0};
darray koch_rule = {6, _koch_rule};
#endif