/*!
 * @project plotC  v.0.1	
 * @file draw.c
 * @brief Here are the all functions of drawing.
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
#include "draw.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

/*!
 * variables
 */

unsigned char bitmap[512*512];
unsigned char ttf_buffer[1<<20];
stbtt_bakedchar cdata[96];
GLuint font_texture;

/*!
 * Aux
 */
 
float plotc_scale(float v, float vmin, float vmax, float margin) {
	float
		range = vmax - vmin;
		if (range == 0.0f) return 0.0f; // védekezés, ne osszon nullával
	float
		normalize = (v - vmin) / range,
		span = 1.0f - 2.0f * margin;
	return -1.0f + margin * 2.0f + normalize * span * 2.0f;

}

float plotc_unscale(int pixel, float vmin, float vmax, float margin, int width) {
    float span = 1.0f - 2.0f * margin;
    float ndc = ((float)pixel / (float)width) * 2.0f - 1.0f;
    float normalize = (ndc - (-1.0f + margin * 2.0f)) / (span * 2.0f);
    return vmin + normalize * (vmax - vmin);
}

bool mouse_in_range() {
	if (
		mouseX <= gridPositionProjectionX[0] ||
		mouseX >= gridPositionProjectionX[10] ||
		mouseY <= gridPositionProjectionY[10] ||
		mouseY >= gridPositionProjectionY[0]
	)
		return false;
	else
		return true;
}

/*!
 * Grid functions
 */
 
bounds plotc_draw_grid_scale_calc(float* x, float* y, int n) {
	
	bounds b = { 
		x[0], 
		x[0],
		y[0], 
		y[0]
	};
	
    for (int i = 1; i < n; i++) {
        if (x[i] < b.xmin) b.xmin = x[i];
        if (x[i] > b.xmax) b.xmax = x[i];
        if (y[i] < b.ymin) b.ymin = y[i];
        if (y[i] > b.ymax) b.ymax = y[i];
    }
	
	return b; 
}

void plotc_draw_grid(float xmin, float xmax, float ymin, float ymax, float margin_x, float margin_y) {
	
	glColor3f(0.8f, 0.8f, 0.8f);
	glLineWidth(1.0f);

	glBegin(GL_LINES);

	// X irányban 11 osztás (10 köz)
	for (int i = 0; i <= 10; i++) {
		float x = xmin + i * (xmax - xmin) / 10.0f;
		float xp = plotc_scale(x, xmin, xmax, margin_x);
		
		// save the positions of Axis-X in Modelview
		gridPositionModelX[i] = xp;
		
		// save the positions of Axis-X in Projection
		gridPositionProjectionX[i] = (int)((xp + 1.0f) * 0.5f * width);
		
		glVertex2f(xp, plotc_scale(ymin, ymin, ymax, margin_y));
		glVertex2f(xp, plotc_scale(ymax, ymin, ymax, margin_y));
	}

	// Y irányban 11 osztás
	for (int i = 0; i <= 10; i++) {
		float y = ymin + i * (ymax - ymin) / 10.0f;
		float yp = plotc_scale(y, ymin, ymax, margin_y);
		
		// save the positions of Axis-Y in Modelview
		gridPositionModelY[i] = yp;
				
		// save the positions of Axis-Y in Projection
		gridPositionProjectionY[i] = (int)((1.0f - yp) * 0.5f * height);

		glVertex2f(plotc_scale(xmin, xmin, xmax, margin_x), yp);
		glVertex2f(plotc_scale(xmax, xmin, xmax, margin_x), yp);
	}

	glEnd();

}

/*!
 * Text functions
 */
 
