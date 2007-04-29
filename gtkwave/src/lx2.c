/* 
 * Copyright (c) Tony Bybell 2003-2006.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <config.h>
#include <stdio.h>
#include "lx2.h"

#ifndef _MSC_VER
#include <unistd.h>
#endif

#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include "symbol.h"
#include "vcd.h"
#include "lxt.h"
#include "vzt.h"
#include "lxt2_read.h"
#include "debug.h"
#include "busy.h"


/*
 * globals
 */
unsigned char is_lx2 = LXT2_IS_INACTIVE;

static struct lxt2_rd_trace *lx2=NULL;
static TimeType first_cycle, last_cycle, total_cycles;

static struct lx2_entry *lx2_table = NULL;
static struct fac *mvlfacs=NULL;


/*
 * mainline
 */
TimeType lx2_main(char *fname, char *skip_start, char *skip_end)
{
int i;
struct Node *n;
struct symbol *s, *prevsymroot=NULL, *prevsym=NULL;
signed char scale;
unsigned int numalias = 0;
struct symbol *sym_block = NULL;
struct Node *node_block = NULL;

lx2 = lxt2_rd_init(fname);
if(!lx2)
        {
        fprintf(stderr, "Could not initialize '%s', exiting.\n", fname);
        exit(0);
        }

/* SPLASH */                            splash_create();

/* lxt2_rd_set_max_block_mem_usage(lx2, 0); */

scale=(signed char)lxt2_rd_get_timescale(lx2);
exponent_to_time_scale(scale);

numfacs=lxt2_rd_get_num_facs(lx2);
mvlfacs=(struct fac *)calloc_2(numfacs,sizeof(struct fac));
lx2_table=(struct lx2_entry *)calloc_2(numfacs, sizeof(struct lx2_entry));
sym_block = (struct symbol *)calloc_2(numfacs, sizeof(struct symbol));
node_block=(struct Node *)calloc_2(numfacs,sizeof(struct Node));

for(i=0;i<numfacs;i++)
	{
	mvlfacs[i].array_height=lxt2_rd_get_fac_rows(lx2, i);
	mvlfacs[i].msb=lxt2_rd_get_fac_msb(lx2, i);
	mvlfacs[i].lsb=lxt2_rd_get_fac_lsb(lx2, i);
	mvlfacs[i].flags=lxt2_rd_get_fac_flags(lx2, i);
	mvlfacs[i].len=lxt2_rd_get_fac_len(lx2, i);
	}

fprintf(stderr, LXT2_RDLOAD"Finished building %d facs.\n", numfacs);
/* SPLASH */                            splash_sync(1, 5);

first_cycle = (TimeType) lxt2_rd_get_start_time(lx2) * time_scale;
last_cycle = (TimeType) lxt2_rd_get_end_time(lx2) * time_scale;
total_cycles = last_cycle - first_cycle + 1;

/* do your stuff here..all useful info has been initialized by now */

if(!hier_was_explicitly_set)    /* set default hierarchy split char */
        {
        hier_delimeter='.';
        }

if(numfacs)
	{
	char *fnam = lxt2_rd_get_facname(lx2, 0);
	int flen = strlen(fnam);

	mvlfacs[0].name=malloc_2(flen+1);
	strcpy(mvlfacs[0].name, fnam);
	}

for(i=0;i<numfacs;i++)
        {
	char buf[65537];
	char *str;	
	struct fac *f;

	if(i!=(numfacs-1))
		{
		char *fnam = lxt2_rd_get_facname(lx2, i+1);
		int flen = strlen(fnam);

		mvlfacs[i+1].name=malloc_2(flen+1);
		strcpy(mvlfacs[i+1].name, fnam);
		}

	if(i>1)
		{
		free_2(mvlfacs[i-2].name);
		mvlfacs[i-2].name = NULL;
		}

	if(mvlfacs[i].flags&LXT2_RD_SYM_F_ALIAS)
		{
		int alias = mvlfacs[i].array_height;
		f=mvlfacs+alias;

		while(f->flags&LXT2_RD_SYM_F_ALIAS)
			{
			f=mvlfacs+f->array_height;
			}

		numalias++;
		}
		else
		{
		f=mvlfacs+i;
		}

	if((f->len>1)&& (!(f->flags&(LXT2_RD_SYM_F_INTEGER|LXT2_RD_SYM_F_DOUBLE|LXT2_RD_SYM_F_STRING))) )
		{
		int len = sprintf(buf, "%s[%d:%d]", mvlfacs[i].name,mvlfacs[i].msb, mvlfacs[i].lsb);
		str=malloc_2(len+1);
		if(!alt_hier_delimeter)
			{
			strcpy(str, buf);
			}
			else
			{
			strcpy_vcdalt(str, buf, alt_hier_delimeter);
			}
                s=&sym_block[i];
                symadd_name_exists_sym_exists(s,str,0);
		prevsymroot = prevsym = NULL;
		}
	else if ( 
			((f->len==1)&&(!(f->flags&(LXT2_RD_SYM_F_INTEGER|LXT2_RD_SYM_F_DOUBLE|LXT2_RD_SYM_F_STRING)))&&
			((i!=numfacs-1)&&(!strcmp(mvlfacs[i].name, mvlfacs[i+1].name))))
			||
			(((i!=0)&&(!strcmp(mvlfacs[i].name, mvlfacs[i-1].name))) &&
			(mvlfacs[i].msb!=-1)&&(mvlfacs[i].lsb!=-1))
		)
		{
		int len = sprintf(buf, "%s[%d]", mvlfacs[i].name,mvlfacs[i].msb);
		str=malloc_2(len+1);
		if(!alt_hier_delimeter)
			{
			strcpy(str, buf);
			}
			else
			{
			strcpy_vcdalt(str, buf, alt_hier_delimeter);
			}
                s=&sym_block[i];
                symadd_name_exists_sym_exists(s,str,0);
		if((prevsym)&&(i>0)&&(!strcmp(mvlfacs[i].name, mvlfacs[i-1].name)))	/* allow chaining for search functions.. */
			{
			prevsym->vec_root = prevsymroot;
			prevsym->vec_chain = s;
			s->vec_root = prevsymroot;
			prevsym = s;
			}
			else
			{
			prevsymroot = prevsym = s;
			}
		}
		else
		{
		str=malloc_2(strlen(mvlfacs[i].name)+1);
		if(!alt_hier_delimeter)
			{
			strcpy(str, mvlfacs[i].name);
			}
			else
			{
			strcpy_vcdalt(str, mvlfacs[i].name, alt_hier_delimeter);
			}
                s=&sym_block[i];
                symadd_name_exists_sym_exists(s,str,0);
		prevsymroot = prevsym = NULL;

		if(f->flags&LXT2_RD_SYM_F_INTEGER)
			{
			mvlfacs[i].msb=31;
			mvlfacs[i].lsb=0;
			mvlfacs[i].len=32;
			}
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

	n=&node_block[i];
        n->nname=s->name;
        n->mv.mvlfac = mvlfacs+i;
	mvlfacs[i].working_node = n;

	if((f->len>1)||(f->flags&&(LXT2_RD_SYM_F_DOUBLE|LXT2_RD_SYM_F_STRING)))
		{
		ExtNode *ext = (ExtNode *)calloc_2(1,sizeof(struct ExtNode));
		ext->msi = mvlfacs[i].msb;
		ext->lsi = mvlfacs[i].lsb;
		n->ext = ext;
		}
                 
        n->head.time=-1;        /* mark 1st node as negative time */
        n->head.v.h_val=AN_X;
        s->n=n;
        }

for(i=0;((i<2)&&(i<numfacs));i++)
	{
	if(mvlfacs[i].name)
		{
		free_2(mvlfacs[i].name);
		mvlfacs[i].name = NULL;
		}
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

	if(numalias)
		{
		unsigned int idx_lft = 0;
		unsigned int idx_lftmax = numfacs - numalias;  		
		unsigned int idx_rgh = numfacs - numalias;  		
		struct symbol **facs_merge=(struct symbol **)malloc_2(numfacs*sizeof(struct symbol *));

		fprintf(stderr, LXT2_RDLOAD"Merging in %d aliases.\n", numalias);

		for(i=0;i<numfacs;i++)	/* fix possible tail appended aliases by remerging in partial one pass merge sort */
			{
			if(strcmp(facs[idx_lft]->name, facs[idx_rgh]->name) <= 0)
				{
				facs_merge[i] = facs[idx_lft++];

				if(idx_lft == idx_lftmax)
					{
					for(i++;i<numfacs;i++)
						{
						facs_merge[i] = facs[idx_rgh++];
						}
					}
				}
				else
				{
				facs_merge[i] = facs[idx_rgh++];

				if(idx_rgh == numfacs)
					{
					for(i++;i<numfacs;i++)
						{
						facs_merge[i] = facs[idx_lft++];
						}
					}
				}
			}

		free_2(facs); facs = facs_merge;
		}
                 
/* SPLASH */                            splash_sync(3, 5);
        fprintf(stderr, LXT2_RDLOAD"Building facility hierarchy tree.\n");
        
        init_tree();
        for(i=0;i<numfacs;i++)
                {
                build_tree_from_name(facs[i]->name, i);
                }
/* SPLASH */                            splash_sync(4, 5);
        treegraft(treeroot);
        
        fprintf(stderr, LXT2_RDLOAD"Sorting facility hierarchy tree.\n");
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
	fprintf(stderr, LXT2_RDLOAD"Sorting facilities at hierarchy boundaries.\n");
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
	fprintf(stderr, LXT2_RDLOAD"Building facility hierarchy tree.\n");

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
is_lx2 = LXT2_IS_LXT2;

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

	if(!lxt2_rd_limit_time_range(lx2, b_start, b_end))
		{
		fprintf(stderr, LXT2_RDLOAD"--begin/--end options yield zero blocks, ignoring.\n");
		lxt2_rd_unlimit_time_range(lx2);
		}
		else
		{
		min_time = b_start;
		max_time = b_end;
		}
	}

return(max_time);
}


/*
 * lx2 callback (only does bits for now)
 */
static void lx2_callback(struct lxt2_rd_trace **lt, lxtint64_t *time, lxtint32_t *facidx, char **value)
{
struct HistEnt *htemp = histent_calloc();
struct lx2_entry *l2e = lx2_table+(*facidx);
struct fac *f = mvlfacs+(*facidx);

static int busycnt = 0;

busycnt++;
if(busycnt==WAVE_BUSY_ITER)
        {
        busy_window_refresh();
        busycnt = 0;
        }

/* fprintf(stderr, "%lld %d %s\n", *time, *facidx, *value); */

if(!(f->flags&(LXT2_RD_SYM_F_DOUBLE|LXT2_RD_SYM_F_STRING)))
	{
	if(f->len>1)        
	        {
	        htemp->v.h_vector = (char *)malloc_2(f->len);
		memcpy(htemp->v.h_vector, *value, f->len);
	        }
	        else
	        {
		switch(**value)
			{
			case '0':	htemp->v.h_val = AN_0; break;
			case '1':	htemp->v.h_val = AN_1; break;
			case 'Z':
			case 'z':	htemp->v.h_val = AN_Z; break;
			default:	htemp->v.h_val = AN_X; break;
			}
	        }
	}
else if(f->flags&LXT2_RD_SYM_F_DOUBLE)
	{
	double *d = malloc_2(sizeof(double));
	sscanf(*value, "%lg", d);
	htemp->v.h_vector = (char *)d;
	htemp->flags = HIST_REAL;
	}
else	/* string */
	{
	char *s = malloc_2(strlen(*value)+1);
	strcpy(s, *value);
	htemp->v.h_vector = s;
	htemp->flags = HIST_REAL|HIST_STRING;
	}


htemp->time = (*time) * (time_scale);

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


/*
 * this is the black magic that handles aliased signals...
 */
static void lx2_resolver(nptr np, nptr resolve)
{ 
np->ext = resolve->ext;
memcpy(&np->head, &resolve->head, sizeof(struct HistEnt));
np->curr = resolve->curr;
np->harray = resolve->harray;
np->numhist = resolve->numhist;
np->mv.mvlfac=NULL;
}



/* 
 * actually import an lx2 trace but don't do it if it's already been imported 
 */
void import_lx2_trace(nptr np)
{
struct HistEnt *htemp, *histent_tail;
int len, i;
struct fac *f;
int txidx;
nptr nold = np;

switch(is_lx2)
	{
#ifdef AET2_IS_PRESENT
	case LXT2_IS_AET2: import_ae2_trace(np); return;
#endif
	case LXT2_IS_VZT:  import_vzt_trace(np); return;
	case LXT2_IS_VLIST: import_vcd_trace(np); return;
	default: break; /* fallthrough */
	}

if(!(f=np->mv.mvlfac)) return;	/* already imported */

txidx = f - mvlfacs;
if(np->mv.mvlfac->flags&LXT2_RD_SYM_F_ALIAS) 
	{
	txidx = lxt2_rd_get_alias_root(lx2, txidx);
	np = mvlfacs[txidx].working_node;

	if(!(f=np->mv.mvlfac)) 
		{
		lx2_resolver(nold, np);
		return;	/* already imported */
		}
	}

fprintf(stderr, "Import: %s\n", np->nname);

/* new stuff */
len = np->mv.mvlfac->len;

if(f->array_height <= 1) /* sorry, arrays not supported, but lx2 doesn't support them yet either */
	{
	lxt2_rd_set_fac_process_mask(lx2, txidx);
	lxt2_rd_iter_blocks(lx2, lx2_callback, NULL);
	lxt2_rd_clr_fac_process_mask(lx2, txidx);
	}

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

if(lx2_table[txidx].histent_curr)
	{
	lx2_table[txidx].histent_curr->next = htemp;
	htemp = lx2_table[txidx].histent_head;
	}

if(!(f->flags&(LXT2_RD_SYM_F_DOUBLE|LXT2_RD_SYM_F_STRING)))
        {
	if(len>1)
		{
		np->head.v.h_vector = (char *)malloc_2(len);
		for(i=0;i<len;i++) np->head.v.h_vector[i] = AN_X;
		}
		else
		{
		np->head.v.h_val = AN_X;	/* x */
		}
	}
        else
        {
        np->head.flags = HIST_REAL;
        if(f->flags&LXT2_RD_SYM_F_STRING) np->head.flags |= HIST_STRING;
        }

np->head.time  = -2;
np->head.next = htemp;
np->numhist=lx2_table[txidx].numtrans +2 /*endcap*/ +1 /*frontcap*/;

memset(lx2_table+txidx, 0, sizeof(struct lx2_entry));	/* zero it out */

np->curr = histent_tail;
np->mv.mvlfac = NULL;	/* it's imported and cached so we can forget it's an mvlfac now */

if(nold!=np)
	{
	lx2_resolver(nold, np);
	}
}


/* 
 * pre-import many traces at once so function above doesn't have to iterate...
 */
void lx2_set_fac_process_mask(nptr np)
{
struct fac *f;
int txidx;

switch(is_lx2)
        {
#ifdef AET2_IS_PRESENT
        case LXT2_IS_AET2: ae2_set_fac_process_mask(np); return;
#endif
        case LXT2_IS_VZT:  vzt_set_fac_process_mask(np); return;
	case LXT2_IS_VLIST: vcd_set_fac_process_mask(np); return;
        default: break; /* fallthrough */
        }

if(!(f=np->mv.mvlfac)) return;	/* already imported */

txidx = f-mvlfacs;

if(np->mv.mvlfac->flags&LXT2_RD_SYM_F_ALIAS) 
	{
	txidx = lxt2_rd_get_alias_root(lx2, txidx);
	np = mvlfacs[txidx].working_node;

	if(!(np->mv.mvlfac)) return;	/* already imported */
	}

if(np->mv.mvlfac->array_height <= 1) /* sorry, arrays not supported, but lx2 doesn't support them yet either */
	{
	lxt2_rd_set_fac_process_mask(lx2, txidx);
	lx2_table[txidx].np = np;
	}
}


void lx2_import_masked(void)
{
int txidx, i, cnt;

switch(is_lx2)
        {
#ifdef AET2_IS_PRESENT
        case LXT2_IS_AET2: ae2_import_masked(); return;
#endif
        case LXT2_IS_VZT:  vzt_import_masked(); return;
        case LXT2_IS_VLIST:  vcd_import_masked(); return;
        default: break; /* fallthrough */
        }

cnt = 0;
for(txidx=0;txidx<numfacs;txidx++)
	{
	if(lxt2_rd_get_fac_process_mask(lx2, txidx))
		{
		cnt++;
		}
	}

if(!cnt) return;

if(cnt>100)
	{
	fprintf(stderr, LXT2_RDLOAD"Extracting %d traces\n", cnt);
	}


set_window_busy(NULL);
lxt2_rd_iter_blocks(lx2, lx2_callback, NULL);
set_window_idle(NULL);

for(txidx=0;txidx<numfacs;txidx++)
	{
	if(lxt2_rd_get_fac_process_mask(lx2, txidx))
		{
		struct HistEnt *htemp, *histent_tail;
		struct fac *f = mvlfacs+txidx;
		int len = f->len;
		nptr np = lx2_table[txidx].np;

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

		if(lx2_table[txidx].histent_curr)
			{
			lx2_table[txidx].histent_curr->next = htemp;
			htemp = lx2_table[txidx].histent_head;
			}

		if(!(f->flags&(LXT2_RD_SYM_F_DOUBLE|LXT2_RD_SYM_F_STRING)))
		        {
			if(len>1)
				{
				np->head.v.h_vector = (char *)malloc_2(len);
				for(i=0;i<len;i++) np->head.v.h_vector[i] = AN_X;
				}
				else
				{
				np->head.v.h_val = AN_X;	/* x */
				}
			}
		        else
		        {
		        np->head.flags = HIST_REAL;
		        if(f->flags&LXT2_RD_SYM_F_STRING) np->head.flags |= HIST_STRING;
		        }

		np->head.time  = -2;
		np->head.next = htemp;
		np->numhist=lx2_table[txidx].numtrans +2 /*endcap*/ +1 /*frontcap*/;

		memset(lx2_table+txidx, 0, sizeof(struct lx2_entry));	/* zero it out */

		np->curr = histent_tail;
		np->mv.mvlfac = NULL;	/* it's imported and cached so we can forget it's an mvlfac now */
		lxt2_rd_clr_fac_process_mask(lx2, txidx);
		}
	}
}

/*
 * $Id$
 * $Log$
 * Revision 1.2  2007/04/20 02:08:13  gtkwave
 * initial release
 *
 */

