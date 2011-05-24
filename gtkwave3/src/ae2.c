/* 
 * Copyright (c) Tony Bybell 2004-2011.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include "globals.h"
#include <config.h>
#include <stdio.h>

#ifndef _MSC_VER
#include <unistd.h>
#endif

#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include "ae2.h"
#include "symbol.h"
#include "vcd.h"
#include "lxt.h"
#include "lxt2_read.h"
#include "fgetdynamic.h"
#include "debug.h"
#include "busy.h"
#include "hierpack.h"


/* 
 * select appropriate entry points based on if aet2
 * support is available
 */
#ifndef AET2_IS_PRESENT 

const char *ae2_loader_fail_msg = "Sorry, AET2 support was not compiled into this executable, exiting.\n\n";

TimeType ae2_main(char *fname, char *skip_start, char *skip_end)
{
fprintf(stderr, "%s", ae2_loader_fail_msg);
exit(255);

return(0); /* for vc++ */
}

void ae2_import_masked(void)
{
fprintf(stderr, "%s", ae2_loader_fail_msg);
exit(255);
}

#else


/*
 * iter mask manipulation util functions
 */
int aet2_rd_get_fac_process_mask(unsigned int facidx)
{
if(facidx<GLOBALS->numfacs)
	{
	int process_idx = facidx/8;
	int process_bit = facidx&7;

	return( (GLOBALS->ae2_process_mask[process_idx]&(1<<process_bit)) != 0 );
	}

return(0);
}


void aet2_rd_set_fac_process_mask(unsigned int facidx)
{
if(facidx<GLOBALS->numfacs)
	{
	int idx = facidx/8;
	int bitpos = facidx&7;

	GLOBALS->ae2_process_mask[idx] |= (1<<bitpos);
	}
}


void aet2_rd_clr_fac_process_mask(unsigned int facidx)
{
if(facidx<GLOBALS->numfacs)
	{
	int idx = facidx/8;
	int bitpos = facidx&7;

	GLOBALS->ae2_process_mask[idx] &= (~(1<<bitpos));
	}
}


static void error_fn(const char *format, ...)
{ 
va_list ap;
va_start(ap, format);
vfprintf(stderr, format, ap);
fprintf(stderr, "\n");
va_end(ap);
exit(255);
}

static char *twirler = "|/-\\";

static void msg_fn(int sev, const char *format, ...)
{
if((!GLOBALS->ae2_msg_suppress)||(sev))
	{
	va_list ap;
	va_start(ap, format);

	fprintf(stderr, "AE2 %03d | ", sev);
	vfprintf(stderr, format, ap);
	fprintf(stderr, "\n");

	va_end(ap);
	}
	else
	{
	va_list ap;
	va_start(ap, format);

	fprintf(stderr, "AE2 %03d | ", sev);
	vfprintf(stderr, format, ap);
	fprintf(stderr, " %c\r", twirler[GLOBALS->ae2_twirl_pos]);
	GLOBALS->ae2_twirl_pos = (GLOBALS->ae2_twirl_pos+1) & 3;
	GLOBALS->ae2_did_twirl = 1;

	va_end(ap);
	}
}

#ifdef AET2_ALIASDB_IS_PRESENT
static void adb_msg_fn(unsigned long sev, const char *format, ...)
{
GLOBALS->ae2_msg_suppress = 0;

if((!GLOBALS->ae2_msg_suppress)||(sev))
        {
        va_list ap;
        va_start(ap, format);

        fprintf(stderr, "AE2 %03d | ", (unsigned int)sev);
        vfprintf(stderr, format, ap);
        fprintf(stderr, "\n");

        va_end(ap);
        }
        else
        {
        va_list ap;
        va_start(ap, format);

        fprintf(stderr, "AE2 %03d | ", (unsigned int)sev);
        vfprintf(stderr, format, ap);
        fprintf(stderr, " %c\r", twirler[GLOBALS->ae2_twirl_pos]);
        GLOBALS->ae2_twirl_pos = (GLOBALS->ae2_twirl_pos+1) & 3;
        GLOBALS->ae2_did_twirl = 1;

        va_end(ap);
        }
}
#endif

static void *alloc_fn(size_t size)
{        
void *pnt = calloc_2(1, size);
return(pnt);
}
        
static void free_fn(void* ptr, size_t size)
{
if(ptr)
	{
	free_2(ptr);
	}
}


/*
 * dynamic alias allocator (memory written to is converted to refer to facidx instead)
 */
#ifdef AET2_ALIASDB_IS_PRESENT

static void *adb_alloc_2(size_t siz)
{
if(GLOBALS->adb_alloc_pool_base)
        {
        if((siz + GLOBALS->adb_alloc_idx) <= WAVE_ADB_ALLOC_POOL_SIZE)
                {
                unsigned char *m = GLOBALS->adb_alloc_pool_base + GLOBALS->adb_alloc_idx;
                GLOBALS->adb_alloc_idx += siz;
                return((void *)m);
                }
        else
        if(siz >= WAVE_ADB_ALLOC_ALTREQ_SIZE)
                {
                return(calloc_2(1, siz));
                }
        }

GLOBALS->adb_alloc_pool_base = calloc_2(1, WAVE_ADB_ALLOC_POOL_SIZE);
GLOBALS->adb_alloc_idx = 0;
return(adb_alloc_2(siz));
}
#endif


/*
 * dynamic alias support on reads
 */
#ifdef AET2_ALIASDB_IS_PRESENT

static unsigned long ae2_read_symbol_rows_2(AE2_HANDLE handle, unsigned long symbol_idx)
{
if(symbol_idx <= GLOBALS->ae2_num_facs)
	{
	return(ae2_read_symbol_rows(handle, symbol_idx));
	}
	else
	{
	return(1);
	}
}


static void ae2_read_value_2a(AE2_HANDLE handle, unsigned long idx, uint64_t cycle, char* value, int tot_length)
{
int i;
int numTerms = GLOBALS->adb_num_terms[--idx];
int length;
int j;
int offs = 0;

if(numTerms)
	{
	for(i=0;i<numTerms;i++)
		{
		struct ADB_TERM *at = &GLOBALS->adb_aliases[idx][i];
		FACREF fr2;
	
		fr2.s = at->id;
		fr2.row = 0;
		fr2.row_high = 0;
		fr2.offset = at->first;

		if(at->last >= at->first)
			{
			fr2.length = length = at->last - at->first + 1;
			}
			else
			{
			fr2.length = -(length = at->first - at->last + 1);
			}

		ae2_read_value(handle, &fr2, cycle, value+offs);
		offs += length;
		}
	}
	else
	{
	memset(value, 'Z', tot_length);
	}

value[tot_length] = 0;
}


