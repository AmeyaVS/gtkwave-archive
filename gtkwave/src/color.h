/* 
 * Copyright (c) Tony Bybell 1999-2005.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef WAVE_COLOR_H
#define WAVE_COLOR_H

#include <stdlib.h>
#include <gtk/gtk.h>

extern int color_back;
extern int color_baseline;
extern int color_grid;
extern int color_high;
extern int color_low;
extern int color_mark;
extern int color_mid;
extern int color_time;
extern int color_timeb;
extern int color_trans;
extern int color_umark;
extern int color_value;
extern int color_vbox;
extern int color_vtrans;
extern int color_x;
extern int color_xfill;

extern int color_0;
extern int color_1;
extern int color_ufill;
extern int color_u;
extern int color_wfill;
extern int color_w;
extern int color_dashfill;
extern int color_dash;

extern int color_white;
extern int color_black;
extern int color_ltgray;
extern int color_normal;
extern int color_mdgray;
extern int color_dkgray;
extern int color_dkblue;

GdkGC *alloc_color(GtkWidget *widget, int tuple, GdkGC *fallback);	/* tuple is encoded as 32bit: --RRGGBB (>=0 is valid) */

#endif

