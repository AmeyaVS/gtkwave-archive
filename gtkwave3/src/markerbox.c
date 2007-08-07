/* 
 * Copyright (c) Tony Bybell 1999.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include "globals.h"
#include <config.h>
#include <gtk/gtk.h>
#include <string.h>
#include "gtk12compat.h"
#include "debug.h"
#include "analyzer.h"
#include "currenttime.h"



static void enter_callback(GtkWidget *widget, gpointer which)
{
GtkWidget *entry;
TimeType *modify;
TimeType temp;
G_CONST_RETURN gchar *entry_text;
char buf[49];
int len, i;
int ent_idx;

ent_idx = ((int) (((long) which) & 31L)) % 26;
 
entry=GLOBALS->entries_markerbox_c_1[ent_idx];

entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
if(!(len=strlen(entry_text))) goto failure;

temp=unformat_time(entry_text, GLOBALS->time_dimension);
if((temp<GLOBALS->tims.start)||(temp>GLOBALS->tims.last)) goto failure;

for(i=0;i<26;i++)
	{
	if(temp==GLOBALS->shadow_markers_markerbox_c_1[i]) goto failure;
	}

reformat_time(buf, temp, GLOBALS->time_dimension);
gtk_entry_set_text (GTK_ENTRY (entry), buf);

GLOBALS->shadow_markers_markerbox_c_1[ent_idx]=temp;
GLOBALS->dirty_markerbox_c_1=1;
gtk_entry_select_region (GTK_ENTRY (entry),
			     0, GTK_ENTRY(entry)->text_length);
return;

failure:
modify=(TimeType *)which;
if(GLOBALS->shadow_markers_markerbox_c_1[ent_idx]==-1)
	{
	sprintf(buf,"<None>");
	}
	else
	{
	reformat_time(buf, GLOBALS->shadow_markers_markerbox_c_1[ent_idx], GLOBALS->time_dimension);
	}
gtk_entry_set_text (GTK_ENTRY (entry), buf);
}

static void ok_callback(GtkWidget *widget, GtkWidget *nothing)
{
if(GLOBALS->dirty_markerbox_c_1)
	{
	int i;
	for(i=0;i<26;i++) GLOBALS->named_markers[i]=GLOBALS->shadow_markers_markerbox_c_1[i];
        MaxSignalLength();
        signalarea_configure_event(GLOBALS->signalarea, NULL);
        wavearea_configure_event(GLOBALS->wavearea, NULL);
	}

  gtk_grab_remove(GLOBALS->window_markerbox_c_4);
  gtk_widget_destroy(GLOBALS->window_markerbox_c_4);

  GLOBALS->cleanup_markerbox_c_4();
}

static void destroy_callback(GtkWidget *widget, GtkWidget *nothing)
{
  gtk_grab_remove(GLOBALS->window_markerbox_c_4);
  gtk_widget_destroy(GLOBALS->window_markerbox_c_4);
}

void markerbox(char *title, GtkSignalFunc func)
{
    GtkWidget *entry;
    GtkWidget *vbox, *hbox, *vbox_g, *label;
    GtkWidget *button1, *button2, *scrolled_win, *frame, *separator;
    GtkWidget *table;
    char labtitle[2]={0,0};
    int i;

    GLOBALS->cleanup_markerbox_c_4=func;
    GLOBALS->dirty_markerbox_c_1=0;

    for(i=0;i<26;i++) GLOBALS->shadow_markers_markerbox_c_1[i]=GLOBALS->named_markers[i];

    /* create a new modal window */
    GLOBALS->window_markerbox_c_4 = gtk_window_new(GLOBALS->disable_window_manager ? GTK_WINDOW_POPUP : GTK_WINDOW_TOPLEVEL);
    gtk_grab_add(GLOBALS->window_markerbox_c_4);
    gtk_window_set_title(GTK_WINDOW (GLOBALS->window_markerbox_c_4), title);
    gtk_signal_connect(GTK_OBJECT (GLOBALS->window_markerbox_c_4), "delete_event",(GtkSignalFunc) destroy_callback, NULL);

    vbox = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox);

    vbox_g = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox_g);

    table = gtk_table_new (256, 1, FALSE);
    gtk_widget_show (table);

    gtk_table_attach (GTK_TABLE (table), vbox, 0, 1, 0, 255,
                        GTK_FILL | GTK_EXPAND,
                        GTK_FILL | GTK_EXPAND | GTK_SHRINK, 1, 1);

    frame = gtk_frame_new (NULL);
    gtk_container_border_width (GTK_CONTAINER (frame), 3);
    gtk_widget_show(frame);

    scrolled_win = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_set_usize( GTK_WIDGET (scrolled_win), -1, 300);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_win),
                                      GTK_POLICY_AUTOMATIC,
                                      GTK_POLICY_AUTOMATIC);
    gtk_widget_show(scrolled_win);
    gtk_container_add (GTK_CONTAINER (frame), scrolled_win);
    gtk_container_add (GTK_CONTAINER (vbox), frame);

    for(i=0;i<26;i++)
    {
    char buf[49];

    if(i)
	{
    	separator = gtk_hseparator_new ();
        gtk_widget_show (separator);
        gtk_box_pack_start (GTK_BOX (vbox_g), separator, TRUE, TRUE, 0);
	}

    labtitle[0]='A'+i;
    label=gtk_label_new(labtitle);
    gtk_widget_show (label);
    gtk_box_pack_start (GTK_BOX (vbox_g), label, TRUE, TRUE, 0);

    GLOBALS->entries_markerbox_c_1[i]=entry = gtk_entry_new_with_max_length (48);
    gtk_signal_connect(GTK_OBJECT(entry), "activate",
		       GTK_SIGNAL_FUNC(enter_callback),
		       (void *)((long) i));
    if(GLOBALS->shadow_markers_markerbox_c_1[i]==-1)
	{
	sprintf(buf,"<None>");
	}
	else
	{
	reformat_time(buf, GLOBALS->shadow_markers_markerbox_c_1[i], GLOBALS->time_dimension);
	}

    gtk_entry_set_text (GTK_ENTRY (entry), buf);
    gtk_box_pack_start (GTK_BOX (vbox_g), entry, TRUE, TRUE, 0);
    gtk_widget_show (entry);
    }

    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_win), vbox_g);

    hbox = gtk_hbox_new (FALSE, 1);
    gtk_widget_show (hbox);

    gtk_table_attach (GTK_TABLE (table), hbox, 0, 1, 255, 256,
                        GTK_FILL | GTK_EXPAND,
                        GTK_FILL | GTK_EXPAND | GTK_SHRINK, 1, 1);

    button1 = gtk_button_new_with_label ("OK");
    gtk_widget_set_usize(button1, 100, -1);
    gtk_signal_connect(GTK_OBJECT (button1), "clicked",
			       GTK_SIGNAL_FUNC(ok_callback),
			       NULL);
    gtk_widget_show (button1);
    gtk_container_add (GTK_CONTAINER (hbox), button1);
    GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);
    gtk_signal_connect_object (GTK_OBJECT (button1),
                                "realize",
                             (GtkSignalFunc) gtk_widget_grab_default,
                             GTK_OBJECT (button1));


    button2 = gtk_button_new_with_label ("Cancel");
    gtk_widget_set_usize(button2, 100, -1);
    gtk_signal_connect(GTK_OBJECT (button2), "clicked",
			       GTK_SIGNAL_FUNC(destroy_callback),
			       NULL);
    GTK_WIDGET_SET_FLAGS (button2, GTK_CAN_DEFAULT);
    gtk_widget_show (button2);
    gtk_container_add (GTK_CONTAINER (hbox), button2);

    gtk_container_add (GTK_CONTAINER (GLOBALS->window_markerbox_c_4), table); /* need this table to keep ok/cancel buttons from stretching! */
    gtk_widget_show(GLOBALS->window_markerbox_c_4);
}

/*
 * $Id$
 * $Log$
 * Revision 1.1.1.1.2.5  2007/08/06 03:50:47  gtkwave
 * globals support for ae2, gtk1, cygwin, mingw.  also cleaned up some machine
 * generated structs, etc.
 *
 * Revision 1.1.1.1.2.4  2007/08/05 02:27:21  kermin
 * Semi working global struct
 *
 * Revision 1.1.1.1.2.3  2007/07/31 03:18:01  kermin
 * Merge Complete - I hope
 *
 * Revision 1.1.1.1.2.2  2007/07/28 19:50:40  kermin
 * Merged in the main line
 *
 * Revision 1.1.1.1  2007/05/30 04:27:35  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:13  gtkwave
 * initial release
 *
 */

