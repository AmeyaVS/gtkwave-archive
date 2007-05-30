/* 
 * Copyright (c) Tony Bybell 1999-2004.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <config.h>
#include <gtk/gtk.h>
#include "splay.h"

void bwmaketree(void);
void bwlogbox(char *title, int width, ds_Tree *t, int display_mode);


static ds_Tree *selectedtree=NULL;

static int is_active=0;
GtkCTree *ctree_main=NULL;

static void select_row_callback(GtkWidget *widget, gint row, gint column,
        GdkEventButton *event, gpointer data)
{
ds_Tree *t=(ds_Tree *)gtk_clist_get_row_data(GTK_CLIST(ctree_main), row);
selectedtree=t;

if(t->filename)
        {
	/*
	printf("%s\n", t->fullname);
        printf("%s -> '%s' %d-%d\n\n", t->item, t->filename, t->s_line, t->e_line);
	*/

	bwlogbox(t->fullname, 640 + 8*8, t, 0);
        }
        else
        {
	/*
	printf("%s\n", t->fullname);
        printf("%s -> *MISSING*\n\n", t->item);
	*/
        }   
}

static void unselect_row_callback(GtkWidget *widget, gint row, gint column,
        GdkEventButton *event, gpointer data)
{
/* ds_Tree *t=(ds_Tree *)gtk_clist_get_row_data(GTK_CLIST(ctree_main), row); */
selectedtree=NULL;
}




int treebox_is_active(void)
{
return(is_active);
}


/***************************************************************************/

static GtkWidget *window;
static GtkWidget *tree;
static GtkSignalFunc cleanup;


static void destroy_callback(GtkWidget *widget, GtkWidget *nothing)
{
  is_active=0;
  gtk_widget_destroy(window);
  gtk_main_quit();
}



/*
 * mainline..
 */
void treebox(char *title, GtkSignalFunc func)
{
    GtkWidget *scrolled_win;
    GtkWidget *hbox;
    GtkWidget *button5;
    GtkWidget *frame2, *frameh;
    GtkWidget *table;
    GtkCList  *clist;

    if(is_active) 
	{
	gdk_window_raise(window->window);
	return;
	}

    is_active=1;
    cleanup=func;

    /* create a new modal window */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW (window), title);
    gtk_signal_connect(GTK_OBJECT (window), "delete_event",
                       (GtkSignalFunc) destroy_callback, NULL);
    gtk_widget_set_usize(GTK_WIDGET(window), 640, 600);


    table = gtk_table_new (256, 1, FALSE);
    gtk_widget_show (table);

    frame2 = gtk_frame_new (NULL);
    gtk_container_border_width (GTK_CONTAINER (frame2), 3);
    gtk_widget_show(frame2);

    gtk_table_attach (GTK_TABLE (table), frame2, 0, 1, 0, 255,
                        GTK_FILL | GTK_EXPAND,
                        GTK_FILL | GTK_EXPAND | GTK_SHRINK, 1, 1);

    tree=gtk_ctree_new(1,0);
    ctree_main=GTK_CTREE(tree);
    gtk_clist_set_column_auto_resize (GTK_CLIST (tree), 0, TRUE);
    gtk_widget_show(tree);

    clist=GTK_CLIST(tree);
    gtk_signal_connect_object (GTK_OBJECT (clist), "select_row",
                               GTK_SIGNAL_FUNC(select_row_callback),
                               NULL);
    gtk_signal_connect_object (GTK_OBJECT (clist), "unselect_row",
                               GTK_SIGNAL_FUNC(unselect_row_callback),
                               NULL);

    gtk_clist_freeze(clist);
    gtk_clist_clear(clist);

    bwmaketree();
    gtk_clist_thaw(clist);
    selectedtree=NULL;

    scrolled_win = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_set_usize( GTK_WIDGET (scrolled_win), -1, 300);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_win),
                                      GTK_POLICY_AUTOMATIC,
                                      GTK_POLICY_AUTOMATIC);
    gtk_widget_show(scrolled_win);
    gtk_container_add (GTK_CONTAINER (scrolled_win), GTK_WIDGET (tree));
    gtk_container_add (GTK_CONTAINER (frame2), scrolled_win);


    frameh = gtk_frame_new (NULL);
    gtk_container_border_width (GTK_CONTAINER (frameh), 3);
    gtk_widget_show(frameh);
    gtk_table_attach (GTK_TABLE (table), frameh, 0, 1, 255, 256,
                        GTK_FILL | GTK_EXPAND,
                        GTK_FILL | GTK_EXPAND | GTK_SHRINK, 1, 1);


    hbox = gtk_hbox_new (FALSE, 1);
    gtk_widget_show (hbox);

    button5 = gtk_button_new_with_label (" Exit ");
    gtk_container_border_width (GTK_CONTAINER (button5), 3);
    gtk_signal_connect_object (GTK_OBJECT (button5), "clicked",
			       GTK_SIGNAL_FUNC(destroy_callback),
			       GTK_OBJECT (window));
    gtk_widget_show (button5);
    gtk_box_pack_start (GTK_BOX (hbox), button5, TRUE, TRUE, 0);

    gtk_container_add (GTK_CONTAINER (frameh), hbox);
    gtk_container_add (GTK_CONTAINER (window), table);

    gtk_widget_show(window);
}



