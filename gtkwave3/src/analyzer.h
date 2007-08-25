/* 
 * Copyright (c) Tony Bybell 1999-2005.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include "globals.h"

#ifndef ANALYZER_H
#define ANALYZER_H

#include <gtk/gtk.h>
#include <stdlib.h>
#include "wavealloca.h"
#include "vlist.h"
#include "debug.h"

typedef struct _SearchProgressData {
    GtkWidget *window;
    GtkWidget *pbar;
    GtkAdjustment *adj;
    int timer;	/* might be used later.. */
    gfloat value, oldvalue;
} SearchProgressData;


typedef struct ExpandInfo *eptr;
typedef struct ExpandReferences *exptr;
typedef struct Node	  *nptr;
typedef struct HistEnt	  *hptr;
typedef struct Bits       *bptr;
typedef struct VectorEnt  *vptr;
typedef struct BitVector  *bvptr;
typedef struct BitAttributes *baptr;

typedef unsigned long  Ulong;
typedef unsigned int   Uint;


/* vvv   Bit representation   vvv */

enum AnalyzerBits  { AN_0, AN_X, AN_Z, AN_1, AN_H, AN_U, AN_W, AN_L, AN_DASH, AN_RSV9, AN_RSVA, AN_RSVB, AN_RSVC, AN_RSVD, AN_RSVE, AN_RSVF, AN_COUNT };
#define AN_NORMAL  { AN_0, AN_X, AN_Z, AN_1, AN_H, AN_U, AN_W, AN_L, AN_DASH, AN_DASH, AN_DASH, AN_DASH, AN_DASH, AN_DASH, AN_DASH, AN_DASH }
#define AN_INVERSE { AN_1, AN_X, AN_Z, AN_0, AN_L, AN_U, AN_W, AN_H, AN_DASH, AN_DASH, AN_DASH, AN_DASH, AN_DASH, AN_DASH, AN_DASH, AN_DASH }

#define AN_MSK     (AN_COUNT-1) /* max index into AN_STR, AN_COUNT *must* be a power of two unless logic AND with AN_MSK is changed */

/* positional ascii 0123456789ABCDEF, question marks should not happen unless something slips through the cracks as AN_RSVA to AN_RSVF are reserved */
#define AN_STR     "0XZ1HUWL-???????"
#define AN_STR_INV "1XZ0LUWH-???????"

/* for writing out 4 state formats (read GHW, write LXT) */
#define AN_STR4ST  "0XZ11XZ0XXXXXXXX"

/* now the recoded "extra" values... */
#define RCV_X (1 | (0<<1))
#define RCV_Z (1 | (1<<1))
#define RCV_H (1 | (2<<1))
#define RCV_U (1 | (3<<1))
#define RCV_W (1 | (4<<1))
#define RCV_L (1 | (5<<1))
#define RCV_D (1 | (6<<1))

#define RCV_STR "XZHUWL-?"
/*               01234567 */

/* ^^^   Bit representation   ^^^ */


typedef struct HistEnt
{
hptr next;	      /* next transition in history */

union
  {
  unsigned char h_val;  /* value: AN_STR[val] or AnalyzerBits which correspond */
  char *h_vector;	/* pointer to a whole vector of h_val type bits */
  } v;

TimeType time;        /* time of transition */
unsigned char flags;  /* so far only set on glitch/real condition */
} HistEnt;

enum HistEntFlagBits
{ HIST_GLITCH_B, HIST_REAL_B, HIST_STRING_B 
};

#define HIST_GLITCH (1<<HIST_GLITCH_B)
#define HIST_REAL   (1<<HIST_REAL_B)

#ifndef STRICT_VCD_ONLY
	#define HIST_STRING (1<<HIST_STRING_B)
#else
	#define HIST_STRING 0	/* for gcc -O2 optimization */
#endif

typedef struct VectorEnt
{
vptr next;
TimeType time;
unsigned char v[1];
} VectorEnt;


#define MAX_HISTENT_TIME (~( (ULLDescriptor(-1)) << (sizeof(TimeType) * 8 - 1)))


typedef struct ExpandInfo	/* only used when expanding atomic vex.. */
{
nptr	*narray;
int	msb, lsb;
int	width;
} ExpandInfo;

typedef struct ExpandReferences
{
nptr   parent;			/* which atomic vec we expanded from */
int    parentbit;		/* which bit from that atomic vec */
int    actual;			/* bit number to be used in [] */
int    refcnt;
} ExpandReferences;

