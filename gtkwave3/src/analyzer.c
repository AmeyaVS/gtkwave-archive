/* 
 * Copyright (c) Tony Bybell 1999-2005.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */
/* AIX may need this for alloca to work */ 
#if defined _AIX
  #pragma alloca
#endif

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol.h"
#include "lxt.h"
#include "debug.h"
#include "bsearch.h"
#include "strace.h"
#include "translate.h"
#include "ptranslate.h"

#ifdef _MSC_VER
#define strcasecmp _stricmp
#endif

unsigned int default_flags=TR_RJUSTIFY;
Times        tims = {0, 0, 0, 0, 0, 0, 0};
Traces       traces = {0, 0, NULL, NULL, NULL, NULL, 0};

/*
 * extract last n levels of hierarchy
 */
int hier_max_level=0;

char *hier_extract(char *pnt, int levels)
{
int i, len;
char ch, *pnt2, *esc;
char only_nums_so_far=1;

if(!pnt) return(NULL);

len=strlen(pnt);
if(!len) return(pnt);

if(levels<1) levels=1;
if((esc=strchr(pnt, '\\'))) 
	{
	return((levels==1) ? esc : pnt); /* shortcut out on escape IDs: level=1, esc char else all */
	}

pnt2=pnt+len-1;
ch=*pnt2;

for(i=0;i<len;i++)
	{
	ch=*(pnt2--);
	if((only_nums_so_far)&&(ch>='0')&&(ch<='9'))		/* skip 1st set of signal.number hier from right if it exists */
		{
		continue;
		/* nothing */
		}
	else
		{
		if(ch==hier_delimeter) 
			{
			if(!only_nums_so_far) levels--;
			if(!levels)
				{
				pnt2+=2;
				return(pnt2);
				}
			}
		only_nums_so_far=0;
		}
	}

return(pnt); /* not as many levels as max, so give the full name.. */
}


/* 
 * Add a trace to the display...
 */
static void AddTrace( Trptr t )
{
if(default_flags&TR_NUMMASK) t->flags=default_flags;
	else t->flags=(t->flags&TR_NUMMASK)|default_flags;

if(default_flags & TR_FTRANSLATED)
	{
	t->f_filter = current_translate_file;
	}
else
if(default_flags & TR_PTRANSLATED)
	{
	t->p_filter = current_translate_proc;
	}


if(shift_timebase_default_for_add)
	t->shift=shift_timebase_default_for_add;

if(!shadow_active)
	{
	if( traces.first == NULL )
		{
		t->t_next = t->t_prev = NULL;
		traces.first = traces.last = t;
	      	}
	    	else
	      	{
		t->t_next = NULL;
		t->t_prev = traces.last;
		traces.last->t_next = t;
		traces.last = t;
	      	}
	traces.total++;
	}
	else	/* hide offscreen */
	{
	struct strace *st = calloc_2(1, sizeof(struct strace));
	st->next = shadow_straces;
	st->value = shadow_type;
	st->trace = t;

	st->string = shadow_string;	/* copy string over */
	shadow_string = NULL;

	shadow_straces = st;
	}
}


/*
 * Add a blank trace to the display...
 */
static char *precondition_string(char *s)
{
int len=0;
char *s2;

if(!s) return(NULL);
s2=s;
while((*s2)&&((*s2)!='\n')&&((*s2)!='\r'))	/* strip off ending CR/LF */
	{
	len++;
	s2++;
	}
if(!len) return(NULL);
s2=(char *)calloc_2(1,len+1);
memcpy(s2,s,len);
return(s2);
}

int AddBlankTrace(char *commentname)
{
Trptr  t;
char *comment;

if( (t = (Trptr) calloc_2( 1, sizeof( TraceEnt ))) == NULL )
	{
	fprintf( stderr, "Out of memory, can't add blank trace to analyzer\n");
	return( 0 );
      	}
AddTrace(t);
t->flags = TR_BLANK | (default_flags & (TR_COLLAPSED|TR_ANALOG_BLANK_STRETCH));
if(t->flags & TR_ANALOG_BLANK_STRETCH)
	{
	t->flags &= ~TR_BLANK;
	}

if((comment=precondition_string(commentname)))
	{
	t->name=comment;
	t->is_alias=1;
	}

return(1);
}


