#ifndef PLOT_H
#define PLOT_H

/*!
 * variables
 */
 
#include <GL/gl.h>
#include "glfw3.h"
#include "stb_truetype.h"
 
extern int renderingNow;
extern int resizedNow;

extern float axisXValues[16];
extern float axisYValues[16];
extern float gridPositionModelX[16];
extern float gridPositionModelY[16];
extern int gridPositionProjectionX[16];
extern int gridPositionProjectionY[16];
extern int width, height;
extern float margin;
extern int margin_px, marginX_px, marginY_px;
extern int mouseX, mouseY;
extern GLFWwindow* window;

/*! 
 * methods
 */
 
void plotc(float*, float*, int, const char*);

void begin_pixel_mode(int, int);
void end_pixel_mode();

void set_axis_values(float*, float*, int);

#endif