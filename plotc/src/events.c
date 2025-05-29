/*!
 * @project plotC  v.0.1	
 * @file events.c
 * @brief This is the eventhandler of plotC.
 *
 * Version information:
 *
 *
 * Features:
 * - resizable callback
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

#include "config.h"
#include "type.h"
#include "glfw3.h"  
#include "dyndll.h"
#include "events.h"

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {

	// position in full window
    mouseX = xpos;
    mouseY = ypos;

	// position inside grid (LEFT, BOTTOM -> UP, RIGHT
	mouseXinGrid = mouseX - (int)gridPositionProjectionX[0];
	mouseYinGrid = height - mouseY - (int)gridPositionProjectionY[10];
		
	// re-rendering
	renderingNow = 1;
		
}

void framebuffer_size_callback(GLFWwindow* window_local, int w, int h) {
    // Megakadályozzuk a 0 felbontást
    if (h == 0) h = 1;
	
    glViewport(0, 0, w, h);
		
	// re-rendering & resized
	renderingNow = 1;
	resizedNow = 1;

    glfwGetFramebufferSize_ptr(window, &width, &height);
	
	#ifdef DEBUG
		printf ("> framebuffer is resized.\n");
	#endif
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    float zoomStep = 0.1f;
    float factor = (yoffset > 0) ? (1.0f - zoomStep) : (1.0f + zoomStep);

    float x_at_mouse = plotc_unscale(mouseX, viewBounds.xmin, viewBounds.xmax, margin_x, width);
    float y_at_mouse = plotc_unscale(mouseY, viewBounds.ymin, viewBounds.ymax, margin_y, height);

    float new_xrange = (viewBounds.xmax - viewBounds.xmin) * factor;
    float new_yrange = (viewBounds.ymax - viewBounds.ymin) * factor;

    viewBounds.xmin = x_at_mouse - (x_at_mouse - viewBounds.xmin) * factor;
    viewBounds.xmax = viewBounds.xmin + new_xrange;

    viewBounds.ymin = y_at_mouse - (y_at_mouse - viewBounds.ymin) * factor;
    viewBounds.ymax = viewBounds.ymin + new_yrange;

    renderingNow = 1;
	
	#ifdef DEBUG
		printf ("> scrolled\n");
	#endif
	
}