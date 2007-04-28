/* 
 * Copyright (c) Tony Bybell 1999-2007
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef CURRENTTIME_H
#define CURRENTTIME_H

#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "analyzer.h"
#include "regex_wave.h"

#define WAVE_VERSION_INFO "GTKWave Analyzer v3.0.27 (w)1999-2007 BSI"

struct blackout_region_t
{
struct blackout_region_t *next;
TimeType bstart, bend;
};

extern struct blackout_region_t *blackout_regions;

extern int  num_cpus;		     /* specify number of CPUs for parallelizable ops */
extern char is_vcd, partial_vcd;
extern char is_lxt;
extern char is_ghw;
extern char lxt_clock_compress_to_z;

extern char signalwindow_width_dirty;/* indicates that nonblank traces were added/removed */

extern char disable_mouseover;	     /* to disable mouseover tooltips by default from rc */
extern char disable_window_manager;  /* for scripting...some windowmanagers would expect user to nail down window */
extern char autoname_bundles;	     /* to dispense with having to hit ok on bundles (let gtkwave name them) */
extern char use_maxtime_display;     /* 1=maxtime, 0=markertime  */
extern char use_frequency_delta;     /* 1=frequency display, 0=time delta */
extern char constant_marker_update;  /* 1 if you want hold downs to const update at expense of speed */
extern char use_roundcaps;           /* for draw_vptr_trace */
extern char use_scrollbar_only;      /* 1 removes the rendering of page/shift/fetch/discard buttons */
extern char show_base;		     /* if zero, omits leading base info */
extern int  vector_padding;	     /* amount of mandatory white space per vector */
extern char use_big_fonts;	     /* use fonts with size 4 larger than normal */
extern char use_nonprop_fonts;	     /* use misc-fixed which allows pane resizing acceleration */
extern char enable_fast_exit;        /* disables exit requester on menu_quit */
extern char enable_ghost_marker;     /* sez whether to draw ghost markers */   
extern char enable_horiz_grid;	     /* sez whether to draw horiz rulers on traces */
extern char enable_vert_grid;	     /* sez whether to draw vert rulers on traces */
extern char wave_scrolling;	     /* sez whether to allow <-> scrolling in wave window */
extern char left_justify_sigs;	     /* when enabled, signals are left, rather than right flushed */
extern int  ps_maxveclen;	     /* maximum vector length that is printable in signal window (includes = and base symbol) */
extern char zoom_pow10_snap;	     /* forces divisible by 10 timescale no matter what the zoom is */
extern char force_toolbars;          /* forces menu+top buttons to be rendered as toolbars */
extern int  cursor_snap;	     /* makes marker jump to nearest transition (default = 0 pixels) */
extern int  hide_sst;		     /* hide sst expander, use regular window */
extern int  sst_expanded;	     /* initial state of sst expander (open/closed) */

extern gdouble page_divisor;	     /* allows fractional page scrolls */

extern char do_initial_zoom_fit;     /* when set forces an initial zoom fit */
extern char zoom_was_explicitly_set; /* set on '*' .sav file read           */

extern char use_full_precision;
extern TimeType time_scale;
extern char time_dimension;
extern char do_resize_signals;
extern char do_zoom_center;
extern int oldusize;
extern TimeType currenttime;
extern TimeType max_time;
extern TimeType min_time;
extern int max_signal_name_pixel_width;
extern int signal_pixmap_width;
extern int signal_fill_width;
extern int fontheight;		/* font height for signals */
extern char display_grid;	/* default to displaying grid */
extern TimeType named_markers[26];
extern TimeType zoom, scale, nsperframe;
extern gdouble pixelsperframe;
extern gdouble hashstep;
extern gdouble pxns, nspx;
extern gdouble zoombase;
extern GtkObject *wave_vslider, *wave_hslider;
extern GtkObject *signal_hslider;
extern GtkWidget *signalarea;
extern GdkPixmap *signalpixmap;
extern GtkWidget *wavearea;  
extern GtkWidget *signalwindow;
extern GtkWidget* expanderwindow;
extern TimeType fetchwindow;
extern GtkWidget *from_entry, *to_entry;
extern char *entrybox_text;
extern char **fileselbox_text;
extern char filesel_ok;
extern Trptr topmost_trace;
extern int waveheight, wavecrosspiece;
extern int wavewidth;
extern GdkFont *wavefont, *wavefont_smaller;
extern GdkFont *signalfont;

extern char *fontname_signals, *fontname_waves, *fontname_logfile;

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

extern GdkGC   *gc_white;                  
extern GdkGC   *gc_black;        
extern GdkGC   *gc_ltgray;        
extern GdkGC   *gc_normal;        
extern GdkGC   *gc_mdgray;        
extern GdkGC   *gc_dkgray;        
extern GdkGC   *gc_dkblue;        
void make_sigarea_gcs(GtkWidget *widget);

gint signalarea_configure_event(GtkWidget *widget, GdkEventConfigure *event);
gint wavearea_configure_event(GtkWidget *widget, GdkEventConfigure *event);
void entrybox(char *title, int width, char *default_text, int maxch, GtkSignalFunc func);
void fileselbox_old(char *title, char **filesel_path, GtkSignalFunc ok_func, GtkSignalFunc notok_func, char *pattn);
void fileselbox(char *title, char **filesel_path, GtkSignalFunc ok_func, GtkSignalFunc notok_func, char *pattn, int is_writemode);
void status_text(char *str);
void searchbox(char *title, GtkSignalFunc func);
void showchange(char *title, Trptr t, GtkSignalFunc func);
void treebox(char *title, GtkSignalFunc func);
GtkWidget* treeboxframe(char *title, GtkSignalFunc func);
void mkmenu_treesearch_cleanup(GtkWidget *widget, gpointer data);

void dnd_setup(GtkWidget *widget); /* dnd from gtk2 tree to signalwindow */

void hier_searchbox(char *title, GtkSignalFunc func);
void renderbox(char *title);

struct tree *fetchlow(struct tree *t);
struct tree *fetchhigh(struct tree *t);
void fetchvex(struct tree *t, char direction);

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
extern int helpbox_is_active;

extern char dnd_state;
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
 * Revision 1.3  2007/04/21 21:15:45  gtkwave
 * updated version string to indicate 3.0.27 cvs version status (not
 * 3.0.26 stable anymore)
 *
 * Revision 1.2  2007/04/20 02:08:11  gtkwave
 * initial release
 *
 */

