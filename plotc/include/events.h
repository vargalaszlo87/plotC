#ifndef EVENTS_H
#define EVENTS_H

#include "config.h"

/*!
 * variables
 */
 
extern
	int
		// rendering bit
		renderingNow,
		resizedNow,

		// mouse position
		mouseX,
		mouseY,
		mouseXinGrid,
		mouseYinGrid,
		
		// window size
		width,
		height,
		
		// grid
		gridPositionProjectionX[MAX_GRID_LINE],
		gridPositionProjectionY[MAX_GRID_LINE];

extern 
	float
		// margins	
		margin,
		margin_x,
		margin_y;


// GLFW window
extern GLFWwindow* window;

// viewbound
extern bounds viewBounds; 

// margins

/*!
 * methods
 */

void framebuffer_size_callback(GLFWwindow*, int, int);
void cursor_position_callback(GLFWwindow*, double, double);
void scroll_callback(GLFWwindow* window, double, double);

extern float plotc_unscale(int, float, float, float, int);

#endif