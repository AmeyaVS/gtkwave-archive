#include"globals.h"/*
 * Copyright (c) Tony Bybell 2006.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */
#include <config.h>
#include "busy.h"



static void GuiDoEvent(GdkEvent *event)
{
if(!GLOBALS.busy_busy_c_1)
	{
	gtk_main_do_event(event);
	}
	else
	{
	switch (event->type) 
		{
	        case GDK_CONFIGURE:
	        case GDK_EXPOSE:
	            	gtk_main_do_event(event);

		default:
			break;
		}
	}
}


void init_busy(void)
{
GLOBALS.busycursor_busy_c_1 = gdk_cursor_new(GDK_WATCH);
gdk_event_handler_set((GdkEventFunc)GuiDoEvent, 0, 0);
}


void set_window_busy(GtkWidget *w)
{
if(w) gdk_window_set_cursor (w->window, GLOBALS.busycursor_busy_c_1);
else
if(GLOBALS.mainwindow) gdk_window_set_cursor (GLOBALS.mainwindow->window, GLOBALS.busycursor_busy_c_1);

GLOBALS.busy_busy_c_1++;
busy_window_refresh();
}


void set_window_idle(GtkWidget *w)
{
if(GLOBALS.busy_busy_c_1)
	{
	if(w) gdk_window_set_cursor (w->window, NULL);
	else
	if(GLOBALS.mainwindow) gdk_window_set_cursor (GLOBALS.mainwindow->window, NULL);

	GLOBALS.busy_busy_c_1--;
	}
}


void busy_window_refresh(void)
{
if(GLOBALS.busy_busy_c_1)
	{
	while (gtk_events_pending()) gtk_main_iteration();
	}
}

/*
 * $Id$
 * $Log$
 * Revision 1.1.1.1  2007/05/30 04:27:22  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:11  gtkwave
 * initial release
 *
 */

