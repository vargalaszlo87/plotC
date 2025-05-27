/*!
 * @project plotC  v.0.1	
 * @file pltoc.c
 * @brief This is a simple plotting tool for C projects with OpenGL
 *
 * Version information:
 *
 *
 * Features:
 * - plotting from one datastream (x, y)
 * - grid
 * - resizable window
 * - status bar (mouse position)
 *
 * @author Varga Laszlo
 *
 * @website https://github.com/vargalaszlo87/plotC
 * @website http://vargalaszlo.com
 * @website http://ha1cx.hu
 *
 * @date 2025-04-16
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
  
#define GL_IMPLEMENTATION
#include <GL/gl.h>

#define GLFW3_IMPLEMENTATION
#include "glfw3.h"  
 
/*!
 *	Version
 */ 
 
#define PLOTC_NAME "plotC"
#define PLOTC_VER "0.1" 

/*!
 *	defined max values
 */
 
#define MAX_GRID_LINE 16

#define MAX_TOKENS 5

/*!
 * standard includes
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

/*!
 * project includes
 */
 
#include "debug.h"
#include "plotc.h"
#include "dyndll.h"
#include "events.h"
#include "statusbar.h"
#include "draw.h"

/*!
 * Variables
 */
 
int renderingNow = 1;
int resizedNow = 1;

GLFWwindow* window;

int width;
int height;

float
	// axis values
	axisYValues[16],
	axisXValues[16],

	// calculated margin in pixel
	margin,
	margin_x,
	margin_y,
	
	// margin-Y space 
	marginYSpace = 1.2,

	// grid 
	gridPositionModelX[16],
	gridPositionModelY[16];

int
	// margin in pixel
	margin_px,
	marginX_px,
	marginY_px,
	
	// mouse
	mouseX,
	mouseY,

	// mouse inside grid
	mouseXinGrid,
	mouseYinGrid,
	
	// grid margin
	gridPositionProjectionX[16],
	gridPositionProjectionY[16],
	
	// text tokens
	tokenCount; 

enum Tokens {
	T_TITLE,
	T_AXIS_LABEL_X,
	T_AXIS_LABEL_Y
};

/*!
 * Aux methods
 */
 
void begin_pixel_mode(int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
	
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();	
}

void end_pixel_mode() {
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();	
}


char** split_by_semicolon(const char* input, int* out_count) {
    char* copy = strdup(input);
    char** tokens = malloc(MAX_TOKENS * sizeof(char*));
    int count = 0;

    char* token = strtok(copy, ";");
    while (token != NULL && count < MAX_TOKENS) {
        tokens[count++] = strdup(token); 
        token = strtok(NULL, ";");
    }

    *out_count = count;
    free(copy);
    return tokens;
}

/* DEV */

