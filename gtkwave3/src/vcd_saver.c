#include"globals.h"/*
 * Copyright (c) Tony Bybell 2005-7.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <config.h>
#include "vcd_saver.h"
#include "helpers/lxt_write.h"
#include "ghw.h"
#include <time.h>


/*
 * unconvert trace data back to VCD representation...use strict mode for LXT
 */
static unsigned char analyzer_demang(int strict, unsigned char ch)
{
if(!strict)
	{
	if(ch < AN_COUNT)
		{
		return(AN_STR[ch]);
		}
		else
		{
		return(ch);
		}
	}
else
	{
	if(ch < AN_COUNT)
		{
		return(AN_STR4ST[ch]);
		}
		else
		{
		return(ch);
		}
	}
}


/*
 * generate a vcd identifier for a given facindx
 */
static char *vcdid(int value)
{
int i;
                                         
for(i=0;i<15;i++)
        {
        GLOBALS.buf_vcd_saver_c_3[i]=(char)((value%94)+33); /* for range 33..126 */
        value=value/94;
        if(!value) {GLOBALS.buf_vcd_saver_c_3[i+1]=0; break;}
        }
                    
return(GLOBALS.buf_vcd_saver_c_3);
}

static char *vcd_truncate_bitvec(char *s)
{
char l, r;

r=*s;
if(r=='1')
        {
        return s;
        }
        else
        {
        s++;
        }

for(;;s++)
        {
        l=r; r=*s;
        if(!r) return (s-1);

        if(l!=r)
                {
                return(((l=='0')&&(r=='1'))?s:s-1);
                }
        }
}


/************************ splay ************************/

/*
 * integer splay
 */
typedef struct vcdsav_tree_node vcdsav_Tree;
struct vcdsav_tree_node {
    vcdsav_Tree * left, * right;
    nptr item;
    int val;
    hptr hist;
    int len;

    union
	{
	void *p;
	long l;
	int i;
	} handle;	/* future expansion for adding other writers that need pnt/handle info */

    unsigned char flags;
};


static long vcdsav_cmp_l(void *i, void *j)
{
long il = (long)i, jl = (long)j;
return(il - jl);
}

static vcdsav_Tree * vcdsav_splay (void *i, vcdsav_Tree * t) {
/* Simple top down splay, not requiring i to be in the tree t.  */
/* What it does is described above.                             */
    vcdsav_Tree N, *l, *r, *y;
    int dir;

    if (t == NULL) return t;
    N.left = N.right = NULL;
    l = r = &N;

    for (;;) {
	dir = vcdsav_cmp_l(i, t->item);
	if (dir < 0) {
	    if (t->left == NULL) break;
	    if (vcdsav_cmp_l(i, t->left->item)<0) {
		y = t->left;                           /* rotate right */
		t->left = y->right;
		y->right = t;
		t = y;
		if (t->left == NULL) break;
	    }
	    r->left = t;                               /* link right */
	    r = t;
	    t = t->left;
	} else if (dir > 0) {
	    if (t->right == NULL) break;
	    if (vcdsav_cmp_l(i, t->right->item)>0) {
		y = t->right;                          /* rotate left */
		t->right = y->left;
		y->left = t;
		t = y;
		if (t->right == NULL) break;
	    }
	    l->right = t;                              /* link left */
	    l = t;
	    t = t->right;
	} else {
	    break;
	}
    }
    l->right = t->left;                                /* assemble */
    r->left = t->right;
    t->left = N.right;
    t->right = N.left;
    return t;
}


