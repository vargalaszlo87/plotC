#ifndef STATUSBAR_H
#define STATUSBAR_H

/*!
 * variables
 */

extern int width;
extern int height;

/*!
 * methods
 */

extern void begin_pixel_mode(int, int);
extern void end_pixel_mode();
extern void draw_text(float, float, const char*);

void plotc_draw_statusbar(float);
void plot_text_statusbar(char *);

#endif