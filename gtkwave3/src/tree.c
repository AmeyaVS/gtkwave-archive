/* 
 * Copyright (c) Tony Bybell 1999-2006.
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
#include "globals.h"
#include "tree.h"
#include "vcd.h"


enum TreeBuildTypes { MAKETREE_FLATTEN, MAKETREE_LEAF, MAKETREE_NODE };


/*
 * init pointers needed for n-way tree
 */
void init_tree(void)
{
/* treeroot=(struct tree *)calloc_2(1,sizeof(struct tree)); */
GLOBALS->module_tree_c_1=(char *)malloc_2(GLOBALS->longestname+1);
}


/*
 * extract the next part of the name in the flattened
 * hierarchy name.  return ptr to next name if it exists
 * else NULL
 */
static const char *get_module_name(const char *s)
{
char ch;
char *pnt;

pnt=GLOBALS->module_tree_c_1;

for(;;)
	{
	ch=*(s++);

	if((ch==GLOBALS->hier_delimeter) || (ch == '|'))
		{
		*(pnt)=0;	
		GLOBALS->module_len_tree_c_1 = pnt - GLOBALS->module_tree_c_1;
		return(s);		
		}

	if(!(*(pnt++)=ch))
		{
		GLOBALS->module_len_tree_c_1 = pnt - GLOBALS->module_tree_c_1;
		return(NULL);	/* nothing left to extract */		
		}
	}
}


/*
 * adds back netnames
 */
int treegraft(struct tree *t)
{
struct tree *tx = GLOBALS->terminals_tchain_tree_c_1;
struct tree *t2;
struct tree *par;

while(tx)
	{
	t2 = tx->next;

	par = tx->child;
	tx->child = NULL;

	if(par)
		{
		if(par->child)
			{
			tx->next = par->child;
			par->child = tx;
			}
			else
			{
			par->child = tx;
			tx->next = NULL;
			}
		}
		else
		{
		tx->next = t->next;
		t->next = tx;
		}

	tx = t2;
	}

return(1);
}


/*
 * unswizzle extended names in tree
 */ 
void treenamefix_str(char *s)
{
while(*s)
	{
	if(*s==VCDNAM_ESCAPE) *s=GLOBALS->hier_delimeter;
	s++;
	}
}

void treenamefix(struct tree *t)
{
struct tree *tnext;
if(t->child) treenamefix(t->child);

tnext = t->next;

while(tnext)
	{
	if(tnext->name) treenamefix_str(tnext->name);
	tnext=tnext->next;
	}

if(t->name) treenamefix_str(t->name);
}


/*
 * for debugging purposes only
 */
void treedebug(struct tree *t, char *s)
{
while(t)
	{
	char *s2;

	s2=(char *)malloc_2(strlen(s)+strlen(t->name)+2);
	strcpy(s2,s);
	strcat(s2,".");
	strcat(s2,t->name);
	
	if(t->child)
		{
		treedebug(t->child, s2);
		}

	if(t->which>=0) /* for when valid netnames like A.B.C, A.B.C.D exist (not legal excluding texsim) */
			/* otherwise this would be an 'else' */
		{
		printf("%3d) %s\n", t->which, s2);
		}

	free_2(s2);
	t=t->next;
	}
}


static GtkCTreeNode *maketree_nodes(GtkCTreeNode *subtree, struct tree *t2, GtkCTreeNode *sibling, int mode)
{
char *tmp, *tmp2, *tmp3;
gchar *text [1];

if(t2->which!=-1)
	{
        if(GLOBALS->facs[t2->which]->vec_root)
        	{
                if(GLOBALS->autocoalesce)
                	{
                        if(GLOBALS->facs[t2->which]->vec_root!=GLOBALS->facs[t2->which])
                        	{
				return(NULL);
                                }

                        tmp2=makename_chain(GLOBALS->facs[t2->which]);
                        tmp3=leastsig_hiername(tmp2);
                        tmp=wave_alloca(strlen(tmp3)+4);
                        strcpy(tmp,   "[] ");
                        strcpy(tmp+3, tmp3);
                        free_2(tmp2);
                        }
                        else
                        {
                        tmp=wave_alloca(strlen(t2->name)+4);
                        strcpy(tmp,   "[] ");
                        strcpy(tmp+3, t2->name);
                        }
		}
                else
                {
                tmp=t2->name;
                }
	}
        else
        {
        tmp=t2->name;
        }

text[0]=tmp;
switch(mode)
	{
	case MAKETREE_FLATTEN:
		if(t2->child)
			{
		        sibling = gtk_ctree_insert_node (GLOBALS->ctree_main, subtree, sibling, text, 3,
                	                       NULL, NULL, NULL, NULL,
                	                       FALSE, FALSE);
			gtk_ctree_node_set_row_data(GLOBALS->ctree_main, sibling, t2);
			maketree(sibling, t2->child);
			}
			else
			{
		        sibling = gtk_ctree_insert_node (GLOBALS->ctree_main, subtree, sibling, text, 3,
                	                       NULL, NULL, NULL, NULL,
                	                       TRUE, FALSE);
			gtk_ctree_node_set_row_data(GLOBALS->ctree_main, sibling, t2);
			}
		break;

	default:
	        sibling = gtk_ctree_insert_node (GLOBALS->ctree_main, subtree, sibling, text, 3,
               	                       NULL, NULL, NULL, NULL,
               	                       (mode==MAKETREE_LEAF), FALSE);
		gtk_ctree_node_set_row_data(GLOBALS->ctree_main, sibling, t2);
		break;
	}

return(sibling);
}


