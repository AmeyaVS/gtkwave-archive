#include"globals.h"/* 
 * Copyright (c) Tony Bybell 1999-2005.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <config.h>
#include "pixmaps.h"


/* XPM */



/* XPM */




/* XPM */



/* XPM */



/* XPM */



/* XPM */



/* XPM */



/* XPM */



/* XPM */



/* XPM */



/* XPM */



/* XPM */



void make_pixmaps(GtkWidget *window)
{
GtkStyle *style;

style=gtk_widget_get_style(window);

GLOBALS.larrow_pixmap=gdk_pixmap_create_from_xpm_d(window->window, &GLOBALS.larrow_mask,
	&style->bg[GTK_STATE_NORMAL], (gchar **)GLOBALS.icon_larrow_pixmaps_c_1);
GLOBALS.rarrow_pixmap=gdk_pixmap_create_from_xpm_d(window->window, &GLOBALS.rarrow_mask,
	&style->bg[GTK_STATE_NORMAL], (gchar **)GLOBALS.icon_rarrow_pixmaps_c_1);

GLOBALS.zoomout_pixmap=gdk_pixmap_create_from_xpm_d(window->window, &GLOBALS.zoomout_mask,
	&style->bg[GTK_STATE_NORMAL], (gchar **)GLOBALS.icon_zoomout_pixmaps_c_1);
GLOBALS.zoomin_pixmap=gdk_pixmap_create_from_xpm_d(window->window, &GLOBALS.zoomin_mask,
	&style->bg[GTK_STATE_NORMAL], (gchar **)GLOBALS.icon_zoomin_pixmaps_c_1);
GLOBALS.zoomfit_pixmap=gdk_pixmap_create_from_xpm_d(window->window, &GLOBALS.zoomfit_mask,
	&style->bg[GTK_STATE_NORMAL], (gchar **)GLOBALS.icon_zoomfit_pixmaps_c_1);
GLOBALS.zoomundo_pixmap=gdk_pixmap_create_from_xpm_d(window->window, &GLOBALS.zoomundo_mask,
	&style->bg[GTK_STATE_NORMAL], (gchar **)GLOBALS.icon_zoomundo_pixmaps_c_1);

GLOBALS.zoom_larrow_pixmap=gdk_pixmap_create_from_xpm_d(window->window, &GLOBALS.zoom_larrow_mask,
	&style->bg[GTK_STATE_NORMAL], (gchar **)GLOBALS.zoom_larrow_pixmaps_c_1);
GLOBALS.zoom_rarrow_pixmap=gdk_pixmap_create_from_xpm_d(window->window, &GLOBALS.zoom_rarrow_mask,
	&style->bg[GTK_STATE_NORMAL], (gchar **)GLOBALS.zoom_rarrow_pixmaps_c_1);

GLOBALS.prev_page_pixmap=gdk_pixmap_create_from_xpm_d(window->window, &GLOBALS.prev_page_mask,
	&style->bg[GTK_STATE_NORMAL], (gchar **)GLOBALS.prev_page_xpm_pixmaps_c_1);
GLOBALS.next_page_pixmap=gdk_pixmap_create_from_xpm_d(window->window, &GLOBALS.next_page_mask,
	&style->bg[GTK_STATE_NORMAL], (gchar **)GLOBALS.next_page_xpm_pixmaps_c_1);

GLOBALS.wave_info_pixmap=gdk_pixmap_create_from_xpm_d(window->window, &GLOBALS.wave_info_mask,
	&style->bg[GTK_STATE_NORMAL], (gchar **)GLOBALS.wave_info_pixmaps_c_1);
GLOBALS.wave_alert_pixmap=gdk_pixmap_create_from_xpm_d(window->window, &GLOBALS.wave_alert_mask,
	&style->bg[GTK_STATE_NORMAL], (gchar **)GLOBALS.wave_alert_pixmaps_c_1);
}

/*
 * $Id$
 * $Log$
 * Revision 1.1.1.1  2007/05/30 04:27:53  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:13  gtkwave
 * initial release
 *
 */

