/*!
 * @project plotC  v.0.1	
 * @file statusbar.c
 * @brief This is the tool of statusbar.
 *
 * Version information:
 *
 *
 * Features:
 * - gray rectangle
 * - mouse position
 *
 * @author Varga Laszlo
 *
 * @website https://github.com/vargalaszlo87/plotC
 * @website http://vargalaszlo.com
 * @website http://ha1cx.hu
 *
 * @date 2025-04-26
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
#include "statusbar.h"

void plotc_draw_statusbar(float margin) {
    float bar_height = 0.1f; // OpenGL koordináta-térben
    float y0 = -1.0f;
    float y1 = y0 + margin * 1.5;

    // háttérsáv
		glColor3f(0.9f, 0.9f, 0.9f);
		glRectf(-1.0f, y0, 1.0f, y1);
}

void plot_text_statusbar(char *text) {
	
	begin_pixel_mode(width, height);

	glColor3f(0, 0, 0); 
	draw_text(8, height - 12, text);
	
	end_pixel_mode();
}