/*
 * Insert a blank [or comment] trace into the display...
 */
int InsertBlankTrace(char *comment, int different_flags)
{
TempBuffer tb;
char *comm;
Trptr  t;

if( (t = (Trptr) calloc_2( 1, sizeof( TraceEnt ))) == NULL )
	{
	fprintf( stderr, "Out of memory, can't insert blank trace to analyzer\n");
	return( 0 );
      	}

if(!different_flags)
	{
	t->flags=TR_BLANK;
	}
	else
	{
	t->flags = different_flags;
	}

if((comm=precondition_string(comment)))
	{
	t->name=comm;
	t->is_alias=1;
	}

if(!traces.first)
	{
	traces.first=traces.last=t;
	traces.total=1;
	return(1);
	}
	else
	{
	tb.buffer=traces.buffer;
	tb.bufferlast=traces.bufferlast;
	tb.buffercount=traces.buffercount;
	
	traces.buffer=traces.bufferlast=t;
	traces.buffercount=1;
	PasteBuffer();

	traces.buffer=tb.buffer;
	traces.bufferlast=tb.bufferlast;
	traces.buffercount=tb.buffercount;

	return(1);
	}
}


/*
 * Adds a single bit signal to the display...
 */
int AddNodeTraceReturn(nptr nd, char *aliasname, Trptr *tret)
  {
    Trptr  t;
    hptr histpnt;
    hptr *harray;
    int histcount;
    int i;

    if(!nd) return(0); /* passed it a null node ptr by mistake */
    if(nd->mv.mvlfac) import_trace(nd);

    signalwindow_width_dirty=1;
    
    if( (t = (Trptr) calloc_2( 1, sizeof( TraceEnt ))) == NULL )
      {
	fprintf( stderr, "Out of memory, can't add %s to analyzer\n",
	  nd->nname );
	return( 0 );
      }

if(!nd->harray)		/* make quick array lookup for aet display */
	{
	histpnt=&(nd->head);
	histcount=0;

	while(histpnt)
		{
		histcount++;
		histpnt=histpnt->next;
		}

	nd->numhist=histcount;
	
	if(!(nd->harray=harray=(hptr *)malloc_2(histcount*sizeof(hptr))))
		{
		fprintf( stderr, "Out of memory, can't add %s to analyzer\n",
		  	nd->nname );
		free_2(t);
		return(0);
		}

	histpnt=&(nd->head);
	for(i=0;i<histcount;i++)
		{
		*harray=histpnt;

		/* printf("%s, time: %d, val: %d\n", nd->nname, 
			(*harray)->time, (*harray)->val); */

		harray++;
		histpnt=histpnt->next;
		}
	}

if(aliasname)
	{	
	char *alias;

	t->name=alias=(char *)malloc_2((strlen(aliasname)+2)+1);
	strcpy(alias,"+ "); /* use plus sign to mark aliases */
	strcpy(alias+2,aliasname);
        t->is_alias=1;	/* means can be freed later */
	}
	else
	{
    	if(!hier_max_level) 
		{
		t->name = nd->nname;
		}
		else
		{
		t->name = hier_extract(nd->nname, hier_max_level);
		}
	}

    if(nd->ext)	/* expansion vectors */
	{	
	int n;

	n = nd->ext->msi - nd->ext->lsi;
	if(n<0)n=-n;
	n++;

	t->flags = (( n > 3 )||( n < -3 )) ? TR_HEX|TR_RJUSTIFY : TR_BIN|TR_RJUSTIFY;
	}
	else
	{
	t->flags |= TR_BIN;	/* binary */
	}
    t->vector = FALSE;
    t->n.nd = nd;
    if(tret) *tret = t;		/* for expand */
    AddTrace( t );
    return( 1 );
  }


