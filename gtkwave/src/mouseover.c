/* 
 * Copyright (c) Tony Bybell 2006.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

/* AIX may need this for alloca to work */ 
#if defined _AIX
  #pragma alloca
#endif

#include <config.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "currenttime.h"
#include "color.h"
#include "bsearch.h"

char disable_mouseover = 1; /* default for now is disabled -- don't know if users will dislike this enabled */

/************************************************************************************************/

static void local_trace_asciival(Trptr t, TimeType tim, int *nmaxlen, int *vmaxlen, char **asciivalue)
{
int len=0;
int vlen=0;

if(t->name)
	{
	len=gdk_string_measure(wavefont, t->name);

	if((tim!=-1)&&(!(t->flags&TR_EXCLUDE)))
		{
		if(t->vector)
			{
			char *str;
			vptr v;

                        v=bsearch_vector(t->n.vec,tim);
                        str=convert_ascii(t,v);
			if(str)
				{
				vlen=gdk_string_measure(wavefont,str);
				*asciivalue=str;
				}
				else
				{
				vlen=0;
				*asciivalue=NULL;
				}

			}
			else
			{
			char *str;
			hptr h_ptr;
			if((h_ptr=bsearch_node(t->n.nd,tim)))
				{
				if(!t->n.nd->ext)
					{
					str=(char *)calloc_2(1,2*sizeof(char));
					if(t->flags&TR_INVERT)
						{
						str[0]=AN_STR_INV[h_ptr->v.h_val];
						}
						else
						{
						str[0]=AN_STR[h_ptr->v.h_val];
						}
					*asciivalue=str;
					vlen=gdk_string_measure(wavefont,str);
					}
					else
					{
					if(h_ptr->flags&HIST_REAL)
						{
						if(!(h_ptr->flags&HIST_STRING))
							{
							str=convert_ascii_real((double *)h_ptr->v.h_vector);
							}
							else
							{
							str=convert_ascii_string((char *)h_ptr->v.h_vector);
							}
						}
						else
						{
		                        	str=convert_ascii_vec(t,h_ptr->v.h_vector);
						}

					if(str)
						{
						vlen=gdk_string_measure(wavefont,str);
						*asciivalue=str;
						}
						else
						{
						vlen=0;
						*asciivalue=NULL;
						}
					}
				}
				else
				{
				vlen=0;
				*asciivalue=NULL;
				}
			}
		}
	}

*nmaxlen = len;
*vmaxlen = vlen;
}

/************************************************************************************************/

static GtkWidget *mouseover = NULL;
static GtkWidget *mo_area=NULL;
static GdkPixmap *mo_pixmap = NULL;
static GdkGC *mo_dk_gray = NULL;
static GdkGC *mo_black = NULL;
static gint mo_width = 0, mo_height = 0;

static gint expose_event(GtkWidget *widget, GdkEventExpose *event)
{       
gdk_draw_pixmap(widget->window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
                mo_pixmap,
                event->area.x, event->area.y,
                event->area.x, event->area.y,
                event->area.width, event->area.height);
return(FALSE);
}


void create_mouseover(gint x, gint y, gint width, gint height)
{
mo_width = width;
mo_height = height;

mouseover = gtk_window_new(GTK_WINDOW_POPUP);
gtk_window_set_default_size(GTK_WINDOW (mouseover), width, height);

#ifdef WAVE_USE_GTK2
	gtk_window_set_type_hint(GTK_WINDOW(mouseover), GDK_WINDOW_TYPE_HINT_SPLASHSCREEN);
        gtk_window_move(GTK_WINDOW(mouseover), x, y);
#endif

mo_area=gtk_drawing_area_new();
gtk_container_add(GTK_CONTAINER(mouseover), mo_area);
gtk_widget_show(mo_area);
gtk_widget_show(mouseover);

#ifndef WAVE_USE_GTK2
gtk_window_reposition(GTK_WINDOW(mouseover), x, y); /* cuts down on GTK+-1.2 visual noise by moving it here */
#endif

mo_pixmap = gdk_pixmap_new(mo_area->window, mouseover->allocation.width, mouseover->allocation.height, -1);

if(!mo_dk_gray) mo_dk_gray = alloc_color(mo_area, 0x00cccccc, NULL);
if(!mo_black)   mo_black   = alloc_color(mo_area, 0x00000000, NULL);

gdk_draw_rectangle(mo_pixmap, mo_dk_gray,
		TRUE,
		0,0, 
		mo_width, mo_height);

gdk_draw_rectangle(mo_pixmap, mo_black,
		TRUE,
		1,1, 
		mo_width-2, mo_height-2);

gtk_signal_connect(GTK_OBJECT(mo_area), "expose_event",
                        GTK_SIGNAL_FUNC(expose_event), NULL);
}

#define MOUSEOVER_BREAKSIZE (32)

