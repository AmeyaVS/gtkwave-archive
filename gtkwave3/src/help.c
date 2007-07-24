/* 
 * Copyright (c) Tony Bybell 1999.
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

int helpbox_is_active=0;
   
/* Add some text to our text widget - this is a callback that is invoked
when our window is realized. We could also force our window to be
realized with gtk_widget_realize, but it would have to be part of
a hierarchy first */

static GtkWidget *text;
static GtkWidget *vscrollbar;

#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
static GtkTextIter iter;
static GtkTextTag *bold_tag;
#endif


void help_text(char *str)
{
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
gtk_text_buffer_insert (GTK_TEXT_VIEW (text)->buffer, &iter, str, -1);
#else
gtk_text_insert (GTK_TEXT (text), NULL, &text->style->black, NULL, str, -1);
#endif
}

void help_text_bold(char *str)
{
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
gtk_text_buffer_insert_with_tags (GTK_TEXT_VIEW (text)->buffer, &iter,
                                 str, -1, bold_tag, NULL);
#else
gtk_text_insert (GTK_TEXT (text), NULL, &text->style->fg[GTK_STATE_SELECTED], &text->style->bg[GTK_STATE_SELECTED], str, -1);
#endif
}

static void
help_realize_text (GtkWidget *text, gpointer data)
{
help_text("Click on any menu item or button that corresponds to a menu item"
		" for its full description.  Pressing a hotkey for a menu item"
		" is also allowed.");
}
   
/* Create a scrolled text area that displays a "message" */
static GtkWidget *create_help_text (void)
{
GtkWidget *table;

/* Create a table to hold the text widget and scrollbars */
table = gtk_table_new (1, 16, FALSE);
   
/* Put a text widget in the upper left hand corner. Note the use of
* GTK_SHRINK in the y direction */
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
text = gtk_text_view_new ();
gtk_text_buffer_get_start_iter (gtk_text_view_get_buffer(GTK_TEXT_VIEW (text)), &iter);
bold_tag = gtk_text_buffer_create_tag (GTK_TEXT_VIEW (text)->buffer, "bold",
                                      "weight", PANGO_WEIGHT_BOLD, NULL);
#else
text = gtk_text_new (NULL, NULL);
#endif
gtk_table_attach (GTK_TABLE (table), text, 0, 14, 0, 1,
		      	GTK_FILL | GTK_EXPAND,
		      	GTK_FILL | GTK_SHRINK | GTK_EXPAND, 0, 0);
gtk_widget_set_usize(GTK_WIDGET(text), 100, 50); 
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
gtk_signal_connect (GTK_OBJECT (text), "realize",GTK_SIGNAL_FUNC (help_realize_text), NULL);
   
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text), GTK_WRAP_WORD);
#else
gtk_text_set_word_wrap(GTK_TEXT(text), TRUE);
#endif
return(table);
}
   
/***********************************************************************************/

static GtkWidget *window;

static void ok_callback(GtkWidget *widget, GtkWidget *nothing)
{
  helpbox_is_active=0;
  DEBUG(printf("OK\n"));
  gtk_widget_destroy(window);
}

void helpbox(char *title, int width, char *default_text)
{
    GtkWidget *vbox, *hbox;
    GtkWidget *button1;
    GtkWidget *label, *separator;
    GtkWidget *ctext;

    if(helpbox_is_active) return;
    helpbox_is_active=1;

    /* create a new nonmodal window */
    window = gtk_window_new(disable_window_manager ? GTK_WINDOW_POPUP : GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_usize( GTK_WIDGET (window), width, 400);
    gtk_window_set_title(GTK_WINDOW (window), title);
    gtk_signal_connect(GTK_OBJECT (window), "delete_event",(GtkSignalFunc) ok_callback, NULL);

    vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (window), vbox);
    gtk_widget_show (vbox);

    label=gtk_label_new(default_text);
    gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
    gtk_widget_show (label);

    separator = gtk_hseparator_new ();
    gtk_box_pack_start (GTK_BOX (vbox), separator, FALSE, TRUE, 0);
    gtk_widget_show (separator);

    ctext=create_help_text();
    gtk_box_pack_start (GTK_BOX (vbox), ctext, TRUE, TRUE, 0);
    gtk_widget_show (ctext);

    separator = gtk_hseparator_new ();
    gtk_box_pack_start (GTK_BOX (vbox), separator, FALSE, TRUE, 0);
    gtk_widget_show (separator);

    hbox = gtk_hbox_new (FALSE, 1);
    gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
    gtk_widget_show (hbox);

    button1 = gtk_button_new_with_label ("Close Help");
    gtk_widget_set_usize(button1, 100, -1);
    gtk_signal_connect(GTK_OBJECT (button1), "clicked",
			       GTK_SIGNAL_FUNC(ok_callback),
			       NULL);
    gtk_widget_show (button1);
    gtk_container_add (GTK_CONTAINER (hbox), button1);
    GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);
    gtk_signal_connect_object (GTK_OBJECT (button1),"realize",(GtkSignalFunc) gtk_widget_grab_default,GTK_OBJECT (button1));

    gtk_widget_show(window);
}

/*
 * $Id$
 * $Log$
 * Revision 1.1.1.1.2.1  2007/07/09 11:47:59  kermin
 * Commit against July 9th deadline.  Devtool to move globals/statics to
 * unified struct mostly complete
 *
 * Revision 1.1.1.1  2007/05/30 04:27:22  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:13  gtkwave
 * initial release
 *
 */

