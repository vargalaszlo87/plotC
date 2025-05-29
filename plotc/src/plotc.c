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
 *	defined values
 */



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
 
#include "config.h"
#include "plotc.h"
#include "dyndll.h"
#include "events.h"
#include "statusbar.h"
#include "draw.h"
#include "calc.h"

/*!
 * Variables
 */
 
int firstStart = 0;
 
int renderingNow = 1;
int resizedNow = 1;

GLFWwindow* window;

int width;
int height;

float
	// axis values
	axisYValues[MAX_GRID_LINE],
	axisXValues[MAX_GRID_LINE],
	
	// div
	divX,
	divY,

	// calculated margin in pixel
	margin,
	margin_x,
	margin_y,
	
	// margin-Y space 
	marginYSpace = 1.2,

	// grid 
	gridPositionModelX[MAX_GRID_LINE],
	gridPositionModelY[MAX_GRID_LINE];

int
	// margin in pixel
	margin_px,
//	marginX_px,
//	marginY_px,
	
	// mouse
	mouseX,
	mouseY,
	
	// y asis value
	maxAxisYValueSizeInPx,

	// mouse inside grid
	mouseXinGrid,
	mouseYinGrid,
	
	// grid margin
	gridPositionProjectionX[MAX_GRID_LINE],
	gridPositionProjectionY[MAX_GRID_LINE],
	
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
	
		// axis values
		set_axis_values(x, y, n);
		
		// divs
		divX = (max_float(x, n) - min_float(x, n)) / (GRID_LINE - 1);
		divY = (max_float(y, n) - min_float(y, n)) / (GRID_LINE - 1);
		
	// statusbar
	
		char *statusbarText = (char*)calloc(128, sizeof(char));
		char *statusbarSetInRange = "X: %s/div  |  Y: %s/div  |  Y[%s] = %s";
		char *statusbarSetNotInRange = "X: %s/div  |  Y: %s/div  |  Y[N/A] = N/A";

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
				
				//if (resizedNow) {
					
					// set margin in px
					margin_px = 50;
					
					// calc margin in float
					margin_x = (float)margin_px  / (float)width;
					margin_x *= 1.0;
					margin_x += (float)maxAxisYValueSizeInPx / (float)width;
					margin_y = ((float)margin_px / (float)height * marginYSpace);
					margin = margin_x < margin_y ? margin_y : margin_x;

					// calc is ready
					resizedNow = 0;							
				//}

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
				
					float xval = plotc_unscale(mouseX, b.xmin, b.xmax, margin_x, width);
					float yval = get_y_from_x(x, y, 1000, xval);
				
					if (!mouse_in_range()) {
							sprintf(statusbarText, statusbarSetNotInRange, format_number_static(divX), format_number_static(divY));
							plot_text_statusbar(statusbarText);
					}
					else {
						
						// mouse grid, aux for future
						
							char* mouseXinGridStr = (char*)calloc(8, sizeof(char));
							char* mouseYinGridStr = (char*)calloc(8, sizeof(char));
					
							sprintf (mouseXinGridStr, "%d", mouseXinGrid);
							sprintf (mouseYinGridStr, "%d", mouseYinGrid);							

						// statusbar text
						
							sprintf(statusbarText, statusbarSetInRange, format_number_static(divX), format_number_static(divY), format_number_static(xval), format_number_static(yval));
							plot_text_statusbar(statusbarText);

					}
					
				

				// swap buffer
				glfwSwapBuffers_ptr(window);
				
				// first start events
				if (!firstStart) {
					framebuffer_size_callback(window, width, height);
					
					firstStart = 1;
				}
					
				
			}
			
			// poll
			glfwPollEvents_ptr();	

		} // end of loop(window)
		
    glfwDestroyWindow_ptr(window); 
    glfwTerminate_ptr();
}

