/* 
 * Copyright (c) Tony Bybell 1999-2005.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <config.h>
#include <gtk/gtk.h>
#include "debug.h"
#include "symbol.h"
#include "currenttime.h"
#include "fgetdynamic.h"

/* only for use locally */
struct wave_logfile_lines_t
{
struct wave_logfile_lines_t *next;
char *text;
};


char *fontname_logfile = NULL;

/* Add some text to our text widget - this is a callback that is invoked
when our window is realized. We could also force our window to be
realized with gtk_widget_realize, but it would have to be part of
a hierarchy first */

static GdkFont *font = NULL;

#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
static GtkTextIter iter;
static GtkTextTag *bold_tag = NULL;
static GtkTextTag *mono_tag = NULL;
static GtkTextTag *size_tag = NULL; 
#endif


void log_text(GtkWidget *text, GdkFont *font, char *str)
{
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
gtk_text_buffer_insert_with_tags (GTK_TEXT_VIEW (text)->buffer, &iter,
                                 str, -1, mono_tag, size_tag, NULL);
#else
gtk_text_insert (GTK_TEXT (text), font, &text->style->black, NULL, str, -1);
#endif
}

void log_text_bold(GtkWidget *text, GdkFont *font, char *str)
{
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
gtk_text_buffer_insert_with_tags (GTK_TEXT_VIEW (text)->buffer, &iter,
                                 str, -1, bold_tag, mono_tag, size_tag, NULL);
#else
gtk_text_insert (GTK_TEXT (text), font, &text->style->fg[GTK_STATE_SELECTED], &text->style->bg[GTK_STATE_SELECTED], str, -1);
#endif
}

static void
log_realize_text (GtkWidget *text, gpointer data)
{
/* nothing for now */
}


static void center_op(void)
{
TimeType middle=0, width;

if((tims.marker<0)||(tims.marker<tims.first)||(tims.marker>tims.last))
	{
        if(tims.end>tims.last) tims.end=tims.last;
        middle=(tims.start/2)+(tims.end/2);
        if((tims.start&1)&&(tims.end&1)) middle++;
        }   
        else
        {
        middle=tims.marker;
        }

width=(TimeType)(((gdouble)wavewidth)*nspx);
tims.start=time_trunc(middle-(width/2));
if(tims.start+width>tims.last) tims.start=time_trunc(tims.last-width);
if(tims.start<tims.first) tims.start=tims.first;
GTK_ADJUSTMENT(wave_hslider)->value=tims.timecache=tims.start;

fix_wavehadj();

gtk_signal_emit_by_name (GTK_OBJECT (GTK_ADJUSTMENT(wave_hslider)), "changed"); /* force zoom update */
gtk_signal_emit_by_name (GTK_OBJECT (GTK_ADJUSTMENT(wave_hslider)), "value_changed"); /* force zoom update */
}



static gboolean
button_release_event (GtkWidget *text, GdkEventButton *event)
{
gchar *sel;

#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
GtkTextIter start;
GtkTextIter end;
                        
if (gtk_text_buffer_get_selection_bounds (GTK_TEXT_VIEW(text)->buffer,
                                         &start, &end))
       {
       if(gtk_text_iter_compare (&start, &end) < 0)
               {
               sel = gtk_text_buffer_get_text(GTK_TEXT_VIEW(text)->buffer,   
                                              &start, &end, FALSE);

               if(sel)
                       {
                       if(strlen(sel)&&(sel[0]>='0')&&(sel[0]<='9'))
                               {
                               TimeType tm = unformat_time(sel, time_dimension);
                               if((tm >= tims.first) && (tm <= tims.last))
                                       {
                                       tims.lmbcache = -1;
                                       update_markertime(tims.marker = tm);
                                       center_op();
                                       signalarea_configure_event(signalarea, NULL);
                                       wavearea_configure_event(wavearea, NULL);
                                       update_markertime(tims.marker = tm); /* centering problem in GTK2 */
                                       }
                               }
                       g_free(sel);
                       }
               }
       }
#else

#ifndef WAVE_USE_GTK2
GtkEditable *oe = GTK_EDITABLE(&GTK_TEXT(text)->editable);
GtkTextClass *tc = (GtkTextClass *) ((GtkObject*) (GTK_OBJECT(text)))->klass;
GtkEditableClass *oec = &tc->parent_class;
#else
GtkOldEditable *oe = GTK_OLD_EDITABLE(&GTK_TEXT(text)->old_editable);
GtkOldEditableClass *oec = GTK_OLD_EDITABLE_GET_CLASS(oe);
#endif

if(oe->has_selection)
	{
	if(oec->get_chars)
		{
	 	sel = oec->get_chars(oe, oe->selection_start_pos, oe->selection_end_pos);
	
		if(sel)
			{
			if(strlen(sel)&&(sel[0]>='0')&&(sel[0]<='9'))
				{
				TimeType tm = unformat_time(sel, time_dimension);
				if((tm >= tims.first) && (tm <= tims.last))
					{
					tims.lmbcache = -1;
				        update_markertime(tims.marker = tm);
					center_op();
					signalarea_configure_event(signalarea, NULL);
        				wavearea_configure_event(wavearea, NULL);
				        update_markertime(tims.marker = tm); /* centering problem in GTK2 */
					}
				}
			g_free(sel);
			}
		}
	}

#endif

return(FALSE); /* call remaining handlers... */
}
   