void init_font_texture(const char* font_path) {
 
 // open the font
	FILE* f = fopen(font_path, "rb");
	if (!f) {
		#ifdef DEBUG
			printf ("! The %s font type is not loaded. \n", font_path);
		#endif
		return;
	}

	size_t res = fread(ttf_buffer, 1, 1<<20, f);
	if (res == 0) {
		#ifdef DEBUG
			printf ("! The font type reading is failed. \n");
		#endif
	}

	fclose(f);

	stbtt_BakeFontBitmap(ttf_buffer, 0, 18.0, bitmap, 512, 512, 32, 96, cdata); // 32px font

	glGenTextures(1, &font_texture);
	glBindTexture(GL_TEXTURE_2D, font_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, bitmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

 
void draw_text(float x, float y, const char* text) {
	
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, font_texture);
    glBegin(GL_QUADS);

    while (*text) {
        if (*text >= 32 && *text < 128) {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(cdata, 512, 512, *text - 32, &x, &y, &q, 1);
            glTexCoord2f(q.s0, q.t0); glVertex2f(q.x0, q.y0);
            glTexCoord2f(q.s1, q.t0); glVertex2f(q.x1, q.y0);
            glTexCoord2f(q.s1, q.t1); glVertex2f(q.x1, q.y1);
            glTexCoord2f(q.s0, q.t1); glVertex2f(q.x0, q.y1);
        }
        ++text;
    }

    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

/*
void plotc_draw_axis_labels(const char* xlabel, const char* ylabel) {
    begin_pixel_mode(width, height);
    glColor3f(0, 0, 0);  // fekete szöveg

    // --- X tengelycímke: középre lentre ---
    int x_pos = width / 2 - strlen(xlabel) * 4;  // kb. középre igazítás
    int y_pos = height - 40;
    draw_text((float)x_pos, (float)y_pos, xlabel);

    // --- Y tengelycímke: balra, középre függőlegesen (karakterenként írjuk le) ---
    int x_y_label = 10;
    int y_y_label = height / 2 - (strlen(ylabel) * 8 / 2);

    for (int i = 0; ylabel[i] != '\0'; i++) {
        char s[2] = { ylabel[i], 0 };
        draw_text((float)x_y_label, (float)(y_y_label + i * 12), s);
    }

    end_pixel_mode();
}
*/

void plotc_draw_axis_labels(const char* xlabel, const char* ylabel) {
    begin_pixel_mode(width, height);
    glColor3f(0, 0, 0);  // fekete szöveg

    // --- X tengelycímke: középre lentre ---
    int x_pos = width / 2 - strlen(xlabel) * 4;  // kb. középre igazítás
    int y_pos = height - 40;
    draw_text((float)x_pos, (float)y_pos, xlabel);

    // --- Y tengelycímke: 90 fokkal elforgatva ---
    glPushMatrix();

        // eltolás: kb. bal szél, középmagasság
        glTranslatef(20.0f, height / 2.0f, 0.0f);  // balról kicsit beljebb, középmagasságban

        // forgatás óramutató járásával ellentétesen (OpenGL koordinátarendszerben)
        glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);

        // szöveg balra, kb. középre igazítás
        int text_width = strlen(ylabel) * 8;
        draw_text((float)(-text_width / 2), 0.0f, ylabel);

    glPopMatrix();

    end_pixel_mode();
}

void plotc_draw_axis_y_values(float paddingY) {

    begin_pixel_mode(width, height);
    glColor3f(0, 0, 0);  // fekete szöveg

	for (int i = 0; i <= 10; i++) {

		char* temp =( char *)calloc(16, sizeof(char));
		
		sprintf (temp, "%.2lf", axisYValues[i]);
		draw_text(40.0,gridPositionProjectionY[i] + paddingY, temp);
	}
	
	end_pixel_mode();
	
}

/*!
 * Crosshair
 */
 
void draw_crosshair(int mouseX, int mouseY) {
			
	if (
		mouseX <= gridPositionProjectionX[0] ||
		mouseX >= gridPositionProjectionX[10] ||
		mouseY <= gridPositionProjectionY[10] ||
		mouseY >= gridPositionProjectionY[0]
	)
		return;
	
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.5f, 0.5f, 0.5f); // szürke célkereszt
	glLineWidth(0.5f);
	
	begin_pixel_mode(width, height);
	
    glBegin(GL_LINES);
        // vertical vonal (the "dummy 0" is just info for me)
        glVertex2i(mouseX, 0 + gridPositionProjectionY[10]);
        glVertex2i(mouseX, gridPositionProjectionY[0]);
		
        // horizontal vonal (the "dummy 0" is just info for me)
        glVertex2i(0 + gridPositionProjectionX[0], mouseY);
        glVertex2i(gridPositionProjectionX[10], mouseY);
    glEnd();
	
	end_pixel_mode();
	
} 

/*!
 * Data display functions
 */
 
void plotc_draw_data(float* x, float* y, int n, bounds b, float margin_x, float margin_y) {
	
	// color, line
		glColor3f(0.0f, 0.0f, 1.0f);
		glLineWidth(2.0f);
	
	// fill data
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < n; i++) {
			float xp = plotc_scale(x[i], b.xmin, b.xmax, margin_x);
			float yp = plotc_scale(y[i], b.ymin, b.ymax, margin_y);
			glVertex2f(xp, yp);
		}
		
    glEnd();
}

/*! 
 * Red dot
*/

void plotc_draw_probe_dot(float* x, float* y, int n, int mouseX, int width, bounds b, float margin_x, float margin_y) {
	// Ellenőrzés: egér X pixel benne van a grid területen
	if (
		mouseX < gridPositionProjectionX[0] ||
		mouseX > gridPositionProjectionX[10]
	) {
		return;
	}

	// 1. Egér X → adatvilág X érték
	float xval = plotc_unscale(mouseX, b.xmin, b.xmax, margin_x, width);

	// 2. Adatvilág X → Y érték interpolációval
	float yval = get_y_from_x(x, y, n, xval);

	// 3. Skálázás OpenGL világba
	float xp = plotc_scale(xval, b.xmin, b.xmax, margin_x);
	float yp = plotc_scale(yval, b.ymin, b.ymax, margin_y);

	// 4. Rajz: OpenGL világban, nem pixelben
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_TEXTURE_2D); // ha esetleg bekapcsolva maradt

	glPointSize(12.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	glVertex2f(xp, yp);
	glEnd();
}


 