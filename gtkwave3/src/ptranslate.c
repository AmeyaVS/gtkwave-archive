/* 
 * Copyright (c) Tony Bybell 2005-6.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include "globals.h"
#include <config.h>
#include <gtk/gtk.h>
#include "gtk12compat.h"
#include "symbol.h"
#include "ptranslate.h"
#include "pipeio.h"
#include "debug.h"

#ifdef _MSC_VER
#define strcasecmp _stricmp
#endif

void init_proctrans_data(void)
{
int i;

if(!GLOBALS->procsel_filter) { GLOBALS->procsel_filter = calloc_2(FILE_FILTER_MAX+1, sizeof(char *)); }
if(!GLOBALS->proc_filter) { GLOBALS->proc_filter = calloc_2(FILE_FILTER_MAX+1, sizeof(struct pipe_ctx *)); }

for(i=0;i<PROC_FILTER_MAX+1;i++)
	{
	GLOBALS->procsel_filter[i] = NULL;
	GLOBALS->proc_filter[i] = NULL;
	}
}

void remove_all_proc_filters(void)
{
int i;

for(i=1;i<PROC_FILTER_MAX+1;i++)
	{
	if(GLOBALS->proc_filter[i])
		{
		pipeio_destroy(GLOBALS->proc_filter[i]);
		GLOBALS->proc_filter[i] = NULL;
		}

	if(GLOBALS->procsel_filter[i])
		{
		free_2(GLOBALS->procsel_filter[i]);
		GLOBALS->procsel_filter[i] = NULL;
		}
	}
}

#if defined __MINGW32__ || defined _MSC_VER

void ptrans_searchbox(char *title) { }
void install_proc_filter(int which) { } 
void set_current_translate_proc(char *name) { }

#else

static void regen_display(void)
{
GLOBALS->signalwindow_width_dirty=1;
MaxSignalLength();
signalarea_configure_event(GLOBALS->signalarea, NULL);
wavearea_configure_event(GLOBALS->wavearea, NULL);
}


void remove_proc_filter(int which, int regen)
{
if(GLOBALS->proc_filter[which])
	{
	pipeio_destroy(GLOBALS->proc_filter[which]);
	GLOBALS->proc_filter[which] = NULL;

	if(regen)
	        {
		regen_display();
	        }
	}
}


void load_proc_filter(int which, char *name)
{
FILE *f = fopen(name, "rb");
if(!f)
	{
	/* looking at permissions might not be applicable on AFS! */
	status_text("Could not open filter process!\n");
	return;
	}
	else
	{
	fclose(f);
	}

remove_proc_filter(which, 0); /* should never happen from GUI, but possible from save files or other weirdness */

GLOBALS->proc_filter[which] = pipeio_create(name);
}

void install_proc_filter(int which)
{
int found = 0;

if(GLOBALS->traces.first)  
        {
        Trptr t = GLOBALS->traces.first;
        while(t)
                {
                if(t->flags&TR_HIGHLIGHT)
                        {
                        if(!(t->flags&(TR_BLANK|TR_ANALOG_BLANK_STRETCH)))
                                {
				t->f_filter = 0;
                                t->p_filter = which;
				if(!which)
					{
					t->flags &= (~(TR_FTRANSLATED|TR_PTRANSLATED));
					}
					else
					{
					t->flags |= TR_PTRANSLATED;
					}
                                found = 1;
                                }
                        }
                t=t->t_next;
                }
        }

if(found)
	{
	regen_display();
	}
}

/************************************************************************/



static void destroy_callback(GtkWidget *widget, GtkWidget *nothing)
{
GLOBALS->is_active_ptranslate_c_2=0;
gtk_widget_destroy(GLOBALS->window_ptranslate_c_5);
GLOBALS->window_ptranslate_c_5 = NULL;
}

static void ok_callback(GtkWidget *widget, GtkWidget *nothing)
{
install_proc_filter(GLOBALS->current_filter_ptranslate_c_1);
destroy_callback(widget, nothing);
}

static void select_row_callback(GtkWidget *widget, gint row, gint column,
	GdkEventButton *event, gpointer data)
{
GLOBALS->current_filter_ptranslate_c_1 = row + 1;
}

static void unselect_row_callback(GtkWidget *widget, gint row, gint column,
	GdkEventButton *event, gpointer data)
{
GLOBALS->current_filter_ptranslate_c_1 = 0; /* none */
}


