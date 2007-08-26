/* 
 * Copyright (c) Tony Bybell 1999-2005.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include "globals.h"
#include <config.h>
#include "gtk12compat.h"
#include "currenttime.h"
#include "pixmaps.h"
#include "symbol.h"
#include "debug.h"


/*
 * complain about certain ops conflict with dnd...
 */
void dnd_error(void)
{ 
status_text("Can't perform that operation when waveform drag and drop is in progress!\n");
}  


static void     
service_hslider(GtkWidget *text, gpointer data)
{
GtkAdjustment *hadj;
gint xsrc;

if(GLOBALS->signalpixmap)
	{
	hadj=GTK_ADJUSTMENT(GLOBALS->signal_hslider);
	xsrc=(gint)hadj->value;
	DEBUG(printf("Signal HSlider Moved to %d\n",xsrc));

	gdk_draw_rectangle(GLOBALS->signalpixmap, GLOBALS->gc_dkgray, TRUE,
	        0, -1, GLOBALS->signal_fill_width, GLOBALS->fontheight);
	gdk_draw_line(GLOBALS->signalpixmap, GLOBALS->gc_white,  
	        0, GLOBALS->fontheight-1, GLOBALS->signal_fill_width-1, GLOBALS->fontheight-1);
	gdk_draw_string(GLOBALS->signalpixmap, GLOBALS->signalfont,
	        GLOBALS->gc_black, 3+xsrc, GLOBALS->fontheight-4, "Time");

	gdk_draw_pixmap(GLOBALS->signalarea->window, GLOBALS->signalarea->style->fg_gc[GTK_WIDGET_STATE(GLOBALS->signalarea)],GLOBALS->signalpixmap,xsrc, 0,0, 0,GLOBALS->signalarea->allocation.width, GLOBALS->signalarea->allocation.height);
	}
}


