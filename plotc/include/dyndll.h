/*!
 * @project plotC  v.0.1	
 * @file dyndll.h
 * @brief This is a dynaminc DLL loader for plotC.
 *
 * Version information:
 *
 * - simple loader
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
 
#include "debug.h"

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

static void loadGlfwDllOnce() {
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
#define loadGlfwDllOnce()
#endif