/* single node */
int AddNode(nptr nd, char *aliasname)
{
return(AddNodeTraceReturn(nd, aliasname, NULL));
}


/* add multiple nodes (if array) */
int AddNodeUnroll(nptr nd, char *aliasname)
{
if(nd->array_height <= 1)
	{
	return(AddNodeTraceReturn(nd, aliasname, NULL));
	}
	else
	{
	int i;
	int rc = 1;

	for(i=0;i<nd->array_height;i++)
		{
		rc |= AddNodeTraceReturn(nd+i, aliasname, NULL);
		}
	return(rc);
	}
}


/*
 * Adds a vector to the display...
 */
int AddVector( bvptr vec )
  {
    Trptr  t;
    int    n;

    if(!vec) return(0); /* must've passed it a null pointer by mistake */

    signalwindow_width_dirty=1;

    n = vec->nbits;
    t = (Trptr) calloc_2(1, sizeof( TraceEnt ) );
    if( t == NULL )
      {
	fprintf( stderr, "Out of memory, can't add %s to analyzer\n",
	  vec->name );
	return( 0 );
      }

    if(!hier_max_level)
	{	
    	t->name = vec->name;
	}
	else
	{
	t->name = hier_extract(vec->name, hier_max_level);
	}
    t->flags = ( n > 3 ) ? TR_HEX|TR_RJUSTIFY : TR_BIN|TR_RJUSTIFY;
    t->vector = TRUE;
    t->n.vec = vec;
    AddTrace( t );
    return( 1 );
  }


/*
 * Free up a trace's mallocs...
 */
void FreeTrace(Trptr t)
{
if(straces)
	{
	struct strace_defer_free *sd = calloc_2(1, sizeof(struct strace_defer_free));
	sd->next = strace_defer_free_head;
	sd->defer = t;

	strace_defer_free_head = sd;
	return;
	}

if(t->vector)
      	{
      	bvptr bv;
	int i;

	bv=t->n.vec;
	for(i=0;i<bv->numregions;i++)
		{
		if(bv->vectors[i]) free_2(bv->vectors[i]);
		}
	
	if(bv->bits)
		{
		if(bv->bits->name) free_2(bv->bits->name);
		if(bv->bits->attribs) free_2(bv->bits->attribs);
		for(i=0;i<bv->nbits;i++)
			{
			DeleteNode(bv->bits->nodes[i]);
			}
		free_2(bv->bits);
		}

	if(bv->name) free_2(bv->name);
      	if(t->n.vec)free_2(t->n.vec);
      	}
	else
	{
	if(t->n.nd && t->n.nd->expansion)
		{
		DeleteNode(t->n.nd);
		}
	}

if(t->asciivalue) free_2(t->asciivalue);
if((t->is_alias)&&(t->name)) free_2(t->name);

free_2( t );
}


/*
 * Remove a trace from the display and optionally 
 * deallocate its memory usage...
 */ 
void RemoveTrace( Trptr t, int dofree )
  {
    traces.total--;
    if( t == traces.first )
      {
	traces.first = t->t_next;
	if( t->t_next )
            t->t_next->t_prev = NULL;
        else
            traces.last = NULL;
      }
    else
      {
        t->t_prev->t_next = t->t_next;
        if( t->t_next )
            t->t_next->t_prev = t->t_prev;
        else
            traces.last = t->t_prev;
      }
    
    if(dofree)
	{
        FreeTrace(t);
	}
  }


/*
 * Deallocate the cut/paste buffer...
 */
void FreeCutBuffer(void)
{
Trptr t, t2;

t=traces.buffer;

while(t)
	{
	t2=t->t_next;
	FreeTrace(t);
	t=t2;	
	}

traces.buffer=traces.bufferlast=NULL;
traces.buffercount=0;
}


