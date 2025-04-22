#ifndef EVENTS_H
#define EVENTS_H

/*!
 * variables
 */
 
// rendering bit
extern int renderingNow;

// GLFW window
extern GLFWwindow* window;

// mouse position
extern int mouseX;
extern int mouseY;

// window size
extern int width;
extern int height;

/*!
 * methods
 */

void framebuffer_size_callback(GLFWwindow*, int, int);
void cursor_position_callback(GLFWwindow*, double, double);

#endif