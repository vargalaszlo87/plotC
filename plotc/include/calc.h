#ifndef CALC_H
#define CALC_H

/*!
 * variables
 */
 
extern float axisXValues[16];
extern float axisYValues[16];

/*!
 * methods
 */

int max_int(int *, int );
int min_int(int *, int );

float max_float(float *, int);
float min_float(float *, int);

char* format_number_static(double);

float get_y_from_x(float*, float*, int, float);
void set_axis_values(float* , float *, int);


#endif