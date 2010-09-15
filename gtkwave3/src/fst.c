/* 
 * Copyright (c) Tony Bybell 2009.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <config.h>
#include "globals.h"
#include <stdio.h>
#include "fstapi.h"
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
#include "fstapi.h"
#include "debug.h"
#include "busy.h"
#include "hierpack.h"
#include "fst.h"

#define FST_RDLOAD "FSTLOAD | "

/******************************************************************/
                
/*
 * doubles going into histent structs are NEVER freed so this is OK.. 
 * (we are allocating as many entries that fit in 4k minus the size of the two
 * bookkeeping void* pointers found in the malloc_2/free_2 routines in
 * debug.c)
 */
#define FST_DOUBLE_GRANULARITY ( ( (4*1024)-(2*sizeof(void *)) ) / sizeof(double) )
                 
static void *double_slab_calloc(void)
{
if(GLOBALS->double_curr_fst==GLOBALS->double_fini_fst)
        {
        GLOBALS->double_curr_fst=(double *)calloc_2(FST_DOUBLE_GRANULARITY, sizeof(double));
        GLOBALS->double_fini_fst=GLOBALS->double_curr_fst+FST_DOUBLE_GRANULARITY;
        }

return((void *)(GLOBALS->double_curr_fst++));
}  
  
/******************************************************************/


static struct fstHier *extractNextVar(void *xc, int *msb, int *lsb, char **nam)
{
struct fstHier *h;
const char *pnts;
char *pnt, *pntd, *lb_last = NULL, *col_last = NULL, *rb_last = NULL;
int acc;
char *s;
unsigned char ttype;

while((h = fstReaderIterateHier(xc)))
        {
        switch(h->htyp)
                {
                case FST_HT_SCOPE:
                        GLOBALS->fst_scope_name = fstReaderPushScope(xc, h->u.scope.name, GLOBALS->mod_tree_parent);

			switch(h->u.scope.typ)
				{
				case FST_ST_VCD_MODULE:		ttype = TREE_VCD_ST_MODULE; break;
				case FST_ST_VCD_TASK:		ttype = TREE_VCD_ST_TASK; break;
				case FST_ST_VCD_FUNCTION:	ttype = TREE_VCD_ST_FUNCTION; break;
				case FST_ST_VCD_BEGIN:		ttype = TREE_VCD_ST_BEGIN; break;
				case FST_ST_VCD_FORK:		ttype = TREE_VCD_ST_FORK; break;
				default:			ttype = TREE_UNKNOWN; break;
				}

			allocate_and_decorate_module_tree_node(ttype, h->u.scope.name);
                        break;
                case FST_HT_UPSCOPE:
			GLOBALS->mod_tree_parent = fstReaderGetCurrentScopeUserInfo(xc);
                        GLOBALS->fst_scope_name = fstReaderPopScope(xc);
                        break;
                case FST_HT_VAR:
                        GLOBALS->fst_scope_name = fstReaderGetCurrentFlatScope(xc);

			s = malloc_2(strlen(h->u.var.name) + 1);
			pnts = h->u.var.name;
			pntd = s;
			while(*pnts)
				{
				if(*pnts != ' ')
					{
					if(*pnts == '[') { lb_last = pntd; col_last = NULL; }
					else if(*pnts == ':') { col_last = pntd; }
					else if(*pnts == ']') { rb_last = pntd; }

					*(pntd++) = *pnts;
					}
				pnts++;
				}			
			*pntd = 0;

			if(!lb_last)
				{
				*msb = *lsb = -1;
				}
			else
				{
				int sgna = 1, sgnb = 1;
				pnt = lb_last + 1;
				acc = 0;
				while(isdigit(*pnt) || (*pnt == '-'))
					{
					if(*pnt != '-')
						{
						acc *= 10;
						acc += (*pnt - '0');
						}
						else
						{
						sgna = -1;
						}
					pnt++;
					}

				*msb = acc * sgna;
				if(!col_last)
					{
					*lsb = acc;
					}
				else
					{
					pnt = col_last + 1;
					acc = 0;
					while(isdigit(*pnt) || (*pnt == '-'))	
						{
						if(*pnt != '-')
							{
							acc *= 10;
							acc += (*pnt - '0');
							}
							else
							{
							sgnb = -1;
							}
						pnt++;
						}
					*lsb = acc * sgnb;
					}
				}			

			if(lb_last) *lb_last = 0;
			*nam = s;
			return(h);
                        break;
                }
        }

*nam = NULL;
return(NULL);
}


static void fst_append_graft_chain(int len, char *nam, int which, struct tree *par)
{
struct tree *t = calloc_2(1, sizeof(struct tree) + len);

strcpy(t->name, nam);
t->which = which;

t->child = par;
t->next = GLOBALS->terminals_tchain_tree_c_1;
GLOBALS->terminals_tchain_tree_c_1 = t;
}


/*
 * mainline
 */
