// standard C
#include <stdio.h>
#include <stdlib.h>

// plotC projekt
#include "plotc.h"

// glfw3 inculde
#include <glfw3.h>  

// load dll dinamically
#ifdef _WIN32

#include <windows.h>
#include <stdio.h>

// ----- GLFW typedef-ek -----
typedef int (*PFN_glfwInit)(void);
typedef void (*PFN_glfwTerminate)(void);
typedef GLFWwindow* (*PFN_glfwCreateWindow)(int, int, const char*, void*, void*);
typedef void (*PFN_glfwMakeContextCurrent)(GLFWwindow*);
typedef int (*PFN_glfwWindowShouldClose)(GLFWwindow*);
typedef void (*PFN_glfwSwapBuffers)(GLFWwindow*);
typedef void (*PFN_glfwPollEvents)(void);
typedef void (*PFN_glfwDestroyWindow)(GLFWwindow*);

// ----- Globális mutatók -----
static PFN_glfwInit glfwInit_ptr;
static PFN_glfwTerminate glfwTerminate_ptr;
static PFN_glfwCreateWindow glfwCreateWindow_ptr;
static PFN_glfwMakeContextCurrent glfwMakeContextCurrent_ptr;
static PFN_glfwWindowShouldClose glfwWindowShouldClose_ptr;
static PFN_glfwSwapBuffers glfwSwapBuffers_ptr;
static PFN_glfwPollEvents glfwPollEvents_ptr;
static PFN_glfwDestroyWindow glfwDestroyWindow_ptr;

static HMODULE glfw = NULL; // 💡 globális, hogy a makró is lássa

static void load_glfw_dll_once() {
    static int once = 0;
	static int loaded = 0;

	if (loaded)
		return;

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

    loaded = 1;
}
#else
#define load_glfw_dll_once()
#endif


// methods

float plotc_scale(float v, float vmin, float vmax, float margin) {
    return -1.0f + margin * 2 + (v - vmin) / (vmax - vmin) * (2.0f - margin * 4);
}

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

static void plotc_draw_data(float* x, float* y, int n) {
    glColor3f(0.0f, 0.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < n; i++) {
        glVertex2f(x[i], y[i]);
    }
    glEnd();
}

void plotc(float* x, float* y, int n, const char* title) {
	
	#ifdef _WIN32
    load_glfw_dll_once();  // ez tölti be a DLL-t és a pointereket
	#endif

    if (!glfwInit_ptr()) return;

    GLFWwindow* window = glfwCreateWindow_ptr(800, 600, title, NULL, NULL);
    if (!window) {
        glfwTerminate_ptr();
        return;
    }

    glfwMakeContextCurrent_ptr(window);

    while (!glfwWindowShouldClose_ptr(window)) {
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

		plotc_draw_grid();            
        plotc_draw_data(x, y, n); 

        glfwSwapBuffers_ptr(window);
        glfwPollEvents_ptr();
    }

    glfwDestroyWindow_ptr(window);  // ez továbbra is linkerrel kell (vagy: GetProcAddress hozzá is)
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