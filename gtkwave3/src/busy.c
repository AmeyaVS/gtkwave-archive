/*
 * Copyright (c) Tony Bybell 2006.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include "globals.h"
#include <config.h>
#include "busy.h"

static void GuiDoEvent(GdkEvent *event)
{
if(!GLOBALS->busy_busy_c_1)
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


void gtkwave_gtk_main_iteration(void)
{
struct Global *g_old = GLOBALS;
struct Global *gcache = NULL;

set_window_busy(NULL);

while (gtk_events_pending()) 
	{
	gtk_main_iteration();
	if(GLOBALS != g_old)
		{	
		/* this should never happen! */
		/* if it does, the program state is probably screwed */
		printf("GTKWAVE | WARNING: globals changed during gtkwave_gtk_main_iteration()!\n");
		gcache = GLOBALS;
		}
	}

GLOBALS = g_old;
set_window_idle(NULL);

if(gcache)
	{
	GLOBALS = gcache;
	}
}


void init_busy(void)
{
GLOBALS->busycursor_busy_c_1 = gdk_cursor_new(GDK_WATCH);
gdk_event_handler_set((GdkEventFunc)GuiDoEvent, 0, 0);
}


void set_window_busy(GtkWidget *w)
{
if(w) gdk_window_set_cursor (w->window, GLOBALS->busycursor_busy_c_1);
else
if(GLOBALS->mainwindow) gdk_window_set_cursor (GLOBALS->mainwindow->window, GLOBALS->busycursor_busy_c_1);

GLOBALS->busy_busy_c_1++;
busy_window_refresh();
}


void set_window_idle(GtkWidget *w)
{
if(GLOBALS->busy_busy_c_1)
	{
	if(w) gdk_window_set_cursor (w->window, NULL);
	else
	if(GLOBALS->mainwindow) gdk_window_set_cursor (GLOBALS->mainwindow->window, NULL);

	GLOBALS->busy_busy_c_1--;
	}
}


void busy_window_refresh(void)
{
if(GLOBALS->busy_busy_c_1)
	{
	while (gtk_events_pending()) gtk_main_iteration();
	}
}

/*
 * $Id$
 * $Log$
 * Revision 1.2  2007/08/26 21:35:39  gtkwave
 * integrated global context management from SystemOfCode2007 branch
 *
 * Revision 1.1.1.1.2.3  2007/08/07 03:18:54  kermin
 * Changed to pointer based GLOBAL structure and added initialization function
 *
 * Revision 1.1.1.1.2.2  2007/08/06 03:50:45  gtkwave
 * globals support for ae2, gtk1, cygwin, mingw.  also cleaned up some machine
 * generated structs, etc.
 *
 * Revision 1.1.1.1.2.1  2007/08/05 02:27:18  kermin
 * Semi working global struct
 *
 * Revision 1.1.1.1  2007/05/30 04:27:22  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:11  gtkwave
 * initial release
 *
 */