static void add_filter_callback_2(GtkWidget *widget, GtkWidget *nothing)
{
int i;
GtkCList *cl;

if(!GLOBALS->filesel_ok) { return; }

if(*GLOBALS->fileselbox_text)
	{
	for(i=0;i<GLOBALS->num_proc_filters;i++)
		{
		if(GLOBALS->procsel_filter[i])
			{
			if(!strcmp(GLOBALS->procsel_filter[i], *GLOBALS->fileselbox_text)) 
				{
				status_text("Filter already imported.\n");
				return;
				}
			}
		}
	}

GLOBALS->num_proc_filters++;
load_proc_filter(GLOBALS->num_proc_filters, *GLOBALS->fileselbox_text);
if(GLOBALS->proc_filter[GLOBALS->num_proc_filters])
	{
	if(GLOBALS->procsel_filter[GLOBALS->num_proc_filters]) free_2(GLOBALS->procsel_filter[GLOBALS->num_proc_filters]);
	GLOBALS->procsel_filter[GLOBALS->num_proc_filters] = malloc_2(strlen(*GLOBALS->fileselbox_text) + 1);
	strcpy(GLOBALS->procsel_filter[GLOBALS->num_proc_filters], *GLOBALS->fileselbox_text);

	cl=GTK_CLIST(GLOBALS->clist_ptranslate_c_2);
	gtk_clist_freeze(cl);
	gtk_clist_append(cl,(gchar **)&(GLOBALS->procsel_filter[GLOBALS->num_proc_filters]));

	gtk_clist_set_column_width(cl,0,gtk_clist_optimal_column_width(cl,0));
	gtk_clist_thaw(cl);
	}
	else
	{
	GLOBALS->num_proc_filters--;
	}
}

static void add_filter_callback(GtkWidget *widget, GtkWidget *nothing)
{
if(GLOBALS->num_proc_filters == PROC_FILTER_MAX)
	{
	status_text("Max number of process filters installed already.\n");
	return;
	}

fileselbox("Select Filter Process",&GLOBALS->fcurr_ptranslate_c_1,GTK_SIGNAL_FUNC(add_filter_callback_2), GTK_SIGNAL_FUNC(NULL),"*", 0);
}

/*
 * mainline..
 */
void ptrans_searchbox(char *title)
{
    int i;

    GtkWidget *scrolled_win;
    GtkWidget *vbox1, *hbox, *hbox0;
    GtkWidget *button1, *button5, *button6;
    gchar *titles[]={"Process Filter Select"};
    GtkWidget *frame2, *frameh, *frameh0;
    GtkWidget *table;
    GtkTooltips *tooltips;

    if(GLOBALS->is_active_ptranslate_c_2) 
	{
	gdk_window_raise(GLOBALS->window_ptranslate_c_5->window);
	return;
	}

    GLOBALS->is_active_ptranslate_c_2=1;
    GLOBALS->current_filter_ptranslate_c_1 = 0;

    /* create a new modal window */
    GLOBALS->window_ptranslate_c_5 = gtk_window_new(GLOBALS->disable_window_manager ? GTK_WINDOW_POPUP : GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW (GLOBALS->window_ptranslate_c_5), title);
    gtk_signal_connect(GTK_OBJECT (GLOBALS->window_ptranslate_c_5), "delete_event",(GtkSignalFunc) destroy_callback, NULL);

    tooltips=gtk_tooltips_new_2();

    table = gtk_table_new (256, 1, FALSE);
    gtk_widget_show (table);

    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_container_border_width (GTK_CONTAINER (vbox1), 3);
    gtk_widget_show (vbox1);


    frame2 = gtk_frame_new (NULL);
    gtk_container_border_width (GTK_CONTAINER (frame2), 3);
    gtk_widget_show(frame2);

    gtk_table_attach (GTK_TABLE (table), frame2, 0, 1, 0, 254,
                        GTK_FILL | GTK_EXPAND,
                        GTK_FILL | GTK_EXPAND | GTK_SHRINK, 1, 1);

    GLOBALS->clist_ptranslate_c_2=gtk_clist_new_with_titles(1,titles);
    gtk_clist_column_titles_passive(GTK_CLIST(GLOBALS->clist_ptranslate_c_2)); 

    gtk_clist_set_selection_mode(GTK_CLIST(GLOBALS->clist_ptranslate_c_2), GTK_SELECTION_EXTENDED);
    gtk_signal_connect_object (GTK_OBJECT (GLOBALS->clist_ptranslate_c_2), "select_row",GTK_SIGNAL_FUNC(select_row_callback),NULL);
    gtk_signal_connect_object (GTK_OBJECT (GLOBALS->clist_ptranslate_c_2), "unselect_row",GTK_SIGNAL_FUNC(unselect_row_callback),NULL);

    for(i=0;i<GLOBALS->num_proc_filters;i++)
	{
	gtk_clist_append(GTK_CLIST(GLOBALS->clist_ptranslate_c_2),(gchar **)&(GLOBALS->procsel_filter[i+1]));
	}
    gtk_clist_set_column_width(GTK_CLIST(GLOBALS->clist_ptranslate_c_2),0,gtk_clist_optimal_column_width(GTK_CLIST(GLOBALS->clist_ptranslate_c_2),0));

    gtk_widget_show (GLOBALS->clist_ptranslate_c_2);

    scrolled_win = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_win),
                                      GTK_POLICY_AUTOMATIC,
                                      GTK_POLICY_AUTOMATIC);
    gtk_widget_set_usize( GTK_WIDGET (scrolled_win), -1, 300);
    gtk_widget_show(scrolled_win);

    /* gtk_scrolled_window_add_with_viewport doesn't seen to work right here.. */
    gtk_container_add (GTK_CONTAINER (scrolled_win), GLOBALS->clist_ptranslate_c_2);

    gtk_container_add (GTK_CONTAINER (frame2), scrolled_win);


    frameh0 = gtk_frame_new (NULL);
    gtk_container_border_width (GTK_CONTAINER (frameh0), 3);
    gtk_widget_show(frameh0);
    gtk_table_attach (GTK_TABLE (table), frameh0, 0, 1, 254, 255,
                        GTK_FILL | GTK_EXPAND,
                        GTK_FILL | GTK_EXPAND | GTK_SHRINK, 1, 1);


    hbox0 = gtk_hbox_new (FALSE, 1);
    gtk_widget_show (hbox0);

    button6 = gtk_button_new_with_label (" Add Proc Filter to List ");
    gtk_container_border_width (GTK_CONTAINER (button6), 3);
    gtk_signal_connect_object (GTK_OBJECT (button6), "clicked",GTK_SIGNAL_FUNC(add_filter_callback),GTK_OBJECT (GLOBALS->window_ptranslate_c_5));
    gtk_widget_show (button6);
    gtk_tooltips_set_tip_2(tooltips, button6, 
		"Bring up a file requester to add a process filter to the filter select window.",NULL);

    gtk_box_pack_start (GTK_BOX (hbox0), button6, TRUE, FALSE, 0);
    gtk_container_add (GTK_CONTAINER (frameh0), hbox0);

    frameh = gtk_frame_new (NULL);
    gtk_container_border_width (GTK_CONTAINER (frameh), 3);
    gtk_widget_show(frameh);
    gtk_table_attach (GTK_TABLE (table), frameh, 0, 1, 255, 256,
                        GTK_FILL | GTK_EXPAND,
                        GTK_FILL | GTK_EXPAND | GTK_SHRINK, 1, 1);


    hbox = gtk_hbox_new (FALSE, 1);
    gtk_widget_show (hbox);

    button1 = gtk_button_new_with_label (" OK ");
    gtk_container_border_width (GTK_CONTAINER (button1), 3);
    gtk_signal_connect_object (GTK_OBJECT (button1), "clicked",GTK_SIGNAL_FUNC(ok_callback),GTK_OBJECT (GLOBALS->window_ptranslate_c_5));
    gtk_widget_show (button1);
    gtk_tooltips_set_tip_2(tooltips, button1, 
		"Add selected signals to end of the display on the main window.",NULL);

    gtk_box_pack_start (GTK_BOX (hbox), button1, TRUE, FALSE, 0);

    button5 = gtk_button_new_with_label (" Cancel ");
    gtk_container_border_width (GTK_CONTAINER (button5), 3);
    gtk_signal_connect_object (GTK_OBJECT (button5), "clicked",GTK_SIGNAL_FUNC(destroy_callback),GTK_OBJECT (GLOBALS->window_ptranslate_c_5));
    gtk_tooltips_set_tip_2(tooltips, button5, 
		"Do nothing and return to the main window.",NULL);
    gtk_widget_show (button5);
    gtk_box_pack_start (GTK_BOX (hbox), button5, TRUE, FALSE, 0);

    gtk_container_add (GTK_CONTAINER (frameh), hbox);
    gtk_container_add (GTK_CONTAINER (GLOBALS->window_ptranslate_c_5), table);

    gtk_widget_set_usize(GTK_WIDGET(GLOBALS->window_ptranslate_c_5), 400, 400);
    gtk_widget_show(GLOBALS->window_ptranslate_c_5);
}


