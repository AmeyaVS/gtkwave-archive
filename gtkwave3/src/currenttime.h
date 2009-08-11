/* 
 * Copyright (c) Tony Bybell 1999-2009
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

#define WAVE_VERSION_INFO "GTKWave Analyzer v3.2.3_pre (w)1999-2009 BSI"

#define WAVE_INF_SCALING (0.5)
#define WAVE_SI_UNITS " munpfaz"

/* for dnd */
#define WAVE_DRAG_TAR_NAME_0         "text/plain"
#define WAVE_DRAG_TAR_INFO_0         0

#define WAVE_DRAG_TAR_NAME_1         "text/uri-list"         /* not url-list */
#define WAVE_DRAG_TAR_INFO_1         1

#define WAVE_DRAG_TAR_NAME_2         "STRING"
#define WAVE_DRAG_TAR_INFO_2         2


struct blackout_region_t
{
struct blackout_region_t *next;
TimeType bstart, bend;
};

void fractional_timescale_fix(char *);
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
void service_left_edge(GtkWidget *text, gpointer data);
void service_right_edge(GtkWidget *text, gpointer data);

void make_sigarea_gcs(GtkWidget *widget);
void force_screengrab_gcs(void);
void force_normal_gcs(void);

void draw_signalarea_focus(void);
gint signalarea_configure_event(GtkWidget *widget, GdkEventConfigure *event);
gint wavearea_configure_event(GtkWidget *widget, GdkEventConfigure *event);
void entrybox(char *title, int width, char *default_text, int maxch, GtkSignalFunc func);
void fileselbox_old(char *title, char **filesel_path, GtkSignalFunc ok_func, GtkSignalFunc notok_func, char *pattn);
void fileselbox(char *title, char **filesel_path, GtkSignalFunc ok_func, GtkSignalFunc notok_func, char *pattn, int is_writemode);
void status_text(char *str);
void searchbox(char *title, GtkSignalFunc func);
void search_enter_callback(GtkWidget *widget, GtkWidget *do_warning);
void search_insert_callback(GtkWidget *widget, char is_prepend);
void showchange(char *title, Trptr t, GtkSignalFunc func);

void treebox(char *title, GtkSignalFunc func, GtkWidget *old_window);
GtkWidget* treeboxframe(char *title, GtkSignalFunc func);
void mkmenu_treesearch_cleanup(GtkWidget *widget, gpointer data);
void dump_open_tree_nodes(FILE *wave, xl_Tree *t);
void force_open_tree_node(char *name);
void select_tree_node(char *name);

void dnd_setup(GtkWidget *src, GtkWidget *widget, int enable_receive); /* dnd from gtk2 tree to signalwindow */
void treeview_select_all_callback(void); /* gtk2 */
void treeview_unselect_all_callback(void); /* gtk2 */

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
void reformat_time_simple(char *buf, TimeType val, char dim);
TimeType unformat_time(const char *buf, char dim);
void time_trunc_set(void);
TimeType time_trunc(TimeType t);
void exponent_to_time_scale(signed char scale);

void from_entry_callback(GtkWidget *widget, GtkWidget *entry);
void to_entry_callback(GtkWidget *widget, GtkWidget *entry);

void load_all_fonts(void);

/* logfile adds */
void logbox(char *title, int width, char *default_text);

/* partial vcd adds */
void update_endcap_times_for_partial_vcd(void);

/* mouseover support */
void move_mouseover(Trptr t, gint xin, gint yin, TimeType tim);
void move_mouseover_sigs(Trptr t, gint xin, gint yin, TimeType tim);

/* session swapping */
gint install_keypress_handler(void);
void remove_keypress_handler(gint id);

/* x/y dataset interpolation */
int ft_interpolate (double *data, double *ndata, double *oscale, int olen, double *nscale, int nlen, int degree);

/* popups */
void do_popup_menu (GtkWidget *my_widget, GdkEventButton *event);

#endif