static gint motion_notify_event(GtkWidget *widget, GdkEventMotion *event)
{
int num_traces_displayable;
int which;
int trwhich, trtarget;
int new_cachedwhich;
GtkAdjustment *wadj;
Trptr t;
gdouble x,y;
GdkModifierType state;
int doloop;

#ifdef WAVE_USE_GTK2
gint xi, yi;
#endif

if(event->is_hint)
	{
	WAVE_GDK_GET_POINTER(event->window, &x, &y, &xi, &yi, &state);
	WAVE_GDK_GET_POINTER_COPY;
	}
	else
	{
	x = event->x;
	y = event->y;
	state = event->state;
	}

top: doloop=0;

/********************* button 1 *********************/

if((GLOBALS->traces.visible)&&(state&(GDK_BUTTON1_MASK|GDK_BUTTON3_MASK))&&(GLOBALS->signalpixmap)&&(GLOBALS->cachedwhich_signalwindow_c_1!=-1))
	{
	num_traces_displayable=widget->allocation.height/(GLOBALS->fontheight);
	num_traces_displayable--;   /* for the time trace that is always there */

	which=(int)(y);
	which=(which/GLOBALS->fontheight)-1;

	if(which>=GLOBALS->traces.visible)
		{
		which=GLOBALS->traces.visible-1;
		}


	wadj=GTK_ADJUSTMENT(GLOBALS->wave_vslider);

	if(num_traces_displayable<GLOBALS->traces.visible)
		{
		if(which>=num_traces_displayable)
			{
			int target;

			target=((int)wadj->value)+1;
			which=num_traces_displayable-1;

			if(target+which>=(GLOBALS->traces.visible-1)) target=GLOBALS->traces.visible-which-1;
			wadj->value=target;

			if(GLOBALS->cachedwhich_signalwindow_c_1==which) GLOBALS->cachedwhich_signalwindow_c_1=which-1; /* force update */

			gtk_signal_emit_by_name (GTK_OBJECT (wadj), "changed");	/* force bar update */
			gtk_signal_emit_by_name (GTK_OBJECT (wadj), "value_changed"); /* force text update */
	
			doloop=1;
			}
			else
			if (which<0)
			{
			int target;
	
			target=((int)wadj->value)-1;
			if(target<0) target=0;
			wadj->value=target;
	
			which=0;
			if(GLOBALS->cachedwhich_signalwindow_c_1==which) GLOBALS->cachedwhich_signalwindow_c_1=-1; /* force update */
	
			gtk_signal_emit_by_name (GTK_OBJECT (wadj), "changed");	/* force bar update */
			gtk_signal_emit_by_name (GTK_OBJECT (wadj), "value_changed"); /* force text update */
	
			doloop=1;
			}
		}
		else
		{
		if(which<0) which=0;
		}

	trtarget=((int)wadj->value)+which;

	GLOBALS->cachedtrace=t=GLOBALS->traces.first;
	trwhich=0;
	while(t)
	        {
	        if((trwhich<trtarget)&&(GiveNextTrace(t)))
	                {
	                trwhich++;
	                t=GiveNextTrace(t);
	                }
	                else
	                {
	                break;
	                }
	        }

	GLOBALS->cachedtrace=t;

	if((GLOBALS->dnd_state==1)&&(state&GDK_BUTTON3_MASK))
		{
		GtkAdjustment *hadj;
		gint xsrc;
		int yval;

		hadj=GTK_ADJUSTMENT(GLOBALS->signal_hslider);
		wadj=GTK_ADJUSTMENT(GLOBALS->wave_vslider);
		gtk_signal_emit_by_name (GTK_OBJECT (wadj), "changed");	/* force bar update */
		gtk_signal_emit_by_name (GTK_OBJECT (wadj), "value_changed"); /* force text update */

		xsrc=(gint)hadj->value;

		yval=RenderSig(t, which, 2);
        	gdk_draw_pixmap(GLOBALS->signalarea->window, GLOBALS->signalarea->style->fg_gc[GTK_WIDGET_STATE(GLOBALS->signalarea)],GLOBALS->signalpixmap,xsrc, yval,0, yval,GLOBALS->signalarea->allocation.width, GLOBALS->fontheight-1);

		}
	else
	if((state&GDK_BUTTON1_MASK)&&(GLOBALS->dnd_state==0))
	if((t)&&(which!=GLOBALS->cachedwhich_signalwindow_c_1))
		{
		GtkAdjustment *hadj;
		gint xsrc;

		hadj=GTK_ADJUSTMENT(GLOBALS->signal_hslider);
		xsrc=(gint)hadj->value;

		new_cachedwhich=which;	/* save so fill ins go from deltas in the future */
		
		do {
		int oldflags;

		oldflags = t->flags;

		t->flags = (t->flags & (~TR_HIGHLIGHT)) | GLOBALS->cachedhiflag_signalwindow_c_1;

		if(oldflags!=t->flags)
			{
			int yval;
			DEBUG(printf("Motion highlight swap in signalarea at x: %d, y: %d row: %d\n",
				(int)x, (int)y, which));

			yval=RenderSig(t, which, 1);
        		gdk_draw_pixmap(GLOBALS->signalarea->window, GLOBALS->signalarea->style->fg_gc[GTK_WIDGET_STATE(GLOBALS->signalarea)],GLOBALS->signalpixmap,xsrc, yval,0, yval,GLOBALS->signalarea->allocation.width, GLOBALS->fontheight-1);
			}

		if(which>GLOBALS->cachedwhich_signalwindow_c_1)
			{
			which--;
			t=GivePrevTrace(t);
			}
		else if(which<GLOBALS->cachedwhich_signalwindow_c_1)
			{
			which++;
			t=GiveNextTrace(t);
			}

		} while((which!=GLOBALS->cachedwhich_signalwindow_c_1)&&(t));

		GLOBALS->cachedwhich_signalwindow_c_1=new_cachedwhich;	/* for next time around */
		}
	}

if(doloop)
	{
	WAVE_GDK_GET_POINTER(event->window, &x, &y, &xi, &yi, &state);
	WAVE_GDK_GET_POINTER_COPY;
	goto top;
	}

return(TRUE);
}


static gint button_release_event(GtkWidget *widget, GdkEventButton *event)
{
int which;

if(event->button==1)
	{
	if(GLOBALS->dnd_state==0) GLOBALS->cachedwhich_signalwindow_c_1=-1;
	gdk_pointer_ungrab(event->time);
	DEBUG(printf("Button 1 released\n"));
	}

/********************* button 3 *********************/

if((event->button==3)&&(GLOBALS->signalpixmap))
	{
	GLOBALS->cachedwhich_signalwindow_c_1=-1;
	gdk_pointer_ungrab(event->time);
	DEBUG(printf("Button 3 released\n"));

	if(GLOBALS->dnd_state==1)
		{
		if(GLOBALS->cachedtrace)
			{
			GLOBALS->cachedtrace->flags|=TR_HIGHLIGHT;
			}

		which=(int)(event->y);
		which=(which/GLOBALS->fontheight)-1;
	
		if( ((which<0) && (GLOBALS->topmost_trace==GLOBALS->traces.first) && PrependBuffer()) || (PasteBuffer()) ) /* short circuit on special which<0 case */
	       		{
			/* status_text("Drop completed.\n"); */

        		MaxSignalLength();
        		signalarea_configure_event(GLOBALS->signalarea, NULL);
        		wavearea_configure_event(GLOBALS->wavearea, NULL);
        		}
		GLOBALS->dnd_state=0;
		}
	}

/********************* button 3 *********************/

return(TRUE);
}

