/* 
 * Copyright (c) Tony Bybell 2008
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef WAVEFONTENGINE_H
#define WAVEFONTENGINE_H

#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
#include <pango/pango.h>
#endif

#include <gtk/gtk.h>


struct font_engine_font_t
{
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
PangoFontDescription *desc;
PangoFont *font;
PangoFontMetrics *metrics;
#endif

int ascent, descent;

GdkFont *gdkfont;

unsigned is_pango : 1;
};


void font_engine_draw_string    
			(GdkDrawable      		*drawable,
                         struct font_engine_font_t 	*font,
                         GdkGC            		*gc,
                         gint              		x,
                         gint              		y,
                         const gchar      		*string);

gint font_engine_string_measure 
			(struct font_engine_font_t      *font,
                         const gchar    		*string);


#endif

/*
 * $Id$
 * $Log$
 * Revision 1.1  2008/02/06 00:00:01  gtkwave
 * initial release
 *
 */

