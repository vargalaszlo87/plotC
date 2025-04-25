/*!
 * @project plotC  v.0.1	
 * @file dyndll.h
 * @brief Dynamic DLL loader header for plotC.
 */

#ifndef DYNDLL_H
#define DYNDLL_H

#include "debug.h"

#ifdef _WIN32

#include <windows.h>
#include <stdio.h>
#include "glfw3.h" // szükséges a GLFWwindow típus miatt

// ----- GLFW typedef-ek -----
typedef int (*PFN_glfwInit)(void);
typedef void (*PFN_glfwTerminate)(void);
typedef GLFWwindow* (*PFN_glfwCreateWindow)(int, int, const char*, void*, void*);
typedef void (*GLFWframebuffersizefun)(GLFWwindow*, int, int);
typedef void (*GLFWcursorposfun)(GLFWwindow*, double, double);
typedef void (*PFN_glfwMakeContextCurrent)(GLFWwindow*);
typedef int (*PFN_glfwWindowShouldClose)(GLFWwindow*);
typedef void (*PFN_glfwSwapBuffers)(GLFWwindow*);
typedef void (*PFN_glfwPollEvents)(void);
typedef void (*PFN_glfwDestroyWindow)(GLFWwindow*);
typedef void (*PFN_glfwSetFramebufferSizeCallback)(GLFWwindow*, GLFWframebuffersizefun);
typedef void (*PFN_glfwGetFramebufferSize)(GLFWwindow*, int*, int*);
typedef void (*PFN_glfwSetCursorPosCallback)(GLFWwindow*, GLFWcursorposfun);

// ----- Csak extern deklarációk -----
extern PFN_glfwInit glfwInit_ptr;
extern PFN_glfwTerminate glfwTerminate_ptr;
extern PFN_glfwCreateWindow glfwCreateWindow_ptr;
extern PFN_glfwMakeContextCurrent glfwMakeContextCurrent_ptr;
extern PFN_glfwWindowShouldClose glfwWindowShouldClose_ptr;
extern PFN_glfwSwapBuffers glfwSwapBuffers_ptr;
extern PFN_glfwPollEvents glfwPollEvents_ptr;
extern PFN_glfwDestroyWindow glfwDestroyWindow_ptr;
extern PFN_glfwSetFramebufferSizeCallback glfwSetFramebufferSizeCallback_ptr;
extern PFN_glfwGetFramebufferSize glfwGetFramebufferSize_ptr;
extern PFN_glfwSetCursorPosCallback glfwSetCursorPosCallback_ptr;

// A DLL betöltő függvény deklarációja
void loadGlfwDllOnce(void);

#else
#define loadGlfwDllOnce()
#endif

#endif // DYNDLL_H