static uint64_t ae2_read_next_value_2a(AE2_HANDLE handle, unsigned long idx, uint64_t cycle, char* value, int tot_length)
{
uint64_t cyc = GLOBALS->max_time + 1;
uint64_t t_cyc;
int i;
int numTerms = GLOBALS->adb_num_terms[--idx];
int length;
int j;
int offs = 0;
int nonew = 0;

if(numTerms)
	{
	for(i=0;i<numTerms;i++)
		{
		struct ADB_TERM *at = &GLOBALS->adb_aliases[idx][i];
		FACREF fr2;
	
		fr2.s = at->id;
		fr2.row = 0;
		fr2.row_high = 0;
		fr2.offset = at->first;

		if(at->last >= at->first)
			{
			fr2.length = length = at->last - at->first + 1;
			}
			else
			{
			fr2.length = -(length = at->first - at->last + 1);
			}
		
		t_cyc = ae2_read_next_value(handle, &fr2, cycle, value+offs); /* simply want to calculate next value change time */
		if(t_cyc != cycle)
			{
			if(t_cyc < cyc) { cyc = t_cyc; }
			}
			else
			{
			nonew++;
			}

		offs += length;
		}

	if(nonew == numTerms) { cyc = cycle; }

	ae2_read_value_2a(handle, idx+1, cyc, value, tot_length); /* reread at that calculated value change time */
	}
	else
	{
	memset(value, 'Z', tot_length);
	value[tot_length] = 0;
	}

return(cyc);
}


static void ae2_read_value_2(AE2_HANDLE handle, FACREF* fr, uint64_t cycle, char* value)
{
if(fr->s <= GLOBALS->ae2_num_facs)
	{
	ae2_read_value(handle, fr, cycle, value);
	}
	else /* complex alias not supported yet */
	{
	unsigned long idx = fr->s - GLOBALS->ae2_num_facs;
	ae2_read_value_2a(handle, idx, cycle, value, fr->length);
	}
}


static uint64_t ae2_read_next_value_2(AE2_HANDLE handle, FACREF* fr, uint64_t cycle, char* value)
{
if(fr->s <= GLOBALS->ae2_num_facs)
        {
	return(ae2_read_next_value(handle, fr, cycle, value));
	}
	else /* complex alias not supported yet */
	{
	unsigned long idx = fr->s - GLOBALS->ae2_num_facs;
	return(ae2_read_next_value_2a(handle, idx, cycle, value, fr->length));
	}
}

#else

#define ae2_read_symbol_rows_2(a,b) ae2_read_symbol_rows((a),(b))
#define ae2_read_value_2(a,b,c,d) ae2_read_value((a),(b),(c),(d))
#define ae2_read_next_value_2(a,b,c,d) ae2_read_next_value((a),(b),(c),(d))

#endif


/*
 * mainline
 */
