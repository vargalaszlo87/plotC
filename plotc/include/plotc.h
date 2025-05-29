#ifndef PLOT_H
#define PLOT_H

/*!
 * variables
 */
 
#include <GL/gl.h>
#include "glfw3.h"
#include "stb_truetype.h"
#include "config.h"
#include "type.h"
 
extern
	int
		renderingNow,
		resizedNow,
		
		// axis
		maxAxisYValueSizeInPx,
		
		// grid
		gridPositionProjectionX[16],
		gridPositionProjectionY[16],
		
		// frame size
		width,
		height,
		
		// margin
		margin_px,
		
		// mouse
		mouseX,
		mouseY;
		
extern
	float
		// axis
		axisXValues[MAX_GRID_LINE],
		axisYValues[MAX_GRID_LINE],
		
		// grid
		gridPositionModelX[MAX_GRID_LINE],
		gridPositionModelY[MAX_GRID_LINE],
 
		// margin
		margin,
		margin_x,
		margin_y;

extern bounds b;
extern bounds viewBounds; 
extern GLFWwindow* window;

/*! 
 * methods
 */
 
// own
 
void plotc(float*, float*, int, const char*);

void begin_pixel_mode(int, int);
void end_pixel_mode();

void set_axis_values(float*, float*, int);

#endif