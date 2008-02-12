/*
 * Copyright (c) Tony Bybell 1999-2008.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include "globals.h"
#include <config.h>
#include "analyzer.h"
#include "currenttime.h"
#include "symbol.h"
#include "bsearch.h"
#include "strace.h"
#include <ctype.h>


static int compar_timechain(const void *s1, const void *s2)
{
TimeType key, obj, delta;
TimeType *cpos;
int rv;

key=*((TimeType *)s1);
obj=*(cpos=(TimeType *)s2);

if((obj<=key)&&(obj>GLOBALS->max_compare_time_tc_bsearch_c_1))
	{
	GLOBALS->max_compare_time_tc_bsearch_c_1=obj;
	GLOBALS->max_compare_pos_tc_bsearch_c_1=cpos;
	}

delta=key-obj;
if(delta<0) rv=-1;
else if(delta>0) rv=1;
else rv=0;

return(rv);
}

int bsearch_timechain(TimeType key)
{
void *bsearch_dummy_rv;

GLOBALS->max_compare_time_tc_bsearch_c_1=-2+GLOBALS->shift_timebase; GLOBALS->max_compare_pos_tc_bsearch_c_1=NULL; 

if(!GLOBALS->timearray) return(-1);

bsearch_dummy_rv = bsearch(&key, GLOBALS->timearray, GLOBALS->timearray_size, sizeof(TimeType), compar_timechain);
if((!GLOBALS->max_compare_pos_tc_bsearch_c_1)||(GLOBALS->max_compare_time_tc_bsearch_c_1<GLOBALS->shift_timebase)) 
	{
	GLOBALS->max_compare_pos_tc_bsearch_c_1=GLOBALS->timearray; /* aix bsearch fix */
	}

return(GLOBALS->max_compare_pos_tc_bsearch_c_1-GLOBALS->timearray);
}

/*****************************************************************************************/

static int compar_histent(const void *s1, const void *s2)
{
TimeType key, obj, delta;
hptr cpos;
int rv;

key=*((TimeType *)s1);
obj=(cpos=(*((hptr *)s2)))->time+GLOBALS->shift_timebase;

if((obj<=key)&&(obj>GLOBALS->max_compare_time_bsearch_c_1))
	{
	GLOBALS->max_compare_time_bsearch_c_1=obj;
	GLOBALS->max_compare_pos_bsearch_c_1=cpos;
	GLOBALS->max_compare_index=(hptr *)s2;
	}

delta=key-obj;
if(delta<0) rv=-1;
else if(delta>0) rv=1;
else rv=0;

return(rv);
}

hptr bsearch_node(nptr n, TimeType key)
{
void *bsearch_dummy_rv;

GLOBALS->max_compare_time_bsearch_c_1=-2+GLOBALS->shift_timebase; GLOBALS->max_compare_pos_bsearch_c_1=NULL; GLOBALS->max_compare_index=NULL;

bsearch_dummy_rv = bsearch(&key, n->harray, n->numhist, sizeof(hptr), compar_histent);
if((!GLOBALS->max_compare_pos_bsearch_c_1)||(GLOBALS->max_compare_time_bsearch_c_1<GLOBALS->shift_timebase)) 
	{
	GLOBALS->max_compare_pos_bsearch_c_1=n->harray[1]; /* aix bsearch fix */
	GLOBALS->max_compare_index=&(n->harray[1]); 
	}

return(GLOBALS->max_compare_pos_bsearch_c_1);
}

/*****************************************************************************************/


static int compar_vectorent(const void *s1, const void *s2)
{
TimeType key, obj, delta;
vptr cpos;
int rv;

key=*((TimeType *)s1);
obj=(cpos=(*((vptr *)s2)))->time+GLOBALS->shift_timebase;

if((obj<=key)&&(obj>GLOBALS->vmax_compare_time_bsearch_c_1))
	{
	GLOBALS->vmax_compare_time_bsearch_c_1=obj;
	GLOBALS->vmax_compare_pos_bsearch_c_1=cpos;
        GLOBALS->vmax_compare_index=(vptr *)s2;
	}

delta=key-obj;
if(delta<0) rv=-1;
else if(delta>0) rv=1;
else rv=0;

return(rv);
}