static gint button_press_event(GtkWidget *widget, GdkEventButton *event)
{
int num_traces_displayable;
int which;
int trwhich, trtarget;
GtkAdjustment *wadj;
Trptr t;

if((GLOBALS->traces.visible)&&(GLOBALS->signalpixmap))
	{
	gdk_pointer_grab(widget->window, FALSE,
		GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON1_MOTION_MASK | GDK_BUTTON3_MOTION_MASK | 
		GDK_BUTTON_RELEASE_MASK, NULL, NULL, event->time);

	num_traces_displayable=widget->allocation.height/(GLOBALS->fontheight);
	num_traces_displayable--;   /* for the time trace that is always there */

	which=(int)(event->y);
	which=(which/GLOBALS->fontheight)-1;

	if((which>=GLOBALS->traces.visible)||(which>=num_traces_displayable)||(which<0))
		{
		if(GLOBALS->dnd_state==0)GLOBALS->cachedwhich_signalwindow_c_1=-1;
		goto check_button_3;	/* off in no man's land, but check 3rd anyways.. */
		}

	GLOBALS->cachedwhich_signalwindow_c_1=which;	/* cache for later fill in */
	wadj=GTK_ADJUSTMENT(GLOBALS->wave_vslider);
	trtarget=((int)wadj->value)+which;

	t=GLOBALS->traces.first;
	trwhich=0;
	while(t)
	        {
	        if((trwhich<trtarget)&&(GiveNextTrace(t)))
	                {
	                trwhich++;
	                t=GiveNextTrace(t);
	                }
	                else
	                {
	                break;
	                }
	        }

	GLOBALS->cachedtrace=t;

	/* frankel add */
	if((event->state&GDK_CONTROL_MASK)&&(GLOBALS->dnd_state==0)&&(event->button==1))
	if(t)
		{
		if(CollapseTrace(t))
			{
			GLOBALS->signalwindow_width_dirty=1;
        		MaxSignalLength();
        		signalarea_configure_event(GLOBALS->signalarea, NULL);
        		wavearea_configure_event(GLOBALS->wavearea, NULL);
			return(TRUE);
			}
		}

	if((GLOBALS->dnd_state==0)&&(event->button==1))
	if(t)
		{
		int yval;
		GtkAdjustment *hadj;
		gint xsrc;

		if((GLOBALS->shift_click_trace)&&(event->state&GDK_SHIFT_MASK))
			{
			Trptr t2;
			unsigned int f;

			t2=GLOBALS->shift_click_trace;
			while(t2)
				{
				if(t2==t)
					{
					t2=GLOBALS->shift_click_trace;
					f=t2->flags&TR_HIGHLIGHT;
					while(t2)
						{
						t2->flags = (t2->flags & (~TR_HIGHLIGHT)) | f;
						if(t2==t) break;
						t2=GivePrevTrace(t2);
						}					
					goto resync_signalarea;
					}
				t2=GivePrevTrace(t2);
				}

			t2=GLOBALS->shift_click_trace;
			while(t2)
				{
				if(t2==t)
					{
					t2=GLOBALS->shift_click_trace;
					f=t2->flags&TR_HIGHLIGHT;
					while(t2)
						{
						t2->flags = (t2->flags & (~TR_HIGHLIGHT)) | f;
						if(t2==t) break;
						t2=GiveNextTrace(t2);
						}					
					goto resync_signalarea;
					}
				t2=GiveNextTrace(t2);
				}

			goto normal_button1_press;	/* couldn't find original so make this original... */
			
			resync_signalarea:
        		MaxSignalLength();
        		signalarea_configure_event(GLOBALS->signalarea, NULL);
			DEBUG(printf("Shift-Click in signalarea!\n"));
			return(TRUE);
			}
			else
			{
			normal_button1_press:
			hadj=GTK_ADJUSTMENT(GLOBALS->signal_hslider);
			xsrc=(gint)hadj->value;

			GLOBALS->shift_click_trace=t;
			t->flags ^= TR_HIGHLIGHT;
			GLOBALS->cachedhiflag_signalwindow_c_1 = t->flags & TR_HIGHLIGHT;

			DEBUG(printf("Button pressed in signalarea at x: %d, y: %d row: %d\n",
				(int)event->x, (int)event->y, which));

			yval=RenderSig(t, which, 1);
	        	gdk_draw_pixmap(GLOBALS->signalarea->window, GLOBALS->signalarea->style->fg_gc[GTK_WIDGET_STATE(GLOBALS->signalarea)],GLOBALS->signalpixmap,xsrc, yval,0, yval,GLOBALS->signalarea->allocation.width, GLOBALS->fontheight-1);
			}
		}

check_button_3:
	if(event->button==3)
		{
		if(GLOBALS->dnd_state==0)
			{
			if(CutBuffer())
	        		{
				/* char buf[32]; */
				/* sprintf(buf,"Dragging %d trace%s.\n",traces.buffercount,traces.buffercount!=1?"s":"");
				status_text(buf); */
	        		MaxSignalLength();
	        		signalarea_configure_event(GLOBALS->signalarea, NULL);
	        		wavearea_configure_event(GLOBALS->wavearea, NULL);
				GLOBALS->dnd_state=1;
				}
			}
		}
	}

return(TRUE);
}