/*
 * return least significant member name of a hierarchy
 * (used for tree and hier vec_root search hits)
 */
char *leastsig_hiername(char *nam)
{
char *t, *pnt=NULL;
char ch;

if(nam)
	{
	t=nam;
	while((ch=*(t++)))
		{
		if(ch==GLOBALS->hier_delimeter) pnt=t;
		}
	}

return(pnt?pnt:nam);
}

/**********************************/
/* Experimental treesorting code  */
/* (won't directly work with lxt2 */
/* because alias hier is after    */
/* fac hier so fix with partial   */
/* mergesort...)                  */
/**********************************/

/*
 * sort the hier tree..should be faster than
 * moving numfacs longer strings around
 */

static int tree_qsort_cmp(const void *v1, const void *v2)
{
struct tree *t1 = *(struct tree **)v1;
struct tree *t2 = *(struct tree **)v2;

return(sigcmp(t2->name, t1->name));	/* because list must be in rvs */
}

void treesort(struct tree *t, struct tree *p)
{
struct tree *it;
struct tree **srt;
int cnt;
int i;

if(t->next)
	{
	it = t;
	cnt = 0;
	do	{
		cnt++;
		it=it->next;
		} while(it);
	
	srt = wave_alloca(cnt * sizeof(struct tree *));
	for(i=0;i<cnt;i++)
		{
		srt[i] = t;
		t=t->next;
		}

	qsort((void *)srt, cnt, sizeof(struct tree *), tree_qsort_cmp);

	if(p)
		{
		p->child = srt[0];
		}
		else
		{
		GLOBALS->treeroot = srt[0];
		}

	for(i=0;i<(cnt-1);i++)
		{
		srt[i]->next = srt[i+1];
		if(srt[i]->child)
			{
			treesort(srt[i]->child, srt[i]);
			}
		}
	srt[i]->next = NULL;
	if(srt[i]->child)
		{
		treesort(srt[i]->child, srt[i]);
		}
	}
else if (t->child)
	{
	treesort(t->child, t);
	}
}


void order_facs_from_treesort_2(struct tree *t)
{
while(t)
	{
	if(t->child)
		{
		order_facs_from_treesort_2(t->child);
		}

	if(t->which>=0) /* for when valid netnames like A.B.C, A.B.C.D exist (not legal excluding texsim) */
			/* otherwise this would be an 'else' */
		{
		GLOBALS->facs2_tree_c_1[GLOBALS->facs2_pos_tree_c_1] = GLOBALS->facs[t->which];
		t->which = GLOBALS->facs2_pos_tree_c_1--;
		}

	t=t->next;
	}
}


void order_facs_from_treesort(struct tree *t, void *v)
{
struct symbol ***f = (struct symbol ***)v; /* eliminate compiler warning in tree.h as symbol.h refs tree.h */

GLOBALS->facs2_tree_c_1=(struct symbol **)malloc_2(GLOBALS->numfacs*sizeof(struct symbol *));
GLOBALS->facs2_pos_tree_c_1 = GLOBALS->numfacs-1;
order_facs_from_treesort_2(t);

if(GLOBALS->facs2_pos_tree_c_1>=0)
	{
	fprintf(stderr, "Internal Error: GLOBALS->facs2_pos_tree_c_1 = %d\n",GLOBALS->facs2_pos_tree_c_1);
	fprintf(stderr, "[This is usually the result of multiply defined facilities.]\n");
	exit(255);
	}

free_2(*f);
*f = GLOBALS->facs2_tree_c_1;
GLOBALS->facs2_tree_c_1 = NULL;
}