/*
 * Cut highlighted traces from the main screen
 * and throw them in the cut buffer.  If anything's
 * in the cut buffer, deallocate it first...
 */
Trptr CutBuffer(void)
{
Trptr t, tnext;
Trptr first=NULL, current=NULL;

shift_click_trace=NULL;		/* so shift-clicking doesn't explode */

t=traces.first;
while(t)
	{
	if((t->flags)&(TR_HIGHLIGHT)) break;
	t=t->t_next;
	}
if(!t) return(NULL);	/* keeps a double cut from blowing out the buffer */

signalwindow_width_dirty=1;

FreeCutBuffer();

/*
 * propagate cut for whole comment group if comment selected and collapsed...
 */
t=traces.first;
while(t)
	{
	top_of_cut:
	if( (t->name) && ((t->flags&(TR_BLANK|TR_HIGHLIGHT))==(TR_BLANK|TR_HIGHLIGHT)) )
		{
		if(t->flags&TR_COLLAPSED)
			{
			t=t->t_next;
			while(t)
				{
				if(t->flags & TR_BLANK) goto top_of_cut;
				t->flags |= TR_HIGHLIGHT;
				t=t->t_next;
				}
			break;
			}	
			else
			{
			t=t->t_next; /* for sanity, ensure we're uncollapsed */
			while(t)
				{
				if(t->flags & TR_BLANK) goto top_of_cut;
				t->flags &= ~TR_COLLAPSED;
				t=t->t_next;
				}
			break;
			}
		}

	t=t->t_next;
	}

t=traces.first;
while(t)
	{
	tnext=t->t_next;
	if(t->flags&TR_HIGHLIGHT)
		{
		traces.bufferlast=t;
		traces.buffercount++;

		t->flags&=(~TR_HIGHLIGHT);
		RemoveTrace(t, 0);
		if(!current)
			{
			first=current=t;
			t->t_prev=NULL;
			t->t_next=NULL;
			}
			else
			{
			current->t_next=t;
			t->t_prev=current;
			current=t;
			t->t_next=NULL;
			}
		}
	t=tnext;
	}

return(traces.buffer=first);
}


/*
 * Paste the cut buffer into the main display one and
 * mark the cut buffer empty...
 */
Trptr PasteBuffer(void)
{
Trptr t, tinsert=NULL, tinsertnext;

if(!traces.buffer) return(NULL);

signalwindow_width_dirty=1;

if(!(t=traces.first))
	{
	t=traces.last=traces.first=traces.buffer;
	while(t)
		{
		traces.last=t;
		traces.total++;
		t=t->t_next;
		}	

	traces.buffer=traces.bufferlast=NULL;
	traces.buffercount=0;

	return(traces.first);
	}

while(t)
	{
	if(t->flags&TR_HIGHLIGHT) 
		{
		if((t->flags & (TR_BLANK|TR_COLLAPSED)) && (t->name))
			{
			tinsert=t;
			t=t->t_next;
			while(t)
				{
				if(t->flags & TR_BLANK) goto nxtl;
				tinsert=t;
				t=t->t_next;
				}
			break;
			}
			else
			{
			tinsert=t;
			}
		}

nxtl:	t=t->t_next;
	}

if(!tinsert) tinsert=traces.last;

tinsertnext=tinsert->t_next;
tinsert->t_next=traces.buffer;
traces.buffer->t_prev=tinsert;
traces.bufferlast->t_next=tinsertnext;
traces.total+=traces.buffercount;

if(!tinsertnext)
	{
	traces.last=traces.bufferlast;
	}
	else
	{
	tinsertnext->t_prev=traces.bufferlast;
	}

traces.buffer=traces.bufferlast=NULL;
traces.buffercount=0;

return(traces.first);
}


/*
 * Prepend the cut buffer into the main display one and
 * mark the cut buffer empty...
 */