vptr bsearch_vector(bvptr b, TimeType key)
{
void *bsearch_dummy_rv;

GLOBALS->vmax_compare_time_bsearch_c_1=-2+GLOBALS->shift_timebase; GLOBALS->vmax_compare_pos_bsearch_c_1=NULL; GLOBALS->vmax_compare_index=NULL;

bsearch_dummy_rv = bsearch(&key, b->vectors, b->numregions, sizeof(vptr), compar_vectorent);
if((!GLOBALS->vmax_compare_pos_bsearch_c_1)||(GLOBALS->vmax_compare_time_bsearch_c_1<GLOBALS->shift_timebase)) 
	{
	GLOBALS->vmax_compare_pos_bsearch_c_1=b->vectors[1]; /* aix bsearch fix */
	GLOBALS->vmax_compare_index=&(b->vectors[1]);
	}

return(GLOBALS->vmax_compare_pos_bsearch_c_1);
}

/*****************************************************************************************/


static int compar_trunc(const void *s1, const void *s2)
{
char *str;
char vcache[2];
int key, obj;

str=(char *)s2;
key=*((int*)s1);

vcache[0]=*str;
vcache[1]=*(str+1);
*str='+';
*(str+1)=0;
obj=font_engine_string_measure(GLOBALS->wavefont,GLOBALS->trunc_asciibase_bsearch_c_1);
*str=vcache[0];
*(str+1)=vcache[1];

if((obj<=key)&&(obj>GLOBALS->maxlen_trunc))
        {
        GLOBALS->maxlen_trunc=obj;
        GLOBALS->maxlen_trunc_pos_bsearch_c_1=str;
        }

return(key-obj);
}


char *bsearch_trunc(char *ascii, int maxlen)
{
void *bsearch_dummy_rv;
int len;

if((maxlen<=0)||(!ascii)||(!(len=strlen(ascii)))) return(NULL);

GLOBALS->maxlen_trunc=0; GLOBALS->maxlen_trunc_pos_bsearch_c_1=NULL;

bsearch_dummy_rv = bsearch(&maxlen, GLOBALS->trunc_asciibase_bsearch_c_1=ascii, len, sizeof(char), compar_trunc);
return(GLOBALS->maxlen_trunc_pos_bsearch_c_1);
}

/*****************************************************************************************/

static int compar_facs(const void *key, const void *v2)
{
struct symbol *s2;
int rc;

s2=*((struct symbol **)v2);
rc=sigcmp((char *)key,s2->name);
return(rc);
}

struct symbol *bsearch_facs(char *ascii, unsigned int *rows_return)
{
struct symbol **rc;
int len;

if ((!ascii)||(!(len=strlen(ascii)))) return(NULL);
if(rows_return)
	{
	*rows_return = 0;
	}

if(ascii[len-1]=='}')
	{
	int i;

	for(i=len-2;i>=2;i--)
		{	
		if(isdigit(ascii[i])) continue;
		if(ascii[i]=='{')
			{
			char *tsc = wave_alloca(i+1);
			memcpy(tsc, ascii, i+1);
			tsc[i] = 0;
			rc=(struct symbol **)bsearch(tsc, GLOBALS->facs, GLOBALS->numfacs, sizeof(struct symbol *), compar_facs);
			if(rc) 
				{
				unsigned int whichrow = atoi(&ascii[i+1]);
				if(rows_return) *rows_return = whichrow;

				if(whichrow <= (*rc)->n->array_height) 
					{	
					return(*rc);
					}
				}
			}
		break;	/* fallthrough to normal handler */
		}
	
	}

rc=(struct symbol **)bsearch(ascii, GLOBALS->facs, GLOBALS->numfacs, sizeof(struct symbol *), compar_facs);
if(rc) return(*rc); else return(NULL);
}

/*
 * $Id$
 * $Log$
 * Revision 1.3  2008/02/08 02:26:36  gtkwave
 * anti-aliased font support add
 *
 * Revision 1.2  2007/08/26 21:35:39  gtkwave
 * integrated global context management from SystemOfCode2007 branch
 *
 * Revision 1.1.1.1.2.3  2007/08/07 03:18:54  kermin
 * Changed to pointer based GLOBAL structure and added initialization function
 *
 * Revision 1.1.1.1.2.2  2007/08/06 03:50:45  gtkwave
 * globals support for ae2, gtk1, cygwin, mingw.  also cleaned up some machine
 * generated structs, etc.
 *
 * Revision 1.1.1.1.2.1  2007/08/05 02:27:18  kermin
 * Semi working global struct
 *
 * Revision 1.1.1.1  2007/05/30 04:27:55  gtkwave
 * Imported sources
 *
 * Revision 1.3  2007/05/28 00:55:05  gtkwave
 * added support for arrays as a first class dumpfile datatype
 *
 * Revision 1.2  2007/04/20 02:08:11  gtkwave
 * initial release
 *
 */

