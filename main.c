#include <stdio.h>
#include <stdlib.h>

#include "plotc.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.1415926535
#endif

int main() {
	
	/*!
	 * real example
	 */
	 
	// time
	float 
		t0 = 0,			// start of time in sec
		t1 = 0.1,		// end of time in sec
		s = 1000,		// stepsize
		dt = (t1 - t0) / s;	// sampling time

	// signal
	float
		f = 50,			// frequency in Hz
		A = 1.2;		// amplitude
	
	float * x = (float*)calloc(s + 1, sizeof(float));
	float * y = (float*)calloc(s + 1, sizeof(float));

	// check
	if (x == NULL || y == NULL) {
		fprintf (stderr, "! Memory allocation is failed.");
		exit(1);
	}
	
	int i = -1;
	while (++i <= s) {
		*(x + i) = i * dt;
		*(y + i) = A * sin(2 * M_PI * f * *(x + i));
	}

	// plotC
	plotc(x, y, s, "50Hz szinusz; Ido [s]; Amplitudo [V]");

	free(x);
	free(y);
	
	
	/*!
	 * default example with normalization
	 */
	/* 
    
	float x[SIZE], y[SIZE];
    for (int i = 0; i < SIZE; i++) {
        x[i] = (float)i / 99.0f * 2.0f - 1.0f;
        y[i] = sinf(x[i] * 3.14f);
    }
    plotc(x, y, SIZE, "Szinusz");
    return 0;
	*/
}