TimeType fst_main(char *fname, char *skip_start, char *skip_end)
{
int i;
struct Node *n;
struct symbol *s, *prevsymroot=NULL, *prevsym=NULL;
signed char scale;
unsigned int numalias = 0;
unsigned int numvars = 0;
struct symbol *sym_block = NULL;
struct Node *node_block = NULL;
struct fstHier *h = NULL;
int msb, lsb;
char *nnam = NULL, *pnam = NULL, *pnam2 = NULL;
uint32_t activity_idx, num_activity_changes;
struct tree *npar = NULL, *ppar = NULL;
char **f_name = NULL;
int allowed_to_autocoalesce;

GLOBALS->fst_fst_c_1 = fstReaderOpen(fname);
if(!GLOBALS->fst_fst_c_1)
        {
	return(LLDescriptor(0));	/* look at GLOBALS->fst_fst_c_1 in caller for success status... */
        }
/* SPLASH */                            splash_create();

allowed_to_autocoalesce = (strstr(fstReaderGetVersionString(GLOBALS->fst_fst_c_1), "Icarus") == NULL);
if(!allowed_to_autocoalesce)
	{
	GLOBALS->autocoalesce = 0;
	}

scale=(signed char)fstReaderGetTimescale(GLOBALS->fst_fst_c_1);
exponent_to_time_scale(scale);

GLOBALS->numfacs=fstReaderGetVarCount(GLOBALS->fst_fst_c_1);
GLOBALS->mvlfacs_fst_c_3=(struct fac *)calloc_2(GLOBALS->numfacs,sizeof(struct fac));
f_name = calloc_2(F_NAME_MODULUS+1,sizeof(char *));
GLOBALS->fst_table_fst_c_1=(struct lx2_entry *)calloc_2(GLOBALS->numfacs, sizeof(struct lx2_entry));
sym_block = (struct symbol *)calloc_2(GLOBALS->numfacs, sizeof(struct symbol));
node_block=(struct Node *)calloc_2(GLOBALS->numfacs,sizeof(struct Node));
GLOBALS->mvlfacs_fst_alias = calloc_2(GLOBALS->numfacs,sizeof(fstHandle));
GLOBALS->mvlfacs_fst_rvs_alias = calloc_2(GLOBALS->numfacs,sizeof(fstHandle));

fprintf(stderr, FST_RDLOAD"Processing %d facs.\n", GLOBALS->numfacs);
/* SPLASH */                            splash_sync(1, 5);

GLOBALS->first_cycle_fst_c_3 = (TimeType) fstReaderGetStartTime(GLOBALS->fst_fst_c_1) * GLOBALS->time_scale;
GLOBALS->last_cycle_fst_c_3 = (TimeType) fstReaderGetEndTime(GLOBALS->fst_fst_c_1) * GLOBALS->time_scale;
GLOBALS->total_cycles_fst_c_3 = GLOBALS->last_cycle_fst_c_3 - GLOBALS->first_cycle_fst_c_3 + 1;

/* blackout region processing */
num_activity_changes = fstReaderGetNumberDumpActivityChanges(GLOBALS->fst_fst_c_1);
for(activity_idx = 0; activity_idx < num_activity_changes; activity_idx++)
	{
	uint32_t activity_idx2;
	uint64_t ct = fstReaderGetDumpActivityChangeTime(GLOBALS->fst_fst_c_1, activity_idx);
	unsigned char ac = fstReaderGetDumpActivityChangeValue(GLOBALS->fst_fst_c_1, activity_idx);

	if(ac == 1) continue;
	if((activity_idx+1) == num_activity_changes)
		{
		struct blackout_region_t *bt = calloc_2(1, sizeof(struct blackout_region_t));
		bt->bstart = (TimeType)(ct * GLOBALS->time_scale);
		bt->bend = (TimeType)(GLOBALS->last_cycle_fst_c_3 * GLOBALS->time_scale);
                bt->next = GLOBALS->blackout_regions;
  
                GLOBALS->blackout_regions = bt;

		activity_idx = activity_idx2;
		break;
		}

	for(activity_idx2 = activity_idx+1; activity_idx2 < num_activity_changes; activity_idx2++)
		{
		uint64_t ct2 = fstReaderGetDumpActivityChangeTime(GLOBALS->fst_fst_c_1, activity_idx2);
		ac = fstReaderGetDumpActivityChangeValue(GLOBALS->fst_fst_c_1, activity_idx2);		
		if((ac == 0) && (activity_idx2 == (num_activity_changes-1)))
			{
			ac = 1;
			ct2 = GLOBALS->last_cycle_fst_c_3;
			}

		if(ac == 1)
			{
			struct blackout_region_t *bt = calloc_2(1, sizeof(struct blackout_region_t));
			bt->bstart = (TimeType)(ct * GLOBALS->time_scale);
			bt->bend = (TimeType)(ct2 * GLOBALS->time_scale);
	                bt->next = GLOBALS->blackout_regions;
  
	                GLOBALS->blackout_regions = bt;

			activity_idx = activity_idx2;
			break;
			}
		}	
	
	}


/* do your stuff here..all useful info has been initialized by now */

if(!GLOBALS->hier_was_explicitly_set)    /* set default hierarchy split char */
        {
        GLOBALS->hier_delimeter='.';
        }

if(GLOBALS->numfacs)
	{
	char *fnam;
	char *pnt = NULL;
	int was_packed = 0;
	int hier_len, name_len;

	h = extractNextVar(GLOBALS->fst_fst_c_1, &msb, &lsb, &nnam);
	if(!h)
		{
		fstReaderClose(GLOBALS->fst_fst_c_1); GLOBALS->fst_fst_c_1 = NULL;
		return(LLDescriptor(0));
		}

	npar = GLOBALS->mod_tree_parent;
	name_len = strlen(nnam);
	hier_len = GLOBALS->fst_scope_name ? strlen(GLOBALS->fst_scope_name) : 0;
	if(hier_len)
		{
		fnam = malloc_2(hier_len + 1 + name_len + 1);
		memcpy(fnam, GLOBALS->fst_scope_name, hier_len);
		fnam[hier_len] = GLOBALS->hier_delimeter;
		memcpy(fnam + hier_len + 1, nnam, name_len + 1);
		}
		else
		{
		fnam = malloc_2(name_len + 1);
		memcpy(fnam, nnam, name_len + 1);
		}
	/* free_2(nnam); ...deallocated through pnam */
	
	if(GLOBALS->do_hier_compress)
		{
		pnt = hier_compress(fnam, HIERPACK_ADD, &was_packed);
		}

	if(was_packed)
		{
		f_name[0] = pnt;
		free_2(fnam);
		}
		else
		{
		f_name[0]=fnam;
		}
	}


for(i=0;i<GLOBALS->numfacs;i++)
        {
	char buf[65537];
	char *str;	
	struct fac *f;
	int hier_len, name_len;
	unsigned char nvt;

	GLOBALS->mvlfacs_fst_c_3[i].array_height = 1;
        if((h->u.var.length > 1) && (msb == -1) && (lsb == -1))
		{
		GLOBALS->mvlfacs_fst_c_3[i].msb = h->u.var.length - 1;
		GLOBALS->mvlfacs_fst_c_3[i].lsb = 0;
		}
		else
		{	
		GLOBALS->mvlfacs_fst_c_3[i].msb = msb;
		GLOBALS->mvlfacs_fst_c_3[i].lsb = lsb;
		}
	GLOBALS->mvlfacs_fst_c_3[i].len = h->u.var.length; 

	switch(h->u.var.typ)
		{
                case FST_VT_VCD_EVENT: 		nvt = ND_VCD_EVENT; break;
                case FST_VT_VCD_INTEGER: 	nvt = ND_VCD_INTEGER; break;
                case FST_VT_VCD_PARAMETER: 	nvt = ND_VCD_PARAMETER; break;
                case FST_VT_VCD_REAL: 		nvt = ND_VCD_REAL; break;
                case FST_VT_VCD_REAL_PARAMETER: nvt = ND_VCD_REAL_PARAMETER; break;
                case FST_VT_VCD_REALTIME:       nvt = ND_VCD_REALTIME; break;
                case FST_VT_VCD_REG: 		nvt = ND_VCD_REG; break;
                case FST_VT_VCD_SUPPLY0: 	nvt = ND_VCD_SUPPLY0; break;
                case FST_VT_VCD_SUPPLY1: 	nvt = ND_VCD_SUPPLY1; break;
                case FST_VT_VCD_TIME: 		nvt = ND_VCD_TIME; break;
                case FST_VT_VCD_TRI: 		nvt = ND_VCD_TRI; break;
                case FST_VT_VCD_TRIAND: 	nvt = ND_VCD_TRIAND; break;
                case FST_VT_VCD_TRIOR: 		nvt = ND_VCD_TRIOR; break;
                case FST_VT_VCD_TRIREG: 	nvt = ND_VCD_TRIREG; break;
                case FST_VT_VCD_TRI0: 		nvt = ND_VCD_TRI0; break;
                case FST_VT_VCD_TRI1: 		nvt = ND_VCD_TRI1; break;
                case FST_VT_VCD_WAND: 		nvt = ND_VCD_WAND; break;
                case FST_VT_VCD_WIRE: 		nvt = ND_VCD_WIRE; break;
                case FST_VT_VCD_WOR: 		nvt = ND_VCD_WOR; break;
                case FST_VT_VCD_PORT: 		nvt = ND_VCD_PORT; break;
		default: 			nvt = ND_UNSPECIFIED_DEFAULT; break;
		}

	switch(h->u.var.typ)
		{
		case FST_VT_VCD_PARAMETER:
		case FST_VT_VCD_INTEGER:
			GLOBALS->mvlfacs_fst_c_3[i].flags = VZT_RD_SYM_F_INTEGER;
			break;	

		case FST_VT_VCD_REAL:
		case FST_VT_VCD_REAL_PARAMETER:
		case FST_VT_VCD_REALTIME:
			GLOBALS->mvlfacs_fst_c_3[i].flags = VZT_RD_SYM_F_DOUBLE;
			break;

		default:
			GLOBALS->mvlfacs_fst_c_3[i].flags = VZT_RD_SYM_F_BITS;
			break;	
		}
	
	if(h->u.var.is_alias)
		{
		GLOBALS->mvlfacs_fst_alias[i] = h->u.var.handle - 1; /* subtract 1 to scale it with gtkwave-style numbering */
		GLOBALS->mvlfacs_fst_c_3[i].flags |= VZT_RD_SYM_F_ALIAS;
		numalias++;
		}
	else
		{
		GLOBALS->mvlfacs_fst_rvs_alias[numvars] = i;
		GLOBALS->mvlfacs_fst_alias[i] = numvars;
		numvars++;
		}

	if(pnam2) { free_2(pnam2); }
	pnam2 = pnam;
	pnam = nnam;

	ppar = npar;

	if(i!=(GLOBALS->numfacs-1))
		{
		char *fnam;
		char *pnt = NULL;
		int was_packed = 0;

		h = extractNextVar(GLOBALS->fst_fst_c_1, &msb, &lsb, &nnam);
		if(!h)
			{
			/* this should never happen */
			fstReaderIterateHierRewind(GLOBALS->fst_fst_c_1);
			h = extractNextVar(GLOBALS->fst_fst_c_1, &msb, &lsb, &nnam);
			}
		npar = GLOBALS->mod_tree_parent;
		name_len = strlen(nnam);
		hier_len = GLOBALS->fst_scope_name ? strlen(GLOBALS->fst_scope_name) : 0;
		if(hier_len)
			{
			fnam = malloc_2(hier_len + 1 + name_len + 1);
			memcpy(fnam, GLOBALS->fst_scope_name, hier_len);
			fnam[hier_len] = GLOBALS->hier_delimeter;
			memcpy(fnam + hier_len + 1, nnam, name_len + 1);
			}
			else
			{
			fnam = malloc_2(name_len + 1);
			memcpy(fnam, nnam, name_len + 1);
			}
		/* free_2(nnam); ...deallocated through pnam */

		if(GLOBALS->do_hier_compress)
			{
			pnt = hier_compress(fnam, HIERPACK_ADD, &was_packed);
			}

		if(was_packed)
			{
			f_name[(i+1)&F_NAME_MODULUS] = pnt;
			free_2(fnam);
			}
			else
			{
			f_name[(i+1)&F_NAME_MODULUS]=fnam;
			}
		}

	if(i>1)
		{
		free_2(f_name[(i-2)&F_NAME_MODULUS]);
		f_name[(i-2)&F_NAME_MODULUS] = NULL;
		}

	f=GLOBALS->mvlfacs_fst_c_3+i;

	if((f->len>1)&& (!(f->flags&(VZT_RD_SYM_F_INTEGER|VZT_RD_SYM_F_DOUBLE|VZT_RD_SYM_F_STRING))) )
		{
		int len=sprintf(buf, "%s[%d:%d]", f_name[(i)&F_NAME_MODULUS],GLOBALS->mvlfacs_fst_c_3[i].msb, GLOBALS->mvlfacs_fst_c_3[i].lsb);
		str=malloc_2(len+1);

		if(!GLOBALS->alt_hier_delimeter)
			{
			strcpy(str, buf);
			}
			else
			{
			strcpy_vcdalt(str, buf, GLOBALS->alt_hier_delimeter);
			}
		s=&sym_block[i];
	        symadd_name_exists_sym_exists(s,str,0);
		prevsymroot = prevsym = NULL;

		len = sprintf(buf, "%s[%d:%d]", pnam,GLOBALS->mvlfacs_fst_c_3[i].msb, GLOBALS->mvlfacs_fst_c_3[i].lsb);
		fst_append_graft_chain(len, buf, i, ppar);
		}
	else if ( 
			((f->len==1)&&(!(f->flags&(VZT_RD_SYM_F_INTEGER|VZT_RD_SYM_F_DOUBLE|VZT_RD_SYM_F_STRING)))&&
			((i!=GLOBALS->numfacs-1)&&(!strcmp(f_name[(i)&F_NAME_MODULUS], f_name[(i+1)&F_NAME_MODULUS]))))
			||
			(((i!=0)&&(!strcmp(f_name[(i)&F_NAME_MODULUS], f_name[(i-1)&F_NAME_MODULUS]))) &&
			(GLOBALS->mvlfacs_fst_c_3[i].msb!=-1)&&(GLOBALS->mvlfacs_fst_c_3[i].lsb!=-1))
		)
		{
		int len = sprintf(buf, "%s[%d]", f_name[(i)&F_NAME_MODULUS],GLOBALS->mvlfacs_fst_c_3[i].msb);
		str=malloc_2(len+1);
		if(!GLOBALS->alt_hier_delimeter)
			{
			strcpy(str, buf);
			}
			else
			{
			strcpy_vcdalt(str, buf, GLOBALS->alt_hier_delimeter);
			}
		s=&sym_block[i];
	        symadd_name_exists_sym_exists(s,str,0);
		if((allowed_to_autocoalesce)&&(prevsym)&&(i>0)&&(!strcmp(f_name[(i)&F_NAME_MODULUS], f_name[(i-1)&F_NAME_MODULUS]))&&(!strchr(f_name[(i)&F_NAME_MODULUS], '\\')))	/* allow chaining for search functions.. */
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

		len = sprintf(buf, "%s[%d]", pnam,GLOBALS->mvlfacs_fst_c_3[i].msb);
		fst_append_graft_chain(len, buf, i, ppar);
		}
		else
		{
		str=malloc_2(strlen(f_name[(i)&F_NAME_MODULUS])+1);
		if(!GLOBALS->alt_hier_delimeter)
			{
			strcpy(str, f_name[(i)&F_NAME_MODULUS]);
			}
			else
			{
			strcpy_vcdalt(str, f_name[(i)&F_NAME_MODULUS], GLOBALS->alt_hier_delimeter);
			}
		s=&sym_block[i];
	        symadd_name_exists_sym_exists(s,str,0);
		prevsymroot = prevsym = NULL;

		if(f->flags&VZT_RD_SYM_F_INTEGER)
			{
			GLOBALS->mvlfacs_fst_c_3[i].msb=31;
			GLOBALS->mvlfacs_fst_c_3[i].lsb=0;
			GLOBALS->mvlfacs_fst_c_3[i].len=32;
			}

		fst_append_graft_chain(strlen(pnam), pnam, i, ppar);
		}
		
        n=&node_block[i];
        n->nname=s->name;
        n->mv.mvlfac = GLOBALS->mvlfacs_fst_c_3+i;
	GLOBALS->mvlfacs_fst_c_3[i].working_node = n;
	n->vartype = nvt;

	if((f->len>1)||(f->flags&(VZT_RD_SYM_F_DOUBLE|VZT_RD_SYM_F_STRING)))
		{
		n->msi = GLOBALS->mvlfacs_fst_c_3[i].msb;
		n->lsi = GLOBALS->mvlfacs_fst_c_3[i].lsb;
		n->extvals = 1;
		}
                 
        n->head.time=-1;        /* mark 1st node as negative time */
        n->head.v.h_val=AN_X;
        s->n=n;
        }			/* for(i) of facs parsing */

if(pnam2) { free_2(pnam2); pnam2 = NULL; }
if(pnam) { free_2(pnam); pnam = NULL; }

for(i=0;i<=F_NAME_MODULUS;i++)
	{
	if(f_name[i])
		{
		free_2(f_name[i]);
		f_name[i] = NULL;
		}
	}
free_2(f_name); f_name = NULL;

fprintf(stderr, FST_RDLOAD"Built %d signal%s and %d alias%s.\n", 
	numvars, (numvars == 1) ? "" : "s", 
	numalias, (numalias == 1) ? "" : "es");

GLOBALS->fst_maxhandle = numvars;

/* SPLASH */                            splash_sync(2, 5);  
GLOBALS->facs=(struct symbol **)malloc_2(GLOBALS->numfacs*sizeof(struct symbol *));

create_hier_array();

if((GLOBALS->fast_tree_sort) && (!GLOBALS->do_hier_compress))
        {
        for(i=0;i<GLOBALS->numfacs;i++)
                {
                int len;
                GLOBALS->facs[i]=&sym_block[i]; 
                if((len=strlen(GLOBALS->facs[i]->name))>GLOBALS->longestname) GLOBALS->longestname=len;
                }
                                
/* SPLASH */                            splash_sync(3, 5);  
        fprintf(stderr, FST_RDLOAD"Building facility hierarchy tree.\n");
                                         
        init_tree();
        treegraft(&GLOBALS->treeroot);

/* SPLASH */                            splash_sync(4, 5);  
                                
        fprintf(stderr, FST_RDLOAD"Sorting facility hierarchy tree.\n");
        treesort(GLOBALS->treeroot, NULL);
/* SPLASH */                            splash_sync(5, 5);  
        order_facs_from_treesort(GLOBALS->treeroot, &GLOBALS->facs);
        GLOBALS->facs_are_sorted=1;
        }
        else
	{
	for(i=0;i<GLOBALS->numfacs;i++)
		{
		char *subst, ch;
		int len;
		int esc = 0;

		GLOBALS->facs[i]=&sym_block[i];
	        if((len=strlen(subst=GLOBALS->facs[i]->name))>GLOBALS->longestname) GLOBALS->longestname=len;
		while((ch=(*subst)))
			{	
#ifdef WAVE_HIERFIX
	                if(ch==GLOBALS->hier_delimeter) { *subst=(!esc) ? VCDNAM_HIERSORT : VCDNAM_ESCAPE; }    /* forces sort at hier boundaries */
#else
	                if((ch==GLOBALS->hier_delimeter)&&(esc)) { *subst = VCDNAM_ESCAPE; }    /* forces sort at hier boundaries */
#endif
	                else if(ch=='\\') { esc = 1; GLOBALS->escaped_names_found_vcd_c_1 = 1; }
	                subst++;
			}
		}

/* SPLASH */                            splash_sync(3, 5);
	fprintf(stderr, FST_RDLOAD"Sorting facilities at hierarchy boundaries.\n");
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
	fprintf(stderr, FST_RDLOAD"Building facility hierarchy tree.\n");

	init_tree();		
	for(i=0;i<GLOBALS->numfacs;i++)	
		{
		char *nf = GLOBALS->facs[i]->name;
		int was_packed;
		char *recon = hier_decompress_flagged(nf, &was_packed);
                
		if(was_packed)   
		        {
		        build_tree_from_name(recon, i);
		        free_2(recon);
		        }
		        else
		        {
		        build_tree_from_name(nf, i);
		        }
		}
/* SPLASH */                            splash_sync(5, 5);
	if(GLOBALS->escaped_names_found_vcd_c_1)
	        {
		for(i=0;i<GLOBALS->numfacs;i++)
			{
		        char *subst, ch;
		        subst=GLOBALS->facs[i]->name;
		        while((ch=(*subst)))
		                {
		                if(ch==VCDNAM_ESCAPE) { *subst=GLOBALS->hier_delimeter; } /* restore back to normal */
		                subst++;
		                }
			}
	        }
	treegraft(&GLOBALS->treeroot);
	treesort(GLOBALS->treeroot, NULL);
	if(GLOBALS->escaped_names_found_vcd_c_1)  
	        {
	        treenamefix(GLOBALS->treeroot);   
	        }
	}

if(GLOBALS->prev_hier_uncompressed_name) 
	{
	free_2(GLOBALS->prev_hier_uncompressed_name);
	GLOBALS->prev_hier_uncompressed_name = NULL; 
	}

#if 0
{
int num_dups = 0;
for(i=0;i<GLOBALS->numfacs-1;i++)
	{
	if(!strcmp(GLOBALS->facs[i]->name, GLOBALS->facs[i+1]->name))
		{
		fprintf(stderr, FST_RDLOAD"DUPLICATE FAC: '%s'\n", GLOBALS->facs[i]->name);
		num_dups++;
		}
	}

if(num_dups)
	{
	fprintf(stderr, FST_RDLOAD"Exiting, %d duplicate signals are present.\n", num_dups);
	exit(255);
	}
}
#endif

GLOBALS->min_time = GLOBALS->first_cycle_fst_c_3; GLOBALS->max_time=GLOBALS->last_cycle_fst_c_3;
GLOBALS->is_lx2 = LXT2_IS_FST;

if(skip_start || skip_end)
	{
	TimeType b_start, b_end;

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

	fstReaderSetLimitTimeRange(GLOBALS->fst_fst_c_1, b_start, b_end);
	GLOBALS->min_time = b_start;
	GLOBALS->max_time = b_end;
	}

fstReaderIterBlocksSetNativeDoublesOnCallback(GLOBALS->fst_fst_c_1, 1); /* to avoid bin -> ascii -> bin double swap */

return(GLOBALS->max_time);
}