static vcdsav_Tree * vcdsav_insert(void *i, vcdsav_Tree * t, int val, unsigned char flags, hptr h) {
/* Insert i into the tree t, unless it's already there.    */
/* Return a pointer to the resulting tree.                 */
    vcdsav_Tree * n;
    int dir;
    
    n = (vcdsav_Tree *) calloc_2(1, sizeof (vcdsav_Tree));
    if (n == NULL) {
	fprintf(stderr, "vcdsav_insert: ran out of memory, exiting.\n");
	exit(255);
    }
    n->item = i;
    n->val = val;
    n->flags = flags;
    n->hist = h;
    if (t == NULL) {
	n->left = n->right = NULL;
	return n;
    }
    t = vcdsav_splay(i,t);
    dir = vcdsav_cmp_l(i,t->item);
    if (dir<0) {
	n->left = t->left;
	n->right = t;
	t->left = NULL;
	return n;
    } else if (dir>0) {
	n->right = t->right;
	n->left = t;
	t->right = NULL;
	return n;
    } else { /* We get here if it's already in the tree */
             /* Don't add it again                      */
	free_2(n);
	return t;
    }
}

/************************ heap ************************/

static int hpcmp(vcdsav_Tree *hp1, vcdsav_Tree *hp2)
{
hptr n1 = hp1->hist;
hptr n2 = hp2->hist;
TimeType t1, t2;

if(n1)  t1 = n1->time; else t1 = MAX_HISTENT_TIME;
if(n2)  t2 = n2->time; else t2 = MAX_HISTENT_TIME;

if(t1 == t2)
	{
	return(0);
	}
else
if(t1 > t2)
	{
	return(-1);
	}
else
	{
	return(1);
	}
}


void recurse_build(vcdsav_Tree *vt, vcdsav_Tree ***hp)
{
if(vt->left) recurse_build(vt->left, hp);

**hp = vt;
*hp = (*hp) + 1;

if(vt->right) recurse_build(vt->right, hp);
}


/* 
 * heapify algorithm...used to grab the next value change
 */
static void heapify(int i, int heap_size)
{
int l, r;
unsigned int largest;
vcdsav_Tree *t;
int maxele=heap_size/2-1;	/* points to where heapswaps don't matter anymore */
                
for(;;)
        {
        l=2*i+1;
        r=l+1;
                         
        if((l<heap_size)&&(hpcmp(GLOBALS.hp_vcd_saver_c_1[l],GLOBALS.hp_vcd_saver_c_1[i])>0))
                {
                largest=l;
                }   
                else
                {
                largest=i;
                }
        if((r<heap_size)&&(hpcmp(GLOBALS.hp_vcd_saver_c_1[r],GLOBALS.hp_vcd_saver_c_1[largest])>0))
                {
                largest=r;
                }
        
        if(i!=largest)
                {
                t=GLOBALS.hp_vcd_saver_c_1[i];
                GLOBALS.hp_vcd_saver_c_1[i]=GLOBALS.hp_vcd_saver_c_1[largest];
                GLOBALS.hp_vcd_saver_c_1[largest]=t;
                
                if(largest<=maxele)
                        {
                        i=largest;
                        }
                        else
                        {
                        break;
                        } 
                }   
                else
                {
                break;
                }
        }
}


/*
 * mainline
 */ 
