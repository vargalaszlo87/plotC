#ifndef DRAW_H
#define DRAW_H

#include <stdio.h>
#include <stdbool.h>

#include "stb_truetype.h"

/*!
 * variables
 */
 
extern
	int
		// gird
		gridPositionProjectionX[MAX_GRID_LINE],
		gridPositionProjectionY[MAX_GRID_LINE],
		
		// axis
		maxAxisYValueSizeInPx,
		
		// frame size
		width,
		height,
		
		// margins
		margin_px,
		marginX_px,
		marginY_px,
		
		// mouse
		mouseX, 
		mouseY;
		
extern
	float
		// margin
		margin,
		axisXValues[MAX_GRID_LINE],
		axisYValues[MAX_GRID_LINE],
		gridPositionModelX[MAX_GRID_LINE],
		gridPositionModelY[MAX_GRID_LINE];

extern
	unsigned char
		bitmap[512*512],
		ttf_buffer[1<<20];
		
	
extern stbtt_bakedchar cdata[96];
extern GLuint font_texture;
extern GLFWwindow* window;

/*!
 * methods
 */
 
// extern
 
	extern void begin_pixel_mode(int, int);
	extern void end_pixel_mode();

	extern int max_int(int *, int);

	extern float get_y_from_x(float*, float*, int, float);

// own
 
	void plotc_draw_grid(float, float, float, float, float, float);
	void plotc_draw_data(float*, float*, int, bounds, float, float);
	void plotc_draw_axis_labels(const char*, const char*);
	void plotc_draw_axis_y_values(float); 
	void draw_crosshair(int, int);
	void init_font_texture(const char*);
	void plotc_draw_probe_dot(float*, float*, int, int, int, bounds, float, float);

	bool mouse_in_range();

	int draw_text(float, float, const char*);
	 
	float plotc_scale(float, float, float, float);
	float plotc_unscale(int, float, float, float, int);

	bounds plotc_draw_grid_scale_calc(float*, float*, int);

#endif