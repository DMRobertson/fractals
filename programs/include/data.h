#ifndef DATA_H_
#define DATA_H_

#include "darray.h"

// should be kept sorted
const char* const named_fractal_names[] = {
	"koch",
	"test"
};

//TODO make this automatic innit
const size_t named_fractal_count = 2;

static darray unit_interval = {4, (double[]) { 0.0, 0.0, 1.0, 0.0} };

darray* named_fractal_data[] = {
	//initial then replacement
	//koch. The fourth double in the replacement rule is tan(pi/3)/6
	&unit_interval,
	&(darray) {6, (double[]) { 1/3.0, 0, 1/2.0, 0.28867513459, 2/3.0, 0 } },
	//test. Random example I asked for just to check it worked.
	&unit_interval,
	&(darray) {8, (double[]) {1/5.0, 0, 2/5.0, 0.2, 3/5.0, -0.2, 4/5.0, 0} },
};

#endif // DATA_H_