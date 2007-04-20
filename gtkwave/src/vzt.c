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
#include "vzt.h"
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
#include "lxt2_read.h"
#include "vzt_read.h"
#include "debug.h"
#include "busy.h"

/*
 * globals
 */
static struct vzt_rd_trace *vzt=NULL;
static TimeType first_cycle, last_cycle, total_cycles;

static struct lx2_entry *vzt_table = NULL;
static struct fac *mvlfacs=NULL;


/*
 * mainline
 */
TimeType vzt_main(char *fname, char *skip_start, char *skip_end)
{
int i;
struct Node *n;
struct symbol *s, *prevsymroot=NULL, *prevsym=NULL;
signed char scale;
unsigned int numalias = 0;
struct symbol *sym_block = NULL;
struct Node *node_block = NULL;

vzt = vzt_rd_init_smp(fname, num_cpus);
if(!vzt)
        {
        fprintf(stderr, "Could not initialize '%s', exiting.\n", fname);
        exit(0);
        }
/* SPLASH */                            splash_create();

vzt_rd_process_blocks_linearly(vzt, 1);
/* vzt_rd_set_max_block_mem_usage(vzt, 0); */

scale=(signed char)vzt_rd_get_timescale(vzt);
exponent_to_time_scale(scale);

numfacs=vzt_rd_get_num_facs(vzt);
mvlfacs=(struct fac *)calloc_2(numfacs,sizeof(struct fac));
vzt_table=(struct lx2_entry *)calloc_2(numfacs, sizeof(struct lx2_entry));
sym_block = (struct symbol *)calloc_2(numfacs, sizeof(struct symbol));
node_block=(struct Node *)calloc_2(numfacs,sizeof(struct Node));

for(i=0;i<numfacs;i++)
	{
	mvlfacs[i].array_height=vzt_rd_get_fac_rows(vzt, i);
	mvlfacs[i].msb=vzt_rd_get_fac_msb(vzt, i);
	mvlfacs[i].lsb=vzt_rd_get_fac_lsb(vzt, i);
	mvlfacs[i].flags=vzt_rd_get_fac_flags(vzt, i);
	mvlfacs[i].len=vzt_rd_get_fac_len(vzt, i);
	}

fprintf(stderr, VZT_RDLOAD"Finished building %d facs.\n", numfacs);
/* SPLASH */                            splash_sync(1, 5);

first_cycle = (TimeType) vzt_rd_get_start_time(vzt) * time_scale;
last_cycle = (TimeType) vzt_rd_get_end_time(vzt) * time_scale;
total_cycles = last_cycle - first_cycle + 1;

/* do your stuff here..all useful info has been initialized by now */

if(!hier_was_explicitly_set)    /* set default hierarchy split char */
        {
        hier_delimeter='.';
        }

if(numfacs)
	{
	char *fnam = vzt_rd_get_facname(vzt, 0);
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
		char *fnam = vzt_rd_get_facname(vzt, i+1);
		int flen = strlen(fnam);

		mvlfacs[i+1].name=malloc_2(flen+1);
		strcpy(mvlfacs[i+1].name, fnam);
		}

	if(i>1)
		{
		free_2(mvlfacs[i-2].name);
		mvlfacs[i-2].name = NULL;
		}

	if(mvlfacs[i].flags&VZT_RD_SYM_F_ALIAS)
		{
		int alias = mvlfacs[i].array_height;
		f=mvlfacs+alias;

		while(f->flags&VZT_RD_SYM_F_ALIAS)
			{
			f=mvlfacs+f->array_height;
			}

		numalias++;
		}
		else
		{
		f=mvlfacs+i;
		}

	if((f->len>1)&& (!(f->flags&(VZT_RD_SYM_F_INTEGER|VZT_RD_SYM_F_DOUBLE|VZT_RD_SYM_F_STRING))) )
		{
		int len=sprintf(buf, "%s[%d:%d]", mvlfacs[i].name,mvlfacs[i].msb, mvlfacs[i].lsb);
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
			((f->len==1)&&(!(f->flags&(VZT_RD_SYM_F_INTEGER|VZT_RD_SYM_F_DOUBLE|VZT_RD_SYM_F_STRING)))&&
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

		if(f->flags&VZT_RD_SYM_F_INTEGER)
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
        n->mvlfac = mvlfacs+i;
	mvlfacs[i].working_node = n;

	if((f->len>1)||(f->flags&&(VZT_RD_SYM_F_DOUBLE|VZT_RD_SYM_F_STRING)))
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

		fprintf(stderr, VZT_RDLOAD"Merging in %d aliases.\n", numalias);

                for(i=0;i<numfacs;i++)  /* fix possible tail appended aliases by remerging in partial one pass merge sort */
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
        fprintf(stderr, VZT_RDLOAD"Building facility hierarchy tree.\n");
                                         
        init_tree();
        for(i=0;i<numfacs;i++)
                {
                build_tree_from_name(facs[i]->name, i);
                }
/* SPLASH */                            splash_sync(4, 5);  
        treegraft(treeroot);
                                
        fprintf(stderr, VZT_RDLOAD"Sorting facility hierarchy tree.\n");
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
	fprintf(stderr, VZT_RDLOAD"Sorting facilities at hierarchy boundaries.\n");
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
	fprintf(stderr, VZT_RDLOAD"Building facility hierarchy tree.\n");

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
is_lx2 = LXT2_IS_VZT;

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

	if(!vzt_rd_limit_time_range(vzt, b_start, b_end))
		{
		fprintf(stderr, VZT_RDLOAD"--begin/--end options yield zero blocks, ignoring.\n");
		vzt_rd_unlimit_time_range(vzt);
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
 * vzt callback (only does bits for now)
 */
static void vzt_callback(struct vzt_rd_trace **lt, lxtint64_t *time, lxtint32_t *facidx, char **value)
{
struct HistEnt *htemp = histent_calloc();
struct lx2_entry *l2e = vzt_table+(*facidx);
struct fac *f = mvlfacs+(*facidx);

static int busycnt = 0;

busycnt++; 
if(busycnt==WAVE_BUSY_ITER)
	{
	busy_window_refresh();
	busycnt = 0;
	}

/* fprintf(stderr, "%lld %d %s\n", *time, *facidx, *value); */

if(!(f->flags&(VZT_RD_SYM_F_DOUBLE|VZT_RD_SYM_F_STRING)))
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
else if(f->flags&VZT_RD_SYM_F_DOUBLE)
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
static void vzt_resolver(nptr np, nptr resolve)
{ 
np->ext = resolve->ext;
memcpy(&np->head, &resolve->head, sizeof(struct HistEnt));
np->curr = resolve->curr;
np->harray = resolve->harray;
np->numhist = resolve->numhist;
np->mvlfac=NULL;
}



/* 
 * actually import a vzt trace but don't do it if it's already been imported 
 */
void import_vzt_trace(nptr np)
{
struct HistEnt *htemp, *histent_tail;
int len, i;
struct fac *f;
int txidx;
nptr nold = np;

if(!(f=np->mvlfac)) return;	/* already imported */

txidx = f - mvlfacs;
if(np->mvlfac->flags&VZT_RD_SYM_F_ALIAS) 
	{
	txidx = vzt_rd_get_alias_root(vzt, txidx);
	np = mvlfacs[txidx].working_node;

	if(!(f=np->mvlfac)) 
		{
		vzt_resolver(nold, np);
		return;	/* already imported */
		}
	}

fprintf(stderr, "Import: %s\n", np->nname);

/* new stuff */
len = np->mvlfac->len;

if(f->array_height <= 1) /* sorry, arrays not supported, but vzt doesn't support them yet either */
	{
	vzt_rd_set_fac_process_mask(vzt, txidx);
	vzt_rd_iter_blocks(vzt, vzt_callback, NULL);
	vzt_rd_clr_fac_process_mask(vzt, txidx);
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

if(vzt_table[txidx].histent_curr)
	{
	vzt_table[txidx].histent_curr->next = htemp;
	htemp = vzt_table[txidx].histent_head;
	}

if(!(f->flags&(VZT_RD_SYM_F_DOUBLE|VZT_RD_SYM_F_STRING)))
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
        if(f->flags&VZT_RD_SYM_F_STRING) np->head.flags |= HIST_STRING;
        }

np->head.time  = -2;
np->head.next = htemp;
np->numhist=vzt_table[txidx].numtrans +2 /*endcap*/ +1 /*frontcap*/;

memset(vzt_table+txidx, 0, sizeof(struct lx2_entry));	/* zero it out */

np->curr = histent_tail;
np->mvlfac = NULL;	/* it's imported and cached so we can forget it's an mvlfac now */

if(nold!=np)
	{
	vzt_resolver(nold, np);
	}
}


/* 
 * pre-import many traces at once so function above doesn't have to iterate...
 */
void vzt_set_fac_process_mask(nptr np)
{
struct fac *f;
int txidx;

if(!(f=np->mvlfac)) return;	/* already imported */

txidx = f-mvlfacs;

if(np->mvlfac->flags&VZT_RD_SYM_F_ALIAS) 
	{
	txidx = vzt_rd_get_alias_root(vzt, txidx);
	np = mvlfacs[txidx].working_node;

	if(!(np->mvlfac)) return;	/* already imported */
	}

if(np->mvlfac->array_height <= 1) /* sorry, arrays not supported, but vzt doesn't support them yet either */
	{
	vzt_rd_set_fac_process_mask(vzt, txidx);
	vzt_table[txidx].np = np;
	}
}


void vzt_import_masked(void)
{
int txidx, i, cnt;

cnt = 0;
for(txidx=0;txidx<numfacs;txidx++)
	{
	if(vzt_rd_get_fac_process_mask(vzt, txidx))
		{
		cnt++;
		}
	}

if(!cnt) return;

if(cnt>100)
	{
	fprintf(stderr, VZT_RDLOAD"Extracting %d traces\n", cnt);
	}

set_window_busy(NULL);
vzt_rd_iter_blocks(vzt, vzt_callback, NULL);
set_window_idle(NULL);

for(txidx=0;txidx<numfacs;txidx++)
	{
	if(vzt_rd_get_fac_process_mask(vzt, txidx))
		{
		struct HistEnt *htemp, *histent_tail;
		struct fac *f = mvlfacs+txidx;
		int len = f->len;
		nptr np = vzt_table[txidx].np;

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

		if(vzt_table[txidx].histent_curr)
			{
			vzt_table[txidx].histent_curr->next = htemp;
			htemp = vzt_table[txidx].histent_head;
			}

		if(!(f->flags&(VZT_RD_SYM_F_DOUBLE|VZT_RD_SYM_F_STRING)))
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
		        if(f->flags&VZT_RD_SYM_F_STRING) np->head.flags |= HIST_STRING;
		        }

		np->head.time  = -2;
		np->head.next = htemp;
		np->numhist=vzt_table[txidx].numtrans +2 /*endcap*/ +1 /*frontcap*/;

		memset(vzt_table+txidx, 0, sizeof(struct lx2_entry));	/* zero it out */

		np->curr = histent_tail;
		np->mvlfac = NULL;	/* it's imported and cached so we can forget it's an mvlfac now */
		vzt_rd_clr_fac_process_mask(vzt, txidx);
		}
	}
}
