/* 
 * Copyright (c) Tony Bybell 1999-2006.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <config.h>
#include <gtk/gtk.h>
#include "gtk12compat.h"
#include "menu.h"
#include "debug.h"
#include "wavealloca.h"
#include <string.h>
#include <stdlib.h>


#ifdef __linux__
extern char *canonicalize_file_name (__const char *__name);
#endif

extern GtkWidget *mainwindow;


static GtkWidget *fs;
char **fileselbox_text=NULL;
char filesel_ok=0;
static GtkSignalFunc cleanup, bad_cleanup;


static void enter_callback(GtkWidget *widget, GtkFileSelection *fw)
{
G_CONST_RETURN char *allocbuf;
int alloclen;

allocbuf=gtk_file_selection_get_filename(GTK_FILE_SELECTION(fs));
if((alloclen=strlen(allocbuf)))
	{
	filesel_ok=1;
	if(*fileselbox_text) free_2(*fileselbox_text);
	*fileselbox_text=(char *)malloc_2(alloclen+1);
	strcpy(*fileselbox_text, allocbuf);
	}

DEBUG(printf("Filesel OK %s\n",allocbuf));
gtk_grab_remove(fs);
gtk_widget_destroy(fs);
while (gtk_events_pending()) gtk_main_iteration();
cleanup();
}

static void cancel_callback(GtkWidget *widget, GtkWidget *nothing)
{
DEBUG(printf("Filesel Entry Cancel\n"));
gtk_grab_remove(fs);
gtk_widget_destroy(fs);
while (gtk_events_pending()) gtk_main_iteration();
if(bad_cleanup) bad_cleanup();
}

static void destroy_callback(GtkWidget *widget, GtkWidget *nothing)
{
DEBUG(printf("Filesel Destroy\n"));
while (gtk_events_pending()) gtk_main_iteration();
if(bad_cleanup) bad_cleanup();
}

void fileselbox_old(char *title, char **filesel_path, GtkSignalFunc ok_func, GtkSignalFunc notok_func, char *pattn)
{
fileselbox_text=filesel_path;
filesel_ok=0;
cleanup=ok_func;
bad_cleanup=notok_func;

fs=gtk_file_selection_new(title);
gtk_signal_connect(GTK_OBJECT(fs), "destroy", (GtkSignalFunc) destroy_callback, NULL);
gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fs)->ok_button), 
	"clicked", (GtkSignalFunc) enter_callback, GTK_OBJECT(fs));
gtk_signal_connect_object(GTK_OBJECT(GTK_FILE_SELECTION(fs)->cancel_button),
	"clicked", (GtkSignalFunc) cancel_callback, GTK_OBJECT(fs));
gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION(fs));
if(*fileselbox_text) gtk_file_selection_set_filename(GTK_FILE_SELECTION(fs), *fileselbox_text);

/*
 * XXX: filter on patterns when this feature eventually works (or for a later version of GTK)!
 * if((pattn)&&(pattn[0])) gtk_file_selection_complete(GTK_FILE_SELECTION(fs), pattn);
 */

gtk_grab_add(fs);
gtk_widget_show(fs);
}


