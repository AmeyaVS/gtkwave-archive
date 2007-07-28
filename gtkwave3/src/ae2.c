/* 
 * Copyright (c) Tony Bybell 2004-2007.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

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

/* 
 * select appropriate entry points based on if aet2
 * support is available
 */
#ifndef AET2_IS_PRESENT 

static char *ae2_loader_fail_msg = "Sorry, AET2 support was not compiled into this executable, exiting.\n\n";

TimeType ae2_main(char *fname, char *skip_start, char *skip_end, char *indirect_fname)
{
fprintf(stderr, ae2_loader_fail_msg);
exit(255);

return(0); /* for vc++ */
}

void ae2_import_masked(void)
{
fprintf(stderr, ae2_loader_fail_msg);
exit(255);
}

#else

/*
 * globals
 */
static TimeType first_cycle, last_cycle, total_cycles;
static unsigned long num_sections = 0;
static struct lx2_entry **lx2_table = NULL;
static FILE *f = NULL; 
static AE2_HANDLE ae2 = NULL;
static FACREF *fr = NULL;
static char *process_mask = NULL;
static char ae2_msg_suppress = 0;

static struct regex_links *regex_head = NULL;
static int regex_matches = 0;


/*
 * iter mask manipulation util functions
 */
int aet2_rd_get_fac_process_mask(unsigned int facidx)
{
if(facidx<numfacs)
	{
	int process_idx = facidx/8;
	int process_bit = facidx&7;

	return( (process_mask[process_idx]&(1<<process_bit)) != 0 );
	}

return(0);
}


int aet2_rd_set_fac_process_mask(unsigned int facidx)
{
int rc=1;

if(facidx<numfacs)
	{
	int idx = facidx/8;
	int bitpos = facidx&7;

	process_mask[idx] |= (1<<bitpos);
	}

return(rc);
}


int aet2_rd_clr_fac_process_mask(unsigned int facidx)
{
int rc=1;

if(facidx<numfacs)
	{
	int idx = facidx/8;
	int bitpos = facidx&7;

	process_mask[idx] &= (~(1<<bitpos));
	}

return(rc);
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
static int twirl_pos = 0;
static int did_twirl = 0;

static void msg_fn(int sev, const char *format, ...)
{
if((!ae2_msg_suppress)||(sev))
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
	fprintf(stderr, " %c\r", twirler[twirl_pos]);
	twirl_pos = (twirl_pos+1) & 3;
	did_twirl = 1;

	va_end(ap);
	}
}

static void *alloc_fn(size_t size)
{        
void *pnt = calloc_2(1, size);
return(pnt);
}
        
static void free_fn(void* ptr, size_t size)
{
free_2(ptr);
}


/*
 * mainline
 */