int save_nodes_to_export(const char *fname, int export_typ)
{
Trptr t = GLOBALS.traces.first;
int nodecnt = 0;
vcdsav_Tree *vt = NULL;
vcdsav_Tree **hp_clone = GLOBALS.hp_vcd_saver_c_1;
nptr n;
ExtNode *e;
int msi, lsi;
int i;
TimeType prevtime = LLDescriptor(-1);
time_t walltime;
struct strace *st = NULL;
int strace_append = 0;
int max_len = 1;
char *row_data = NULL;
struct lt_trace *lt = NULL;
int lxt = (export_typ == WAVE_EXPORT_LXT);

errno = 0;
if(lxt)
	{
	lt = lt_init(fname);
	if(!lt)
		{
		return(VCDSAV_FILE_ERROR);
		}
	}
	else
	{
	GLOBALS.f_vcd_saver_c_1 = fopen(fname, "wb");
	if(!GLOBALS.f_vcd_saver_c_1)
		{
		return(VCDSAV_FILE_ERROR);
		}
	}

while(t)
	{
	if(!t->vector)
		{
		if(t->n.nd) 
			{
			n = t->n.nd;
			if(n->expansion) n = n->expansion->parent;
			e = n->ext;
			if(e) { msi = e->msi; lsi = e->lsi; } else { msi = lsi = -1; }
			vt = vcdsav_splay(n, vt);
			if(!vt || vt->item != n)
				{
				unsigned char flags = 0;

				if(n->head.next)
				if(n->head.next->next)
					{
					flags = n->head.next->next->flags;
					}

				vt = vcdsav_insert(n, vt, ++nodecnt, flags, &n->head);
				}
			}
		}	
		else
		{
		bvptr b = t->n.vec;
		if(b)
			{
			bptr bt = b->bits;
			if(bt)
				{
				for(i=0;i<bt->nbits;i++)
					{
					if(bt->nodes[i]) 
						{
						n = bt->nodes[i];
	
						if(n->expansion) n = n->expansion->parent;
						e = n->ext;
						if(e) { msi = e->msi; lsi = e->lsi; } else { msi = lsi = -1; }
						vt = vcdsav_splay(n, vt);
						if(!vt || vt->item != n)
							{
							unsigned char flags = 0;

							if(n->head.next)
							if(n->head.next->next)
								{
								flags = n->head.next->next->flags;
								}

							vt = vcdsav_insert(n, vt, ++nodecnt, flags, &n->head);
							}
						}
					}
				}
			}
		}

	if(!strace_append)
		{
		t=t->t_next;
		if(!t) 
			{
	                if(GLOBALS.shadow_straces)
	                        {
	                        swap_strace_contexts();
				st = GLOBALS.straces;

				strace_append = 1;
				t = st ? st->trace : NULL;
				}
			}
		}
		else
		{
		st = st->next;
		t = st ? st->trace : NULL;
		}
	}

if(strace_append) swap_strace_contexts();
if(!nodecnt) return(VCDSAV_EMPTY);


/* header */
if(lxt)
	{
	int dim;

	lt_set_chg_compress(lt);
	lt_set_clock_compress(lt);
	lt_set_initial_value(lt, 'x');	
	lt_set_time64(lt, 0);
	lt_symbol_bracket_stripping(lt, 1);

	switch(GLOBALS.time_dimension)
		{
		case 'm':	dim = -3; break;
		case 'u':	dim = -6; break;
		case 'n':	dim = -9; break;
		case 'p':	dim = -12; break;
		case 'f':	dim = -15; break;
		default: 	dim = 0; break;
		}

	lt_set_timescale(lt, dim);
	}
	else
	{
	time(&walltime);
	fprintf(GLOBALS.f_vcd_saver_c_1, "$date\n");
	fprintf(GLOBALS.f_vcd_saver_c_1, "\t%s",asctime(localtime(&walltime)));
	fprintf(GLOBALS.f_vcd_saver_c_1, "$end\n");
	fprintf(GLOBALS.f_vcd_saver_c_1, "$version\n\t"WAVE_VERSION_INFO"\n$end\n");
	fprintf(GLOBALS.f_vcd_saver_c_1, "$timescale\n\t%d%c%s\n$end\n", (int)GLOBALS.time_scale, GLOBALS.time_dimension, (GLOBALS.time_dimension=='s') ? "" : "s");
	}

/* write out netnames here ... */
hp_clone = GLOBALS.hp_vcd_saver_c_1 = calloc_2(nodecnt, sizeof(vcdsav_Tree *));
recurse_build(vt, &hp_clone);

for(i=0;i<nodecnt;i++)
	{
	char *netname = lxt ? GLOBALS.hp_vcd_saver_c_1[i]->item->nname : output_hier(GLOBALS.hp_vcd_saver_c_1[i]->item->nname);

	if(GLOBALS.hp_vcd_saver_c_1[i]->flags & (HIST_REAL|HIST_STRING))
		{
		if(lxt)
			{
			GLOBALS.hp_vcd_saver_c_1[i]->handle.p = lt_symbol_add(lt, netname, 0, 0, 0, GLOBALS.hp_vcd_saver_c_1[i]->flags & HIST_STRING ? LT_SYM_F_STRING : LT_SYM_F_DOUBLE);
			}
			else
			{
			fprintf(GLOBALS.f_vcd_saver_c_1, "$var real 1 %s %s $end\n", vcdid(GLOBALS.hp_vcd_saver_c_1[i]->val), netname);
			}
		}
		else
		{
		int msi = -1, lsi = -1;

		if(GLOBALS.hp_vcd_saver_c_1[i]->item->ext)
			{
			msi = GLOBALS.hp_vcd_saver_c_1[i]->item->ext->msi;
			lsi = GLOBALS.hp_vcd_saver_c_1[i]->item->ext->lsi;
			}
		
		if(msi==lsi)
			{
			if(lxt)
				{
				int strand_idx = strand_pnt(netname);
				if(strand_idx >= 0)
					{
					msi = lsi = atoi(netname + strand_idx + 1);
					}
				GLOBALS.hp_vcd_saver_c_1[i]->handle.p = lt_symbol_add(lt, netname, 0, msi, lsi, LT_SYM_F_BITS);
				}
				else
				{
				fprintf(GLOBALS.f_vcd_saver_c_1, "$var wire 1 %s %s $end\n", vcdid(GLOBALS.hp_vcd_saver_c_1[i]->val), netname);
				}
			}
			else
			{
			int len = (msi < lsi) ? (lsi - msi + 1) : (msi - lsi + 1);
			if(lxt)
				{
				GLOBALS.hp_vcd_saver_c_1[i]->handle.p = lt_symbol_add(lt, netname, 0, msi, lsi, LT_SYM_F_BITS);
				}
				else
				{
				fprintf(GLOBALS.f_vcd_saver_c_1, "$var wire %d %s %s $end\n", len, vcdid(GLOBALS.hp_vcd_saver_c_1[i]->val), netname);
				}
			GLOBALS.hp_vcd_saver_c_1[i]->len = len;
			if(len > max_len) max_len = len;
			}
		}

	}

row_data = malloc_2(max_len + 1);

if(!lxt)
	{
	output_hier("");
	free_hier();

	fprintf(GLOBALS.f_vcd_saver_c_1, "$enddefinitions $end\n");
	fprintf(GLOBALS.f_vcd_saver_c_1, "$dumpvars\n");
	}

/* value changes */

for(i=(nodecnt/2-1);i>0;i--)        /* build nodes into having heap property */
        {
        heapify(i,nodecnt);  
        }

for(;;)
	{
	heapify(0, nodecnt);

	if(!GLOBALS.hp_vcd_saver_c_1[0]->hist) break;
	if(GLOBALS.hp_vcd_saver_c_1[0]->hist->time > GLOBALS.max_time) break;

	if((GLOBALS.hp_vcd_saver_c_1[0]->hist->time != prevtime) && (GLOBALS.hp_vcd_saver_c_1[0]->hist->time >= LLDescriptor(0)))
		{
		TimeType tnorm = GLOBALS.hp_vcd_saver_c_1[0]->hist->time;
		if(GLOBALS.time_scale != 1)
			{
			tnorm /= GLOBALS.time_scale;
			}

		if(lxt)
			{
			lt_set_time64(lt, tnorm);
			}
			else
			{
			fprintf(GLOBALS.f_vcd_saver_c_1, "#"TTFormat"\n", tnorm);
			}
		prevtime = GLOBALS.hp_vcd_saver_c_1[0]->hist->time;
		}
	
	if(GLOBALS.hp_vcd_saver_c_1[0]->hist->time >= LLDescriptor(0))
		{
		if(GLOBALS.hp_vcd_saver_c_1[0]->flags & (HIST_REAL|HIST_STRING))
			{
			if(GLOBALS.hp_vcd_saver_c_1[0]->flags & HIST_STRING)
				{
				char *vec = GLOBALS.hp_vcd_saver_c_1[0]->hist->v.h_vector ? GLOBALS.hp_vcd_saver_c_1[0]->hist->v.h_vector : "UNDEF";

				if(lxt)
					{
					lt_emit_value_string(lt, GLOBALS.hp_vcd_saver_c_1[0]->handle.p, 0, vec);
					}
					else
					{
					fprintf(GLOBALS.f_vcd_saver_c_1, "s%s %s\n", vec, vcdid(GLOBALS.hp_vcd_saver_c_1[0]->val));
					}
				}
				else
				{
				double *d = (double *)GLOBALS.hp_vcd_saver_c_1[0]->hist->v.h_vector;
                                double value;

				if(!d)
					{
	                                sscanf("NaN", "%lg", &value);
					}
					else
					{
					value = *d;
					}

				if(lxt)
					{
					lt_emit_value_double(lt, GLOBALS.hp_vcd_saver_c_1[0]->handle.p, 0, value);
					}
					else
					{
					fprintf(GLOBALS.f_vcd_saver_c_1, "r%.16g %s\n", value, vcdid(GLOBALS.hp_vcd_saver_c_1[0]->val));	
					}
				}	
			}
		else
		if(GLOBALS.hp_vcd_saver_c_1[0]->len)
			{
			if(GLOBALS.hp_vcd_saver_c_1[0]->hist->v.h_vector)
				{
				for(i=0;i<GLOBALS.hp_vcd_saver_c_1[0]->len;i++)
					{
					row_data[i] = analyzer_demang(lxt, GLOBALS.hp_vcd_saver_c_1[0]->hist->v.h_vector[i]);
					}				
				}
				else
				{
				for(i=0;i<GLOBALS.hp_vcd_saver_c_1[0]->len;i++)
					{
					row_data[i] = 'x';
					}				
				}
			row_data[i] = 0;
			
			if(lxt)
				{
				lt_emit_value_bit_string(lt, GLOBALS.hp_vcd_saver_c_1[0]->handle.p, 0, row_data);
				}
				else
				{
				fprintf(GLOBALS.f_vcd_saver_c_1, "b%s %s\n", vcd_truncate_bitvec(row_data), vcdid(GLOBALS.hp_vcd_saver_c_1[0]->val));
				}
			}
		else
			{
			if(lxt)
				{
				row_data[0] = analyzer_demang(lxt, GLOBALS.hp_vcd_saver_c_1[0]->hist->v.h_val);
				row_data[1] = 0;

				lt_emit_value_bit_string(lt, GLOBALS.hp_vcd_saver_c_1[0]->handle.p, 0, row_data);
				}
				else
				{
				fprintf(GLOBALS.f_vcd_saver_c_1, "%c%s\n", analyzer_demang(lxt, GLOBALS.hp_vcd_saver_c_1[0]->hist->v.h_val), vcdid(GLOBALS.hp_vcd_saver_c_1[0]->val));
				}
			}
		}

	GLOBALS.hp_vcd_saver_c_1[0]->hist = GLOBALS.hp_vcd_saver_c_1[0]->hist->next;
	}

if(prevtime < GLOBALS.max_time)
	{
	if(lxt)
		{
		lt_set_time64(lt, GLOBALS.max_time / GLOBALS.time_scale);
		}
		else
		{
		fprintf(GLOBALS.f_vcd_saver_c_1, "#"TTFormat"\n", GLOBALS.max_time / GLOBALS.time_scale);
		}
	}


for(i=0;i<nodecnt;i++)
	{
	free_2(GLOBALS.hp_vcd_saver_c_1[i]);
	}

free_2(GLOBALS.hp_vcd_saver_c_1); GLOBALS.hp_vcd_saver_c_1 = NULL;
free_2(row_data); row_data = NULL;

if(lxt)
	{
	lt_close(lt); lt = NULL;
	}
	else
	{
	fclose(GLOBALS.f_vcd_saver_c_1); GLOBALS.f_vcd_saver_c_1 = NULL;
	}

return(VCDSAV_OK);
}

