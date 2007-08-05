#include"globals.h"/* 
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




GdkGC *alloc_color(GtkWidget *widget, int tuple, GdkGC *fallback);	/* tuple is encoded as 32bit: --RRGGBB (>=0 is valid) */

#endif

/*
 * $Id$
 * $Log$
 * Revision 1.1.1.1  2007/05/30 04:27:54  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:11  gtkwave
 * initial release
 *
 */