/*
 * conversion from evcd -> vcd format
 */
static void evcd_memcpy(char *dst, const char *src, int len)
{
static const char *evcd="DUNZduLHXTlh01?FAaBbCcf";
static const char  *vcd="01xz0101xz0101xzxxxxxxz";
                                                
char ch;
int i, j;
                                                        
for(j=0;j<len;j++)
        {
	ch=*src;
        for(i=0;i<23;i++)
                {
                if(evcd[i]==ch)
                        {
                        *dst=vcd[i];
                        break;
                        }
                }
        if(i==23) *dst='x';
        
        src++;
        dst++;
        }
}


/*
 * fst callback (only does bits for now)
 */
static void fst_callback(void *user_callback_data_pointer, uint64_t tim, fstHandle txidx, const unsigned char *value)
{
fstHandle facidx = GLOBALS->mvlfacs_fst_rvs_alias[--txidx];
struct HistEnt *htemp;
struct lx2_entry *l2e = GLOBALS->fst_table_fst_c_1+facidx;
struct fac *f = GLOBALS->mvlfacs_fst_c_3+facidx;

GLOBALS->busycnt_fst_c_2++; 
if(GLOBALS->busycnt_fst_c_2==WAVE_BUSY_ITER)
	{
	busy_window_refresh();
	GLOBALS->busycnt_fst_c_2 = 0;
	}

/* fprintf(stderr, "%lld %d '%s'\n", tim, facidx, value); */

if(!(f->flags&(VZT_RD_SYM_F_DOUBLE|VZT_RD_SYM_F_STRING)))
	{
	unsigned char vt = ND_UNSPECIFIED_DEFAULT;
	if(f->working_node)
		{
		vt = f->working_node->vartype;
		}

	if(f->len>1)        
	        {
		char *h_vector = (char *)malloc_2(f->len);
		if(vt != ND_VCD_PORT)
			{
			memcpy(h_vector, value, f->len);
			}
			else
			{
			evcd_memcpy(h_vector, (const char *)value, f->len);
			}

		if((l2e->histent_curr)&&(l2e->histent_curr->v.h_vector)) /* remove duplicate values */
			{
			if(!memcmp(l2e->histent_curr->v.h_vector, h_vector, f->len))
				{
				free_2(h_vector);
				return;
				}
			}

		htemp = histent_calloc();
		htemp->v.h_vector = h_vector;
	        }
	        else
	        {
		unsigned char h_val;

		if(vt != ND_VCD_PORT)
			{
			switch(*value)
				{
				case '0':	h_val = AN_0; break;
				case '1':	h_val = AN_1; break;
				case 'Z':
				case 'z':	h_val = AN_Z; break;
				default:	h_val = AN_X; break;
				}
			}
			else
			{
			char membuf[1];
			evcd_memcpy(membuf, (const char *)value, 1);
			switch(*membuf)
				{
				case '0':	h_val = AN_0; break;
				case '1':	h_val = AN_1; break;
				case 'Z':
				case 'z':	h_val = AN_Z; break;
				default:	h_val = AN_X; break;
				}
			}

		if((vt != ND_VCD_EVENT) && (l2e->histent_curr)) /* remove duplicate values */
			{
			if(l2e->histent_curr->v.h_val == h_val)
				{
				return;
				}
			}

		htemp = histent_calloc();
		htemp->v.h_val = h_val;
	        }
	}
else if(f->flags&VZT_RD_SYM_F_DOUBLE)
	{
	if((l2e->histent_curr)&&(l2e->histent_curr->v.h_vector)) /* remove duplicate values */
		{
		if(!memcmp(l2e->histent_curr->v.h_vector, value, sizeof(double)))
			{
			return;
			}
		}

	/* if(fstReaderIterBlocksSetNativeDoublesOnCallback is disabled...)

	double *d = double_slab_calloc();
	sscanf(value, "%lg", d);
	htemp = histent_calloc();
	htemp->v.h_vector = (char *)d;

	otherwise...
	*/

	htemp = histent_calloc();
	htemp->v.h_vector = double_slab_calloc();
	memcpy(htemp->v.h_vector, value, sizeof(double));
	htemp->flags = HIST_REAL;
	}
else	/* string */
	{
	char *s;

	if((l2e->histent_curr)&&(l2e->histent_curr->v.h_vector)) /* remove duplicate values */
		{
		if(!strcmp(l2e->histent_curr->v.h_vector, (const char *)value))
			{
			return;
			}
		}

	s = malloc_2(strlen((const char *)value)+1);
	strcpy(s, (const char *)value);
	htemp = histent_calloc();
	htemp->v.h_vector = s;
	htemp->flags = HIST_REAL|HIST_STRING;
	}


htemp->time = (tim) * (GLOBALS->time_scale);

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
static void fst_resolver(nptr np, nptr resolve)
{ 
np->extvals = resolve->extvals;
np->msi = resolve->msi;
np->lsi = resolve->lsi;
memcpy(&np->head, &resolve->head, sizeof(struct HistEnt));
np->curr = resolve->curr;
np->harray = resolve->harray;
np->numhist = resolve->numhist;
np->mv.mvlfac=NULL;
}



/* 
 * actually import a fst trace but don't do it if it's already been imported 
 */
void import_fst_trace(nptr np)
{
hptr htemp, htempx=NULL, histent_tail;
int len, i;
struct fac *f;
int txidx;
nptr nold = np;

if(!(f=np->mv.mvlfac)) return;	/* already imported */

txidx = f - GLOBALS->mvlfacs_fst_c_3;
if(np->mv.mvlfac->flags&VZT_RD_SYM_F_ALIAS) 
	{
	txidx = GLOBALS->mvlfacs_fst_alias[txidx]; /* this is to map to fstHandles, so even non-aliased are remapped */
	txidx = GLOBALS->mvlfacs_fst_rvs_alias[txidx];
	np = GLOBALS->mvlfacs_fst_c_3[txidx].working_node;

	if(!(f=np->mv.mvlfac)) 
		{
		fst_resolver(nold, np);
		return;	/* already imported */
		}
	}

fprintf(stderr, "Import: %s\n", np->nname);

/* new stuff */
len = np->mv.mvlfac->len;

if(f->array_height <= 1) /* sorry, arrays not supported, but fst doesn't support them yet either */
	{
	fstReaderSetFacProcessMask(GLOBALS->fst_fst_c_1, GLOBALS->mvlfacs_fst_alias[txidx]+1);
	fstReaderIterBlocks(GLOBALS->fst_fst_c_1, fst_callback, NULL, NULL);
	fstReaderClrFacProcessMask(GLOBALS->fst_fst_c_1, GLOBALS->mvlfacs_fst_alias[txidx]+1);
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
	if(!(f->flags&VZT_RD_SYM_F_DOUBLE))
		{
		htemp->v.h_vector = (char *)malloc_2(len);
		for(i=0;i<len;i++) htemp->v.h_vector[i] = AN_X;
		}
		else
		{
                double *d = malloc_2(sizeof(double));

                *d = strtod("NaN", NULL);
                htemp->v.h_vector = (char *)d;
                htemp->flags = HIST_REAL;
		}
        htempx = htemp;
	}
	else
	{
	htemp->v.h_val = AN_X;		/* x */
	}
htemp->time = MAX_HISTENT_TIME-1;
htemp->next = histent_tail;			

if(GLOBALS->fst_table_fst_c_1[txidx].histent_curr)
	{
	GLOBALS->fst_table_fst_c_1[txidx].histent_curr->next = htemp;
	htemp = GLOBALS->fst_table_fst_c_1[txidx].histent_head;
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
        if(f->flags&VZT_RD_SYM_F_STRING) 
		{
		np->head.flags |= HIST_STRING;
		}
        }

	{
        struct HistEnt *htemp2 = calloc_2(1, sizeof(struct HistEnt));
        htemp2->time = -1;
        if(len>1)
        	{
                htemp2->v.h_vector = htempx->v.h_vector;
		htemp2->flags = htempx->flags;
                }
                else
                {
                htemp2->v.h_val = htemp->v.h_val;
                }
	htemp2->next = htemp;
        htemp = htemp2;
        GLOBALS->fst_table_fst_c_1[txidx].numtrans++;
        }

np->head.time  = -2;
np->head.next = htemp;
np->numhist=GLOBALS->fst_table_fst_c_1[txidx].numtrans +2 /*endcap*/ +1 /*frontcap*/;

memset(GLOBALS->fst_table_fst_c_1+txidx, 0, sizeof(struct lx2_entry));	/* zero it out */

np->curr = histent_tail;
np->mv.mvlfac = NULL;	/* it's imported and cached so we can forget it's an mvlfac now */

if(nold!=np)
	{
	fst_resolver(nold, np);
	}
}


/* 
 * pre-import many traces at once so function above doesn't have to iterate...
 */
void fst_set_fac_process_mask(nptr np)
{
struct fac *f;
int txidx;

if(!(f=np->mv.mvlfac)) return;	/* already imported */

txidx = f-GLOBALS->mvlfacs_fst_c_3;
if(np->mv.mvlfac->flags&VZT_RD_SYM_F_ALIAS) 
	{
	txidx = GLOBALS->mvlfacs_fst_alias[txidx];
	txidx = GLOBALS->mvlfacs_fst_rvs_alias[txidx]; 
	np = GLOBALS->mvlfacs_fst_c_3[txidx].working_node;

	if(!(np->mv.mvlfac)) return;	/* already imported */
	}

if(np->mv.mvlfac->array_height <= 1) /* sorry, arrays not supported, but fst doesn't support them yet either */
	{
	fstReaderSetFacProcessMask(GLOBALS->fst_fst_c_1, GLOBALS->mvlfacs_fst_alias[txidx]+1);
	GLOBALS->fst_table_fst_c_1[txidx].np = np;
	}
}


void fst_import_masked(void)
{
int txidxi, i, cnt;
hptr htempx = NULL;

cnt = 0;
for(txidxi=0;txidxi<GLOBALS->fst_maxhandle;txidxi++)
	{
	if(fstReaderGetFacProcessMask(GLOBALS->fst_fst_c_1, txidxi+1))
		{
		cnt++;
		}
	}

if(!cnt) 
	{
	return;
	}

if(cnt>100)
	{
	fprintf(stderr, FST_RDLOAD"Extracting %d traces\n", cnt);
	}

set_window_busy(NULL);
fstReaderIterBlocks(GLOBALS->fst_fst_c_1, fst_callback, NULL, NULL);
set_window_idle(NULL);

for(txidxi=0;txidxi<GLOBALS->fst_maxhandle;txidxi++)
	{
	if(fstReaderGetFacProcessMask(GLOBALS->fst_fst_c_1, txidxi+1))
		{
		int txidx = GLOBALS->mvlfacs_fst_rvs_alias[txidxi];
		struct HistEnt *htemp, *histent_tail;
		struct fac *f = GLOBALS->mvlfacs_fst_c_3+txidx;
		int len = f->len;
		nptr np = GLOBALS->fst_table_fst_c_1[txidx].np;

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
			if(!(f->flags&VZT_RD_SYM_F_DOUBLE))
				{
				htemp->v.h_vector = (char *)malloc_2(len);
				for(i=0;i<len;i++) htemp->v.h_vector[i] = AN_X;
				htempx = htemp;
				}
				else
				{
				double *d = malloc_2(sizeof(double));

				*d = strtod("NaN", NULL);
				htemp->v.h_vector = (char *)d;
				htemp->flags = HIST_REAL;
				htempx = htemp;
				}
			}
			else
			{
			htemp->v.h_val = AN_X;		/* x */
			}
		htemp->time = MAX_HISTENT_TIME-1;
		htemp->next = histent_tail;			

		if(GLOBALS->fst_table_fst_c_1[txidx].histent_curr)
			{
			GLOBALS->fst_table_fst_c_1[txidx].histent_curr->next = htemp;
			htemp = GLOBALS->fst_table_fst_c_1[txidx].histent_head;
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
		        if(f->flags&VZT_RD_SYM_F_STRING) 
				{
				np->head.flags |= HIST_STRING;
				}
		        }

                        {
                        struct HistEnt *htemp2 = calloc_2(1, sizeof(struct HistEnt));
                        htemp2->time = -1;
                        if(len>1)
                                {
                                htemp2->v.h_vector = htempx->v.h_vector;
                                htemp2->flags = htempx->flags;
                                }
                                else
                                {
                                htemp2->v.h_val = htemp->v.h_val;
                                }
                        htemp2->next = htemp;
                        htemp = htemp2;
                        GLOBALS->fst_table_fst_c_1[txidx].numtrans++;
                        }

		np->head.time  = -2;
		np->head.next = htemp;
		np->numhist=GLOBALS->fst_table_fst_c_1[txidx].numtrans +2 /*endcap*/ +1 /*frontcap*/;

		memset(GLOBALS->fst_table_fst_c_1+txidx, 0, sizeof(struct lx2_entry));	/* zero it out */

		np->curr = histent_tail;
		np->mv.mvlfac = NULL;	/* it's imported and cached so we can forget it's an mvlfac now */
		fstReaderClrFacProcessMask(GLOBALS->fst_fst_c_1, txidxi+1);
		}
	}
}