Trptr PrependBuffer(void)
{
Trptr t, prev;

if(!traces.buffer) return(NULL);

signalwindow_width_dirty=1;

t=traces.buffer;

while(t)
	{
	prev=t;
	t->flags&=(~TR_HIGHLIGHT);
	traces.total++;
	t=t->t_next;
	}

if((prev->t_next=traces.first))
	{
	/* traces.last current value is ok as it stays the same */
	traces.first->t_prev=prev; /* but we need the reverse link back up */
	}
	else
	{
	traces.last=prev;
	}

traces.first=traces.buffer;

traces.buffer=traces.bufferlast=NULL;
traces.buffercount=0;

return(traces.first);
}


/*
 * reversal of traces
 */
int TracesReverse(void)
{
Trptr t;
Trptr *tsort, *tsort_pnt;
int i;
   
if(!traces.total) return(0);

t=traces.first;
tsort=tsort_pnt=wave_alloca(sizeof(Trptr)*traces.total);   
for(i=0;i<traces.total;i++)
        {
        if(!t)
                {
                fprintf(stderr, "INTERNAL ERROR: traces.total vs traversal mismatch!  Exiting.\n");
                exit(255);
                }
        *(tsort_pnt++)=t;

        t=t->t_next;
        }

traces.first=*(--tsort_pnt);

for(i=traces.total-1;i>=0;i--)
        {
        t=*tsort_pnt;

	if(i==traces.total-1)
		{
		t->t_prev=NULL;
		}
		else
		{
		t->t_prev=*(tsort_pnt+1);
		}

	if(i)
		{
		t->t_next=*(--tsort_pnt);
		}
        }

traces.last=*tsort;
traces.last->t_next=NULL;

return(1);
}  


/*************************************************************/


/*
 * sort on tracename pointers (alpha/caseins alpha/sig sort)
 */
static int tracenamecompare(const void *s1, const void *s2)
{
char *str1, *str2;

str1=(*((Trptr *)s1))->name;
str2=(*((Trptr *)s2))->name;

if((!str1) || (!*str1))	/* force blank lines to go to bottom */
	{
	if((!str2) || (!*str2))
		{
		return(0);
		}
		else
		{
		return(1);
		}
	}
else
if((!str2) || (!*str2))
	{
	return(-1);		/* str1==str2==zero case is covered above */
	}
  
return(strcmp(str1, str2));
}


static int traceinamecompare(const void *s1, const void *s2)
{
char *str1, *str2;

str1=(*((Trptr *)s1))->name;
str2=(*((Trptr *)s2))->name;

if((!str1) || (!*str1))	/* force blank lines to go to bottom */
	{
	if((!str2) || (!*str2))
		{
		return(0);
		}
		else
		{
		return(1);
		}
	}
else
if((!str2) || (!*str2))
	{
	return(-1);		/* str1==str2==zero case is covered above */
	}
  
return(strcasecmp(str1, str2));
}

static int tracesignamecompare(const void *s1, const void *s2)
{
char *str1, *str2;

str1=(*((Trptr *)s1))->name;
str2=(*((Trptr *)s2))->name;

if((!str1) || (!*str1))	/* force blank lines to go to bottom */
	{
	if((!str2) || (!*str2))
		{
		return(0);
		}
		else
		{
		return(1);
		}
	}
else
if((!str2) || (!*str2))
	{
	return(-1);		/* str1==str2==zero case is covered above */
	}
  
return(sigcmp(str1, str2));
}


/*
 * alphabetization of traces
 */