TimeType ae2_main(char *fname, char *skip_start, char *skip_end, char *indirect_fname)
{
int i;
int match_idx;
struct Node *n;
struct symbol *s, *prevsymroot=NULL, *prevsym=NULL;
FILE *ind_h = NULL;

ae2_initialize(error_fn, msg_fn, alloc_fn, free_fn);

if ( (!(f=fopen(fname, "rb"))) || (!(ae2 = ae2_read_initialize(f))) )
        {
        fprintf(stderr, "Could not initialize '%s', exiting.\n", fname);
        exit(0);
        }

/* SPLASH */                            splash_create();

time_dimension = 'n';

num_sections=ae2_read_num_sections(ae2);
numfacs=ae2_read_num_symbols(ae2);
process_mask = calloc_2(1, numfacs/8+1);

if(indirect_fname)
	{
	ind_h = fopen(indirect_fname, "rb");
	if(!ind_h)
		{
		fprintf(stderr, AET2_RDLOAD"Could not open indirect file '%s', skipping.\n", indirect_fname);
		}
		else
		{
		int added = 0;

		fprintf(stderr, AET2_RDLOAD"Using indirect file '%s' for facility selection...\n", indirect_fname);
		while(!feof(ind_h))
			{
			char *exp, *exp2, *exp3;
			void *regex;
		
			exp = fgetmalloc(ind_h);
			if(!exp) continue;
			exp2 = exp;
			while(isspace(*exp2)) exp2++;

			exp3 = exp2 + strlen(exp2) - 1;
			while(exp3 != exp2)
				{
				if(!isspace(*exp3))				
					{
					*(exp3+1) = 0;
					break;
					}
				exp3--;
				}

			if((*exp2)&&(*exp2!='#'))
				{
				regex = wave_regex_alloc_compile(exp2);
				if(regex)
					{
					struct regex_links *rpnt = malloc_2(sizeof(struct regex_links));
		
					rpnt->pnt = regex;
					rpnt->next = regex_head;
					regex_head = rpnt;
		
					if(added < 31)
						{
						fprintf(stderr, AET2_RDLOAD"Added indirect regex '%s'\n", exp2);
						}
					else
					if(added == 31)
						{
						fprintf(stderr, AET2_RDLOAD"Added indirect regex '%s', adding more quietly...\n", exp2);
						}
					added++;
					}
				}

			free_2(exp);
			}
	
		fclose(ind_h); ind_h = NULL;
	
		regex_matches = 0;
		if(regex_head)
			{
			struct regex_links *rpnt;
	
			for(i=0;i<numfacs;i++)
				{
			        char buf[65537];
			        int idx = i+1;
			
			        ae2_read_symbol_name(ae2, idx, buf);
				rpnt = regex_head;
				while(rpnt)
					{
					if(wave_regex_alloc_match(rpnt->pnt, buf))
						{
						/* fprintf(stderr, "Matched '%s'\n", buf); */
						aet2_rd_set_fac_process_mask(i);
						regex_matches++;
						break;
						}
	
					rpnt=rpnt->next;
					}
				}
	
			rpnt = regex_head;
			while(rpnt)
				{
				struct regex_links *rpnt2 = rpnt->next;
				wave_regex_alloc_free(rpnt->pnt);			
				free_2(rpnt);
				rpnt = rpnt2;
				}
	
			regex_head=NULL;
			}
	
		if(regex_matches)
			{
			fprintf(stderr, AET2_RDLOAD"Matched %d/%d facilities against indirect file.\n", regex_matches, numfacs);
			}
			else
			{
			fprintf(stderr, AET2_RDLOAD"Matched %d/%d facilities against indirect file, exiting.\n", regex_matches, numfacs);
			exit(0);
			}
		}
	}

if(!regex_matches)
	{
	fr=calloc_2(numfacs, sizeof(FACREF));
	lx2_table=(struct lx2_entry **)calloc_2(numfacs, sizeof(struct lx2_entry *));
	}
	else
	{
	fr=calloc_2(regex_matches, sizeof(FACREF));
	lx2_table=(struct lx2_entry **)calloc_2(regex_matches, sizeof(struct lx2_entry *));
	}

match_idx = 0;
for(i=0;i<numfacs;i++)
	{
        char buf[65537];
        int idx = i+1;

	if((regex_matches)&&(!aet2_rd_get_fac_process_mask(i))) continue;

        ae2_read_symbol_name(ae2, idx, buf);
 
        fr[match_idx].facname = NULL;
        fr[match_idx].row = ae2_read_symbol_rows(ae2, idx);
	if(fr[match_idx].row == 1) fr[match_idx].row = 0;
        fr[match_idx].length = ae2_read_symbol_length(ae2, idx);
        fr[match_idx].s = idx;
        fr[match_idx].row_high = 0;
        fr[match_idx].offset = 0;

	match_idx++;
	}

fprintf(stderr, AET2_RDLOAD"Finished building %d facs.\n", match_idx);
/* SPLASH */                            splash_sync(1, 5);

first_cycle = (TimeType) ae2_read_start_cycle(ae2);
last_cycle = (TimeType) ae2_read_end_cycle(ae2);
total_cycles = last_cycle - first_cycle + 1;

/* do your stuff here..all useful info has been initialized by now */

if(!hier_was_explicitly_set)    /* set default hierarchy split char */
        {
        hier_delimeter='.';
        }

match_idx = 0;
for(i=0;i<numfacs;i++)
        {
	char *str;	
        char buf[65537];
        int idx = i+1;
	unsigned long len;
	int row_iter, mx_row, mx_row_adjusted;

	if((regex_matches)&&(!aet2_rd_get_fac_process_mask(i))) continue;

	len = ae2_read_symbol_name(ae2, idx, buf);	/* previously had -1, but it no longer returns the zero byte counted */
	buf[len] = 0;					/* only being defensive here, not really necessary */

	if(fr[match_idx].length>1)
		{
		int len2 = sprintf(buf+len, "[%d:%d]", 0, fr[match_idx].length-1);
		str=malloc_2(len + len2 + 1);
		if(!alt_hier_delimeter)
			{
			strcpy(str, buf);
			}
			else
			{
			strcpy_vcdalt(str, buf, alt_hier_delimeter);
			}
	        s=symadd_name_exists(str,0);
		prevsymroot = prevsym = NULL;
		}
		else
		{
		str=malloc_2(len+1);
		if(!alt_hier_delimeter)
			{
			strcpy(str, buf);
			}
			else
			{
			strcpy_vcdalt(str, buf, alt_hier_delimeter);
			}
	        s=symadd_name_exists(str,0);
		prevsymroot = prevsym = NULL;
		}
		
        if(!firstnode)
                {
                firstnode=curnode=s;   
                }
                else
                {
                curnode->nextinaet=s;
                curnode=s;   
                }


        mx_row = (fr[match_idx].row < 1) ? 1 : fr[match_idx].row;
	mx_row_adjusted = (mx_row < 2) ? 0 : mx_row;
        n=(struct Node *)calloc_2(mx_row,sizeof(struct Node));
	s->n = n;

	for(row_iter = 0; row_iter < mx_row; row_iter++)
		{
	        n[row_iter].nname=s->name;
	        n[row_iter].mv.mvlfac = (struct fac *)(fr+match_idx); /* to keep from having to allocate duplicate mvlfac struct */
							               /* use the info in the FACREF array instead                */
		n[row_iter].array_height = mx_row_adjusted;
		n[row_iter].this_row = row_iter;

		if(fr[match_idx].length>1)
			{
			ExtNode *ext = (ExtNode *)calloc_2(1,sizeof(struct ExtNode));
			ext->msi = 0;
			ext->lsi = fr[match_idx].length-1;
			n[row_iter].ext = ext;
			}
                 
	        n[row_iter].head.time=-1;        /* mark 1st node as negative time */
	        n[row_iter].head.v.h_val=AN_X;
		}

	match_idx++;
        }

if(regex_matches)
	{
	free_2(process_mask);
	numfacs = regex_matches;
	regex_matches = 0;
	process_mask = calloc_2(1, numfacs/8+1);
	}

/* SPLASH */                            splash_sync(2, 5);
facs=(struct symbol **)malloc_2(numfacs*sizeof(struct symbol *));

if(fast_tree_sort)
	{
	curnode=firstnode;
	for(i=0;i<numfacs;i++)
		{
		int len;
		facs[i]=curnode;
	        if((len=strlen(curnode->name))>longestname) longestname=len;
		curnode=curnode->nextinaet;
		}

/* SPLASH */                            splash_sync(3, 5);
	fprintf(stderr, AET2_RDLOAD"Building facility hierarchy tree.\n");

	init_tree();		
	for(i=0;i<numfacs;i++)	
		{
		build_tree_from_name(facs[i]->name, i);
		}
/* SPLASH */                            splash_sync(4, 5);
	treegraft(treeroot);

	fprintf(stderr, AET2_RDLOAD"Sorting facility hierarchy tree.\n");
	treesort(treeroot, NULL);
/* SPLASH */                            splash_sync(5, 5);
	order_facs_from_treesort(treeroot, &facs);

	facs_are_sorted=1;
	}
	else
	{
	curnode=firstnode;
	for(i=0;i<numfacs;i++)
		{
		char *subst, ch;	
		int len;

		facs[i]=curnode;
	        if((len=strlen(subst=curnode->name))>longestname) longestname=len;
		curnode=curnode->nextinaet;
		while((ch=(*subst)))
			{	
			if(ch==hier_delimeter) { *subst=VCDNAM_HIERSORT; }	/* forces sort at hier boundaries */
			subst++;
			}
		}
	
/* SPLASH */                            splash_sync(3, 5);
	fprintf(stderr, AET2_RDLOAD"Sorting facilities at hierarchy boundaries.\n");
	wave_heapsort(facs,numfacs);
	
	for(i=0;i<numfacs;i++)
		{
		char *subst, ch;
	
		subst=facs[i]->name;
		while((ch=(*subst)))
			{	
			if(ch==VCDNAM_HIERSORT) { *subst=hier_delimeter; }	/* restore back to normal */
			subst++;
			}
		}
	
	facs_are_sorted=1;

/* SPLASH */                            splash_sync(4, 5);
	fprintf(stderr, AET2_RDLOAD"Building facility hierarchy tree.\n");

	init_tree();		
	for(i=0;i<numfacs;i++)	
		{
		build_tree_from_name(facs[i]->name, i);
		}
/* SPLASH */                            splash_sync(5, 5);
	treegraft(treeroot);
	treesort(treeroot, NULL);
	}

min_time = first_cycle; max_time=last_cycle;
is_lx2 = LXT2_IS_AET2;

if(skip_start || skip_end)
	{
	TimeType b_start, b_end;

	if(!skip_start) b_start = min_time; else b_start = unformat_time(skip_start, time_dimension);
	if(!skip_end) b_end = max_time; else b_end = unformat_time(skip_end, time_dimension);

	if(b_start<min_time) b_start = min_time;
	else if(b_start>max_time) b_start = max_time;

	if(b_end<min_time) b_end = min_time;
	else if(b_end>max_time) b_end = max_time;

        if(b_start > b_end)
                {
		TimeType tmp_time = b_start;
                b_start = b_end;
                b_end = tmp_time;
                }

	min_time = b_start;
	max_time = b_end;
	}

fprintf(stderr, AET2_RDLOAD"["TTFormat"] start time.\n"AET2_RDLOAD"["TTFormat"] end time.\n", min_time, max_time);
return(max_time);
}