TimeType ae2_main(char *fname, char *skip_start, char *skip_end)
{
int i;
int match_idx;
struct Node *n;
struct symbol *s;
TimeType first_cycle, last_cycle, total_cycles;
int total_rows = 0;
int mono_row_offset = 0;
struct Node *monolithic_node = NULL;
struct symbol *monolithic_sym = NULL;
#ifdef AET2_ALIASDB_IS_PRESENT
unsigned long kw;
#endif

ae2_initialize(error_fn, msg_fn, alloc_fn, free_fn);

if ( (!(GLOBALS->ae2_f=fopen(fname, "rb"))) || (!(GLOBALS->ae2 = ae2_read_initialize(GLOBALS->ae2_f))) )
        {
	if(GLOBALS->ae2_f)
		{
		fclose(GLOBALS->ae2_f);
		GLOBALS->ae2_f = NULL;
		}

        return(LLDescriptor(0));        /* look at GLOBALS->ae2 in caller for success status... */
        }


GLOBALS->time_dimension = 'n';

if(!GLOBALS->fast_tree_sort)
        {
        GLOBALS->do_hier_compress = 0;
        }

init_facility_pack();

/* SPLASH */                            splash_create();

sym_hash_initialize(GLOBALS);

#ifdef AET2_ALIASDB_IS_PRESENT
kw = ae2_read_locate_keyword(GLOBALS->ae2, "aliasdb");
if(kw)
        {
        GLOBALS->adb_alias_stream_file = ae2_read_keyword_stream(GLOBALS->ae2, kw);
        GLOBALS->adb = adb_open_embed(GLOBALS->adb_alias_stream_file, "-order", alloc_fn, free_fn, adb_msg_fn, error_fn);
        if(GLOBALS->adb)
                {
                GLOBALS->ae2_num_aliases = adb_num_aliases(GLOBALS->adb);
                GLOBALS->adb_max_terms  = adb_max_alias_terms(GLOBALS->adb);
                GLOBALS->adb_terms = calloc_2(GLOBALS->adb_max_terms + 1, sizeof(struct ADB_TERM));

                GLOBALS->adb_aliases = calloc_2(GLOBALS->ae2_num_aliases, sizeof(struct ADB_TERM *));
		GLOBALS->adb_num_terms = calloc_2(GLOBALS->ae2_num_aliases, sizeof(unsigned short));
		GLOBALS->adb_idx_first = calloc_2(GLOBALS->ae2_num_aliases, sizeof(unsigned short));
		GLOBALS->adb_idx_last = calloc_2(GLOBALS->ae2_num_aliases, sizeof(unsigned short));

		fprintf(stderr, AET2_RDLOAD"Encountered %d aliases.\n", (unsigned int)GLOBALS->ae2_num_aliases);
                }
        }
#endif

GLOBALS->ae2_num_sections=ae2_read_num_sections(GLOBALS->ae2);
GLOBALS->ae2_num_facs = ae2_read_num_symbols(GLOBALS->ae2);
GLOBALS->numfacs = GLOBALS->ae2_num_facs + GLOBALS->ae2_num_aliases;
GLOBALS->ae2_process_mask = calloc_2(1, GLOBALS->numfacs/8+1);

GLOBALS->ae2_fr=calloc_2(GLOBALS->numfacs, sizeof(FACREF));
GLOBALS->ae2_lx2_table=(struct lx2_entry **)calloc_2(GLOBALS->numfacs, sizeof(struct lx2_entry *));

match_idx = 0;
for(i=0;i<GLOBALS->ae2_num_facs;i++)
	{
        int idx = i+1;

        GLOBALS->ae2_fr[match_idx].facname = NULL;
        GLOBALS->ae2_fr[match_idx].row = ae2_read_symbol_rows(GLOBALS->ae2, idx);
	total_rows += (GLOBALS->ae2_fr[match_idx].row > 0) ? GLOBALS->ae2_fr[match_idx].row : 1;
	if(GLOBALS->ae2_fr[match_idx].row == 1) GLOBALS->ae2_fr[match_idx].row = 0;
        GLOBALS->ae2_fr[match_idx].length = ae2_read_symbol_length(GLOBALS->ae2, idx);
        GLOBALS->ae2_fr[match_idx].s = idx;
        GLOBALS->ae2_fr[match_idx].row_high = 0;
        GLOBALS->ae2_fr[match_idx].offset = 0;

	match_idx++;
	}

#ifdef AET2_ALIASDB_IS_PRESENT
for(i=0;i<GLOBALS->ae2_num_aliases;i++)
	{
	unsigned long numTerms;
	unsigned long u;
        int idx = i+1;
	int ii;
	FACREF f2;
        char buf[AE2_MAX_NAME_LENGTH+1];

	total_rows++;

	if((numTerms = adb_load_alias_def(GLOBALS->adb, idx, GLOBALS->adb_terms)))
		{
		if(GLOBALS->adb_terms[0].first > GLOBALS->adb_terms[0].last)
			{
			GLOBALS->ae2_fr[match_idx].length = GLOBALS->adb_terms[0].first - GLOBALS->adb_terms[0].last + 1;
			}
			else
			{
			GLOBALS->ae2_fr[match_idx].length = GLOBALS->adb_terms[0].last - GLOBALS->adb_terms[0].first + 1;
			}

		GLOBALS->adb_idx_first[i] = GLOBALS->adb_terms[0].first;
		GLOBALS->adb_idx_last[i] = GLOBALS->adb_terms[0].last;

	        GLOBALS->ae2_fr[match_idx].s = idx + GLOBALS->ae2_num_facs; /* bias aliases after regular facs */

		GLOBALS->ae2_fr[match_idx].facname = NULL;
		GLOBALS->ae2_fr[match_idx].row = 0;
	        GLOBALS->ae2_fr[match_idx].row_high = 0;
	        GLOBALS->ae2_fr[match_idx].offset = 0;

		GLOBALS->adb_num_terms[i] = numTerms;
		GLOBALS->adb_aliases[i] = adb_alloc_2(numTerms * sizeof(struct ADB_TERM));

		for(ii=0;ii<(numTerms);ii++)
			{
	 	       	adb_symbol_name(GLOBALS->adb, GLOBALS->adb_terms[ii+1].id, buf);
			u = ae2_read_find_symbol(GLOBALS->ae2, buf, &f2);

			GLOBALS->adb_aliases[i][ii].id = u;
			GLOBALS->adb_aliases[i][ii].first = GLOBALS->adb_terms[ii+1].first;
			GLOBALS->adb_aliases[i][ii].last = GLOBALS->adb_terms[ii+1].last;
        		}
		}
		else
		{
                adb_symbol_name(GLOBALS->adb, GLOBALS->adb_terms[1].id, buf);
                u = ae2_read_find_symbol(GLOBALS->ae2, buf, &GLOBALS->ae2_fr[match_idx]);
		memcpy(&GLOBALS->ae2_fr[match_idx], &GLOBALS->ae2_fr[u-1], sizeof(struct facref));

		GLOBALS->adb_idx_first[i] = 0;
		GLOBALS->adb_idx_last[i] = GLOBALS->ae2_fr[u-1].length - 1;
		}

	match_idx++;
	}
#endif

monolithic_node = calloc_2(total_rows, sizeof(struct Node));
monolithic_sym = calloc_2(match_idx, sizeof(struct symbol));

fprintf(stderr, AET2_RDLOAD"Finished building %d facs.\n", match_idx);
/* SPLASH */                            splash_sync(1, 5);

first_cycle = (TimeType) ae2_read_start_cycle(GLOBALS->ae2);
last_cycle = (TimeType) ae2_read_end_cycle(GLOBALS->ae2);
total_cycles = last_cycle - first_cycle + 1;

/* do your stuff here..all useful info has been initialized by now */

if(!GLOBALS->hier_was_explicitly_set)    /* set default hierarchy split char */
        {
        GLOBALS->hier_delimeter='.';
        }

match_idx = 0;
for(i=0;i<GLOBALS->numfacs;i++)
        {
	char *str;	
        char buf[AE2_MAX_NAME_LENGTH+1];
        int idx;
	unsigned long len, clen;
	int row_iter, mx_row, mx_row_adjusted;

#ifdef AET2_ALIASDB_IS_PRESENT
	if(i < GLOBALS->ae2_num_facs)
#endif
		{
		idx = i+1;
		len = ae2_read_symbol_name(GLOBALS->ae2, idx, buf);
		}
#ifdef AET2_ALIASDB_IS_PRESENT
		else
		{
		idx = i - GLOBALS->ae2_num_facs + 1;
		len = adb_alias_name(GLOBALS->adb, idx, buf) - 1; /* it counts the null character */
		}
#endif

	if(GLOBALS->ae2_fr[match_idx].length>1)
		{
		int len2;

#ifdef AET2_ALIASDB_IS_PRESENT
		if(i < GLOBALS->ae2_num_facs)
#endif
			{
			len2 = sprintf(buf+len, "[0:%d]", GLOBALS->ae2_fr[match_idx].length-1);
			}
#ifdef AET2_ALIASDB_IS_PRESENT
			else
			{
			len2 = sprintf(buf+len, "[%d:%d]", GLOBALS->adb_idx_first[i - GLOBALS->ae2_num_facs], GLOBALS->adb_idx_last[i - GLOBALS->ae2_num_facs]);
			}
#endif

		clen = (len + len2 + 1);
                if(!GLOBALS->do_hier_compress)
                        {
			str=malloc_2(clen);
                        }
                        else
                        {
                        str = buf;
                        }

		if(clen > GLOBALS->longestname) GLOBALS->longestname = clen;
		if(!GLOBALS->alt_hier_delimeter)
			{
			if(!GLOBALS->do_hier_compress) strcpy(str, buf);
			}
			else
			{
			strcpy_vcdalt(str, buf, GLOBALS->alt_hier_delimeter);
			}
		s = &monolithic_sym[match_idx];
	        symadd_name_exists_sym_exists(s, str,0);
		}
		else
		{
		clen = (len+1);
                if(!GLOBALS->do_hier_compress)
                        {
			str=malloc_2(clen);
                        }
                        else
                        {
                        str = buf;
                        }

		if(clen > GLOBALS->longestname) GLOBALS->longestname = clen;
		if(!GLOBALS->alt_hier_delimeter)
			{
			if(!GLOBALS->do_hier_compress) strcpy(str, buf);
			}
			else
			{
			strcpy_vcdalt(str, buf, GLOBALS->alt_hier_delimeter);
			}
		s = &monolithic_sym[match_idx];
	        symadd_name_exists_sym_exists(s, str,0);
		}
		
        mx_row = (GLOBALS->ae2_fr[match_idx].row < 1) ? 1 : GLOBALS->ae2_fr[match_idx].row;
	mx_row_adjusted = (mx_row < 2) ? 0 : mx_row;
        n=&monolithic_node[mono_row_offset];
	s->n = n;
	mono_row_offset += mx_row;

        if(GLOBALS->do_hier_compress)
                {
                s->name = compress_facility((unsigned char *)str, clen - 1);
                }

	for(row_iter = 0; row_iter < mx_row; row_iter++)
		{
	        n[row_iter].nname=s->name;
	        n[row_iter].mv.mvlfac = (struct fac *)(GLOBALS->ae2_fr+match_idx); /* to keep from having to allocate duplicate mvlfac struct */
							               /* use the info in the FACREF array instead                */
		n[row_iter].array_height = mx_row_adjusted;
		n[row_iter].this_row = row_iter;

		if(GLOBALS->ae2_fr[match_idx].length>1)
			{
#ifdef AET2_ALIASDB_IS_PRESENT
			if(i < GLOBALS->ae2_num_facs)
#endif
				{
				n[row_iter].msi = 0;
				n[row_iter].lsi = GLOBALS->ae2_fr[match_idx].length-1;
				}
#ifdef AET2_ALIASDB_IS_PRESENT
				else
				{
				n[row_iter].msi = GLOBALS->adb_idx_first[i - GLOBALS->ae2_num_facs];
				n[row_iter].lsi = GLOBALS->adb_idx_last[i - GLOBALS->ae2_num_facs];
				}
#endif
			n[row_iter].extvals = 1;
			}
                 
	        n[row_iter].head.time=-1;        /* mark 1st node as negative time */
	        n[row_iter].head.v.h_val=AN_X;
		}

	match_idx++;
        }

#ifdef AET2_ALIASDB_IS_PRESENT
free_2(GLOBALS->adb_idx_last); GLOBALS->adb_idx_last = NULL;
free_2(GLOBALS->adb_idx_first); GLOBALS->adb_idx_first = NULL;
#endif

freeze_facility_pack();

/* SPLASH */                            splash_sync(2, 5);
GLOBALS->facs=(struct symbol **)malloc_2(GLOBALS->numfacs*sizeof(struct symbol *));

if(GLOBALS->fast_tree_sort)
	{
	for(i=0;i<GLOBALS->numfacs;i++)
		{
		GLOBALS->facs[i]=&monolithic_sym[i];
		}

/* SPLASH */                            splash_sync(3, 5);
	fprintf(stderr, AET2_RDLOAD"Building facility hierarchy tree.\n");

	init_tree();		

	for(i=0;i<GLOBALS->numfacs;i++)	
		{
		int was_packed = HIER_DEPACK_STATIC; /* no need to free_2() afterward then */
		char *sb = hier_decompress_flagged(GLOBALS->facs[i]->name, &was_packed);
		build_tree_from_name(sb, i);
		}

/* SPLASH */                            splash_sync(4, 5);
	treegraft(&GLOBALS->treeroot);

	fprintf(stderr, AET2_RDLOAD"Sorting facility hierarchy tree.\n");
	treesort(GLOBALS->treeroot, NULL);
/* SPLASH */                            splash_sync(5, 5);
	order_facs_from_treesort(GLOBALS->treeroot, &GLOBALS->facs);

	GLOBALS->facs_are_sorted=1;
	}
	else
	{
	for(i=0;i<GLOBALS->numfacs;i++)
		{
#ifdef WAVE_HIERFIX
		char *subst;
		char ch;	
#endif
		GLOBALS->facs[i]=&monolithic_sym[i];
#ifdef WAVE_HIERFIX
		while((ch=(*subst)))
			{	
			if(ch==GLOBALS->hier_delimeter) { *subst=VCDNAM_HIERSORT; }	/* forces sort at hier boundaries */
			subst++;
			}
#endif
		}
	
/* SPLASH */                            splash_sync(3, 5);
	fprintf(stderr, AET2_RDLOAD"Sorting facilities at hierarchy boundaries.\n");
	wave_heapsort(GLOBALS->facs,GLOBALS->numfacs);
	
#ifdef WAVE_HIERFIX
	for(i=0;i<GLOBALS->numfacs;i++)
		{
		char *subst, ch;
	
		subst=GLOBALS->facs[i]->name;
		while((ch=(*subst)))
			{	
			if(ch==VCDNAM_HIERSORT) { *subst=GLOBALS->hier_delimeter; }	/* restore back to normal */
			subst++;
			}
		}
#endif	

	GLOBALS->facs_are_sorted=1;

/* SPLASH */                            splash_sync(4, 5);
	fprintf(stderr, AET2_RDLOAD"Building facility hierarchy tree.\n");

	init_tree();		
	for(i=0;i<GLOBALS->numfacs;i++)	
		{
		build_tree_from_name(GLOBALS->facs[i]->name, i);
		}
/* SPLASH */                            splash_sync(5, 5);
	treegraft(&GLOBALS->treeroot);
	treesort(GLOBALS->treeroot, NULL);
	}


if(GLOBALS->ae2_time_xlate)
	{
	GLOBALS->min_time = GLOBALS->ae2_time_xlate[0];
	GLOBALS->max_time = GLOBALS->ae2_time_xlate[last_cycle - first_cycle];
	}
	else
	{
	GLOBALS->min_time = first_cycle; GLOBALS->max_time=last_cycle;
	}

GLOBALS->ae2_start_cyc = GLOBALS->ae2_start_limit_cyc = first_cycle;
GLOBALS->ae2_end_cyc = GLOBALS->ae2_end_limit_cyc = last_cycle;

GLOBALS->is_lx2 = LXT2_IS_AET2;

if(skip_start || skip_end)
	{
	TimeType b_start, b_end;
	TimeType lim_idx;

	if(!skip_start) b_start = GLOBALS->min_time; else b_start = unformat_time(skip_start, GLOBALS->time_dimension);
	if(!skip_end) b_end = GLOBALS->max_time; else b_end = unformat_time(skip_end, GLOBALS->time_dimension);

	if(b_start<GLOBALS->min_time) b_start = GLOBALS->min_time;
	else if(b_start>GLOBALS->max_time) b_start = GLOBALS->max_time;

	if(b_end<GLOBALS->min_time) b_end = GLOBALS->min_time;
	else if(b_end>GLOBALS->max_time) b_end = GLOBALS->max_time;

        if(b_start > b_end)
                {
		TimeType tmp_time = b_start;
                b_start = b_end;
                b_end = tmp_time;
                }

	GLOBALS->min_time = b_start;
	GLOBALS->max_time = b_end;

	if(GLOBALS->ae2_time_xlate)
		{
		for(lim_idx = first_cycle; lim_idx <= last_cycle; lim_idx++)
			{
			if(GLOBALS->ae2_time_xlate[lim_idx - first_cycle] <= GLOBALS->min_time)
				{
				GLOBALS->ae2_start_limit_cyc = lim_idx;			
				}
	
			if(GLOBALS->ae2_time_xlate[lim_idx - first_cycle] >= GLOBALS->min_time)
				{
				break;
				}
			}
	
		for(; lim_idx <= last_cycle; lim_idx++)
			{
			if(GLOBALS->ae2_time_xlate[lim_idx - first_cycle] >= GLOBALS->max_time)
				{
				GLOBALS->ae2_end_limit_cyc = lim_idx;			
				break;
				}
			}
		}
	}

fprintf(stderr, AET2_RDLOAD"["TTFormat"] start time.\n"AET2_RDLOAD"["TTFormat"] end time.\n", GLOBALS->min_time, GLOBALS->max_time);
return(GLOBALS->max_time);
}