/*
 * $Id$
 * $Log$
 * Revision 1.58  2009/08/04 18:01:56  gtkwave
 * version bump to 3.2.2
 *
 * Revision 1.57  2009/04/11 21:57:53  gtkwave
 * changed exit() to vcd_exit() in some instances for tab loader hardening
 *
 * Revision 1.56  2009/04/09 20:38:41  gtkwave
 * promote to 3.2.1
 *
 * Revision 1.55  2009/02/23 18:29:49  gtkwave
 * add more information on init failure for mac users
 *
 * Revision 1.54  2009/02/17 03:20:35  gtkwave
 * promote to 3.2.0
 *
 * Revision 1.53  2009/01/16 19:27:00  gtkwave
 * added more tcl commands
 *
 * Revision 1.52  2009/01/10 19:53:52  gtkwave
 * documentation updates
 *
 * Revision 1.51  2008/12/28 03:03:33  gtkwave
 * Added scale_to_time_dimension rc variable and menu options.
 *
 * Revision 1.50  2008/12/09 00:36:42  gtkwave
 * added mouseover support for signal window
 *
 * Revision 1.49  2008/10/21 04:04:28  gtkwave
 * getting ready for version bump
 *
 * Revision 1.48  2008/09/17 04:33:38  gtkwave
 * support for smaller timescales in VCD files
 *
 * Revision 1.47  2008/09/16 00:01:23  gtkwave
 * prelim drag and drop from external apps (now disabled)
 *
 * Revision 1.46  2008/08/22 20:19:12  gtkwave
 * fix for strings in vcd recoder
 *
 * Revision 1.45  2008/08/21 03:48:45  gtkwave
 * promote to 3.1.13
 *
 * Revision 1.44  2008/07/20 01:29:36  gtkwave
 * added command line option --comphier
 *
 * Revision 1.43  2008/07/14 22:39:22  gtkwave
 * promote to 3.1.12
 *
 * Revision 1.42  2008/07/12 22:54:12  gtkwave
 * array of wires malformed vcd dump load abort fixed
 *
 * Revision 1.41  2008/07/01 18:51:06  gtkwave
 * compiler warning fixes for amd64
 *
 * Revision 1.40  2008/06/18 19:56:07  gtkwave
 * 3.0.11 release building
 *
 * Revision 1.39  2008/06/11 08:01:47  gtkwave
 * gcc 4.3.x compiler warning fixes
 *
 * Revision 1.38  2008/05/14 21:08:19  gtkwave
 * revision bump to 3.1.10
 *
 * Revision 1.37  2008/05/05 19:27:03  gtkwave
 * support for DND from regex search window to sig/waveareas
 *
 * Revision 1.36  2008/04/25 21:31:09  gtkwave
 * added missing t->shift in mouseover time calculation
 *
 * Revision 1.35  2008/04/21 00:03:15  gtkwave
 * fix for crash on reload with file filters active
 *
 * Revision 1.34  2008/04/06 23:09:19  gtkwave
 * promote to 3.1.8
 *
 * Revision 1.33  2008/03/25 03:22:09  gtkwave
 * expanded zero fill to include also a one fill (for pre-inverted nets)
 *
 * Revision 1.32  2008/03/24 19:33:59  gtkwave
 * added zero range fill feature
 *
 * Revision 1.31  2008/03/23 04:26:42  gtkwave
 * promote to 3.1.7
 *
 * Revision 1.30  2008/02/28 16:14:48  gtkwave
 * starting with 3.1.7 changes
 *
 * Revision 1.29  2008/02/26 04:57:51  gtkwave
 * version bump to 3.1.6
 *
 * Revision 1.28  2008/02/15 16:25:21  gtkwave
 * gtk2 fixes for versions < 2.8.0
 *
 * Revision 1.27  2008/02/15 00:26:45  gtkwave
 * promote to 3.1.5
 *
 * Revision 1.26  2008/02/08 02:26:36  gtkwave
 * anti-aliased font support add
 *
 * Revision 1.25  2008/01/31 04:09:47  gtkwave
 * revision bump to 3.1.4
 *
 * Revision 1.24  2008/01/28 23:08:50  gtkwave
 * added infinite scaling define in currenttime.h
 *
 * Revision 1.23  2008/01/23 11:07:19  gtkwave
 * integration of preliminary signalwindow popup menu code
 *
 * Revision 1.22  2008/01/22 20:11:47  gtkwave
 * track and hold experimentation
 *
 * Revision 1.21  2008/01/20 00:30:09  gtkwave
 * fixes dnd bug on new versions of gtk (2.10.14)
 *
 * Revision 1.20  2008/01/14 18:37:19  gtkwave
 * 3.1.3 version bump
 *
 * Revision 1.19  2008/01/12 21:36:44  gtkwave
 * added black and white vs color rendering menu options
 *
 * Revision 1.18  2008/01/09 04:09:11  gtkwave
 * fix keyboard focus sighandler when multi-tabs are being used
 *
 * Revision 1.17  2008/01/08 23:03:35  gtkwave
 * added toolbar using use_toolbutton_interface rc variable
 *
 * Revision 1.16  2008/01/08 07:13:08  gtkwave
 * more limiting of ctrl-a focus (away from tree and filter entry)
 *
 * Revision 1.15  2008/01/08 04:01:12  gtkwave
 * more accelerator key ergonomic updates
 *
 * Revision 1.14  2008/01/05 22:25:46  gtkwave
 * degate busy during treeview dnd as it disrupts focus; dnd cleanups
 *
 * Revision 1.13  2008/01/03 21:55:45  gtkwave
 * various cleanups
 *
 * Revision 1.12  2007/12/29 20:19:30  gtkwave
 * added dynamic string updates for entrybox in pattern search and sst
 *
 * Revision 1.11  2007/12/24 19:56:03  gtkwave
 * preparing for 3.1.2 version bump
 *
 * Revision 1.10  2007/09/23 18:33:54  gtkwave
 * warnings cleanups from sun compile
 *
 * Revision 1.9  2007/09/20 16:06:47  gtkwave
 * revision bump prior to 3.1.1 tar creation
 *
 * Revision 1.8  2007/09/09 04:18:57  gtkwave
 * fixed hierarchy crash bug
 *
 * Revision 1.7  2007/08/29 23:38:46  gtkwave
 * 3.1.0 RC2 minor compatibility/bugfixes
 *
 * Revision 1.6  2007/08/26 21:35:40  gtkwave
 * integrated global context management from SystemOfCode2007 branch
 *
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