/* Create a scrolled text area that displays a "message" */
static GtkWidget *create_log_text (GtkWidget **textpnt)
{
GtkWidget *text;
GtkWidget *table;
GtkWidget *vscrollbar;

/* Create a table to hold the text widget and scrollbars */
table = gtk_table_new (1, 16, FALSE);

/* Put a text widget in the upper left hand corner. Note the use of
* GTK_SHRINK in the y direction */
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
text = gtk_text_view_new ();
gtk_text_buffer_get_start_iter (gtk_text_view_get_buffer(GTK_TEXT_VIEW (text)), &iter);
bold_tag = gtk_text_buffer_create_tag (GTK_TEXT_VIEW (text)->buffer, "bold",
                                      "weight", PANGO_WEIGHT_BOLD, NULL);
mono_tag = gtk_text_buffer_create_tag (GTK_TEXT_VIEW (text)->buffer, "monospace", "family", "monospace", NULL);

size_tag = gtk_text_buffer_create_tag (GTK_TEXT_VIEW (text)->buffer, "fsiz",
			      "size", (use_big_fonts ? 18 : 10) * PANGO_SCALE, NULL);


#else
text = gtk_text_new (NULL, NULL);
#endif
*textpnt = text;
gtk_table_attach (GTK_TABLE (table), text, 0, 14, 0, 1,
                        GTK_FILL | GTK_EXPAND,
                        GTK_FILL | GTK_SHRINK | GTK_EXPAND, 0, 0);
gtk_widget_set_usize(GTK_WIDGET(text), 100, 100);
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
gtk_text_view_set_editable(GTK_TEXT_VIEW(text), TRUE);   
#else
gtk_text_set_editable(GTK_TEXT(text), TRUE);
#endif
gtk_widget_show (text);

/* And a VScrollbar in the upper right */
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
{
GtkTextViewClass *tc = (GtkTextViewClass*)GTK_OBJECT_GET_CLASS(GTK_OBJECT(text));
 
tc->set_scroll_adjustments(GTK_TEXT_VIEW (text), NULL, NULL);
vscrollbar = gtk_vscrollbar_new (GTK_TEXT_VIEW (text)->vadjustment);  
}
#else
vscrollbar = gtk_vscrollbar_new (GTK_TEXT (text)->vadj);
#endif
gtk_table_attach (GTK_TABLE (table), vscrollbar, 15, 16, 0, 1,
                        GTK_FILL, GTK_FILL | GTK_SHRINK | GTK_EXPAND, 0, 0);
gtk_widget_show (vscrollbar);

/* Add a handler to put a message in the text widget when it is realized */
gtk_signal_connect (GTK_OBJECT (text), "realize",
                        GTK_SIGNAL_FUNC (log_realize_text), NULL);

gtk_signal_connect(GTK_OBJECT(text), "button_release_event",
                       GTK_SIGNAL_FUNC(button_release_event), NULL);

#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text), GTK_WRAP_CHAR);
#else
gtk_text_set_word_wrap(GTK_TEXT(text), FALSE);
gtk_text_set_line_wrap(GTK_TEXT(text), TRUE); 
#endif
return(table);
}
   
/***********************************************************************************/

static void ok_callback(GtkWidget *widget, GtkWidget *cached_window)
{
  DEBUG(printf("WM destroy\n"));
  gtk_widget_destroy(cached_window);
}

static void destroy_callback(GtkWidget *widget, GtkWidget *nothing)
{
  DEBUG(printf("OK\n"));
  gtk_widget_destroy(widget);
}

