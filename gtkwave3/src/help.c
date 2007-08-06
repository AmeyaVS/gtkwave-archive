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
#include "debug.h"
#include "symbol.h"
#include "currenttime.h"

   
/* Add some text to our text widget - this is a callback that is invoked
when our window is realized. We could also force our window to be
realized with gtk_widget_realize, but it would have to be part of
a hierarchy first */


void help_text(char *str)
{
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
gtk_text_buffer_insert (GTK_TEXT_VIEW (GLOBALS.text_help_c_1)->buffer, &GLOBALS.iter_help_c_1, str, -1);
#else
gtk_text_insert (GTK_TEXT (GLOBALS.text_help_c_1), NULL, &GLOBALS.text_help_c_1->style->black, NULL, str, -1);
#endif
}

void help_text_bold(char *str)
{
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
gtk_text_buffer_insert_with_tags (GTK_TEXT_VIEW (GLOBALS.text_help_c_1)->buffer, &GLOBALS.iter_help_c_1,
                                 str, -1, GLOBALS.bold_tag_help_c_1, NULL);
#else
gtk_text_insert (GTK_TEXT (GLOBALS.text_help_c_1), NULL, &GLOBALS.text_help_c_1->style->fg[GTK_STATE_SELECTED], &GLOBALS.text_help_c_1->style->bg[GTK_STATE_SELECTED], str, -1);
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
GLOBALS.text_help_c_1 = gtk_text_view_new ();
gtk_text_buffer_get_start_iter (gtk_text_view_get_buffer(GTK_TEXT_VIEW (GLOBALS.text_help_c_1)), &GLOBALS.iter_help_c_1);
GLOBALS.bold_tag_help_c_1 = gtk_text_buffer_create_tag (GTK_TEXT_VIEW (GLOBALS.text_help_c_1)->buffer, "bold",
                                      "weight", PANGO_WEIGHT_BOLD, NULL);
#else
GLOBALS.text_help_c_1 = gtk_text_new (NULL, NULL);
#endif
gtk_table_attach (GTK_TABLE (table), GLOBALS.text_help_c_1, 0, 14, 0, 1,
		      	GTK_FILL | GTK_EXPAND,
		      	GTK_FILL | GTK_SHRINK | GTK_EXPAND, 0, 0);
gtk_widget_set_usize(GTK_WIDGET(GLOBALS.text_help_c_1), 100, 50); 
gtk_widget_show (GLOBALS.text_help_c_1);

/* And a VScrollbar in the upper right */
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
{
GtkTextViewClass *tc = (GtkTextViewClass*)GTK_OBJECT_GET_CLASS(GTK_OBJECT(GLOBALS.text_help_c_1));

tc->set_scroll_adjustments(GTK_TEXT_VIEW (GLOBALS.text_help_c_1), NULL, NULL);
GLOBALS.vscrollbar_help_c_1 = gtk_vscrollbar_new (GTK_TEXT_VIEW (GLOBALS.text_help_c_1)->vadjustment);
}
#else 
GLOBALS.vscrollbar_help_c_1 = gtk_vscrollbar_new (GTK_TEXT (GLOBALS.text_help_c_1)->vadj);
#endif
gtk_table_attach (GTK_TABLE (table), GLOBALS.vscrollbar_help_c_1, 15, 16, 0, 1,GTK_FILL, GTK_FILL | GTK_SHRINK | GTK_EXPAND, 0, 0);
gtk_widget_show (GLOBALS.vscrollbar_help_c_1);
   
/* Add a handler to put a message in the text widget when it is realized */
gtk_signal_connect (GTK_OBJECT (GLOBALS.text_help_c_1), "realize",
			GTK_SIGNAL_FUNC (help_realize_text), NULL);

   
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(GLOBALS.text_help_c_1), GTK_WRAP_WORD);
#else
gtk_text_set_word_wrap(GTK_TEXT(GLOBALS.text_help_c_1), TRUE);
#endif
return(table);
}
   
/***********************************************************************************/


static void ok_callback(GtkWidget *widget, GtkWidget *nothing)
{
  GLOBALS.helpbox_is_active=0;
  DEBUG(printf("OK\n"));
  gtk_widget_destroy(GLOBALS.window_help_c_2);
}

void helpbox(char *title, int width, char *default_text)
{
    GtkWidget *vbox, *hbox;
    GtkWidget *button1;
    GtkWidget *label, *separator;
    GtkWidget *ctext;

    if(GLOBALS.helpbox_is_active) return;
    GLOBALS.helpbox_is_active=1;

    /* create a new nonmodal window */
    GLOBALS.window_help_c_2 = gtk_window_new(GLOBALS.disable_window_manager ? GTK_WINDOW_POPUP : GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_usize( GTK_WIDGET (GLOBALS.window_help_c_2), width, 400);
    gtk_window_set_title(GTK_WINDOW (GLOBALS.window_help_c_2), title);
    gtk_signal_connect(GTK_OBJECT (GLOBALS.window_help_c_2), "delete_event",(GtkSignalFunc) ok_callback, NULL);

    vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (GLOBALS.window_help_c_2), vbox);
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
    gtk_signal_connect_object (GTK_OBJECT (button1),
                                "realize",
                             (GtkSignalFunc) gtk_widget_grab_default,
                             GTK_OBJECT (button1));

    gtk_widget_show(GLOBALS.window_help_c_2);
}

/*
 * $Id$
 * $Log$
 * Revision 1.1.1.1.2.6  2007/08/05 02:27:20  kermin
 * Semi working global struct
 *
 * Revision 1.1.1.1.2.5  2007/08/01 01:13:56  kermin
 * Fix compile issue related to line bug
 *
 * Revision 1.1.1.1.2.4  2007/07/31 03:18:01  kermin
 * Merge Complete - I hope
 *
 * Revision 1.1.1.1.2.3  2007/07/28 19:50:39  kermin
 * Merged in the main line
 *
 * Revision 1.1.1.1  2007/05/30 04:27:22  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:13  gtkwave
 * initial release
 *
 */