/*
 * currently only called by parsewavline
 */
void set_current_translate_proc(char *name)
{
int i;

for(i=1;i<GLOBALS->num_proc_filters+1;i++)
	{
	if(!strcmp(GLOBALS->procsel_filter[i], name)) { GLOBALS->current_translate_proc = i; return; } 
	}

if(GLOBALS->num_proc_filters < PROC_FILTER_MAX)
	{
	GLOBALS->num_proc_filters++;
	load_proc_filter(GLOBALS->num_proc_filters, name);
	if(!GLOBALS->proc_filter[GLOBALS->num_proc_filters])
		{
		GLOBALS->num_proc_filters--;
		GLOBALS->current_translate_proc = 0;
		}
		else
		{
		if(GLOBALS->procsel_filter[GLOBALS->num_proc_filters]) free_2(GLOBALS->procsel_filter[GLOBALS->num_proc_filters]);
		GLOBALS->procsel_filter[GLOBALS->num_proc_filters] = malloc_2(strlen(name) + 1);
		strcpy(GLOBALS->procsel_filter[GLOBALS->num_proc_filters], name);
		GLOBALS->current_translate_proc = GLOBALS->num_proc_filters;
		}
	}
}

#endif

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
 * Revision 1.1.1.1.2.4  2007/08/05 02:27:22  kermin
 * Semi working global struct
 *
 * Revision 1.1.1.1.2.3  2007/07/31 03:18:01  kermin
 * Merge Complete - I hope
 *
 * Revision 1.1.1.1.2.2  2007/07/28 19:50:40  kermin
 * Merged in the main line
 *
 * Revision 1.1.1.1  2007/05/30 04:27:30  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:13  gtkwave
 * initial release
 *
 */