/************************ scopenav ************************/

struct namehier
{
struct namehier *next;
char *name;
char not_final;
};



void free_hier(void)
{
struct namehier *nhtemp;

while(GLOBALS.nhold_vcd_saver_c_1)
	{
	nhtemp=GLOBALS.nhold_vcd_saver_c_1->next;	
	free_2(GLOBALS.nhold_vcd_saver_c_1->name);
	free_2(GLOBALS.nhold_vcd_saver_c_1);
	GLOBALS.nhold_vcd_saver_c_1=nhtemp;
	}
}

/*
 * navigate up and down the scope hierarchy and
 * emit the appropriate vcd scope primitives
 */
static void diff_hier(struct namehier *nh1, struct namehier *nh2)
{
struct namehier *nhtemp;

if(!nh2)
	{
	while((nh1)&&(nh1->not_final))
		{
		fprintf(GLOBALS.f_vcd_saver_c_1, "$scope module %s $end\n", nh1->name);
		nh1=nh1->next;
		}
	return;
	}

for(;;)
	{
	if((nh1->not_final==0)&&(nh2->not_final==0)) /* both are equal */
		{
		break;
		}

	if(nh2->not_final==0)	/* old hier is shorter */
		{
		nhtemp=nh1;
		while((nh1)&&(nh1->not_final))
			{
			fprintf(GLOBALS.f_vcd_saver_c_1, "$scope module %s $end\n", nh1->name);
			nh1=nh1->next;
			}
		break;
		}

	if(nh1->not_final==0)	/* new hier is shorter */
		{
		nhtemp=nh2;
		while((nh2)&&(nh2->not_final))
			{
			fprintf(GLOBALS.f_vcd_saver_c_1, "$upscope $end\n");
			nh2=nh2->next;
			}
		break;
		}

	if(strcmp(nh1->name, nh2->name))
		{
		nhtemp=nh2;				/* prune old hier */
		while((nh2)&&(nh2->not_final))
			{
			fprintf(GLOBALS.f_vcd_saver_c_1, "$upscope $end\n");
			nh2=nh2->next;
			}

		nhtemp=nh1;				/* add new hier */
		while((nh1)&&(nh1->not_final))
			{
			fprintf(GLOBALS.f_vcd_saver_c_1, "$scope module %s $end\n", nh1->name);
			nh1=nh1->next;
			}
		break;
		}

	nh1=nh1->next;
	nh2=nh2->next;
	}
}


