#include"globals.h"/* 
 * Copyright (c) Tony Bybell 1999.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef __MFMAIN_H__
#define __MFMAIN_H__

#include "busy.h"

#define HAVE_PANED_PACK	/* undefine this if you have an older GTK */

struct logfile_chain
{
struct logfile_chain *next;
char *name;
};


GtkWidget *create_text(void);
GtkWidget *create_zoom_buttons(void);
GtkWidget *create_page_buttons(void);
GtkWidget *create_fetch_buttons(void);
GtkWidget *create_discard_buttons(void);
GtkWidget *create_shift_buttons(void);
GtkWidget *create_entry_box(void);
GtkWidget *create_time_box(void);
GtkWidget *create_wavewindow(void);
GtkWidget *create_signalwindow(void);

/* Get/set the current size of the window.  */
extern void get_window_size (int *x, int *y);
extern void set_window_size (int x, int y);

/* Get/set the x/y pos of the window */
void get_window_xypos(int *root_x, int *root_y);
void set_window_xypos(int root_x, int root_y);

/* stems helper activation */
int stems_are_active(void);
void activate_stems_reader(char *stems_name);





#endif

/*
 * $Id$
 * $Log$
 * Revision 1.1.1.1  2007/05/30 04:27:28  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:13  gtkwave
 * initial release
 *
 */

