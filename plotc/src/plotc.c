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

/*!
 * standard includes
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

/*!
 * extern includes
 */
 
#define GL_IMPLEMENTATION
#include <GL/gl.h>

#define GLFW3_IMPLEMENTATION
#include "glfw3.h"  

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

/*!
 * project includes
 */
 
#include "debug.h"
#include "plotc.h"
#include "dyndll.h"
#include "events.h"

/*!
 * global variables
 */

int renderingNow = 1;

unsigned char ttf_buffer[1<<20];
unsigned char bitmap[512*512];

GLuint font_texture;
stbtt_bakedchar cdata[96]; // ASCII 32..126

//int width, height;
static int width;
static int height;

int mouseX;
int mouseY;

// methods

float plotc_scale(float v, float vmin, float vmax, float margin) {
    return -1.0f + margin * 2 + (v - vmin) / (vmax - vmin) * (2.0f - margin * 4);
}

/*!
 * TEXT
 */
 
 void init_font_texture(const char* font_path) {
	 
	 // open the font
    FILE* f = fopen(font_path, "rb");
	if (!f) {
		#ifdef DEBUG
			printf ("! The %s font type is not loaded. \n", font_path);
		#endif
		return;
	}

    size_t res = fread(ttf_buffer, 1, 1<<20, f);
	if (res != 5) {
		#ifdef DEBUG
			printf ("! The font type reading is failed. \n");
		#endif
	}

    fclose(f);
	
	// 

    stbtt_BakeFontBitmap(ttf_buffer, 0, 18.0, bitmap, 512, 512, 32, 96, cdata); // 32px font

    glGenTextures(1, &font_texture);
    glBindTexture(GL_TEXTURE_2D, font_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, bitmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void draw_text(float x, float y, const char* text) {
	
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, font_texture);
    glBegin(GL_QUADS);

    while (*text) {
        if (*text >= 32 && *text < 128) {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(cdata, 512, 512, *text - 32, &x, &y, &q, 1);
            glTexCoord2f(q.s0, q.t0); glVertex2f(q.x0, q.y0);
            glTexCoord2f(q.s1, q.t0); glVertex2f(q.x1, q.y0);
            glTexCoord2f(q.s1, q.t1); glVertex2f(q.x1, q.y1);
            glTexCoord2f(q.s0, q.t1); glVertex2f(q.x0, q.y1);
        }
        ++text;
    }

    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}


/*!
 * GRID
 */

typedef struct {
	float xmin, xmax, ymin, ymax;
} bounds;

bounds plotc_draw_grid_scale_calc(float* x, float* y, int n) {
	
	bounds b = { 
		x[0], 
		x[0],
		y[0], 
		y[0]
	};
	
    for (int i = 1; i < n; i++) {
        if (x[i] < b.xmin) b.xmin = x[i];
        if (x[i] > b.xmax) b.xmax = x[i];
        if (y[i] < b.ymin) b.ymin = y[i];
        if (y[i] > b.ymax) b.ymax = y[i];
    }
	
	return b; 
}

static void plotc_draw_grid(float xmin, float xmax, float ymin, float ymax, float margin) {
	
	glColor3f(0.8f, 0.8f, 0.8f);
	glLineWidth(1.0f);

	glBegin(GL_LINES);

	// X irányban 11 osztás (10 köz)
	for (int i = 0; i <= 10; i++) {
		float x = xmin + i * (xmax - xmin) / 10.0f;
		float xp = plotc_scale(x, xmin, xmax, margin);
		
		glVertex2f(xp, plotc_scale(ymin, ymin, ymax, margin));
		glVertex2f(xp, plotc_scale(ymax, ymin, ymax, margin));
	}

	// Y irányban 11 osztás
	for (int i = 0; i <= 10; i++) {
		float y = ymin + i * (ymax - ymin) / 10.0f;
		float yp = plotc_scale(y, ymin, ymax, margin);

		glVertex2f(plotc_scale(xmin, xmin, xmax, margin), yp);
		glVertex2f(plotc_scale(xmax, xmin, xmax, margin), yp);
	}

	glEnd();
}

/*!
 * DATA
 */
 
static void plotc_draw_data(float* x, float* y, int n, bounds b, float margin) {
	
	// color, line
		glColor3f(0.0f, 0.0f, 1.0f);
		glLineWidth(2.0f);
	
	// fill data
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < n; i++) {
			float xp = plotc_scale(x[i], b.xmin, b.xmax, margin);
			float yp = plotc_scale(y[i], b.ymin, b.ymax, margin);
			glVertex2f(xp, yp);
		}
		
    glEnd();
}

/*!
 * DRAW elements
 */
 

static void plotc_draw_statusbar(float margin) {
    float bar_height = 0.1f; // OpenGL koordináta-térben
    float y0 = -1.0f;
    float y1 = y0 + margin * 1.5;

	//printf ("%lf", margin);

    // háttérsáv
   glColor3f(0.9f, 0.9f, 0.9f);
	 glRectf(-1.0f, y0, 1.0f, y1);
}

void plot_text_statusbar() {
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, -1, 1);

	glColor3f(0, 0, 0); // piros szöveg
	draw_text(8, height - 8, "Mouse position: ");

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
}

void plotc(float* x, float* y, int n, const char* title) {
	
	#ifdef _WIN32
    loadGlfwDllOnce();  // ez tölti be a DLL-t és a pointereket
	#endif

	// check
		if (!glfwInit_ptr()) return;

		GLFWwindow* window = glfwCreateWindow_ptr(800, 600, title, NULL, NULL);
		if (!window) {
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
		
			
	// Alap viewport
	
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		//int width, height;
		glfwGetFramebufferSize_ptr(window, &width, &height);
		framebuffer_size_callback(window, width, height);

	// calc bounds of grid
	
		bounds b = plotc_draw_grid_scale_calc(x, y, n);
		
	// fonts
	
		init_font_texture("plotc/font/arial.ttf"); // vagy bármilyen .ttf fájl
	
	// window
		while (!glfwWindowShouldClose_ptr(window)) {	
			
			// rendering
			if (renderingNow) {
								
				// default color
				glClearColor(1, 1, 1, 1);
				glClear(GL_COLOR_BUFFER_BIT);

				// margin			
				//float margin = 0.1f;
				
				int margin_px = 50;
				float margin_x = (float)margin_px / (float)width;
				float margin_y = (float)margin_px / (float)height;
				float margin = margin_x < margin_y ? margin_x : margin_y;
		
				// grid
				plotc_draw_grid(b.xmin, b.xmax, b.ymin, b.ymax, margin);  

				// data
				plotc_draw_data(x, y, n, b, margin); 
				
				// status bar
				plotc_draw_statusbar(margin);
				
				// statusbar text
					char* mouseXStr;
					char* mouseYStr;
					
					sprintf (mouseXStr, "%d", mouseX);

					printf ("%s \n",mouseXStr);

					//char* mouseXYStr;
					//strcpy(mouseXYStr, mouseXStr);
				
					plot_text_statusbar();
															
				// rendering
				renderingNow = 0;	
				
				// swap buffer
				glfwSwapBuffers_ptr(window);
													
			}
			
			// poll
			glfwPollEvents_ptr();	

		} // end of loop(window)

    glfwDestroyWindow_ptr(window); 
    glfwTerminate_ptr();
}