/*
 * $Id$
 * $Log$
 * Revision 1.29  2010/09/14 21:51:39  gtkwave
 * fixed dangling fnam alloc
 *
 * Revision 1.28  2010/06/02 03:51:30  gtkwave
 * don't autocoalesce escape identifiers
 *
 * Revision 1.27  2010/06/02 03:23:04  gtkwave
 * disable autocoalesce if Icarus Verilog is detected
 *
 * Revision 1.26  2010/03/14 07:09:49  gtkwave
 * removed ExtNode and merged with Node
 *
 * Revision 1.25  2010/03/13 21:38:16  gtkwave
 * fixed && used in logical operations for allocating ExtNode
 *
 * Revision 1.24  2010/03/13 19:16:00  gtkwave
 * removal of useless symbol->nextinaet code
 *
 * Revision 1.23  2010/03/12 16:11:49  gtkwave
 * added missing hierarchy boundary sort
 *
 * Revision 1.22  2010/03/11 23:31:52  gtkwave
 * remove name field from struct fac
 *
 * Revision 1.21  2010/03/01 05:16:26  gtkwave
 * move compressed hier tree traversal to hierpack
 *
 * Revision 1.20  2010/02/22 21:13:36  gtkwave
 * added "realtime" VCD variable
 *
 * Revision 1.19  2009/08/06 20:03:31  gtkwave
 * warnings fixes
 *
 * Revision 1.18  2009/07/26 20:18:43  gtkwave
 * fix "x" value for time -1 on reals to be "NaN"
 *
 * Revision 1.17  2009/07/12 21:01:03  gtkwave
 * elide duplicate consecutive values for histents
 *
 * Revision 1.16  2009/07/07 20:12:53  gtkwave
 * convert hex capitalization to match verilog
 *
 * Revision 1.15  2009/07/07 15:48:37  gtkwave
 * EVCD "f" value fix (should be z not x)
 *
 * Revision 1.14  2009/07/06 21:41:36  gtkwave
 * evcd support issues
 *
 * Revision 1.13  2009/07/03 18:48:33  gtkwave
 * fst read compatibility fixes for mingw
 *
 * Revision 1.12  2009/07/01 16:47:47  gtkwave
 * move decorated module alloc routine to tree.c
 *
 * Revision 1.11  2009/07/01 08:16:33  gtkwave
 * hardening of fst reader for cygwin multiple loading
 *
 * Revision 1.10  2009/07/01 07:39:12  gtkwave
 * decorating hierarchy tree with module type info
 *
 * Revision 1.9  2009/06/29 18:16:23  gtkwave
 * adding framework for module type annotation on inner tree nodes
 *
 * Revision 1.8  2009/06/27 23:10:32  gtkwave
 * added display of type info for variables in tree view
 *
 * Revision 1.7  2009/06/25 18:31:19  gtkwave
 * added event types for VCD+FST and impulse arrows on event types
 *
 * Revision 1.6  2009/06/24 21:54:53  gtkwave
 * added sign bits to bitfield parsing for vars
 *
 * Revision 1.5  2009/06/23 22:18:09  gtkwave
 * added slab allocator for doubles in FST traces
 *
 * Revision 1.4  2009/06/20 19:36:56  gtkwave
 * floating-point read optimizations in read iter blocks
 *
 * Revision 1.3  2009/06/08 03:51:46  gtkwave
 * added reverse mappings to facidx for interleaved normal + alias signal fix
 *
 * Revision 1.2  2009/06/07 19:39:41  gtkwave
 * move to one pass hier processing algorithm, add blackout region support
 *
 * Revision 1.1  2009/06/07 08:40:44  gtkwave
 * adding FST support
 *
 */

