/* 
 * Copyright (c) Tony Bybell 2005-6.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <config.h>
#include <gtk/gtk.h>
#include "gtk12compat.h"
#include "symbol.h"
#include "translate.h"
#include "debug.h"


#ifdef _MSC_VER
#define strcasecmp _stricmp
#endif

/************************ splay ************************/


xl_Tree * xl_splay (char *i, xl_Tree * t) {
/* Simple top down splay, not requiring i to be in the tree t.  */
/* What it does is described above.                             */
    xl_Tree N, *l, *r, *y;
    int dir;

    if (t == NULL) return t;
    N.left = N.right = NULL;
    l = r = &N;

    for (;;) {
	dir = strcasecmp(i, t->item);
	if (dir < 0) {
	    if (t->left == NULL) break;
	    if (strcasecmp(i, t->left->item)<0) {
		y = t->left;                           /* rotate right */
		t->left = y->right;
		y->right = t;
		t = y;
		if (t->left == NULL) break;
	    }
	    r->left = t;                               /* link right */
	    r = t;
	    t = t->left;
	} else if (dir > 0) {
	    if (t->right == NULL) break;
	    if (strcasecmp(i, t->right->item)>0) {
		y = t->right;                          /* rotate left */
		t->right = y->left;
		y->left = t;
		t = y;
		if (t->right == NULL) break;
	    }
	    l->right = t;                              /* link left */
	    l = t;
	    t = t->right;
	} else {
	    break;
	}
    }
    l->right = t->left;                                /* assemble */
    r->left = t->right;
    t->left = N.right;
    t->right = N.left;
    return t;
}


static xl_Tree * xl_insert(char *i, xl_Tree * t, char *trans) {
/* Insert i into the tree t, unless it's already there.    */
/* Return a pointer to the resulting tree.                 */
    xl_Tree * n;
    int dir;
    
    n = (xl_Tree *) calloc_2(1, sizeof (xl_Tree));
    if (n == NULL) {
	fprintf(stderr, "xl_insert: ran out of memory, exiting.\n");
	exit(255);
    }
    n->item = strcpy(malloc_2(strlen(i)+1), i);
    n->trans = strcpy(malloc_2(strlen(trans)+1), trans);

    if (t == NULL) {
	n->left = n->right = NULL;
	return n;
    }
    t = xl_splay(i,t);
    dir = strcasecmp(i,t->item);
    if (dir<0) {
	n->left = t->left;
	n->right = t;
	t->left = NULL;
	return n;
    } else if (dir>0) {
	n->right = t->right;
	n->left = t;
	t->right = NULL;
	return n;
    } else { /* We get here if it's already in the tree */
             /* Don't add it again                      */
	free_2(n->trans);
	free_2(n->item);
	free_2(n);
	return t;
    }
}

/************************ splay ************************/

int current_translate_file = 0;	/* for the viewer */
static int current_filter = 0; 	/* in requester */
int num_file_filters = 0;
char *filesel_filter[FILE_FILTER_MAX+1];
xl_Tree *xl_file_filter[FILE_FILTER_MAX+1];

void init_filetrans_data(void)
{
int i;

for(i=0;i<FILE_FILTER_MAX+1;i++)
	{
	filesel_filter[i] = NULL;
	xl_file_filter[i] = NULL;
	}
}

static void regen_display(void)
{
signalwindow_width_dirty=1;
MaxSignalLength();
signalarea_configure_event(signalarea, NULL);
wavearea_configure_event(wavearea, NULL);
}


static void remove_file_filter_2(xl_Tree *t)
{
if(t->left) remove_file_filter_2(t->left);
if(t->right) remove_file_filter_2(t->right);
if(t->item) free_2(t->item);
if(t->trans) free_2(t->trans);
free_2(t);
}

void remove_file_filter(int which, int regen)
{
if(xl_file_filter[which])
	{
	remove_file_filter_2(xl_file_filter[which]);
	xl_file_filter[which] = NULL;
	}

if(regen)
        {
	regen_display();
        }
}