void fileselbox(char *title, char **filesel_path, GtkSignalFunc ok_func, GtkSignalFunc notok_func, char *pattn, int is_writemode)
{

#if GTK_CHECK_VERSION(2,4,0)
GtkWidget *pFileChoose;
GtkWidget *pWindowMain;
GtkFileFilter *filter;
#endif
 
if(script_handle)
	{
	char *s = NULL;

	fileselbox_text=filesel_path;
	filesel_ok=1;

	while((!s)&&(!feof(script_handle))) s = fgetmalloc_stripspaces(script_handle);

	if(*fileselbox_text) free_2(*fileselbox_text); 
	if(!s)
		{
		fprintf(stderr, "Null filename passed to fileselbox in script, exiting.\n");
		exit(255);
		}
	*fileselbox_text = s;
	fprintf(stderr, "GTKWAVE | Filename '%s'\n", s);

	ok_func();	
	return;
	}


#if !GTK_CHECK_VERSION(2,4,0)

fileselbox_old(title, filesel_path, ok_func, notok_func, pattn);
return;

#else


pWindowMain = mainwindow;
fileselbox_text=filesel_path;
filesel_ok=0;

if(*fileselbox_text && (!g_path_is_absolute(*fileselbox_text)))
	{
#ifdef __linux__
	char *can = canonicalize_file_name(*fileselbox_text);

	if(*fileselbox_text) free_2(*fileselbox_text);
        *fileselbox_text=(char *)malloc_2(strlen(can)+1);
        strcpy(*fileselbox_text, can);
	free(can);

#else
#if defined __USE_BSD || defined __USE_XOPEN_EXTENDED || defined __CYGWIN__ || defined HAVE_REALPATH
	char *can = realpath(*fileselbox_text, NULL);

	if(can)
		{
		if(*fileselbox_text) free_2(*fileselbox_text);
	        *fileselbox_text=(char *)malloc_2(strlen(can)+1);
	        strcpy(*fileselbox_text, can);
		free(can);
		}
#else
#if __GNUC__
#warning Absolute file path warnings might be issued by the file chooser dialogue on this system!
#endif
#endif
#endif
	}

if(is_writemode)
	{
	pFileChoose = gtk_file_chooser_dialog_new(
		title,
	        NULL,
	        GTK_FILE_CHOOSER_ACTION_SAVE,
	        GTK_STOCK_CANCEL,
	        GTK_RESPONSE_CANCEL,
	        GTK_STOCK_SAVE,
	        GTK_RESPONSE_ACCEPT,
	        NULL);
	}
	else
	{
	pFileChoose = gtk_file_chooser_dialog_new(
		title,
	        NULL,
	        GTK_FILE_CHOOSER_ACTION_OPEN,
	        GTK_STOCK_CANCEL,
	        GTK_RESPONSE_CANCEL,
	        GTK_STOCK_OPEN,
	        GTK_RESPONSE_ACCEPT,
	        NULL);
	}

if(*filesel_path) gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(pFileChoose), *filesel_path);

if(pattn)
	{
	filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(filter, pattn);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(pFileChoose), filter);
	}

gtk_object_set_data(GTK_OBJECT(pFileChoose), "FileChooseWindow", pFileChoose);
gtk_container_set_border_width(GTK_CONTAINER(pFileChoose), 10);
gtk_window_set_position(GTK_WINDOW(pFileChoose), GTK_WIN_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(pFileChoose), TRUE);
gtk_window_set_policy(GTK_WINDOW(pFileChoose), FALSE, FALSE, FALSE);
if(pWindowMain)
	{
	gtk_window_set_transient_for(GTK_WINDOW(pFileChoose), GTK_WINDOW(pWindowMain));
	}
gtk_grab_add(pFileChoose);
gtk_widget_show(pFileChoose);

if (gtk_dialog_run(GTK_DIALOG (pFileChoose)) == GTK_RESPONSE_ACCEPT)
	{
	G_CONST_RETURN char *allocbuf;
	int alloclen;

	allocbuf = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (pFileChoose));
	if((alloclen=strlen(allocbuf)))
	        {
	        filesel_ok=1;
	        if(*fileselbox_text) free_2(*fileselbox_text);
	        *fileselbox_text=(char *)malloc_2(alloclen+1);
	        strcpy(*fileselbox_text, allocbuf);

		/* add missing suffix to write files */
		if(pattn && is_writemode)
			{
			char *s = *fileselbox_text;
			char *s2;
			char *suffix = wave_alloca(strlen(pattn) + 1);					
			char *term;

			strcpy(suffix, pattn);
			while((*suffix) && (*suffix != '.')) suffix++;
			term = *suffix ? suffix+1 : suffix;
			while((*term) && (isalnum(*term))) term++;
			*term = 0;

                        if(strlen(s) > strlen(suffix))
                                {
                                if(strcmp(s + strlen(s) - strlen(suffix), suffix))
                                        {
                                        goto fix_suffix;
                                        }
                                }
                                else
                                {
fix_suffix:                     s2 = malloc_2(strlen(s) + strlen(suffix) + 1);
                                strcpy(s2, s);
                                strcat(s2, suffix);
                                free_2(s);
				*fileselbox_text = s2;
                                }
			}
	        }

	DEBUG(printf("Filesel OK %s\n",allocbuf));
	gtk_grab_remove(pFileChoose);
	gtk_widget_destroy(pFileChoose);

	while (gtk_events_pending()) gtk_main_iteration();
	ok_func();
	}
	else
	{
	DEBUG(printf("Filesel Entry Cancel\n"));
	gtk_grab_remove(pFileChoose);
	gtk_widget_destroy(pFileChoose);

	while (gtk_events_pending()) gtk_main_iteration();
	if(bad_cleanup) notok_func();
	}
#endif
}