gint signalarea_configure_event(GtkWidget *widget, GdkEventConfigure *event)
{
GtkAdjustment *wadj, *hadj;
int num_traces_displayable;
int width;

make_sigarea_gcs(widget);
UpdateTracesVisible();

num_traces_displayable=widget->allocation.height/(GLOBALS->fontheight);
num_traces_displayable--;   /* for the time trace that is always there */

DEBUG(printf("SigWin Configure Event h: %d, w: %d\n",
		widget->allocation.height,
		widget->allocation.width));

GLOBALS->old_signal_fill_width=GLOBALS->signal_fill_width;
GLOBALS->signal_fill_width = ((width=widget->allocation.width) > GLOBALS->signal_pixmap_width)
        ? widget->allocation.width : GLOBALS->signal_pixmap_width;

if(GLOBALS->signalpixmap)
	{
	if((GLOBALS->old_signal_fill_width!=GLOBALS->signal_fill_width)||(GLOBALS->old_signal_fill_height!=widget->allocation.height))
		{
		gdk_pixmap_unref(GLOBALS->signalpixmap);
		GLOBALS->signalpixmap=gdk_pixmap_new(widget->window, 
			GLOBALS->signal_fill_width, widget->allocation.height, -1);
		}
	}
	else
	{
	GLOBALS->signalpixmap=gdk_pixmap_new(widget->window, 
		GLOBALS->signal_fill_width, widget->allocation.height, -1);
	}

GLOBALS->old_signal_fill_height= widget->allocation.height;
gdk_draw_rectangle(GLOBALS->signalpixmap, widget->style->bg_gc[GTK_STATE_PRELIGHT], TRUE, 0, 0,
			GLOBALS->signal_fill_width, widget->allocation.height);

hadj=GTK_ADJUSTMENT(GLOBALS->signal_hslider);
hadj->page_size=hadj->page_increment=(gfloat)width;
hadj->step_increment=(gfloat)10.0;  /* approx 1ch at a time */
hadj->lower=(gfloat)0.0;
hadj->upper=(gfloat)GLOBALS->signal_pixmap_width;

if( ((int)hadj->value)+width > GLOBALS->signal_fill_width)
	{
	hadj->value = (gfloat)(GLOBALS->signal_fill_width-width);
	}


wadj=GTK_ADJUSTMENT(GLOBALS->wave_vslider);
wadj->page_size=wadj->page_increment=(gfloat) num_traces_displayable;
wadj->step_increment=(gfloat)1.0;
wadj->lower=(gfloat)0.0;
wadj->upper=(gfloat)(GLOBALS->traces.visible ? GLOBALS->traces.visible : 1);

if(num_traces_displayable>GLOBALS->traces.visible)
	{
	wadj->value=(gfloat)(GLOBALS->trtarget_signalwindow_c_1=0);
	}
	else
	if (wadj->value + num_traces_displayable > GLOBALS->traces.visible)
	{
	wadj->value=(gfloat)(GLOBALS->trtarget_signalwindow_c_1=GLOBALS->traces.visible-num_traces_displayable);
	}

gtk_signal_emit_by_name (GTK_OBJECT (wadj), "changed");	/* force bar update */
gtk_signal_emit_by_name (GTK_OBJECT (wadj), "value_changed"); /* force text update */

gtk_signal_emit_by_name (GTK_OBJECT (hadj), "changed");	/* force bar update */

return(TRUE);
}