typedef struct ExtNode
  {
  int msi, lsi;
  } ExtNode;


struct Node
  {
    exptr    expansion; /* indicates which nptr this node was expanded from (if it was expanded at all) and (when implemented) refcnts */
    char     *nname;	/* ascii name of node */
    ExtNode  *ext;	/* extension to node for vectors */    
    HistEnt  head;	/* first entry in transition history */
    hptr     curr;      /* ptr. to current history entry */

    hptr     *harray;   /* fill this in when we make a trace.. contains  */
			/*  a ptr to an array of histents for bsearching */
    union {
      struct fac *mvlfac; /* for use with mvlsim aets */
      struct vlist_t *mvlfac_vlist;
      char *value;	/* for use when unrolling ae2 values */
    } mv; 		/* anon union is a gcc extension so use mv instead.  using this union avoids crazy casting warnings */

    int      numhist;	/* number of elements in the harray */
    unsigned int array_height, this_row;
  };

typedef struct BitAttributes
  {
  TimeType  shift;
  unsigned int flags;
  } BitAttributes;

typedef struct Bits
  {
    char    *name;		/* name of this vector of bits   */
    int     nbits;		/* number of bits in this vector */
    baptr attribs;  		/* for keeping track of combined timeshifts and inversions (and for savefile) */
    
    nptr    nodes[1];		/* pointers to the bits (nodes)  */
  } Bits;

typedef struct BitVector
  {
    char    *name;		/* name of this vector of bits           */
    int     nbits;		/* number of bits in this vector         */
    int     numregions;		/* number of regions that follow         */
    bptr    bits;		/* pointer to Bits structs for save file */
    vptr    vectors[1];		/* pointers to the vectors               */
  } BitVector;


typedef struct
  {
    TimeType    first;		/* beginning time of trace */
    TimeType    last;		/* end time of trace */
    TimeType    start;		/* beginning time of trace on screen */
    TimeType    end;		/* ending time of trace on screen */
    TimeType    marker;
    TimeType    prevmarker;	/* from last drawmarker()	        */
    TimeType    lmbcache;	/* initial marker pos                   */
    TimeType    timecache;	/* to get around floating pt limitation */
    TimeType    laststart;      /* caches last set value                */
    TimeType    baseline;       /* baseline (center button) marker      */

    gdouble    	zoom;		/* current zoom  */
    gdouble    	prevzoom;	/* for zoom undo */
  } Times;

typedef struct TraceEnt *Trptr;

typedef struct
  {
    int      total;		/* total number of traces */
    int      visible;		/* total number of (uncollapsed) traces */
    Trptr    first;		/* ptr. to first trace in list */
    Trptr    last;		/* end of list of traces */
    Trptr    buffer;		/* cut/copy buffer of traces */
    Trptr    bufferlast;	/* last element of bufferchain */
    int      buffercount;	/* number of traces in buffer */
  } Traces;


typedef struct
  {
    Trptr    buffer;            /* cut/copy buffer of traces */
    Trptr    bufferlast;        /* last element of bufferchain */
    int      buffercount;       /* number of traces in buffer */
  } TempBuffer;

typedef struct TraceEnt
  {
    Trptr    t_next;		/* doubly linked list of traces */
    Trptr    t_prev;

    Trptr    t_group;
    char     *t_group_name;

    char     *name;		/* name stripped of path */
    char     *asciivalue;	/* value that marker points to */
    TimeType asciitime;		/* time this value corresponds with */
    TimeType shift;		/* offset added to all entries in the trace */
    TimeType shift_drag;	/* cached initial offset for CTRL+LMB drag on highlighted */

    int	     f_filter;		/* file filter */
    int	     p_filter;		/* process filter */

    unsigned is_alias : 1;	/* set when it's an alias (safe to free t->name then) */
    unsigned vector : 1;	/* 1 if bit vector, 0 if node */
    unsigned shift_drag_valid : 1; /* qualifies shift_drag above */
    unsigned interactive_vector_needs_regeneration : 1; /* for interactive VCDs */

    unsigned int flags;		/* see def below in TraceEntFlagBits */
    union
      {
	nptr    nd;		/* what makes up this trace */
	bvptr   vec;
      } n;
  } TraceEnt;