void build_tree_from_name(const char *s, int which)
{
struct tree *t, *nt;
struct tree *tchain;
struct tree *prevt;

if(s==NULL || !s[0]) return;

t = GLOBALS->treeroot;

if(t)
	{
	prevt = NULL;
	while(s)
		{
rs:		s=get_module_name(s);

		if(t && !strcmp(t->name, GLOBALS->module_tree_c_1))
			{
			prevt = t;
			t = t->child;
			continue;
			}

		tchain = t;
		if(s && t)
			{
		      	nt = t->next;
		      	while(nt)
				{
				if(nt && !strcmp(nt->name, GLOBALS->module_tree_c_1))
					{
					prevt = nt;
					t = nt->child;
					goto rs;
					}

				nt = nt->next;
				}
			}

		nt=(struct tree *)calloc_2(1,sizeof(struct tree)+GLOBALS->module_len_tree_c_1);
		memcpy(nt->name, GLOBALS->module_tree_c_1, GLOBALS->module_len_tree_c_1);

		if(s)
			{
			nt->which=-1;

			if(prevt)				/* make first in chain */
				{
				nt->next = prevt->child;
				prevt->child = nt;
				}
				else				/* make second in chain as it's toplevel */
				{
				nt->next = tchain->next;	
				tchain->next = nt;
				}
			}
			else
			{
			nt->child = prevt;			/* parent */
			nt->which = which;
			nt->next = GLOBALS->terminals_tchain_tree_c_1;
			GLOBALS->terminals_tchain_tree_c_1 = nt;
			return;
			}
	
		/* blindly clone fac from next part of hier on down */
		t = nt;
		while(s)
			{
			s=get_module_name(s);
		
			nt=(struct tree *)calloc_2(1,sizeof(struct tree)+GLOBALS->module_len_tree_c_1);
			memcpy(nt->name, GLOBALS->module_tree_c_1, GLOBALS->module_len_tree_c_1);

			if(s)
				{
				nt->which = -1;
				t->child = nt;
				t = nt;
				}
				else
				{
				nt->child = t;			/* parent */
				nt->which = which;
				nt->next = GLOBALS->terminals_tchain_tree_c_1;
				GLOBALS->terminals_tchain_tree_c_1 = nt;
				}
			}
		}
	}
else	
	{
	/* blindly create first fac in the tree (only ever called once) */
	while(s)
		{
		s=get_module_name(s);

		nt=(struct tree *)calloc_2(1,sizeof(struct tree)+GLOBALS->module_len_tree_c_1);
		memcpy(nt->name, GLOBALS->module_tree_c_1, GLOBALS->module_len_tree_c_1);

		if(!s) nt->which=which; else nt->which=-1;

		if(GLOBALS->treeroot)
			{
			t->child = nt;
			t = nt;
			}
			else
			{
			GLOBALS->treeroot = t = nt;
			}
		}
	
	}

}


/* ######################## */
/* ## compatibility code ## */
/* ######################## */

/*
 * tree widgets differ between GTK2 and GTK1 so we need two different
 * maketree() routines
 */
#if WAVE_USE_GTK2

/*
 * GTK2: build the tree.
 */
void maketree(GtkCTreeNode *subtree, struct tree *t)
{
GtkCTreeNode *sibling=NULL, *sibling_test;
struct tree *t2;

/* 
 * TG reworked treesearch widget so there is no need to 
 * process anything other than nodes.  Leaves are handled
 * in the filtered list below the node expand/contract
 * tree
 */
t2=t;
while(t2)
	{
	if(t2->child)
		{
		sibling_test=maketree_nodes(subtree, t2, sibling, MAKETREE_NODE);
		if(sibling_test)
			{
			GLOBALS->any_tree_node = sibling_test;
			maketree(sibling=sibling_test, t2->child);
			}
		}

	t2=t2->next;
	}
}

#else

/*
 * GTK1: build the tree.
 */
void maketree(GtkCTreeNode *subtree, struct tree *t)
{
GtkCTreeNode *sibling=NULL, *sibling_test;
struct tree *t2;

if(!GLOBALS->hier_grouping)
	{
	t2=t;
	while(t2)
		{
		sibling_test=maketree_nodes(subtree, t2, sibling, MAKETREE_FLATTEN);	
		sibling=sibling_test?sibling_test:sibling;
		t2=t2->next;
		}
	}
	else
	{
	t2=t;
	while(t2)
		{
		if(!t2->child)
			{
			sibling_test=maketree_nodes(subtree, t2, sibling, MAKETREE_LEAF);
			if(sibling_test)
				{
				maketree(sibling=sibling_test, t2->child);
				}
			}
	
		t2=t2->next;
		}

	t2=t;
	while(t2)
		{
		if(t2->child)
			{
			sibling_test=maketree_nodes(subtree, t2, sibling, MAKETREE_NODE);
			if(sibling_test)
				{
				maketree(sibling=sibling_test, t2->child);
				}
			}
	
		t2=t2->next;
		}
	}
}

#endif

/*
 * $Id$
 * $Log$
 * Revision 1.1.1.1.2.5  2007/08/25 19:43:46  gtkwave
 * header cleanups
 *
 * Revision 1.1.1.1.2.4  2007/08/21 22:35:40  gtkwave
 * prelim tree state merge
 *
 * Revision 1.1.1.1.2.3  2007/08/07 03:18:55  kermin
 * Changed to pointer based GLOBAL structure and added initialization function
 *
 * Revision 1.1.1.1.2.2  2007/08/06 03:50:49  gtkwave
 * globals support for ae2, gtk1, cygwin, mingw.  also cleaned up some machine
 * generated structs, etc.
 *
 * Revision 1.1.1.1.2.1  2007/08/05 02:27:24  kermin
 * Semi working global struct
 *
 * Revision 1.1.1.1  2007/05/30 04:27:35  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:17  gtkwave
 * initial release
 *
 */

