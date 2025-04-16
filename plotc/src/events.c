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

#include "debug.h"
#include "glfw3.h"  
#include "dyndll.h"
#include "events.h"

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
	renderingNow = 1;
	
	#ifdef DEBUG
		printf ("> framebuffer is resized.\n");
	#endif
}