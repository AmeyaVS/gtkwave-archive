/* 
 * Copyright (c) Tony Bybell 1999-2007
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include "globals.h"

#ifndef CURRENTTIME_H
#define CURRENTTIME_H

#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "analyzer.h"
#include "regex_wave.h"
#include "translate.h"

#define WAVE_VERSION_INFO "GTKWave Analyzer v3.1.0_rc1 (w)1999-2007 BSI"

struct blackout_region_t
{
struct blackout_region_t *next;
TimeType bstart, bend;
};


char *convert_ascii(Trptr t, vptr v);
char *convert_ascii_vec(Trptr t, char *vec);
char *convert_ascii_real(double *d);
char *convert_ascii_string(char *s);
double convert_real_vec(Trptr t, char *vec);
double convert_real(Trptr t, vptr v);


int vtype(Trptr t, char *vec);
int vtype2(Trptr t, vptr v);

void button_press_release_common(void);

void update_markertime(TimeType val);
void update_maxtime(TimeType val);
void update_basetime(TimeType val);
void update_currenttime(TimeType val);
void update_maxmarker_labels(void);
void UpdateSigValue(Trptr t);
void MaxSignalLength(void);
void RenderSigs(int trtarget, int update_waves);
int RenderSig(Trptr t, int i, int dobackground);
void calczoom(double z0);
void fix_wavehadj(void);
void service_zoom_in(GtkWidget *text, gpointer data);
void service_zoom_out(GtkWidget *text, gpointer data);
void service_zoom_fit(GtkWidget *text, gpointer data);
void service_zoom_full(GtkWidget *text, gpointer data);
void service_zoom_undo(GtkWidget *text, gpointer data);
void service_zoom_left(GtkWidget *text, gpointer data);
void service_zoom_right(GtkWidget *text, gpointer data);
void service_dragzoom(TimeType time1, TimeType time2);
void fetch_left(GtkWidget *text, gpointer data);
void fetch_right(GtkWidget *text, gpointer data);
void time_update(void);
void discard_left(GtkWidget *text, gpointer data);
void discard_right(GtkWidget *text, gpointer data);
void service_left_shift(GtkWidget *text, gpointer data);
void service_right_shift(GtkWidget *text, gpointer data);
void service_left_page(GtkWidget *text, gpointer data);
void service_right_page(GtkWidget *text, gpointer data);

void make_sigarea_gcs(GtkWidget *widget);

gint signalarea_configure_event(GtkWidget *widget, GdkEventConfigure *event);
gint wavearea_configure_event(GtkWidget *widget, GdkEventConfigure *event);
void entrybox(char *title, int width, char *default_text, int maxch, GtkSignalFunc func);
void fileselbox_old(char *title, char **filesel_path, GtkSignalFunc ok_func, GtkSignalFunc notok_func, char *pattn);
void fileselbox(char *title, char **filesel_path, GtkSignalFunc ok_func, GtkSignalFunc notok_func, char *pattn, int is_writemode);
void status_text(char *str);
void searchbox(char *title, GtkSignalFunc func);
void search_enter_callback(GtkWidget *widget, GtkWidget *do_warning);
void showchange(char *title, Trptr t, GtkSignalFunc func);

void treebox(char *title, GtkSignalFunc func, GtkWidget *old_window);
GtkWidget* treeboxframe(char *title, GtkSignalFunc func);
void mkmenu_treesearch_cleanup(GtkWidget *widget, gpointer data);
void dump_open_tree_nodes(FILE *wave, xl_Tree *t);
void force_open_tree_node(char *name);
void select_tree_node(char *name);

void dnd_setup(GtkWidget *widget); /* dnd from gtk2 tree to signalwindow */

void hier_searchbox(char *title, GtkSignalFunc func);
void renderbox(char *title);

struct tree *fetchlow(struct tree *t);
struct tree *fetchhigh(struct tree *t);
void fetchvex(struct tree *t, char direction);
void refresh_hier_tree(struct tree *t);