/*
 * ae2 callback
 */
static void ae2_callback(uint64_t *time, unsigned int *facidx, char **value, unsigned int row)
{
struct HistEnt *htemp = histent_calloc();
struct lx2_entry *l2e = &lx2_table[*facidx][row];
FACREF *f = fr+(*facidx);

static int busycnt = 0;

busycnt++;
if(busycnt==WAVE_BUSY_ITER)
        {
        busy_window_refresh();
        busycnt = 0;
        }

/* fprintf(stderr, "%lld %d %d %s\n", *time, *facidx, row, *value); */

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

htemp->time = (*time);

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
char buf[65537];


ae2_msg_suppress = 1;
did_twirl = 0;

autofacs = calloc_2(numfacs, sizeof(struct ae2_ncycle_autosort));

for(i=0;i<numfacs;i++)
	{
	if(aet2_rd_get_fac_process_mask(i))
		{
		int nr = ae2_read_symbol_rows(ae2,fr[i].s);
		if(!nr) nr = 1;
		for(r=0;r<nr;r++)
			{
			nptr np = lx2_table[i][r].np;
			np->mv.value = calloc_2(1, fr[i].length+1);
			}		
		}
	}


for(j=0;j<num_sections;j++)
	{
	struct ae2_ncycle_autosort **autosort = NULL;
	uint64_t *ith_range = ae2_read_ith_section_range(ae2, j);

	cyc = *ith_range;
	ecyc = *(ith_range+1);

	if(ecyc<start_cycle) continue;
	if(cyc>end_cycle) break;

	if((ecyc<cyc)||(ecyc==~ULLDescriptor(0))) continue;

	autosort = calloc(ecyc - cyc + 1, sizeof(struct ae2_ncycle_autosort *));
	
	for(i=0;i<numfacs;i++)
		{
		if(aet2_rd_get_fac_process_mask(i))
			{
			int nr = ae2_read_symbol_rows(ae2,fr[i].s);

			if(nr<2)
				{
				nptr np = lx2_table[i][0].np;
	
				ae2_read_value(ae2, fr+i, cyc, buf);
				if(strcmp(np->mv.value, buf))
					{
					strcpy(np->mv.value, buf);
					ae2_callback(&cyc, &i, &np->mv.value, 0);
					}
				}
				else
				{
				int rows = ae2_read_num_sparse_rows(ae2, fr[i].s, cyc);
				if(rows)
					{
		                        for(r=1;r<rows+1;r++)
		                                {
						nptr np; 
						char *value;
		                                uint64_t row = ae2_read_ith_sparse_row(ae2, fr[i].s, cyc, r);

		                                fr[i].row = row;

						np = lx2_table[i][row].np;
		                                ae2_read_value(ae2, fr+i, cyc, buf);
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

	deadlist=NULL;

	for(i=0;i<numfacs;i++)
		{
		uint64_t ncyc;
		nptr np;
		int nr;

		if(!aet2_rd_get_fac_process_mask(i)) continue;

		nr = ae2_read_symbol_rows(ae2,fr[i].s);
		if(nr < 2)
			{
			np = lx2_table[i][0].np;
			ncyc =	ae2_read_next_value(ae2, fr+i, cyc, np->mv.value);
			}
			else
			{
			int rows = ae2_read_num_sparse_rows(ae2, fr[i].s, cyc);
			uint64_t mxcyc = end_cycle+1;

                        for(r=1;r<rows+1;r++)
                                {
				nptr np; 
                                uint64_t row = ae2_read_ith_sparse_row(ae2, fr[i].s, cyc, r);

                                fr[i].row = row;
				np = lx2_table[i][row].np;
				ncyc =	ae2_read_next_value(ae2, fr+i, cyc, buf);

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
				nr = ae2_read_symbol_rows(ae2,fr[i].s);

				if(nr<2)
					{
					np = lx2_table[i][0].np;

					ae2_callback(&step_cyc, &i, &np->mv.value, 0);
		
					ncyc = ae2_read_next_value(ae2, fr+i, step_cyc, np->mv.value);
					}
					else
					{
					int rows = ae2_read_num_sparse_rows(ae2, fr[i].s, step_cyc);
					uint64_t mxcyc = end_cycle+1;

		                        for(r=1;r<rows+1;r++)
	        	                        {
						nptr np; 
		                                uint64_t row = ae2_read_ith_sparse_row(ae2, fr[i].s, step_cyc, r);

		                                fr[i].row = row;
						np = lx2_table[i][row].np;

						ae2_read_value(ae2, fr+i, step_cyc, buf);
						if(strcmp(buf, np->mv.value))
							{
							strcpy(np->mv.value, buf);
							ae2_callback(&step_cyc, &i, &np->mv.value, row);
							}

						ncyc =	ae2_read_next_value(ae2, fr+i, step_cyc, buf);
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
		
				if(ncyc!=step_cyc)
					{
					int offset2 = ncyc-cyc;
					struct ae2_ncycle_autosort *t = autosort[offset2];
				
					autofacs[i].next = t;
					autosort[offset2] = autofacs+i; 
					}
					else
					{
					struct ae2_ncycle_autosort *t = deadlist;
					autofacs[i].next = t;
					deadlist = autofacs+i;
					}
				t = tn;
				}
			}
		}

	if(autosort) free(autosort);
	}


for(i=0;i<numfacs;i++)
	{
	if(aet2_rd_get_fac_process_mask(i))
		{
		int nr = ae2_read_symbol_rows(ae2,fr[i].s);
		if(!nr) nr = 1;
		for(r=0;r<nr;r++)
			{
			nptr np = lx2_table[i][r].np;
			free(np->mv.value);
			np->mv.value = NULL;
			}		
		}
	}

free_2(autofacs);

ae2_msg_suppress = 0;
if(did_twirl)
	{
	fprintf(stderr,"\n");
	did_twirl = 0;
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

txidx = f - fr;
nr = ae2_read_symbol_rows(ae2, f->s);

/* new stuff */
len = f->length;

if((1)||(f->row <= 1)) /* sorry, arrays not supported yet in the viewer */
	{
	fprintf(stderr, "Import: %s\n", np->nname);

	aet2_rd_set_fac_process_mask(txidx);
	ae2_iterator(min_time, max_time);
	aet2_rd_clr_fac_process_mask(txidx);
	}
	else
	{
	fprintf(stderr, AET2_RDLOAD"Skipping array: %s (%d rows)\n", np->nname, f->row);
	}


if(nr<1) nr=1;
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

	if(lx2_table[txidx][r].histent_curr)
		{
		lx2_table[txidx][r].histent_curr->next = htemp;
		htemp = lx2_table[txidx][r].histent_head;
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

	np[r].head.time  = -2;
	np[r].head.next = htemp;
	np[r].numhist=lx2_table[txidx][r].numtrans +2 /*endcap*/ +1 /*frontcap*/;

	memset(lx2_table+txidx, 0, sizeof(struct lx2_entry));	/* zero it out */

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

txidx = f - fr;

if((1)||(f->row <= 1)) /* sorry, arrays not supported */
	{
	aet2_rd_set_fac_process_mask(txidx);
	nr = f->row;
	if(!nr) nr=1;
	lx2_table[txidx] = calloc(nr, sizeof(struct lx2_entry));
	for(r=0;r<nr;r++)
		{
		lx2_table[txidx][r].np = &np[r];
		}
	}
}


void ae2_import_masked(void)
{
int txidx, i, cnt=0;

for(txidx=0;txidx<numfacs;txidx++)
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
ae2_iterator(min_time, max_time);
set_window_idle(NULL);

for(txidx=0;txidx<numfacs;txidx++)
	{
	if(aet2_rd_get_fac_process_mask(txidx))
		{
		struct HistEnt *htemp, *histent_tail;
		FACREF *f = fr+txidx;
		int r, nr = ae2_read_symbol_rows(ae2, f->s);
		int len = f->length;

		if(nr<1) nr=1;

		for(r = 0; r < nr; r++)
			{
			nptr np = lx2_table[txidx][r].np;

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
	
			if(lx2_table[txidx][r].histent_curr)
				{
				lx2_table[txidx][r].histent_curr->next = htemp;
				htemp = lx2_table[txidx][r].histent_head;
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
			np->numhist=lx2_table[txidx][r].numtrans +2 /*endcap*/ +1 /*frontcap*/;

			np->curr = histent_tail;
			np->mv.mvlfac = NULL;	/* it's imported and cached so we can forget it's an mvlfac now */
			}
		free(lx2_table[txidx]);
		lx2_table[txidx] = NULL;
		aet2_rd_clr_fac_process_mask(txidx);
		}
	}
}

#endif
/* ...of AET2_IS_PRESENT */

/*
 * $Id$
 * $Log$
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
