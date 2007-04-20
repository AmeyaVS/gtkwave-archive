/* 
 * Copyright (c) Tony Bybell 1999-2005.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef WAVE_PIXMAPS_H
#define WAVE_PIXMAPS_H

#include <gtk/gtk.h>

void make_pixmaps(GtkWidget *window);

extern GdkPixmap *larrow_pixmap;
extern GdkBitmap *larrow_mask;
extern GdkPixmap *rarrow_pixmap;
extern GdkBitmap *rarrow_mask;

extern GdkPixmap *zoomout_pixmap;
extern GdkBitmap *zoomout_mask;
extern GdkPixmap *zoomin_pixmap;
extern GdkBitmap *zoomin_mask;
extern GdkPixmap *zoomfit_pixmap;
extern GdkBitmap *zoomfit_mask;
extern GdkPixmap *zoomundo_pixmap;
extern GdkBitmap *zoomundo_mask;

extern GdkPixmap *zoom_larrow_pixmap;
extern GdkBitmap *zoom_larrow_mask;
extern GdkPixmap *zoom_rarrow_pixmap;
extern GdkBitmap *zoom_rarrow_mask;

extern GdkPixmap *prev_page_pixmap;
extern GdkBitmap *prev_page_mask;
extern GdkPixmap *next_page_pixmap;
extern GdkBitmap *next_page_mask;

extern GdkPixmap *wave_info_pixmap;
extern GdkBitmap *wave_info_mask;
extern GdkPixmap *wave_alert_pixmap;
extern GdkBitmap *wave_alert_mask;

#ifdef WAVE_USE_GTK2

#define WAVE_SPLASH_X (512)
#define WAVE_SPLASH_Y (384)

void make_splash_pixmaps(GtkWidget *window);

extern GdkPixmap *wave_splash_pixmap;
extern GdkBitmap *wave_splash_mask;
#endif

#endif

/*
 * $Id$
 * $Log$
 */