/*
 * ae2 callback
 */
static void ae2_callback(uint64_t *tim, unsigned int *facidx, char **value, unsigned int row)
{
struct HistEnt *htemp = histent_calloc();
struct lx2_entry *l2e = &GLOBALS->ae2_lx2_table[*facidx][row];
FACREF *f = GLOBALS->ae2_fr+(*facidx);

static int busycnt = 0;

busycnt++;
if(busycnt==WAVE_BUSY_ITER)
        {
        busy_window_refresh();
        busycnt = 0;
        }

/* fprintf(stderr, "%lld %d %d %s\n", *tim, *facidx, row, *value); */

if(f->length>1)        
        {
        htemp->v.h_vector = (char *)malloc_2(f->length);
	memcpy(htemp->v.h_vector, *value, f->length);
        }
        else
        {
	switch(**value)
		{
		case '0':	htemp->v.h_val = AN_0; break;
		case '1':	htemp->v.h_val = AN_1; break;
		case 'H':
		case 'Z':
		case 'z':	htemp->v.h_val = AN_Z; break;
		default:	htemp->v.h_val = AN_X; break;
		}
        }

if(!GLOBALS->ae2_time_xlate)
	{
	htemp->time = (*tim);
	}
	else
	{
	htemp->time = GLOBALS->ae2_time_xlate[(*tim) - GLOBALS->ae2_start_cyc];
	}

if(l2e->histent_head)
	{
	l2e->histent_curr->next = htemp;
	l2e->histent_curr = htemp;
	}
	else
	{
	l2e->histent_head = l2e->histent_curr = htemp;
	}

l2e->numtrans++;
}