void logbox(char *title, int width, char *default_text)
{
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *hbox, *button1;
    GtkWidget *label, *separator;
    GtkWidget *ctext;
    GtkWidget *text;
    FILE *handle;
    struct wave_logfile_lines_t *wlog_head=NULL, *wlog_curr=NULL;
    int wlog_size = 0;

    handle = fopen(default_text, "rb");
    if(!handle)
	{
	char *buf = malloc_2(strlen(default_text)+128);
	sprintf(buf, "Could not open logfile '%s'\n", default_text);
	status_text(buf);
	free_2(buf);
	return;
	}

#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
/* nothing */
#else
    if(!font) 
	{
	if(fontname_logfile)
		{
		font=gdk_font_load(fontname_logfile);
		}

	if(!font)
		{
#ifndef __CYGWIN__
		font=gdk_font_load(use_big_fonts 
				? "-*-courier-*-r-*-*-18-*-*-*-*-*-*-*"
				: "-*-courier-*-r-*-*-10-*-*-*-*-*-*-*");
#else
		font=gdk_font_load(use_big_fonts 
				? "-misc-fixed-*-*-*-*-18-*-*-*-*-*-*-*"
				: "-misc-fixed-*-*-*-*-10-*-*-*-*-*-*-*");

#endif
		}
	}
#endif

    /* create a new nonmodal window */
    window = gtk_window_new(disable_window_manager ? GTK_WINDOW_POPUP : GTK_WINDOW_TOPLEVEL);
    if(use_big_fonts || fontname_logfile)
	{
    	gtk_widget_set_usize( GTK_WIDGET (window), width*1.8, 600);
	}
	else
	{
    	gtk_widget_set_usize( GTK_WIDGET (window), width, 400);
	}
    gtk_window_set_title(GTK_WINDOW (window), title);

    gtk_signal_connect(GTK_OBJECT (window), "delete_event",
                       (GtkSignalFunc) destroy_callback, NULL);

    vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (window), vbox);
    gtk_widget_show (vbox);

    label=gtk_label_new(default_text);
    gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
    gtk_widget_show (label);

    separator = gtk_hseparator_new ();
    gtk_box_pack_start (GTK_BOX (vbox), separator, FALSE, TRUE, 0);
    gtk_widget_show (separator);

    ctext=create_log_text(&text);
    gtk_box_pack_start (GTK_BOX (vbox), ctext, TRUE, TRUE, 0);
    gtk_widget_show (ctext);

    separator = gtk_hseparator_new ();
    gtk_box_pack_start (GTK_BOX (vbox), separator, FALSE, TRUE, 0);
    gtk_widget_show (separator);

    hbox = gtk_hbox_new (FALSE, 1);
    gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
    gtk_widget_show (hbox);

    button1 = gtk_button_new_with_label ("Close Logfile");
    gtk_widget_set_usize(button1, 100, -1);
    gtk_signal_connect(GTK_OBJECT (button1), "clicked",
                               GTK_SIGNAL_FUNC(ok_callback),
                               window);
    gtk_widget_show (button1);
    gtk_container_add (GTK_CONTAINER (hbox), button1);  
    GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);
    gtk_signal_connect_object (GTK_OBJECT (button1),
                                "realize",
                             (GtkSignalFunc) gtk_widget_grab_default,
                             GTK_OBJECT (button1));

    gtk_widget_show(window);

    log_text_bold(text, NULL, "Click-select");
    log_text(text, NULL, " on numbers to jump to that time value in the wave viewer.\n");
    log_text(text, NULL, " \n");

    while(!feof(handle))
	{
	char *pnt = fgetmalloc(handle);
	if(pnt)
		{
		struct wave_logfile_lines_t *w = calloc_2(1, sizeof(struct wave_logfile_lines_t));

		wlog_size += (fgetmalloc_len+1);
		w->text = pnt;
		if(!wlog_curr) { wlog_head = wlog_curr = w; } else { wlog_curr->next = w; wlog_curr = w; }
		}
	}

    if(wlog_curr)
	{
	struct wave_logfile_lines_t *w = wlog_head;
	struct wave_logfile_lines_t *wt;
	char *pnt = malloc_2(wlog_size + 1);
	char *pnt2 = pnt;

	while(w)
		{
		int len = strlen(w->text);
		memcpy(pnt2, w->text, len);
		pnt2 += len;
		*pnt2 = '\n';
		pnt2++;

		free_2(w->text);
		wt = w;
		w = w->next;
		free_2(wt);
		}
	wlog_head = wlog_curr = NULL;
	*pnt2 = 0;
	log_text(text, font, pnt);
	free_2(pnt);
	}

    fclose(handle);
}

/*
 * $Id$
 * $Log$
 * Revision 1.2  2007/04/20 02:08:13  gtkwave
 * initial release
 *
 */

