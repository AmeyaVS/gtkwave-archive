/* 
 * Copyright (c) Tony Bybell 1999-2006.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef VCD_H
#define VCD_H

#include <stdio.h> 
#include <stdlib.h>

#ifndef _MSC_VER
#include <unistd.h>
#endif

#ifndef HAVE_FSEEKO
#define fseeko fseek
#define ftello ftell
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "symbol.h"
#include "wavealloca.h"
#include "debug.h"
#include "tree.h"

#define VCD_SIZE_WARN (256)	/* number of MB size where converter warning message appears */
#define VCD_BSIZ 32768	/* size of getch() emulation buffer--this val should be ok */
#define VCD_INDEXSIZ  (8 * 1024 * 1024)

enum VCDName_ByteSubstitutions { VCDNAM_NULL=0, VCDNAM_HIERSORT, VCDNAM_ESCAPE };

/* fix for contrib/rtlbrowse */
#ifndef VLEX_DEFINES_H
enum VarTypes { V_EVENT, V_PARAMETER,
                V_INTEGER, V_REAL, V_REAL_PARAMETER=V_REAL, V_REG, V_SUPPLY0,
                V_SUPPLY1, V_TIME, V_TRI, V_TRIAND, V_TRIOR,
                V_TRIREG, V_TRI0, V_TRI1, V_WAND, V_WIRE, V_WOR, V_PORT, V_IN=V_PORT, V_OUT=V_PORT, V_INOUT=V_PORT,
                V_END, V_LB, V_COLON, V_RB, V_STRING };
#endif

TimeType vcd_main(char *fname);
TimeType vcd_recoder_main(char *fname);

TimeType vcd_partial_main(char *fname);
void kick_partial_vcd(void);


struct sym_chain
{
struct sym_chain *next;
struct symbol *val;
};

struct slist
{  
struct slist *next;
char *str;
int len;
};


struct vcdsymbol
{
struct vcdsymbol *root, *chain;
struct symbol *sym_chain;

struct vcdsymbol *next;
char *name;
char *id;
unsigned int nid;
unsigned char vartype;
int msi, lsi;
int size;
char *value;
struct queuedevent *ev; /* only if vartype==V_EVENT */
struct Node **narray;
hptr         *tr_array;   /* points to synthesized trailers (which can move) */
hptr         *app_array;   /* points to hptr to append to (which can move) */
};


struct queuedevent
{
struct queuedevent *next;
struct vcdsymbol *sym;
TimeType last_event_time;    /* make +1 == 0 if there's not an event there too */
};

extern int splash_disable;

extern int vcd_warning_filesize;
extern char autocoalesce;
extern char autocoalesce_reversal;
extern int vcd_explicit_zero_subscripts;  /* 0=yes, -1=no */
extern char vcd_preserve_glitches;
extern char convert_to_reals;
extern char atomic_vectors;
extern char make_vcd_save_file;
extern FILE *vcd_save_handle;

extern struct slist *slistroot, *slistcurr;     
extern char *slisthier;     
extern int slisthier_len;  
extern char vcd_hier_delimeter[2];

char *build_slisthier(void);
void append_vcd_slisthier(char *str);

struct HistEnt *histent_calloc(void);
void strcpy_vcdalt(char *too, char *from, char delim);
int strcpy_delimfix(char *too, char *from);
void vcd_sortfacs(void);


void vcd_import_masked(void);
void vcd_set_fac_process_mask(nptr np);
void import_vcd_trace(nptr np);

int vcd_keyword_code(const char *s, unsigned int len);

#endif

/*
 * $Id$
 * $Log$
 * Revision 1.2  2007/04/20 02:08:17  gtkwave
 * initial release
 *
 */

