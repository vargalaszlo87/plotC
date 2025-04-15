// standard C
#include <stdio.h>
#include <stdlib.h>

// plotC projekt
#include "plotc.h"

// glfw3 inculde
#include "glfw3.h"  

// stb_easy_font
#include "stb_easy_font.h"

// DEBUG mode

#define DEBUG

// load dll dinamically
#ifdef _WIN32

#include <windows.h>
#include <stdio.h>

// ----- GLFW typedef-ek -----
typedef int (*PFN_glfwInit)(void);
typedef void (*PFN_glfwTerminate)(void);
typedef GLFWwindow* (*PFN_glfwCreateWindow)(int, int, const char*, void*, void*);
typedef void (*GLFWframebuffersizefun)(GLFWwindow*, int, int);
typedef void (*PFN_glfwMakeContextCurrent)(GLFWwindow*);
typedef int (*PFN_glfwWindowShouldClose)(GLFWwindow*);
typedef void (*PFN_glfwSwapBuffers)(GLFWwindow*);
typedef void (*PFN_glfwPollEvents)(void);
typedef void (*PFN_glfwDestroyWindow)(GLFWwindow*);
typedef void (*PFN_glfwSetFramebufferSizeCallback)(GLFWwindow*, GLFWframebuffersizefun);
typedef void (*PFN_glfwGetFramebufferSize)(GLFWwindow*, int*, int*);

// ----- Globális mutatók -----
static PFN_glfwInit glfwInit_ptr;
static PFN_glfwTerminate glfwTerminate_ptr;
static PFN_glfwCreateWindow glfwCreateWindow_ptr;
static PFN_glfwMakeContextCurrent glfwMakeContextCurrent_ptr;
static PFN_glfwWindowShouldClose glfwWindowShouldClose_ptr;
static PFN_glfwSwapBuffers glfwSwapBuffers_ptr;
static PFN_glfwPollEvents glfwPollEvents_ptr;
static PFN_glfwDestroyWindow glfwDestroyWindow_ptr;
static PFN_glfwSetFramebufferSizeCallback glfwSetFramebufferSizeCallback_ptr;
static PFN_glfwGetFramebufferSize glfwGetFramebufferSize_ptr;

static HMODULE glfw = NULL; // globális, hogy a makró is lássa

// global 

static int rendering_now = 1;


static void load_glfw_dll_once() {
    static int once = 0;
	static int loaded = 0;

	// check state of loaded
	if (loaded)
		return;

	// check state of once
    if (!once) {
        SetDllDirectoryA("plotc/lib/windows");
        glfw = LoadLibraryA("glfw3.dll");

        if (!glfw) {
            DWORD err = GetLastError();
            printf("! glfw3.dll loading is failed! Errorcode: %lu\n", err);
            exit(1);
        } else {
            printf("> glfw3.dll has been loaded!\n");
        }

        once = 1;
    }

	#define LOAD_PROC(name) \
        name##_ptr = (PFN_##name)GetProcAddress(glfw, #name); \
        if (!name##_ptr) { \
            printf("! Loading is failed: %s\n", #name); \
            exit(1); \
        }

    LOAD_PROC(glfwInit);
    LOAD_PROC(glfwTerminate);
    LOAD_PROC(glfwCreateWindow);
    LOAD_PROC(glfwMakeContextCurrent);
    LOAD_PROC(glfwWindowShouldClose);
    LOAD_PROC(glfwSwapBuffers);
    LOAD_PROC(glfwPollEvents);
	LOAD_PROC(glfwDestroyWindow);
	LOAD_PROC(glfwSetFramebufferSizeCallback);
	LOAD_PROC(glfwGetFramebufferSize);

    loaded = 1;
}
#else
#define load_glfw_dll_once()
#endif

// events

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // Megakadályozzuk a 0 felbontást
    if (height == 0) height = 1;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Ortografikus vetítés (bal, jobb, lent, fent, near, far)
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	
	// re-rendering
	rendering_now = 1;
	
	#ifdef DEBUG
		printf ("> framebuffer is resized.\n");
	#endif
}

// methods

float plotc_scale(float v, float vmin, float vmax, float margin) {
    return -1.0f + margin * 2 + (v - vmin) / (vmax - vmin) * (2.0f - margin * 4);
}

/*
static void plotc_draw_grid() {
    glColor3f(0.8f, 0.8f, 0.8f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    for (float x = -1.0f; x <= 1.0f; x += 0.2f) {
        glVertex2f(x, -1.0f);
        glVertex2f(x,  1.0f);
    }
    for (float y = -1.0f; y <= 1.0f; y += 0.2f) {
        glVertex2f(-1.0f, y);
        glVertex2f( 1.0f, y);
    }
    glEnd();
}
*/

/* 
 *
 * GRID
 *
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

/*
 * 
 * DATA
 *
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

/*
 *
 * DRAW elements
 *
 */
 
 
static void plotc_draw_text_bitmap(const char* text, float x, float y, float scale) {
    char buffer[99999]; // elég nagy buffer
    int num_quads;

    num_quads = stb_easy_font_print(
        x * 100.0f,               // pixelalapú pozícióra konvertálás
        y * 100.0f,
        (char*)text,
        NULL,
        buffer, sizeof(buffer)
    );

    glPushMatrix();
    glScalef(scale / 100.0f, scale / 100.0f, 1.0f); // pixelkoordináta → OpenGL világba

    glColor3f(0.1f, 0.1f, 0.1f); // sötétszürke
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, buffer);
    glDrawArrays(GL_QUADS, 0, num_quads * 4);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();
}
 

static void plotc_draw_statusbar(float margin) {
    float bar_height = 0.1f; // OpenGL koordináta-térben
    float y0 = -1.0f;
    float y1 = y0 + margin * 1.5;

	//printf ("%lf", margin);

    // háttérsáv
    glColor3f(0.9f, 0.9f, 0.9f);
    glBegin(GL_QUADS);
        glVertex2f(-1.0f, y0);
        glVertex2f( 1.0f, y0);
        glVertex2f( 1.0f, y1);
        glVertex2f(-1.0f, y1);
    glEnd();
	
}

void plotc(float* x, float* y, int n, const char* title) {
	
	#ifdef _WIN32
    load_glfw_dll_once();  // ez tölti be a DLL-t és a pointereket
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
			
	// Alap viewport
		int width, height;
		glfwGetFramebufferSize_ptr(window, &width, &height);
		framebuffer_size_callback(window, width, height);

	// calc bounds of grid
	
		bounds b = plotc_draw_grid_scale_calc(x, y, n);
				
	// window
		while (!glfwWindowShouldClose_ptr(window)) {	
			
			// rendering
			if (rendering_now) {
								
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
								
				// rendering
				rendering_now = 0;	
				
				// swap buffer
				glfwSwapBuffers_ptr(window);
													
			}
			
			// poll
			glfwPollEvents_ptr();	

		} // end of loop(window)

    glfwDestroyWindow_ptr(window); 
    glfwTerminate_ptr();
}

/*
void plot_line(float* x, float* y, int n, const char* title) {
	
	#ifdef _WIN32
		load_glfw_dll_once();
	#endif
	
    if (!glfwInit()) return;

    GLFWwindow* window = glfwCreateWindow(800, 600, title, NULL, NULL);
    if (!window) {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glColor3f(0.0f, 0.0f, 1.0f);
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < n; i++) {
            glVertex2f(x[i], y[i]);
        }
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
*/