/*
 * output scopedata for a given name if needed, return pointer to name string
 */
char *output_hier(char *name)
{
char *pnt, *pnt2;
char *s;
int len;
struct namehier *nh_head=NULL, *nh_curr=NULL, *nhtemp;

pnt=pnt2=name;

for(;;)
{
if(*pnt2 == '\\') 
	{
	while(*pnt2) pnt2++;
	}
	else
	{
	while((*pnt2!='.')&&(*pnt2)) pnt2++;
	}

s=(char *)calloc_2(1,(len=pnt2-pnt)+1);
memcpy(s, pnt, len);
nhtemp=(struct namehier *)calloc_2(1,sizeof(struct namehier));
nhtemp->name=s;

if(!nh_curr)
	{
	nh_head=nh_curr=nhtemp;
	}
	else
	{
	nh_curr->next=nhtemp;
	nh_curr->not_final=1;
	nh_curr=nhtemp;
	}

if(!*pnt2) break;
pnt=(++pnt2);
}

diff_hier(nh_head, GLOBALS.nhold_vcd_saver_c_1);
free_hier();
GLOBALS.nhold_vcd_saver_c_1=nh_head;

if(*nh_curr->name == '\\')
	{
	char *mti_sv_patch = strstr(nh_curr->name, "]["); 	/* case is: #implicit-var###VarElem:ram_di[0.0] [63:0] */
	if(mti_sv_patch)
		{
		char *t = calloc_2(1, strlen(nh_curr->name) + 1 + 1);

		*mti_sv_patch = 0;
		sprintf(t, "%s] %s", nh_curr->name, mti_sv_patch+1);

		free_2(nh_curr->name);
		nh_curr->name = t;
		}

	if(nh_curr->name[1] == '#')
		{
		return(nh_curr->name+1);
		}
	}

return(nh_curr->name);
}

/*
 * $Id$
 * $Log$
 * Revision 1.1.1.1  2007/05/30 04:27:54  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:18  gtkwave
 * initial release
 *
 */

