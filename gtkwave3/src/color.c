/* 
 * Copyright (c) Tony Bybell 1999-2005.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <config.h>
#include "color.h"
#include "debug.h"

int color_back=-1;
int color_baseline=-1;
int color_grid=-1;
int color_high=-1;
int color_low=-1;
int color_mark=-1;
int color_mid=-1;
int color_time=-1;
int color_timeb=-1;
int color_trans=-1;
int color_umark=-1;
int color_value=-1;
int color_vbox=-1;
int color_vtrans=-1;
int color_x=-1;
int color_xfill=-1;

int color_0=-1;
int color_1=-1;
int color_ufill=-1;
int color_u=-1;
int color_wfill=-1;
int color_w=-1;
int color_dashfill=-1;
int color_dash=-1;

int color_white = -1;
int color_black = -1;
int color_ltgray = -1;
int color_normal = -1;
int color_mdgray = -1;
int color_dkgray = -1;
int color_dkblue = -1;


/* 
 * return graphics context with tuple's color or
 * a fallback context.  Note that if tuple<0,
 * the fallback will be used!
 */
GdkGC *alloc_color(GtkWidget *widget, int tuple, GdkGC *fallback)
{
GdkColor color;
GdkGC    *gc;
int red, green, blue;

red=  (tuple>>16)&0x000000ff;
green=(tuple>>8) &0x000000ff;
blue= (tuple)    &0x000000ff;

if(tuple>=0)
if((gc=gdk_gc_new(widget->window)))
	{
	color.red=red*(65535/255);
	color.blue=blue*(65535/255);  
	color.green=green*(65535/255);
	color.pixel=(gulong)(tuple&0x00ffffff);
	gdk_color_alloc(gtk_widget_get_colormap(widget),&color);
	gdk_gc_set_foreground(gc,&color);
	return(gc);
	}

return(fallback);
}

/*
 * $Id$
 * $Log$
 * Revision 1.2  2007/04/20 02:08:11  gtkwave
 * initial release
 *
 */

