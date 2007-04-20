/* 
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

extern char *whoami;
extern char *stems_name, *aet_name;

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


extern GtkWidget *mainwindow;
extern GtkWidget *signalwindow;
extern GtkWidget *wavewindow;
extern char paned_pack_semantics;    /* 1 for paned_pack, 0 for paned_add */
extern int initial_window_x, initial_window_y; /* inital window sizes */
extern int initial_window_xpos, initial_window_ypos; /* inital window position */
extern int xpos_delta, ypos_delta;

extern int splash_disable;
extern int possibly_use_rc_defaults;

extern int stems_type;

#endif

/*
 * $Id$
 * $Log$
 */

