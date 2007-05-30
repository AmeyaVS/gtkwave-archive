/* 
 * Copyright (c) Tony Bybell 1999-2004
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <config.h>
#include <gtk/gtk.h>
#include "symbol.h"
#include "lxt2_read.h"
#include "lx2.h"
   
/* Add some text to our text widget - this is a callback that is invoked
when our window is realized. We could also force our window to be
realized with gtk_widget_realize, but it would have to be part of
a hierarchy first */

static GtkWidget *text=NULL;
static GtkWidget *vscrollbar;

#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
static GtkTextIter iter;
static GtkTextTag *bold_tag;
#endif


void status_text(char *str)
{
if(text)
	{
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
        gtk_text_buffer_insert (GTK_TEXT_VIEW (text)->buffer, &iter, str, -1);
#else
        gtk_text_insert (GTK_TEXT (text), NULL, &text->style->black, NULL, str, -1);
#endif
	}
	else
	{
	int len = strlen(str);
	char ch = len ? str[len-1] : 0;
	fprintf(stderr, "GTKWAVE | %s%s", str, (ch=='\n') ? "" : "\n");
	}
}

void
realize_text (GtkWidget *text, gpointer data)
{
char buf[128];

if(is_vcd)
	{
	if(partial_vcd)
		{
		status_text("VCD loading interactively.\n");
		}
		else
		{
		status_text("VCD loaded successfully.\n");
		}
	}
else
if(is_lxt)
	{
	status_text("LXT loaded successfully.\n");
	}
else
if(is_ghw)
	{
	status_text("GHW loaded successfully.\n");
	}
else
if(is_lx2)
	{
	switch(is_lx2)
		{
		case LXT2_IS_LXT2: status_text("LXT2 loaded successfully.\n"); break;
		case LXT2_IS_AET2: status_text("AET2 loaded successfully.\n"); break;
		case LXT2_IS_VZT:  status_text("VZT loaded successfully.\n"); break;
		case LXT2_IS_VLIST:  status_text("VCD loaded successfully.\n"); break;
		}
	}

sprintf(buf,"[%d] facilities found.\n",numfacs);
status_text(buf);

if((is_vcd)||(is_ghw))
	{
	if(!partial_vcd)
		{
		sprintf(buf,"[%d] regions found.\n",regions);
		status_text(buf);
		}
	}
else
	{
	if(is_lx2 == LXT2_IS_VLIST)
		{
		sprintf(buf,"Regions formed on demand.\n");
		}
		else
		{
		sprintf(buf,"Regions loaded on demand.\n");
		}
	status_text(buf);
	}
}
   
/* Create a scrolled text area that displays a "message" */
GtkWidget *
create_text (void)
{
GtkWidget *table;

GtkTooltips *tooltips;

tooltips=gtk_tooltips_new_2();
gtk_tooltips_set_delay_2(tooltips,1500);

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
vscrollbar = gtk_vscrollbar_new ((GTK_TEXT (text))->vadj);
#endif
gtk_table_attach (GTK_TABLE (table), vscrollbar, 15, 16, 0, 1,
			GTK_FILL, GTK_FILL | GTK_SHRINK | GTK_EXPAND, 0, 0);
gtk_widget_show (vscrollbar);
   
/* Add a handler to put a message in the text widget when it is realized */
gtk_signal_connect (GTK_OBJECT (text), "realize",
			GTK_SIGNAL_FUNC (realize_text), NULL);
   
gtk_tooltips_set_tip_2(tooltips, text, "Status Window", NULL);
return(table);
}
   
/*
 * $Id$
 * $Log$
 * Revision 1.2  2007/04/20 02:08:17  gtkwave
 * initial release
 *
 */