int TracesAlphabetize(int mode)
{
Trptr t, prev = NULL;
Trptr *tsort, *tsort_pnt;
char *subst, ch;
int i;
int (*cptr)(const void*, const void*);
   
if(!traces.total) return(0);

t=traces.first;
tsort=tsort_pnt=wave_alloca(sizeof(Trptr)*traces.total);   
for(i=0;i<traces.total;i++)
        {
        if(!t)
                {
                fprintf(stderr, "INTERNAL ERROR: traces.total vs traversal mismatch!  Exiting.\n");
                exit(255);
                }
        *(tsort_pnt++)=t;

	if((subst=t->name))
	        while((ch=(*subst)))
        	        {
        	        if(ch==hier_delimeter) { *subst=VCDNAM_HIERSORT; } /* forces sort at hier boundaries */
        	        subst++;
        	        }

        t=t->t_next;
        }

switch(mode)
	{
	case 0:		cptr=traceinamecompare;   break;
	case 1:		cptr=tracenamecompare;	  break;
	default:	cptr=tracesignamecompare; break;
	}

qsort(tsort, traces.total, sizeof(Trptr), cptr);

tsort_pnt=tsort;
for(i=0;i<traces.total;i++)
        {
        t=*(tsort_pnt++);

	if(!i)
		{
		traces.first=t;
		t->t_prev=NULL;
		}
		else
		{
		prev->t_next=t;
		t->t_prev=prev;
		}

	prev=t;

	if((subst=t->name))
	        while((ch=(*subst)))
        	        {
        	        if(ch==VCDNAM_HIERSORT) { *subst=hier_delimeter; } /* restore hier */
        	        subst++;
        	        }
        }

traces.last=prev;
prev->t_next=NULL;

return(1);
}  


/*
 * trace traversal with collapsed groups
 */
Trptr GiveNextTrace(Trptr t)
{
if((t->name)&&((t->flags & TR_ISCOLLAPSED)==TR_ISCOLLAPSED))
	{
	t=t->t_next;
	while(t)
		{
		if(t->flags & TR_BLANK) break;
		t=t->t_next;
		}
	}
	else
	{
	t=t->t_next;
	}

return(t);
}


Trptr GivePrevTrace(Trptr t)
{
Trptr t2 = t;
int iter = 0;

while(t2)
	{
	if((t2->flags & TR_COLLAPSED) && (!(t2->flags & TR_BLANK)))
		{
		if(!iter)
			{
			t2=t2->t_prev;
			iter=1;
			continue;
			}
			else
			{
			break;
			}
		}
		else
		{
		if(iter) break;
		return(t2->t_prev);
		}
	}

return(t2);
}


int CollapseTrace(Trptr t)
{
int rc;
int flg;

if((rc=((t->name)&&(t->flags&TR_BLANK))))
	{
        Trptr t2 = t;

        t2->flags ^= TR_COLLAPSED;
	flg = t2->flags & TR_COLLAPSED;
	t2->flags &= ~TR_HIGHLIGHT;

        t2=t->t_next;
        while(t2)
        	{
                if(t2->flags & TR_BLANK) break;
                t2->flags &= ~(TR_COLLAPSED|TR_HIGHLIGHT);
		t2->flags |= flg;
                t2=t2->t_next;
                }

	UpdateTracesVisible();
	}

return(rc);
}


int UpdateTracesVisible(void)
{
Trptr t = traces.first;
int cnt = 0;

while(t)
	{
	if( (t->flags & TR_BLANK) || (!(t->flags & TR_COLLAPSED)) )
		{
		cnt++;
		}

	t=t->t_next;
	}

traces.visible = cnt;
return(cnt);
}


void CollapseAllGroups(void)
{
Trptr t = traces.first;
int mode = 0;

while(t)
	{
	t->flags &= ~TR_HIGHLIGHT;

	if(t->flags & TR_BLANK)
		{
		mode = (t->name != NULL);
		}

	if(mode)
		{
		t->flags |= TR_COLLAPSED;
		}
		else
		{
		t->flags &= ~TR_COLLAPSED;
		}

	t=t->t_next;
	}

UpdateTracesVisible();
}


void ExpandAllGroups(void)
{
Trptr t = traces.first;

while(t)
	{
	t->flags &= ~(TR_HIGHLIGHT|TR_COLLAPSED);
	t=t->t_next;
	}

UpdateTracesVisible();
}

/*
 * $Id$
 * $Log$
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

