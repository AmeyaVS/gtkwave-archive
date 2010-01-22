/* 
 * Copyright (c) Tony Bybell 1999-2010.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include "globals.h"

#ifndef GTKWAVE_STRACE_H
#define GTKWAVE_STRACE_H

#include <gtk/gtk.h>
#include <string.h>
#include <stdarg.h>
#include "debug.h"
#include "analyzer.h"
#include "currenttime.h"
#include "bsearch.h"

#define WAVE_NUM_STRACE_WINDOWS (2)
#define WAVE_STRACE_ITERATOR(x) for((x)=((WAVE_NUM_STRACE_WINDOWS)-1); (x)>=0 ; (x)--)
#define WAVE_STRACE_ITERATOR_FWD(x) for((x)=0;(x)<(WAVE_NUM_STRACE_WINDOWS);(x)++)

enum strace_directions
{ STRACE_BACKWARD, STRACE_FORWARD };

enum st_stype
        {ST_DC, ST_HIGH, ST_MID, ST_X, ST_LOW, ST_STRING,
         ST_RISE, ST_FALL, ST_ANY, WAVE_STYPE_COUNT};

struct strace_defer_free
{
struct strace_defer_free *next;
Trptr defer;
};
         
struct strace_back
{
struct strace *parent;
int which;
};

struct strace
{
struct strace *next;   
char *string;           /* unmalloc this when all's done! */
Trptr trace;
char value;
char search_result;

union
	{
        hptr    h;             /* what makes up this trace */
        vptr    v;
      	} his;

struct strace_back *back[WAVE_STYPE_COUNT];    /* dealloc these too! */   
};


struct timechain
{
struct timechain *next;
TimeType t;
};


struct mprintf_buff_t
{
struct mprintf_buff_t *next;
char *str;
};

struct item_mark_string {
   char *str;
   unsigned char idx;
};


/* for being able to handle multiple strace sessions at once, context is moved here */
struct strace_ctx_t
{
TimeType *timearray; /* from strace.c 430 */
int timearray_size; /* from strace.c 431 */
GtkWidget *ptr_mark_count_label_strace_c_1; /* from strace.c 432 */
struct strace *straces; /* from strace.c 433 */
struct strace *shadow_straces; /* from strace.c 434 */
struct strace_defer_free *strace_defer_free_head; /* from strace.c 435 */
GtkWidget *window_strace_c_10; /* from strace.c 436 */
void (*cleanup_strace_c_7)(); /* from strace.c 437 */
char logical_mutex[6]; /* from strace.c 440 */   
char shadow_logical_mutex[6]; /* from strace.c 441 */
char shadow_active; /* from strace.c 442 */
char shadow_type; /* from strace.c 443 */
char *shadow_string; /* from strace.c 444 */
signed char mark_idx_start; /* from strace.c 445 */
signed char mark_idx_end; /* from strace.c 446 */
signed char shadow_mark_idx_start; /* from strace.c 447 */
signed char shadow_mark_idx_end; /* from strace.c 448 */
struct mprintf_buff_t *mprintf_buff_head; /* from strace.c 451 */
struct mprintf_buff_t *mprintf_buff_current; /* from strace.c 452 */
};


void strace_search(int direction);
void strace_maketimetrace(int mode); /* 1=create, zero=delete */
TimeType strace_adjust(TimeType a, TimeType b);

void swap_strace_contexts(void);
void delete_strace_context(void);
void cache_actual_pattern_mark_traces(void);

void edge_search(int direction); /* from edgebuttons.c */

int mprintf(const char *fmt, ... );
void delete_mprintf(void);

#endif

/*
 * $Id$
 * $Log$
 * Revision 1.4  2008/10/17 14:42:35  gtkwave
 * added findNextEdge/findPrevEdge to tcl interpreter
 *
 * Revision 1.3  2007/12/30 04:27:39  gtkwave
 * added edge buttons to main window
 *
 * Revision 1.2  2007/08/26 21:35:45  gtkwave
 * integrated global context management from SystemOfCode2007 branch
 *
 * Revision 1.1.1.1.2.2  2007/08/25 19:43:46  gtkwave
 * header cleanups
 *
 * Revision 1.1.1.1.2.1  2007/08/05 02:27:23  kermin
 * Semi working global struct
 *
 * Revision 1.1.1.1  2007/05/30 04:27:20  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:17  gtkwave
 * initial release
 *
 */

