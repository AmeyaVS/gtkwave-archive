/*
 * Copyright (c) Tony Bybell 2006.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */
#include <config.h>
#include "busy.h"


static GdkCursor *busycursor = NULL;
static int busy = 0;

static void GuiDoEvent(GdkEvent *event)
{
if(!busy)
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
busycursor = gdk_cursor_new(GDK_WATCH);
gdk_event_handler_set((GdkEventFunc)GuiDoEvent, 0, 0);
}


void set_window_busy(GtkWidget *w)
{
if(w) gdk_window_set_cursor (w->window, busycursor);
else
if(mainwindow) gdk_window_set_cursor (mainwindow->window, busycursor);

busy++;
busy_window_refresh();
}


void set_window_idle(GtkWidget *w)
{
if(busy)
	{
	if(w) gdk_window_set_cursor (w->window, NULL);
	else
	if(mainwindow) gdk_window_set_cursor (mainwindow->window, NULL);

	busy--;
	}
}


void busy_window_refresh(void)
{
if(busy)
	{
	while (gtk_events_pending()) gtk_main_iteration();
	}
}

/*
 * $Id$
 * $Log$
 * Revision 1.2  2007/04/20 02:08:11  gtkwave
 * initial release
 *
 */

