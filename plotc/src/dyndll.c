/*!
 * @project plotC  v.0.1
 * @file dyndll.c
 * @brief Dynamic DLL loader implementation for plotC.
 */

#include "dyndll.h"

#ifdef _WIN32

// ----- Valódi definíciók -----
PFN_glfwInit glfwInit_ptr = NULL;
PFN_glfwTerminate glfwTerminate_ptr = NULL;
PFN_glfwCreateWindow glfwCreateWindow_ptr = NULL;
PFN_glfwMakeContextCurrent glfwMakeContextCurrent_ptr = NULL;
PFN_glfwWindowShouldClose glfwWindowShouldClose_ptr = NULL;
PFN_glfwSwapBuffers glfwSwapBuffers_ptr = NULL;
PFN_glfwPollEvents glfwPollEvents_ptr = NULL;
PFN_glfwDestroyWindow glfwDestroyWindow_ptr = NULL;
PFN_glfwSetFramebufferSizeCallback glfwSetFramebufferSizeCallback_ptr = NULL;
PFN_glfwGetFramebufferSize glfwGetFramebufferSize_ptr = NULL;
PFN_glfwSetCursorPosCallback glfwSetCursorPosCallback_ptr = NULL;
PFN_glfwWindowHint glfwWindowHint_ptr = NULL;
PFN_glfwGetError glfwGetError_ptr = NULL;

static HMODULE glfw = NULL;

void loadGlfwDllOnce(void) {
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
    LOAD_PROC(glfwSetFramebufferSizeCallback);
    LOAD_PROC(glfwGetFramebufferSize);
    LOAD_PROC(glfwSetCursorPosCallback);
	LOAD_PROC(glfwWindowHint);
	LOAD_PROC(glfwGetError);

    loaded = 1;
}

#endif