enum TraceEntFlagBits
{ TR_HIGHLIGHT_B, TR_HEX_B, TR_DEC_B, TR_BIN_B, TR_OCT_B, 
  TR_RJUSTIFY_B, TR_INVERT_B, TR_REVERSE_B, TR_EXCLUDE_B,
  TR_BLANK_B, TR_SIGNED_B, TR_ASCII_B, TR_COLLAPSED_B, TR_FTRANSLATED_B, TR_PTRANSLATED_B,
  TR_ANALOG_STEP_B, TR_ANALOG_INTERPOLATED_B, TR_ANALOG_BLANK_STRETCH_B, TR_REAL_B
};
 
#define TR_HIGHLIGHT 		(1<<TR_HIGHLIGHT_B)
#define TR_HEX			(1<<TR_HEX_B)
#define TR_ASCII		(1<<TR_ASCII_B)
#define TR_DEC			(1<<TR_DEC_B)
#define TR_BIN			(1<<TR_BIN_B)
#define TR_OCT			(1<<TR_OCT_B)
#define TR_RJUSTIFY		(1<<TR_RJUSTIFY_B)
#define TR_INVERT		(1<<TR_INVERT_B)
#define TR_REVERSE		(1<<TR_REVERSE_B)
#define TR_EXCLUDE		(1<<TR_EXCLUDE_B)
#define TR_BLANK		(1<<TR_BLANK_B)
#define TR_SIGNED		(1<<TR_SIGNED_B)
#define TR_ANALOG_STEP 		(1<<TR_ANALOG_STEP_B)
#define TR_ANALOG_INTERPOLATED	(1<<TR_ANALOG_INTERPOLATED_B)
#define TR_ANALOG_BLANK_STRETCH	(1<<TR_ANALOG_BLANK_STRETCH_B)
#define TR_REAL			(1<<TR_REAL_B)

#define TR_NUMMASK	(TR_ASCII|TR_HEX|TR_DEC|TR_BIN|TR_OCT|TR_SIGNED|TR_REAL)

#define TR_COLLAPSED	(1<<TR_COLLAPSED_B)
#define TR_ISCOLLAPSED	(TR_BLANK|TR_COLLAPSED)

#define TR_FTRANSLATED	(1<<TR_FTRANSLATED_B)
#define TR_PTRANSLATED	(1<<TR_PTRANSLATED_B)

#define TR_ANALOGMASK	(TR_ANALOG_STEP|TR_ANALOG_INTERPOLATED)

Trptr GiveNextTrace(Trptr t);
Trptr GivePrevTrace(Trptr t);
int CollapseTrace(Trptr t);
int UpdateTracesVisible(void);
void CollapseAllGroups(void);
void ExpandAllGroups(void);

void DisplayTraces(int val);
int AddNodeTraceReturn(nptr nd, char *aliasname, Trptr *tret);
int AddNode(nptr nd, char *aliasname);
int AddNodeUnroll(nptr nd, char *aliasname);
int AddVector(bvptr vec);
int AddBlankTrace(char *commentname);
int InsertBlankTrace(char *comment, int different_flags);
void RemoveNode(nptr n);
void RemoveTrace(Trptr t, int dofree);
void FreeTrace(Trptr t);
Trptr CutBuffer(void);
void FreeCutBuffer(void);
Trptr PasteBuffer(void);
Trptr PrependBuffer(void);
int TracesAlphabetize(int mode);
int TracesReverse(void);

void import_trace(nptr np);

eptr ExpandNode(nptr n);
void DeleteNode(nptr n);
nptr ExtractNodeSingleBit(nptr n, int bit);



/* hierarchy depths */
char *hier_extract(char *pnt, int levels);

/* vector matching */
char *attempt_vecmatch(char *s1, char *s2);

#endif

/*
 * $Id$
 * $Log$
 * Revision 1.1.1.1.2.1  2007/08/05 02:27:18  kermin
 * Semi working global struct
 *
 * Revision 1.1.1.1  2007/05/30 04:27:50  gtkwave
 * Imported sources
 *
 * Revision 1.4  2007/05/28 00:55:05  gtkwave
 * added support for arrays as a first class dumpfile datatype
 *
 * Revision 1.3  2007/04/29 04:13:49  gtkwave
 * changed anon union defined in struct Node to a named one as anon unions
 * are a gcc extension
 *
 * Revision 1.2  2007/04/20 02:08:11  gtkwave
 * initial release
 *
 */
