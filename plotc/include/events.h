#ifndef EVENTS_H
#define EVENTS_H

/*!
 * variables
 */
 
extern int renderingNow;

extern int mouseX;
extern int mouseY;

/*!
 * methods
 */

void framebuffer_size_callback(GLFWwindow*, int, int);
void cursor_position_callback(GLFWwindow*, double, double);

#endif