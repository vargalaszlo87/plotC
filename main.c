#include "plotc.h"
#include <math.h>

int main() {
	
	
    float x[100], y[100];
    for (int i = 0; i < 100; i++) {
        x[i] = (float)i / 99.0f * 2.0f - 1.0f;
        y[i] = sinf(x[i] * 3.14f);
    }
    plotc(x, y, 100, "Szinusz");
    return 0;
}