void move_mouseover(Trptr t, gint xin, gint yin, TimeType tim)
{
#if defined _MSC_VER || defined __MINGW32__

return;

#else

gint xd = 0, yd = 0;
char *asciivalue = NULL;
int nmaxlen = 0, vmaxlen = 0;
int totalmax;
int name_charlen = 0, value_charlen = 0;
int num_info_rows = 2;
char *alternate_name = NULL;
int fh;

if(disable_mouseover)
	{
	if(mouseover)
		{
		gtk_widget_destroy(mouseover); mouseover = NULL;
		gdk_pixmap_unref(mo_pixmap);   mo_pixmap = NULL;
		}
	goto bot;
	}

fh = wavefont->ascent+wavefont->descent;

if(t)
	{
	local_trace_asciival(t, tim, &nmaxlen, &vmaxlen, &asciivalue);

	name_charlen = t->name ? strlen(t->name) : 0;
	value_charlen = asciivalue ? strlen(asciivalue) : 0;

	if(name_charlen > MOUSEOVER_BREAKSIZE)
		{
		alternate_name = malloc_2(MOUSEOVER_BREAKSIZE + 1);
		strcpy(alternate_name, "...");
		strcpy(alternate_name + 3, t->name + name_charlen - (MOUSEOVER_BREAKSIZE - 3));
	
		nmaxlen=gdk_string_measure(wavefont, alternate_name);
		}

	if(value_charlen > MOUSEOVER_BREAKSIZE)
		{
		char breakbuf[MOUSEOVER_BREAKSIZE+1];
		int i, localmax;

		num_info_rows = (value_charlen + (MOUSEOVER_BREAKSIZE-1)) / MOUSEOVER_BREAKSIZE;
		vmaxlen = 0;

		for(i=0;i<num_info_rows;i++)
			{
			memset(breakbuf, 0, MOUSEOVER_BREAKSIZE+1);
			strncpy(breakbuf, asciivalue + (i*MOUSEOVER_BREAKSIZE), MOUSEOVER_BREAKSIZE);
			localmax = gdk_string_measure(wavefont, breakbuf);
			vmaxlen = (localmax > vmaxlen) ? localmax : vmaxlen;
			}

		num_info_rows++;
		}

	totalmax = (nmaxlen > vmaxlen) ? nmaxlen : vmaxlen;
	totalmax += 8;
	totalmax = (totalmax + 1) & ~1; /* round up to next even pixel count */

	if((mouseover)&&((totalmax != mo_width)||((num_info_rows * fh + 7) != mo_height)))
		{
		gtk_widget_destroy(mouseover); mouseover = NULL;
		gdk_pixmap_unref(mo_pixmap);   mo_pixmap = NULL;
		}
	}

if((!t)||(yin<0)||(yin>waveheight))
	{
	if(mouseover)
		{
		gtk_widget_destroy(mouseover); mouseover = NULL;
		gdk_pixmap_unref(mo_pixmap);   mo_pixmap = NULL;
		}
	goto bot;
	}

if(!mouseover)
	{
#ifdef WAVE_USE_GTK2
	gdk_window_get_origin(wavearea->window, &xd, &yd);
#else
	gdk_window_get_deskrelative_origin(wavearea->window, &xd, &yd);
#endif
	create_mouseover(xin + xd + 8, yin + yd + 20, totalmax, num_info_rows * fh + 7);
	}
	else
	{
#ifdef WAVE_USE_GTK2
	gdk_window_get_origin(wavearea->window, &xd, &yd);
        gtk_window_move(GTK_WINDOW(mouseover), xin + xd + 8, yin + yd + 20);
#else
	gdk_window_get_deskrelative_origin(wavearea->window, &xd, &yd);
        gtk_window_reposition(GTK_WINDOW(mouseover), xin + xd + 8, yin + yd + 20);
#endif
	}

gdk_draw_rectangle(mo_pixmap, mo_dk_gray,
		TRUE,
		0,0, 
		mo_width, mo_height);

gdk_draw_rectangle(mo_pixmap, mo_black,
		TRUE,
		1,1, 
		mo_width-2, mo_height-2);

gdk_draw_string(mo_pixmap, wavefont, mo_dk_gray, 4, fh + 2, alternate_name ? alternate_name : t->name);

if(num_info_rows == 2)
	{
	if(asciivalue) gdk_draw_string(mo_pixmap, wavefont, mo_dk_gray, 4, 2*fh+2, asciivalue);
	}
	else
	{
	char breakbuf[MOUSEOVER_BREAKSIZE+1];
	int i;

	num_info_rows--;
	for(i=0;i<num_info_rows;i++)
		{
		memset(breakbuf, 0, MOUSEOVER_BREAKSIZE+1);
		strncpy(breakbuf, asciivalue + (i*MOUSEOVER_BREAKSIZE), MOUSEOVER_BREAKSIZE);
		gdk_draw_string(mo_pixmap, wavefont, mo_dk_gray, 4, ((2+i)*fh)+2, breakbuf);
		}
	}

gdk_window_raise(mouseover->window);
gdk_draw_pixmap(mo_area->window, mo_area->style->fg_gc[GTK_WIDGET_STATE(mouseover)],
                mo_pixmap,
		0,0,
		0,0,
		mo_width, mo_height);

bot:
if(asciivalue) { free_2(asciivalue); }
if(alternate_name) { free_2(alternate_name); }
#endif
}

/*
 * $Id$
 * $Log$
 */

