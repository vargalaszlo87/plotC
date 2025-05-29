/*!
 * @project plotC  v.0.1	
 * @file calc.c
 * @brief Some math tools for project.
 *
 * Version information:
 *
 *
 * Features:
 * - min / max in float and int
 *
 * @author Varga Laszlo
 *
 * @website https://github.com/vargalaszlo87/plotC
 * @website http://vargalaszlo.com
 * @website http://ha1cx.hu
 *
 * @date 2025-05-27
 *
 * @license
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "calc.h"
#include "config.h"

int max_int(int *a, int n) {
    int temp = a[0];
    int i = 0;
    while (++i < n)
        temp = (a[i] > temp) ? a[i] : temp;
    return temp;
}

int min_int(int *a, int n) {
    int temp = a[0];
    int i = 0;
    while (++i < n)
        temp = (a[i] < temp) ? a[i] : temp;
    return temp;
}

float max_float(float *a, int n) {
    float temp = a[0];
    int i = 0;
    while (++i < n)
		temp = (a[i] > temp) ? a[i] : temp;	
    return temp;
}

float min_float(float *a, int n) {
	float temp = a[0];
    int i = 0;
    while (++i < n)
		temp = (a[i] < temp) ? a[i] : temp;	
    return temp;
}

char* format_number_static(double value) {
    char * out = (char*)calloc(32, sizeof(char));
    double abs_val = fabs(value);
    if ((abs_val >= 1000.0) || (abs_val > 0 && abs_val <= 0.01))
        sprintf(out, "%.3e", value);
    else
        sprintf(out, "%.3f", value);
    return out;
}

float get_y_from_x(float* x, float* y, int n, float xval) {
    for (int i = 0; i < n - 1; i++) {
        if (xval >= x[i] && xval <= x[i + 1]) {
            float t = (xval - x[i]) / (x[i + 1] - x[i]);
            return y[i] + t * (y[i + 1] - y[i]);  // lineáris interpoláció
        }
    }
    return 0.0f;  // ha kívül van, vissza 0 vagy hibajelzés
}


void set_axis_values(float* x, float *y, int n) {

	// get min, max value
	float maxValue = max_float(y, n);
	float minValue = min_float(y, n);
	float step = (maxValue - minValue) / 10;

	// fill the axis Y value matrix
	for (int i = 0; i <= 10 ; i++) {
		axisYValues[i] = minValue + (step * i);
	}
}