int ae2_iterator(uint64_t start_cycle, uint64_t end_cycle)
{
unsigned int i, j, r;
uint64_t cyc, ecyc, step_cyc;
struct ae2_ncycle_autosort *deadlist=NULL;
struct ae2_ncycle_autosort *autofacs=NULL;
char buf[AE2_MAXFACLEN+1];


GLOBALS->ae2_msg_suppress = 1;
GLOBALS->ae2_did_twirl = 0;

autofacs = calloc_2(GLOBALS->numfacs, sizeof(struct ae2_ncycle_autosort));

for(i=0;i<GLOBALS->numfacs;i++)
	{
	if(aet2_rd_get_fac_process_mask(i))
		{
		int nr = ae2_read_symbol_rows_2(GLOBALS->ae2,GLOBALS->ae2_fr[i].s);
		if(!nr) nr = 1;
		for(r=0;r<nr;r++)
			{
			nptr np = GLOBALS->ae2_lx2_table[i][r].np;
			np->mv.value = calloc_2(1, GLOBALS->ae2_fr[i].length+1);
			}		
		}
	}


for(j=0;j<GLOBALS->ae2_num_sections;j++)
	{
	struct ae2_ncycle_autosort **autosort = NULL;
	const uint64_t *ith_range = ae2_read_ith_section_range(GLOBALS->ae2, j);

	cyc = *ith_range;
	ecyc = *(ith_range+1);

	if(ecyc<start_cycle) continue;
	if(cyc>end_cycle) break;

	if((ecyc<cyc)||(ecyc==~ULLDescriptor(0))) continue;

	autosort = calloc_2(ecyc - cyc + 1, sizeof(struct ae2_ncycle_autosort *));
	
	for(i=0;i<GLOBALS->numfacs;i++)
		{
		if(aet2_rd_get_fac_process_mask(i))
			{
			int nr = ae2_read_symbol_rows_2(GLOBALS->ae2,GLOBALS->ae2_fr[i].s);

			if(nr<2)
				{
				nptr np = GLOBALS->ae2_lx2_table[i][0].np;
	
				ae2_read_value_2(GLOBALS->ae2, GLOBALS->ae2_fr+i, cyc, buf);
				if(strcmp(np->mv.value, buf))
					{
					strcpy(np->mv.value, buf);
					ae2_callback(&cyc, &i, &np->mv.value, 0);
					}
				}
				else
				{
				unsigned long sf = ae2_read_symbol_sparse_flag(GLOBALS->ae2, GLOBALS->ae2_fr[i].s);
				if(sf)
					{
					int rows = ae2_read_num_sparse_rows(GLOBALS->ae2, GLOBALS->ae2_fr[i].s, cyc);
					if(rows)
						{
			                        for(r=1;r<rows+1;r++)
			                                {
							nptr np; 
			                                uint64_t row = ae2_read_ith_sparse_row(GLOBALS->ae2, GLOBALS->ae2_fr[i].s, cyc, r);
	
			                                GLOBALS->ae2_fr[i].row = row;
	
							np = GLOBALS->ae2_lx2_table[i][row].np;
			                                ae2_read_value_2(GLOBALS->ae2, GLOBALS->ae2_fr+i, cyc, buf);
							if(strcmp(np->mv.value, buf))
								{
								strcpy(np->mv.value, buf);
								ae2_callback(&cyc, &i, &np->mv.value, row);
								}
			                                }
						}
					}
					else
					{
					int rows = ae2_read_symbol_rows_2(GLOBALS->ae2, GLOBALS->ae2_fr[i].s);
					if(rows)
						{
			                        for(r=0;r<rows;r++)
			                                {
							nptr np; 
			                                uint64_t row = r;
	
			                                GLOBALS->ae2_fr[i].row = row;
	
							np = GLOBALS->ae2_lx2_table[i][row].np;
			                                ae2_read_value_2(GLOBALS->ae2, GLOBALS->ae2_fr+i, cyc, buf);
							if(strcmp(np->mv.value, buf))
								{
								strcpy(np->mv.value, buf);
								ae2_callback(&cyc, &i, &np->mv.value, row);
								}
			                                }
						}
					}
				}
			}
		}

	deadlist=NULL;

	for(i=0;i<GLOBALS->numfacs;i++)
		{
		uint64_t ncyc;
		nptr np;
		int nr;

		if(!aet2_rd_get_fac_process_mask(i)) continue;

		nr = ae2_read_symbol_rows_2(GLOBALS->ae2,GLOBALS->ae2_fr[i].s);
		if(nr < 2)
			{
			np = GLOBALS->ae2_lx2_table[i][0].np;
			ncyc =	ae2_read_next_value_2(GLOBALS->ae2, GLOBALS->ae2_fr+i, cyc, np->mv.value);
			}
			else
			{
			unsigned long sf = ae2_read_symbol_sparse_flag(GLOBALS->ae2, GLOBALS->ae2_fr[i].s);
			if(sf)
				{
				int rows = ae2_read_num_sparse_rows(GLOBALS->ae2, GLOBALS->ae2_fr[i].s, cyc);
				uint64_t mxcyc = end_cycle+1;

	                        for(r=1;r<rows+1;r++)
	                                {
					/* nptr np; */
	                                uint64_t row = ae2_read_ith_sparse_row(GLOBALS->ae2, GLOBALS->ae2_fr[i].s, cyc, r);

	                                GLOBALS->ae2_fr[i].row = row;
					/* np = GLOBALS->ae2_lx2_table[i][row].np; */
					ncyc =	ae2_read_next_value_2(GLOBALS->ae2, GLOBALS->ae2_fr+i, cyc, buf);
	
					if((ncyc > cyc) && (ncyc < mxcyc)) mxcyc = ncyc;
					}

				if(mxcyc != (end_cycle+1))
					{
					ncyc = mxcyc;
					}
					else
					{
					ncyc = cyc;
					}
				}
				else
				{
				int rows = ae2_read_symbol_rows_2(GLOBALS->ae2, GLOBALS->ae2_fr[i].s);
				uint64_t mxcyc = end_cycle+1;

	                        for(r=0;r<rows;r++)
	                                {
					/* nptr np; */
	                                uint64_t row = r;

	                                GLOBALS->ae2_fr[i].row = row;
					/* np = GLOBALS->ae2_lx2_table[i][row].np; */
					ncyc =	ae2_read_next_value_2(GLOBALS->ae2, GLOBALS->ae2_fr+i, cyc, buf);
	
					if((ncyc > cyc) && (ncyc < mxcyc)) mxcyc = ncyc;
					}

				if(mxcyc != (end_cycle+1))
					{
					ncyc = mxcyc;
					}
					else
					{
					ncyc = cyc;
					}
				}
			}

		if(ncyc!=cyc)
			{
			int offset = ncyc-cyc;
			struct ae2_ncycle_autosort *t = autosort[offset];
		
			autofacs[i].next = t;
			autosort[offset] = autofacs+i; 
			}
			else
			{
			struct ae2_ncycle_autosort *t = deadlist;
			autofacs[i].next = t;
			deadlist = autofacs+i;
			}
		}

	for(step_cyc = cyc+1 ; step_cyc <= ecyc ; step_cyc++)
		{
		int offset = step_cyc-cyc;
		struct ae2_ncycle_autosort *t = autosort[offset];

		if(step_cyc > end_cycle) break;
	
		if(t)
			{
			while(t)
				{
				uint64_t ncyc;
				struct ae2_ncycle_autosort *tn = t->next;
				nptr np;
				int nr;	
	
				i = t-autofacs;
				nr = ae2_read_symbol_rows_2(GLOBALS->ae2,GLOBALS->ae2_fr[i].s);

				if(nr<2)
					{
					np = GLOBALS->ae2_lx2_table[i][0].np;

					ae2_callback(&step_cyc, &i, &np->mv.value, 0);
		
					ncyc = ae2_read_next_value_2(GLOBALS->ae2, GLOBALS->ae2_fr+i, step_cyc, np->mv.value);
					}
					else
					{
					unsigned long sf = ae2_read_symbol_sparse_flag(GLOBALS->ae2, GLOBALS->ae2_fr[i].s);
					if(sf)
						{
						int rows = ae2_read_num_sparse_rows(GLOBALS->ae2, GLOBALS->ae2_fr[i].s, step_cyc);
						uint64_t mxcyc = end_cycle+1;

			                        for(r=1;r<rows+1;r++)
		        	                        {
							nptr npr; 
			                                uint64_t row = ae2_read_ith_sparse_row(GLOBALS->ae2, GLOBALS->ae2_fr[i].s, step_cyc, r);

			                                GLOBALS->ae2_fr[i].row = row;
							npr = GLOBALS->ae2_lx2_table[i][row].np;

							ae2_read_value_2(GLOBALS->ae2, GLOBALS->ae2_fr+i, step_cyc, buf);
							if(strcmp(buf, npr->mv.value))
								{
								strcpy(npr->mv.value, buf);
								ae2_callback(&step_cyc, &i, &npr->mv.value, row);
								}

							ncyc =	ae2_read_next_value_2(GLOBALS->ae2, GLOBALS->ae2_fr+i, step_cyc, buf);
							if((ncyc > step_cyc) && (ncyc < mxcyc)) mxcyc = ncyc;
							}
	
						if(mxcyc != (end_cycle+1))
							{
							ncyc = mxcyc;
							}
							else
							{
							ncyc = step_cyc;
							}
						}
						else
						{
						int rows = ae2_read_symbol_rows_2(GLOBALS->ae2, GLOBALS->ae2_fr[i].s);
						uint64_t mxcyc = end_cycle+1;

			                        for(r=0;r<rows;r++)
		        	                        {
							nptr npr; 
			                                uint64_t row = r;

			                                GLOBALS->ae2_fr[i].row = row;
							npr = GLOBALS->ae2_lx2_table[i][row].np;

							ae2_read_value_2(GLOBALS->ae2, GLOBALS->ae2_fr+i, step_cyc, buf);
							if(strcmp(buf, npr->mv.value))
								{
								strcpy(npr->mv.value, buf);
								ae2_callback(&step_cyc, &i, &npr->mv.value, row);
								}

							ncyc =	ae2_read_next_value_2(GLOBALS->ae2, GLOBALS->ae2_fr+i, step_cyc, buf);
							if((ncyc > step_cyc) && (ncyc < mxcyc)) mxcyc = ncyc;
							}
	
						if(mxcyc != (end_cycle+1))
							{
							ncyc = mxcyc;
							}
							else
							{
							ncyc = step_cyc;
							}
						}
					}
		
				if(ncyc!=step_cyc)
					{
					int offset2 = ncyc-cyc;
					struct ae2_ncycle_autosort *ta = autosort[offset2];
				
					autofacs[i].next = ta;
					autosort[offset2] = autofacs+i; 
					}
					else
					{
					struct ae2_ncycle_autosort *ta = deadlist;
					autofacs[i].next = ta;
					deadlist = autofacs+i;
					}
				t = tn;
				}
			}
		}

	if(autosort) free_2(autosort);
	}


for(i=0;i<GLOBALS->numfacs;i++)
	{
	if(aet2_rd_get_fac_process_mask(i))
		{
		int nr = ae2_read_symbol_rows_2(GLOBALS->ae2,GLOBALS->ae2_fr[i].s);
		if(!nr) nr = 1;
		for(r=0;r<nr;r++)
			{
			nptr np = GLOBALS->ae2_lx2_table[i][r].np;
			free_2(np->mv.value);
			np->mv.value = NULL;
			}		
		}
	}

free_2(autofacs);

GLOBALS->ae2_msg_suppress = 0;
if(GLOBALS->ae2_did_twirl)
	{
	fprintf(stderr,"\n");
	GLOBALS->ae2_did_twirl = 0;
	}
return(0);
}


