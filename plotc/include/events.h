#ifndef EVENTS_H
#define EVENTS_H

/*!
 * variables
 */
 
// rendering bit
extern int renderingNow;
extern int resizedNow;

// GLFW window
extern GLFWwindow* window;

// mouse position
extern int mouseX;
extern int mouseY;
extern int mouseXinGrid;
extern int mouseYinGrid;

// window size
extern int width;
extern int height;

// grid
extern int gridPositionProjectionX[16];
extern int gridPositionProjectionY[16];

/*!
 * methods
 */

void framebuffer_size_callback(GLFWwindow*, int, int);
void cursor_position_callback(GLFWwindow*, double, double);

#endif