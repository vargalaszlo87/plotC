#ifndef DRAW_H
#define DRAW_H

#include <stdio.h>

#include "stb_truetype.h"

/*!
 * variables
 */

extern unsigned char bitmap[512*512];
extern stbtt_bakedchar cdata[96];
extern unsigned char ttf_buffer[1<<20];
extern GLuint font_texture;
extern float gridPositionModelX[16];
extern float gridPositionModelY[16];
extern int gridPositionProjectionX[16];
extern int gridPositionProjectionY[16];
extern int width, height;
extern float margin;
extern int margin_px, marginX_px, marginY_px;
extern int mouseX, mouseY;
extern GLFWwindow* window;

typedef struct {
	float xmin, xmax, ymin, ymax;
} bounds;


/*!
 * methods
 */
 
extern void begin_pixel_mode(int, int);
extern void end_pixel_mode();
 
float plotc_scale(float, float, float, float);
float plotc_unscale(float, float, float, float, int);
bounds plotc_draw_grid_scale_calc(float*, float*, int);
void plotc_draw_grid(float, float, float, float, float);
void plotc_draw_data(float*, float*, int, bounds, float);
void draw_text(float, float, const char*);
void draw_crosshair(int, int);
void init_font_texture(const char*);


#endif