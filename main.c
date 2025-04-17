#include "plotc.h"
#include <math.h>

#define SIZE 200

int main() {
	
	
    float x[SIZE], y[SIZE];
    for (int i = 0; i < SIZE; i++) {
        x[i] = (float)i / 99.0f * 2.0f - 1.0f;
        y[i] = sinf(x[i] * 3.14f);
    }
    plotc(x, y, SIZE, "Szinusz");
    return 0;
}