float get_y_from_x(float* x, float* y, int n, float xval) {
    for (int i = 0; i < n - 1; i++) {
        if (xval >= x[i] && xval <= x[i + 1]) {
            float t = (xval - x[i]) / (x[i + 1] - x[i]);
            return y[i] + t * (y[i + 1] - y[i]);  // lineáris interpoláció
        }
    }
    return 0.0f;  // ha kívül van, vissza 0 vagy hibajelzés
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

/*!
 * Main function of plotC
 */
 
// data X of sample, data Y of sample, size of sample, title text
void plotc(float* x, float* y, int n, const char* title) {
	
	#ifdef _WIN32
	//loadOpenGlDllOnce();	// openGL DLL betöltés
    loadGlfwDllOnce();  // ez tölti be a DLL-t és a pointereket
	#endif

	// check
	
		if (!glfwInit_ptr()) return;
		
	// tokenizer

	tokenCount = sizeof(title)/sizeof(char);
    char** tokenizedText = split_by_semicolon(title, &tokenCount);
			
	// make window

		char windowTitle[128] = {0};
		snprintf(windowTitle, sizeof(windowTitle), "%s - %s (ver. %s)", tokenizedText[T_TITLE], PLOTC_NAME, PLOTC_VER);
		
		glfwWindowHint_ptr(GLFW_CONTEXT_VERSION_MAJOR, 2); // v2.0 minimum
		glfwWindowHint_ptr(GLFW_CONTEXT_VERSION_MINOR, 0);
		
		window = glfwCreateWindow_ptr(800, 600, windowTitle, NULL, NULL);
		
		if (!window) {
			int errcode;
			const char* errmsg = glfwGetError_ptr(&errcode);
			printf("! glfwCreateWindow_ptr() failed. Error %d: %s\n", errcode, errmsg ? errmsg : "unknown");
			glfwTerminate_ptr();
			return;
		}
				
		glfwMakeContextCurrent_ptr(window);
	
	// events
	
		if (!glfwSetFramebufferSizeCallback_ptr) {
			printf("! glfwSetFramebufferSizeCallback method is not loaded!\n");
		}
		else {
			glfwSetFramebufferSizeCallback_ptr(window, framebuffer_size_callback);
			#ifdef DEBUG
				printf("> address of glfwSetFramebufferSizeCallback : %p\n", glfwSetFramebufferSizeCallback_ptr);
			#endif
		}
		
		if (!glfwSetCursorPosCallback_ptr) {
			printf ("! glfwSetCursorPosCallback method is not loaded!\n");
		}
		else {
			glfwSetCursorPosCallback_ptr(window, cursor_position_callback);
			#ifdef DEBUG
				printf("> address of glfwSetFramebufferSizeCallback : %p\n", glfwSetCursorPosCallback_ptr);			
			#endif
		}
		
			
	// base viewport
	
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		//int width, height;
		glfwGetFramebufferSize_ptr(window, &width, &height);
		framebuffer_size_callback(window, width, height);

	// calc bounds of grid
	
		bounds b = plotc_draw_grid_scale_calc(x, y, n);
		
	// fonts
	
		init_font_texture("plotc/font/arial.ttf"); // vagy bármilyen .ttf fájl
		
	// calc
	
		set_axis_values(x, y, n);
		
	// window
		while (!glfwWindowShouldClose_ptr(window)) {	
			
			// rendering
			if (renderingNow) {
				
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
								
				// default color
				glClearColor(1, 1, 1, 1);
				glClear(GL_COLOR_BUFFER_BIT);

				// margin			
				
				if (resizedNow) {
					
					// set margin in px
					margin_px = 50;
					
					// calc margin in float
					margin_x = (float)margin_px / (float)width;
					margin_x *= 2.0;
					margin_y = ((float)margin_px / (float)height * marginYSpace);
					margin = margin_x < margin_y ? margin_y : margin_x;
					
					// reset margin x and y in px
					//marginX_px = 0;
					//marginY_px = 0;

					// calc is ready
					resizedNow = 0;							
				}

		// IN WORLD
		
				// grid
				plotc_draw_grid(b.xmin, b.xmax, b.ymin, b.ymax, margin_x, margin_y);  // margin 
										
				// data
				plotc_draw_data(x, y, n, b, margin_x, margin_y); 

				// status bar
				plotc_draw_statusbar(margin_y / marginYSpace);

				// red dot
				if (mouse_in_range())
					plotc_draw_probe_dot(x, y, n, mouseX, width, b, margin_x, margin_y);
					
				// rendering
				renderingNow = 0;	
												
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				
			// IN PIXEL
			
				// draw crosshair
				draw_crosshair(mouseX, mouseY/*, width, height*/);
				
				// axis labels and values
				if (tokenCount > 2)
					plotc_draw_axis_labels(tokenizedText[T_AXIS_LABEL_X], tokenizedText[T_AXIS_LABEL_Y]);
				
				plotc_draw_axis_y_values(5.0);
							
				// DEV (inline)
				
					if (!mouse_in_range()) {
						plot_text_statusbar("Mouse position: Out of range.");
					}
					else {

						float xval = plotc_unscale(mouseX, b.xmin, b.xmax, margin_x, width);
						float yval = get_y_from_x(x, y, 1000, xval);

						// statusbar text
							char* mouseXinGridStr = (char*)calloc(8, sizeof(char));
							char* mouseYinGridStr = (char*)calloc(8, sizeof(char));
						
							sprintf (mouseXinGridStr, "%d", mouseXinGrid);
							sprintf (mouseYinGridStr, "%d", mouseYinGrid);
							
							char *statusbarText = (char*)calloc(128, sizeof(char));
							sprintf(statusbarText, "Mouse position: %s x %s  \t  Y[x] value = %lf",mouseXinGridStr, mouseYinGridStr, yval);

							plot_text_statusbar(statusbarText);

					}
					
				

				// swap buffer
				glfwSwapBuffers_ptr(window);
				
			}
			
			// poll
			glfwPollEvents_ptr();	

		} // end of loop(window)
		
    glfwDestroyWindow_ptr(window); 
    glfwTerminate_ptr();
}