/* 
 * actually import an ae2 trace but don't do it if it's already been imported 
 */
void import_ae2_trace(nptr np)
{
struct HistEnt *htemp, *histent_tail;
int len, i;
FACREF *f;
int txidx;
int r, nr;

if(!(f=(FACREF *)(np->mv.mvlfac))) return;	/* already imported */

txidx = f - GLOBALS->ae2_fr;
nr = ae2_read_symbol_rows_2(GLOBALS->ae2, f->s);

/* new stuff */
len = f->length;

if((1)||(f->row <= 1)) /* sorry, arrays not supported yet in the viewer */
	{
	int flagged = HIER_DEPACK_STATIC;
	char *str = hier_decompress_flagged(np->nname, &flagged);
	fprintf(stderr, "Import: %s\n", str);

	if(nr<1) nr=1;
	if(!GLOBALS->ae2_lx2_table[txidx])
		{
	        GLOBALS->ae2_lx2_table[txidx] = calloc_2(nr, sizeof(struct lx2_entry));
	        for(r=0;r<nr;r++)
	                {
	                GLOBALS->ae2_lx2_table[txidx][r].np = &np[r];
	                }
		}

	aet2_rd_set_fac_process_mask(txidx);
	ae2_iterator(GLOBALS->ae2_start_limit_cyc, GLOBALS->ae2_end_limit_cyc);
	aet2_rd_clr_fac_process_mask(txidx);
	}
	else
	{
	int flagged = HIER_DEPACK_STATIC;
	char *str = hier_decompress_flagged(np->nname, &flagged);

	fprintf(stderr, AET2_RDLOAD"Skipping array: %s (%d rows)\n", str, f->row);

	if(nr<1) nr=1;
	if(!GLOBALS->ae2_lx2_table[txidx])
		{
	        GLOBALS->ae2_lx2_table[txidx] = calloc_2(nr, sizeof(struct lx2_entry));
	        for(r=0;r<nr;r++)
	                {
	                GLOBALS->ae2_lx2_table[txidx][r].np = &np[r];
	                }
		}
	}


for(r = 0; r < nr; r++)
	{
	histent_tail = htemp = histent_calloc();
	if(len>1)
		{
		htemp->v.h_vector = (char *)malloc_2(len);
		for(i=0;i<len;i++) htemp->v.h_vector[i] = AN_Z;
		}
		else
		{
		htemp->v.h_val = AN_Z;		/* z */
		}
	htemp->time = MAX_HISTENT_TIME;

	htemp = histent_calloc();
	if(len>1)
		{
		htemp->v.h_vector = (char *)malloc_2(len);
		for(i=0;i<len;i++) htemp->v.h_vector[i] = AN_X;
		}
		else
		{
		htemp->v.h_val = AN_X;		/* x */
		}
	htemp->time = MAX_HISTENT_TIME-1;
	htemp->next = histent_tail;			

	if(GLOBALS->ae2_lx2_table[txidx][r].histent_curr)
		{
		GLOBALS->ae2_lx2_table[txidx][r].histent_curr->next = htemp;
		htemp = GLOBALS->ae2_lx2_table[txidx][r].histent_head;
		}

	if(len>1)
		{
		np[r].head.v.h_vector = (char *)malloc_2(len);
		for(i=0;i<len;i++) np[r].head.v.h_vector[i] = AN_X;
		}
		else
		{
		np[r].head.v.h_val = AN_X;	/* x */
		}


                {
                struct HistEnt *htemp2 = calloc_2(1, sizeof(struct HistEnt));
                htemp2->time = -1;  
                if(len>1)
                	{
                        htemp2->v.h_vector = htemp->v.h_vector;
                        }
                        else
                        {  
                        htemp2->v.h_val = htemp->v.h_val;
                        }
		htemp2->next = htemp;
                htemp = htemp2;
                GLOBALS->ae2_lx2_table[txidx][r].numtrans++;
                }

	np[r].head.time  = -2;
	np[r].head.next = htemp;
	np[r].numhist=GLOBALS->ae2_lx2_table[txidx][r].numtrans +2 /*endcap*/ +1 /*frontcap*/;

	np[r].curr = histent_tail;
	np[r].mv.mvlfac = NULL;	/* it's imported and cached so we can forget it's an mvlfac now */
	}
}