void load_file_filter(int which, char *name)
{
FILE *f = fopen(name, "rb");
if(!f)
	{
	status_text("Could not open filter file!\n");
	return;
	}

remove_file_filter(which, 0); /* should never happen from GUI, but possible from save files or other weirdness */

while(!feof(f))
	{
	char *s = fgetmalloc(f);
	if(s)
		{
		char *lhs = s;

		while(*lhs && isspace(*lhs)) lhs++;
		if(lhs)
			{
			char *rhs = lhs;

			if(*lhs != '#')	/* ignore comments */
				{
				while(*rhs && !isspace(*rhs)) rhs++;
				if(*rhs)
					{
					char *xlt = rhs+1;
					*rhs = 0;
	
					while(*xlt && isspace(*xlt)) xlt++;
					if(*xlt)
						{
						xl_file_filter[which] =  xl_insert(lhs, xl_file_filter[which], xlt);
						}
					}
				}
			}

		free_2(s);
		}
	}

fclose(f);
}

void install_file_filter(int which)
{
int found = 0;

if(traces.first)  
        {
        Trptr t = traces.first;
        while(t)
                {
                if(t->flags&TR_HIGHLIGHT)
                        {
                        if(!(t->flags&(TR_BLANK|TR_ANALOG_BLANK_STRETCH)))
                                {
                                t->f_filter = which;
				t->p_filter = 0;
				if(!which)
					{
					t->flags &= (~(TR_FTRANSLATED|TR_PTRANSLATED));
					}
					else
					{
					t->flags |= TR_FTRANSLATED;
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

static int is_active=0;
static char *fcurr = NULL;

static GtkWidget *window;
static GtkWidget *clist;

static void destroy_callback(GtkWidget *widget, GtkWidget *nothing)
{
is_active=0;
gtk_widget_destroy(window);
window = NULL;
}

static void ok_callback(GtkWidget *widget, GtkWidget *nothing)
{
install_file_filter(current_filter);
destroy_callback(widget, nothing);
}

static void select_row_callback(GtkWidget *widget, gint row, gint column,
	GdkEventButton *event, gpointer data)
{
current_filter = row + 1;
}

static void unselect_row_callback(GtkWidget *widget, gint row, gint column,
	GdkEventButton *event, gpointer data)
{
current_filter = 0; /* none */
}


static void add_filter_callback_2(GtkWidget *widget, GtkWidget *nothing)
{
int i;
GtkCList *cl;

if(!filesel_ok) { return; }

if(*fileselbox_text)
	{
	for(i=0;i<num_file_filters;i++)
		{
		if(filesel_filter[i])
			{
			if(!strcmp(filesel_filter[i], *fileselbox_text)) 
				{
				status_text("Filter already imported.\n");
				return;
				}
			}
		}
	}

num_file_filters++;
load_file_filter(num_file_filters, *fileselbox_text);
if(xl_file_filter[num_file_filters])
	{
	if(filesel_filter[num_file_filters]) free_2(filesel_filter[num_file_filters]);
	filesel_filter[num_file_filters] = malloc_2(strlen(*fileselbox_text) + 1);
	strcpy(filesel_filter[num_file_filters], *fileselbox_text);

	cl=GTK_CLIST(clist);
	gtk_clist_freeze(cl);
	gtk_clist_append(cl,(gchar **)&(filesel_filter[num_file_filters]));

	gtk_clist_set_column_width(cl,0,gtk_clist_optimal_column_width(cl,0));
	gtk_clist_thaw(cl);
	}
	else
	{
	num_file_filters--;
	}
}

static void add_filter_callback(GtkWidget *widget, GtkWidget *nothing)
{
if(num_file_filters == FILE_FILTER_MAX)
	{
	status_text("Max number of file filters installed already.\n");
	return;
	}

fileselbox("Select Filter File",&fcurr,GTK_SIGNAL_FUNC(add_filter_callback_2), GTK_SIGNAL_FUNC(NULL),NULL, 0);
}

/*
 * mainline..
 */
void trans_searchbox(char *title)
{
    int i;

    GtkWidget *scrolled_win;
    GtkWidget *vbox1, *hbox, *hbox0;
    GtkWidget *button1, *button5, *button6;
    gchar *titles[]={"Filter Select"};
    GtkWidget *frame2, *frameh, *frameh0;
    GtkWidget *table;
    GtkTooltips *tooltips;

    if(is_active) 
	{
	gdk_window_raise(window->window);
	return;
	}

    is_active=1;
    current_filter = 0;

    /* create a new modal window */
    window = gtk_window_new(disable_window_manager ? GTK_WINDOW_POPUP : GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW (window), title);
    gtk_signal_connect(GTK_OBJECT (window), "delete_event",
                       (GtkSignalFunc) destroy_callback, NULL);

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

    clist=gtk_clist_new_with_titles(1,titles);
    gtk_clist_column_titles_passive(GTK_CLIST(clist)); 

    gtk_clist_set_selection_mode(GTK_CLIST(clist), GTK_SELECTION_EXTENDED);
    gtk_signal_connect_object (GTK_OBJECT (clist), "select_row",
			       GTK_SIGNAL_FUNC(select_row_callback),
			       NULL);
    gtk_signal_connect_object (GTK_OBJECT (clist), "unselect_row",
			       GTK_SIGNAL_FUNC(unselect_row_callback),
			       NULL);

    for(i=0;i<num_file_filters;i++)
	{
	gtk_clist_append(GTK_CLIST(clist),(gchar **)&(filesel_filter[i+1]));
	}
    gtk_clist_set_column_width(GTK_CLIST(clist),0,gtk_clist_optimal_column_width(GTK_CLIST(clist),0));

    gtk_widget_show (clist);

    scrolled_win = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_win),
                                      GTK_POLICY_AUTOMATIC,
                                      GTK_POLICY_AUTOMATIC);
    gtk_widget_set_usize( GTK_WIDGET (scrolled_win), -1, 300);
    gtk_widget_show(scrolled_win);

    /* gtk_scrolled_window_add_with_viewport doesn't seen to work right here.. */
    gtk_container_add (GTK_CONTAINER (scrolled_win), clist);

    gtk_container_add (GTK_CONTAINER (frame2), scrolled_win);


    frameh0 = gtk_frame_new (NULL);
    gtk_container_border_width (GTK_CONTAINER (frameh0), 3);
    gtk_widget_show(frameh0);
    gtk_table_attach (GTK_TABLE (table), frameh0, 0, 1, 254, 255,
                        GTK_FILL | GTK_EXPAND,
                        GTK_FILL | GTK_EXPAND | GTK_SHRINK, 1, 1);


    hbox0 = gtk_hbox_new (FALSE, 1);
    gtk_widget_show (hbox0);

    button6 = gtk_button_new_with_label (" Add Filter to List ");
    gtk_container_border_width (GTK_CONTAINER (button6), 3);
    gtk_signal_connect_object (GTK_OBJECT (button6), "clicked",
			       GTK_SIGNAL_FUNC(add_filter_callback),
			       GTK_OBJECT (window));
    gtk_widget_show (button6);
    gtk_tooltips_set_tip_2(tooltips, button6, 
		"Bring up a file requester to add a filter to the filter select window.",NULL);

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
    gtk_signal_connect_object (GTK_OBJECT (button1), "clicked",
			       GTK_SIGNAL_FUNC(ok_callback),
			       GTK_OBJECT (window));
    gtk_widget_show (button1);
    gtk_tooltips_set_tip_2(tooltips, button1, 
		"Add selected signals to end of the display on the main window.",NULL);

    gtk_box_pack_start (GTK_BOX (hbox), button1, TRUE, FALSE, 0);

    button5 = gtk_button_new_with_label (" Cancel ");
    gtk_container_border_width (GTK_CONTAINER (button5), 3);
    gtk_signal_connect_object (GTK_OBJECT (button5), "clicked",
			       GTK_SIGNAL_FUNC(destroy_callback),
			       GTK_OBJECT (window));
    gtk_tooltips_set_tip_2(tooltips, button5, 
		"Do nothing and return to the main window.",NULL);
    gtk_widget_show (button5);
    gtk_box_pack_start (GTK_BOX (hbox), button5, TRUE, FALSE, 0);

    gtk_container_add (GTK_CONTAINER (frameh), hbox);
    gtk_container_add (GTK_CONTAINER (window), table);

    gtk_widget_set_usize(GTK_WIDGET(window), 400, 400);
    gtk_widget_show(window);
}


/*
 * currently only called by parsewavline
 */
void set_current_translate_file(char *name)
{
int i;

for(i=1;i<num_file_filters+1;i++)
	{
	if(!strcmp(filesel_filter[i], name)) { current_translate_file = i; return; } 
	}

if(num_file_filters < FILE_FILTER_MAX)
	{
	num_file_filters++;
	load_file_filter(num_file_filters, name);
	if(!xl_file_filter[num_file_filters])
		{
		num_file_filters--;
		current_translate_file = 0;
		}
		else
		{
		if(filesel_filter[num_file_filters]) free_2(filesel_filter[num_file_filters]);
		filesel_filter[num_file_filters] = malloc_2(strlen(name) + 1);
		strcpy(filesel_filter[num_file_filters], name);
		current_translate_file = num_file_filters;
		}
	}
}

/*
 * $Id$
 * $Log$
 */