void markerbox(char *title, GtkSignalFunc func);

void tracesearchbox(char *title, GtkSignalFunc func);

int searchbox_is_active(void);	
int treebox_is_active(void);
int hier_searchbox_is_active(void);

void simplereqbox(char *title, int width, char *default_text,
        char *oktext, char *canceltext, GtkSignalFunc func, int is_alert);

void helpbox(char *title, int width, char *default_text);
void help_text(char *str);
void help_text_bold(char *str);

void dnd_error(void);

void reformat_time(char *buf, TimeType val, char dim);
TimeType unformat_time(const char *buf, char dim);
void time_trunc_set(void);
TimeType time_trunc(TimeType t);
void exponent_to_time_scale(signed char scale);

void load_all_fonts(void);

/* logfile adds */
void logbox(char *title, int width, char *default_text);

/* partial vcd adds */
void update_endcap_times_for_partial_vcd(void);

/* mouseover support */
void move_mouseover(Trptr t, gint xin, gint yin, TimeType tim);

#endif

/*
 * $Id$
 * $Log$
 * Revision 1.2.2.10  2007/08/25 19:45:24  gtkwave
 * update version number to 3.1.0 release candidate 1
 *
 * Revision 1.2.2.9  2007/08/25 19:43:45  gtkwave
 * header cleanups
 *
 * Revision 1.2.2.8  2007/08/23 02:19:48  gtkwave
 * merge GLOBALS state from old hier_search widget into new one
 *
 * Revision 1.2.2.7  2007/08/22 22:11:05  gtkwave
 * make regex search for signals re-entrant
 *
 * Revision 1.2.2.6  2007/08/22 02:06:38  gtkwave
 * merge in treebox() similar to treeboxframe()
 *
 * Revision 1.2.2.5  2007/08/21 23:29:16  gtkwave
 * merge in tree select state from old ctx
 *
 * Revision 1.2.2.4  2007/08/21 22:35:38  gtkwave
 * prelim tree state merge
 *
 * Revision 1.2.2.3  2007/08/07 04:54:58  gtkwave
 * slight modifications to global initialization scheme
 *
 * Revision 1.2.2.2  2007/08/05 02:27:19  kermin
 * Semi working global struct
 *
 * Revision 1.2.2.1  2007/07/28 19:50:39  kermin
 * Merged in the main line
 *
 * Revision 1.5  2007/07/23 23:13:08  gtkwave
 * adds for color tags in filtered trace data
 *
 * Revision 1.4  2007/06/22 03:07:18  gtkwave
 * added AC_SYS_LARGEFILE to configure.ac for largefile support on systems
 * that don't natively compile it in
 *
 * Revision 1.3  2007/06/12 02:50:37  gtkwave
 * added libbz2 to configure.ac plus header checking for zlib.h/bzlib.h
 *
 * Revision 1.2  2007/06/08 14:04:19  gtkwave
 * added missing files/changed old ones that were out of data with old info
 *
 * Revision 1.1.1.1  2007/05/30 04:27:22  gtkwave
 * Imported sources
 *
 * Revision 1.7  2007/05/28 00:55:06  gtkwave
 * added support for arrays as a first class dumpfile datatype
 *
 * Revision 1.6  2007/04/30 01:10:21  gtkwave
 * splash_sync() causes errno to be set when GTK main event loop is called
 * by the vcd parsers for newer versions of gnome (2.18) and/or other
 * various X11 Gentoo dependencies.
 *
 * Revision 1.5  2007/04/29 04:13:49  gtkwave
 * changed anon union defined in struct Node to a named one as anon unions
 * are a gcc extension
 *
 * Revision 1.4  2007/04/28 02:36:19  gtkwave
 * committing this release to 3.0.27
 *
 * Revision 1.3  2007/04/21 21:15:45  gtkwave
 * updated version string to indicate 3.0.27 cvs version status (not
 * 3.0.26 stable anymore)
 *
 * Revision 1.2  2007/04/20 02:08:11  gtkwave
 * initial release
 *
 */