/* 
 * pre-import many traces at once so function above doesn't have to iterate...
 */
void ae2_set_fac_process_mask(nptr np)
{
FACREF *f;
int txidx;
int r, nr;

if(!(f=(FACREF *)(np->mv.mvlfac))) return;	/* already imported */

txidx = f - GLOBALS->ae2_fr;

if((1)||(f->row <= 1)) /* sorry, arrays not supported */
	{
	aet2_rd_set_fac_process_mask(txidx);
	nr = f->row;
	if(!nr) nr=1;
	GLOBALS->ae2_lx2_table[txidx] = calloc_2(nr, sizeof(struct lx2_entry));
	for(r=0;r<nr;r++)
		{
		GLOBALS->ae2_lx2_table[txidx][r].np = &np[r];
		}
	}
}


void ae2_import_masked(void)
{
int txidx, i, cnt=0;

for(txidx=0;txidx<GLOBALS->numfacs;txidx++)
	{
	if(aet2_rd_get_fac_process_mask(txidx))
		{
		cnt++;
		}
	}

if(!cnt) return;

if(cnt>100)
	{
	fprintf(stderr, AET2_RDLOAD"Extracting %d traces\n", cnt);
	}

set_window_busy(NULL);
ae2_iterator(GLOBALS->ae2_start_limit_cyc, GLOBALS->ae2_end_limit_cyc);
set_window_idle(NULL);

for(txidx=0;txidx<GLOBALS->numfacs;txidx++)
	{
	if(aet2_rd_get_fac_process_mask(txidx))
		{
		struct HistEnt *htemp, *histent_tail;
		FACREF *f = GLOBALS->ae2_fr+txidx;
		int r, nr = ae2_read_symbol_rows_2(GLOBALS->ae2, f->s);
		int len = f->length;

		if(nr<1) nr=1;

		for(r = 0; r < nr; r++)
			{
			nptr np = GLOBALS->ae2_lx2_table[txidx][r].np;

			histent_tail = htemp = histent_calloc();
			if(len>1)
				{
				htemp->v.h_vector = (char *)malloc_2(len);
				for(i=0;i<len;i++) htemp->v.h_vector[i] = AN_Z;
				}
				else
				{
				htemp->v.h_val = AN_Z;		/* z */
				}
			htemp->time = MAX_HISTENT_TIME;
			
			htemp = histent_calloc();
			if(len>1)
				{
				htemp->v.h_vector = (char *)malloc_2(len);
				for(i=0;i<len;i++) htemp->v.h_vector[i] = AN_X;
				}
				else
				{
				htemp->v.h_val = AN_X;		/* x */
				}
			htemp->time = MAX_HISTENT_TIME-1;
			htemp->next = histent_tail;			
	
			if(GLOBALS->ae2_lx2_table[txidx][r].histent_curr)
				{
				GLOBALS->ae2_lx2_table[txidx][r].histent_curr->next = htemp;
				htemp = GLOBALS->ae2_lx2_table[txidx][r].histent_head;
				}


                        {
                        struct HistEnt *htemp2 = calloc_2(1, sizeof(struct HistEnt));
                        htemp2->time = -1;  
                        if(len>1)
                                {
                                htemp2->v.h_vector = htemp->v.h_vector;
                                }
                                else
                                {  
                                htemp2->v.h_val = htemp->v.h_val;
                                }
                        htemp2->next = htemp;
                        htemp = htemp2;
                        GLOBALS->ae2_lx2_table[txidx][r].numtrans++;
                        }

			if(len>1)
				{
				np->head.v.h_vector = (char *)malloc_2(len);
				for(i=0;i<len;i++) np->head.v.h_vector[i] = AN_X;
				}
				else
				{
				np->head.v.h_val = AN_X;	/* x */
				}

			np->head.time  = -2;
			np->head.next = htemp;
			np->numhist=GLOBALS->ae2_lx2_table[txidx][r].numtrans +2 /*endcap*/ +1 /*frontcap*/;

			np->curr = histent_tail;
			np->mv.mvlfac = NULL;	/* it's imported and cached so we can forget it's an mvlfac now */
			}
		free_2(GLOBALS->ae2_lx2_table[txidx]);
		GLOBALS->ae2_lx2_table[txidx] = NULL;
		aet2_rd_clr_fac_process_mask(txidx);
		}
	}
}