static gint expose_event(GtkWidget *widget, GdkEventExpose *event)
{
GtkAdjustment *hadj;
int xsrc;

hadj=GTK_ADJUSTMENT(GLOBALS->signal_hslider);
xsrc=(gint)hadj->value;

gdk_draw_pixmap(widget->window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
		GLOBALS->signalpixmap, 
		xsrc+event->area.x, event->area.y,
		event->area.x, event->area.y,
		event->area.width, event->area.height);

return(FALSE);
}

GtkWidget *
create_signalwindow(void)
{
GtkWidget *table;
GtkWidget *frame;

table = gtk_table_new(10, 10, FALSE);

GLOBALS->signalarea=gtk_drawing_area_new();

gtk_widget_show(GLOBALS->signalarea);
MaxSignalLength();

gtk_widget_set_events(GLOBALS->signalarea, 
		GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK | 
		GDK_BUTTON_RELEASE_MASK | 
		GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK
		);

gtk_signal_connect(GTK_OBJECT(GLOBALS->signalarea), "configure_event", GTK_SIGNAL_FUNC(signalarea_configure_event), NULL);
gtk_signal_connect(GTK_OBJECT(GLOBALS->signalarea), "expose_event",GTK_SIGNAL_FUNC(expose_event), NULL);
gtk_signal_connect(GTK_OBJECT(GLOBALS->signalarea), "button_press_event",GTK_SIGNAL_FUNC(button_press_event), NULL);
gtk_signal_connect(GTK_OBJECT(GLOBALS->signalarea), "button_release_event", GTK_SIGNAL_FUNC(button_release_event), NULL);
gtk_signal_connect(GTK_OBJECT(GLOBALS->signalarea), "motion_notify_event",GTK_SIGNAL_FUNC(motion_notify_event), NULL);

dnd_setup(GLOBALS->signalarea);

gtk_table_attach (GTK_TABLE (table), GLOBALS->signalarea, 0, 10, 0, 9,
                        GTK_FILL | GTK_EXPAND,
                        GTK_FILL | GTK_EXPAND | GTK_SHRINK, 3, 2);

GLOBALS->signal_hslider=gtk_adjustment_new(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
gtk_signal_connect(GTK_OBJECT(GLOBALS->signal_hslider), "value_changed",GTK_SIGNAL_FUNC(service_hslider), NULL);
GLOBALS->hscroll_signalwindow_c_1=gtk_hscrollbar_new(GTK_ADJUSTMENT(GLOBALS->signal_hslider));
gtk_widget_show(GLOBALS->hscroll_signalwindow_c_1);
gtk_table_attach (GTK_TABLE (table), GLOBALS->hscroll_signalwindow_c_1, 0, 10, 9, 10,
                        GTK_FILL,
                        GTK_FILL | GTK_SHRINK, 3, 4);
gtk_widget_show(table);

frame=gtk_frame_new("Signals");
gtk_container_border_width(GTK_CONTAINER(frame),2);

gtk_container_add(GTK_CONTAINER(frame),table);

return(frame);
}
   
/*
 * $Id$
 * $Log$
 * Revision 1.1.1.1.2.6  2007/08/07 03:18:55  kermin
 * Changed to pointer based GLOBAL structure and added initialization function
 *
 * Revision 1.1.1.1.2.5  2007/08/06 03:50:48  gtkwave
 * globals support for ae2, gtk1, cygwin, mingw.  also cleaned up some machine
 * generated structs, etc.
 *
 * Revision 1.1.1.1.2.4  2007/08/05 02:27:23  kermin
 * Semi working global struct
 *
 * Revision 1.1.1.1.2.3  2007/07/31 03:18:01  kermin
 * Merge Complete - I hope
 *
 * Revision 1.1.1.1.2.2  2007/07/28 19:50:40  kermin
 * Merged in the main line
 *
 * Revision 1.1.1.1  2007/05/30 04:27:50  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:17  gtkwave
 * initial release
 *
 */