#endif
/* ...of AET2_IS_PRESENT */

/*
 * $Id: ae2.c,v 1.19 2010/09/16 15:24:25 gtkwave Exp $
 * $Log: ae2.c,v $
 * Revision 1.19  2010/09/16 15:24:25  gtkwave
 * non-sparse array fix
 *
 * Revision 1.18  2010/09/16 05:05:16  gtkwave
 * dummy up sparse vs regular array handling
 *
 * Revision 1.17  2010/09/16 04:40:25  gtkwave
 * disable arrays for now to track down sparse vs regular array crashes
 *
 * Revision 1.16  2010/05/27 06:56:39  gtkwave
 * printf warning fixes
 *
 * Revision 1.15  2010/03/19 17:07:22  gtkwave
 * added missing ->symbol after structure type changed
 *
 * Revision 1.14  2010/03/15 15:57:28  gtkwave
 * only allocate hash when necessary
 *
 * Revision 1.13  2010/03/14 07:09:49  gtkwave
 * removed ExtNode and merged with Node
 *
 * Revision 1.12  2010/03/13 19:48:53  gtkwave
 * remove nextinaet field and replace with temp symchain
 *
 * Revision 1.11  2010/01/23 03:21:11  gtkwave
 * hierarchy fixes when characters < "." are in the signal names
 *
 * Revision 1.10  2009/07/01 07:39:12  gtkwave
 * decorating hierarchy tree with module type info
 *
 * Revision 1.9  2008/12/25 03:21:57  gtkwave
 * -Wshadow warning fixes
 *
 * Revision 1.8  2008/09/27 19:08:39  gtkwave
 * compiler warning fixes
 *
 * Revision 1.7  2008/06/17 18:03:45  gtkwave
 * added time = -1 endcaps
 *
 * Revision 1.6  2008/02/19 22:00:28  gtkwave
 * added aetinfo support
 *
 * Revision 1.5  2007/08/31 22:42:43  gtkwave
 * 3.1.0 RC3 updates
 *
 * Revision 1.4  2007/08/26 21:35:39  gtkwave
 * integrated global context management from SystemOfCode2007 branch
 *
 * Revision 1.1.1.1.2.6  2007/08/23 23:28:48  gtkwave
 * reload fail handling and retries
 *
 * Revision 1.1.1.1.2.5  2007/08/18 21:51:57  gtkwave
 * widget destroys and teardown of file formats which use external loaders
 * and are outside of malloc_2/free_2 control
 *
 * Revision 1.1.1.1.2.4  2007/08/07 03:18:54  kermin
 * Changed to pointer based GLOBAL structure and added initialization function
 *
 * Revision 1.1.1.1.2.3  2007/08/06 03:50:45  gtkwave
 * globals support for ae2, gtk1, cygwin, mingw.  also cleaned up some machine
 * generated structs, etc.
 *
 * Revision 1.1.1.1.2.2  2007/08/05 02:27:18  kermin
 * Semi working global struct
 *
 * Revision 1.1.1.1.2.1  2007/07/28 19:50:39  kermin
 * Merged in the main line
 *
 * Revision 1.3  2007/06/23 02:58:26  gtkwave
 * added bounds checking on start vs end cycle so they don't invert
 *
 * Revision 1.2  2007/06/23 02:37:27  gtkwave
 * static section size is now dynamic
 *
 * Revision 1.1.1.1  2007/05/30 04:27:40  gtkwave
 * Imported sources
 *
 * Revision 1.4  2007/05/28 00:55:05  gtkwave
 * added support for arrays as a first class dumpfile datatype
 *
 * Revision 1.3  2007/04/29 04:13:49  gtkwave
 * changed anon union defined in struct Node to a named one as anon unions
 * are a gcc extension
 *
 * Revision 1.2  2007/04/20 01:39:00  gtkwave
 * initial release
 *
 */
