/*  GHDL Wavefile reader interface.
    Copyright (C) 2005-2006 Tristan Gingold and Tony Bybell

    GHDL is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free
    Software Foundation; either version 2, or (at your option) any later
    version.

    GHDL is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with GCC; see the file COPYING.  If not, write to the Free
    Software Foundation, 59 Temple Place - Suite 330, Boston, MA
    02111-1307, USA.
*/

#include <config.h>
#include "ghw.h"

/************************ splay ************************/

/* 
 * NOTE: 
 * a GHW tree's "which" is not the same as a gtkwave "which"
 * in that gtkwave's points to the facs[] array and
 * GHW's functions as an alias handle.  The following
 * vars are used to resolve those differences...
 */
static struct Node **nxp;
static struct symbol *sym_head = NULL, *sym_curr = NULL;
static int sym_which = 0;


/*
 * pointer splay
 */
typedef struct ghw_tree_node ghw_Tree;
struct ghw_tree_node {
    ghw_Tree * left, * right;
    void *item;
    int val_old;
    struct symbol *sym;
};


long ghw_cmp_l(void *i, void *j)
{
long il = (long)i, jl = (long)j;
return(il - jl);
}

static ghw_Tree * ghw_splay (void *i, ghw_Tree * t) {
/* Simple top down splay, not requiring i to be in the tree t.  */
/* What it does is described above.                             */
    ghw_Tree N, *l, *r, *y;
    int dir;

    if (t == NULL) return t;
    N.left = N.right = NULL;
    l = r = &N;

    for (;;) {
	dir = ghw_cmp_l(i, t->item);
	if (dir < 0) {
	    if (t->left == NULL) break;
	    if (ghw_cmp_l(i, t->left->item)<0) {
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
	    if (ghw_cmp_l(i, t->right->item)>0) {
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


static ghw_Tree * ghw_insert(void *i, ghw_Tree * t, int val, struct symbol *sym) {
/* Insert i into the tree t, unless it's already there.    */
/* Return a pointer to the resulting tree.                 */
    ghw_Tree * n;
    int dir;
    
    n = (ghw_Tree *) calloc_2(1, sizeof (ghw_Tree));
    if (n == NULL) {
	fprintf(stderr, "ghw_insert: ran out of memory, exiting.\n");
	exit(255);
    }
    n->item = i;
    n->val_old = val;
    n->sym = sym;
    if (t == NULL) {
	n->left = n->right = NULL;
	return n;
    }
    t = ghw_splay(i,t);
    dir = ghw_cmp_l(i,t->item);
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


/*
 * chain together bits of the same fac
 */
int strand_pnt(char *s)
{
int len = strlen(s) - 1;
int i;
int rc = -1;

if(s[len] == ']')
	{
	for(i=len-1;i>0;i--)
		{
		if((s[i]>='0') && (s[i]<='9')) continue;
		if(s[i] != '[') break;
		return(i);	/* position right before left bracket for strncmp */
		}
	}

return(rc);
}

void rechain_facs(void)
{
int i;
struct symbol *psr = NULL;
struct symbol *root = NULL;

for(i=0;i<numfacs;i++)
	{
	if(psr)
		{
		char *fstr1 = facs[i-1]->name;
		char *fstr2 = facs[i]->name;
		int p1 = strand_pnt(fstr1);
		int p2 = strand_pnt(fstr2);

		if(!root)
			{
			if((p1>=0)&&(p1==p2))
				{
				if(!strncmp(fstr1, fstr2, p1))
					{
					root = facs[i-1];
					root->vec_root = root;
					root->vec_chain = facs[i];
					facs[i]->vec_root = root;
					}
				}
			}				
			else
			{
			if((p1>=0)&&(p1==p2))
				{
				if(!strncmp(fstr1, fstr2, p1))
					{
					psr->vec_chain = facs[i];
					facs[i]->vec_root = root;
					}
					else
					{
					root = NULL;
					}
				}
				else
				{
				root = NULL;
				}
			}
		}
	
	psr = facs[i];
	}
}


/*
 * preserve tree->which ordering so hierarchy children index pointers don't get corrupted
 */
static ghw_Tree *gwt = NULL;
static ghw_Tree *gwt_corr = NULL;

static void recurse_tree_build_whichcache(struct tree *t)
{
if(t)
	{
	if(t->child) { recurse_tree_build_whichcache(t->child); }
	if(t->next) { recurse_tree_build_whichcache(t->next); }

	if(t->which >= 0) gwt = ghw_insert(t, gwt, t->which, facs[t->which]);
	}
}

static void recurse_tree_fix_from_whichcache(struct tree *t)
{
if(t)
	{
	if(t->child) { recurse_tree_fix_from_whichcache(t->child); }
	if(t->next) { recurse_tree_fix_from_whichcache(t->next); }

	if(t->which >= 0) 
		{
		gwt = ghw_splay(t, gwt);
		gwt_corr = ghw_splay(gwt->sym, gwt_corr); /* all facs are in this tree so this is OK */

		t->which = gwt_corr->val_old;				
		}
	}
}


static void incinerate_whichcache_tree(ghw_Tree *t)
{
if(t->left) incinerate_whichcache_tree(t->left);
if(t->right) incinerate_whichcache_tree(t->right);

free_2(t);
}


/*
 * sort facs and also cache/reconstruct tree->which pointers
 */
static void ghw_sortfacs(void)
{
int i;  

recurse_tree_build_whichcache(treeroot);

for(i=0;i<numfacs;i++)
        {
        char *subst, ch;
        int len;
        struct symbol *curnode = facs[i];

        if((len=strlen(subst=curnode->name))>longestname) longestname=len;
        while((ch=(*subst)))
                { 
                if(ch==hier_delimeter) { *subst=VCDNAM_HIERSORT; } /* forces sort at hier boundaries */
                subst++;
                }
        }

wave_heapsort(facs,numfacs);

for(i=0;i<numfacs;i++)
	{
	gwt_corr = ghw_insert(facs[i], gwt_corr, i, NULL);
	}

recurse_tree_fix_from_whichcache(treeroot);
incinerate_whichcache_tree(gwt); gwt = NULL;
incinerate_whichcache_tree(gwt_corr); gwt_corr = NULL;
 
for(i=0;i<numfacs;i++)
        {
        char *subst, ch;

        subst=facs[i]->name;
        while((ch=(*subst)))
                {
                if(ch==VCDNAM_HIERSORT) { *subst=hier_delimeter; } /* restore back to normal */
                subst++;
                }
        }

facs_are_sorted=1;
}

/*******************************************************************************/

static int xlat_1164 = 1;
char is_ghw = 0;
static char asbuf[4097];

static int nbr_sig_ref;
static int num_glitches = 0, num_glitch_regions = 0;

static struct tree *
build_hierarchy_type (struct ghw_handler *h, union ghw_type *t,
		      const char *pfx, unsigned int **sig);

static void
build_hierarchy_array (struct ghw_handler *h, union ghw_type *arr, int dim,
		       const char *pfx, struct tree **res, unsigned int **sig)
{
  union ghw_type *idx;
  struct ghw_type_array *base = arr->sa.base;
  char *name = NULL;

  if (dim == base->nbr_dim)
    {
      struct tree *t;
      sprintf (asbuf, "%s]", pfx);
      name = strdup(asbuf);

      t = build_hierarchy_type (h, base->el, name, sig);
      
      if (*res != NULL)
	(*res)->next = t;
      *res = t;
      return;
    }

  idx = ghw_get_base_type (base->dims[dim]);
  switch (idx->kind)
    {
    case ghdl_rtik_type_i32:
      {
	int32_t v;
	char *name;
	struct ghw_range_i32 *r;
	struct tree *last;
	
	last = NULL;
	r = &arr->sa.rngs[dim]->i32;
	v = r->left;
	while (1)
	  {
	    sprintf(asbuf, "%s%c%d", pfx, dim == 0 ? '[' : ',', v);
            name = strdup(asbuf);
	    build_hierarchy_array (h, arr, dim + 1, name, res, sig);
	    free_2(name);
	    if (v == r->right)
	      break;
	    if (r->dir == 0)
	      v++;
	    else
	      v--;
	  }
      }
      return;

    case ghdl_rtik_type_e8:
      {
	int32_t v;
	char *name;
	struct ghw_range_e8 *r;
	struct tree *last;
	
	last = NULL;
	r = &arr->sa.rngs[dim]->e8;
	v = r->left;
	while (1)
	  {
	    sprintf(asbuf, "%s%c%d", pfx, dim == 0 ? '[' : ',', v);
            name = strdup(asbuf);
	    build_hierarchy_array (h, arr, dim + 1, name, res, sig);
	    free_2(name);
	    if (v == r->right)
	      break;
	    if (r->dir == 0)
	      v++;
	    else
	      v--;
	  }
      }
      return;

    default:
      fprintf (stderr, "build_hierarchy_array: unhandled type %d\n",
	       idx->kind);
      abort ();
    }
}

static struct tree *
build_hierarchy_type (struct ghw_handler *h, union ghw_type *t,
		      const char *pfx, unsigned int **sig)
{
  struct tree *res;
  struct symbol *s;

  switch (t->kind)
    {
    case ghdl_rtik_subtype_scalar:
      return build_hierarchy_type (h, t->ss.base, pfx, sig);
    case ghdl_rtik_type_b2:
    case ghdl_rtik_type_e8:
    case ghdl_rtik_type_f64:
    case ghdl_rtik_type_i32:
    case ghdl_rtik_type_i64:
    case ghdl_rtik_type_p32:
    case ghdl_rtik_type_p64:

      s = calloc(1, sizeof(struct symbol));
      if(!sym_head)
		{
		sym_head = sym_curr = s;
		}
		else
		{
		sym_curr->next = s; sym_curr = s;
		}

      nbr_sig_ref++;
      res = (struct tree *) calloc_2(1, sizeof (struct tree) + strlen(pfx));
      strcpy(res->name, (char *)pfx);
      res->which = *(*sig)++;

      s->n = nxp[res->which];
      return res;
    case ghdl_rtik_subtype_array:
    case ghdl_rtik_subtype_array_ptr:
      {
	struct tree *r;
	res = (struct tree *) calloc_2(1, sizeof (struct tree) + strlen(pfx));
	strcpy(res->name, (char *)pfx);
	res->which = -1;
	r = res;
	build_hierarchy_array (h, t, 0, "", &res, sig);
	r->child = r->next;
	r->next = NULL;
	return r;
      }
    case ghdl_rtik_type_record:
      {
	struct tree *last;
	struct tree *c;
	int i;

	res = (struct tree *) calloc_2(1, sizeof (struct tree) + strlen(pfx));
	strcpy(res->name, (char *)pfx);
	res->which = -1;

	last = NULL;
	for (i = 0; i < t->rec.nbr_fields; i++)
	  {
	    c = build_hierarchy_type
	      (h, t->rec.el[i].type, t->rec.el[i].name, sig);
	    if (last == NULL)
	      res->child = c;
	    else
	      last->next = c;
	    last = c;
	  }
	return res;
      }
    default:
      fprintf (stderr, "build_hierarchy_type: unhandled type %d\n", t->kind);
      abort ();
    }
}

static struct tree *
build_hierarchy (struct ghw_handler *h, struct ghw_hie *hie)
{
  struct tree *t;
  struct tree *t_ch;
  struct tree *prev;
  struct ghw_hie *ch;

  switch (hie->kind)
    {
    case ghw_hie_design:
    case ghw_hie_block:
    case ghw_hie_instance:
    case ghw_hie_generate_for:
    case ghw_hie_generate_if:
    case ghw_hie_package:
      if (hie->kind == ghw_hie_generate_for)
	{
	  char buf[128];
	  int name_len, buf_len;
	  char *n;

	  ghw_get_value (buf, sizeof (buf),
			 hie->u.blk.iter_value, hie->u.blk.iter_type);
	  name_len = strlen (hie->name);
	  buf_len = strlen (buf);

          t = (struct tree *) calloc_2(1, sizeof (struct tree) + (2 + buf_len + name_len));
	  n = t->name;

	  memcpy (n, hie->name, name_len);
	  n += name_len;
	  *n++ = '[';
	  memcpy (n, buf, buf_len);
	  n += buf_len;
	  *n++ = ']';
	  *n = 0;
	}
      else
        {
          if(hie->name)
		{
          	t = (struct tree *) calloc_2(1, sizeof (struct tree) + strlen(hie->name));
          	strcpy(t->name, (char *)hie->name);
		}
		else
		{
          	t = (struct tree *) calloc_2(1, sizeof (struct tree));
		}
        }

      t->which = -1;
      prev = NULL;
      for (ch = hie->u.blk.child; ch != NULL; ch = ch->brother)
	{
	  t_ch = build_hierarchy (h, ch);
	  if (t_ch != NULL)
	    {
	      if (prev == NULL)
		t->child = t_ch;
	      else
		prev->next = t_ch;
	      prev = t_ch;
	    }
	}
      return t;
    case ghw_hie_process:
      return NULL;
    case ghw_hie_signal:
    case ghw_hie_port_in:
    case ghw_hie_port_out:
    case ghw_hie_port_inout:
    case ghw_hie_port_buffer:
    case ghw_hie_port_linkage:
      {
	unsigned int *ptr = hie->u.sig.sigs;
	t = build_hierarchy_type (h, hie->u.sig.type, hie->name, &ptr);
	if (*ptr != 0)
	  abort ();
	return t;
      }
    default:
      fprintf (stderr, "ghw: build_hierarchy: cannot handle hie %d\n",
	       hie->kind);
      abort ();
    }
}

void
facs_debug (void)
{
  int i;
  struct Node *n;

  for (i = 0; i < numfacs; i++)
    {
      hptr h;

      n = facs[i]->n;
      printf ("%d: %s\n", i, n->nname);
      if (n->ext)
	printf ("  ext: %d - %d\n", n->ext->msi, n->ext->lsi);
      for (h = &n->head; h; h = h->next)
	printf ("  time:"TTFormat" flags:%02x vect:%p\n",
		h->time, h->flags, h->v.h_vector);
    }
}

static struct ExtNode dummy_en = { 0, 0 };

static void
create_facs (struct ghw_handler *h)
{
  int i;
  struct symbol *s = sym_head;

  numfacs = nbr_sig_ref;
  facs=(struct symbol **)malloc_2(numfacs*sizeof(struct symbol *));

  i = 0;
  while(s)
	{
	facs[i++] = s;
	s = s->next;
	}

  for (i = 0; i < h->nbr_sigs; i++)
    {
      struct Node *n = nxp[i];

      if (h->sigs[i].type)
	switch (h->sigs[i].type->kind)
	  {
	  case ghdl_rtik_type_b2:
	    if (h->sigs[i].type->en.wkt == ghw_wkt_bit)
	      {
		n->ext = NULL;
		break;
	      }
	    /* FALLTHROUGH */
	  case ghdl_rtik_type_e8:
	    if (xlat_1164 && h->sigs[i].type->en.wkt == ghw_wkt_std_ulogic)
	      {
		n->ext = NULL;
		break;
	      }
	    /* FALLTHROUGH */
	  case ghdl_rtik_type_e32:
	  case ghdl_rtik_type_i32:
	  case ghdl_rtik_type_i64:
	  case ghdl_rtik_type_f64:
	  case ghdl_rtik_type_p32:
	  case ghdl_rtik_type_p64:
	    n->ext = &dummy_en;
	    break;
	  default:
	    fprintf (stderr, "ghw:create_facs: unhandled kind %d\n",
		     h->sigs[i].type->kind);
	    n->ext = NULL;
	  }
    }

}

static char *fac_name;
static int fac_name_len;
static int fac_name_max;
static int last_fac;



static void
set_fac_name_1 (struct tree *t)
{
  for (; t != NULL; t = t->next)
    {
      int prev_len = fac_name_len;

      /* Complete the name.  */
      if(t->name[0]) /* originally (t->name != NULL) when using pointers */
	{
	  int len;
	  
	  len = strlen (t->name) + 1;
	  if (len + fac_name_len >= fac_name_max)
	    {
	      fac_name_max *= 2;
	      if (fac_name_max <= len + fac_name_len)
		fac_name_max = len + fac_name_len + 1;
	      fac_name = realloc_2(fac_name, fac_name_max);
	    }
          if(t->name[0] != '[')
		{
	  	fac_name[fac_name_len] = '.';
	  	/* The NUL is copied, since LEN is 1 + strlen.  */
	  	memcpy (fac_name + fac_name_len + 1, t->name, len);
	  	fac_name_len += len;
		}
		else
		{
	  	memcpy (fac_name + fac_name_len, t->name, len);
	  	fac_name_len += (len - 1);
		}
	}

      if (t->which >= 0)
	{
        struct symbol *s = sym_head;

	s->name = strdup (fac_name);
	s->n = nxp[t->which];
	if(!s->n->nname) s->n->nname = s->name;

	t->which = sym_which++; /* patch in gtkwave "which" as node is correct */

	sym_head = sym_head->next;
	}

      if (t->child)
	set_fac_name_1 (t->child);

      /* Revert name.  */
      fac_name_len = prev_len;
      fac_name[fac_name_len] = 0;
    }
}

static void
set_fac_name (struct ghw_handler *h)
{
  if (fac_name_max == 0)
    {
      fac_name_max = 1024;
      fac_name = malloc_2(fac_name_max);
    }
  fac_name_len = 3;
  memcpy (fac_name, "top", 4);
  last_fac = h->nbr_sigs;
  set_fac_name_1 (treeroot);
}

static void
add_history (struct ghw_handler *h, struct Node *n, int sig_num)
{
  struct HistEnt *he;
  struct ghw_sig *sig = &h->sigs[sig_num];
  union ghw_type *sig_type = sig->type;
  int flags;
  static int warned;
  int is_vector = 0;

  if (sig_type == NULL)
    return;

  regions++;

  switch (sig_type->kind)
    {
    case ghdl_rtik_type_b2:
      if (sig_type->en.wkt == ghw_wkt_bit)
	{
	  flags = 0;
	  break;
	}
      /* FALLTHROUGH */
    case ghdl_rtik_type_e8:
      if (xlat_1164 && sig_type->en.wkt == ghw_wkt_std_ulogic)
	{
	  flags = 0;
	  break;
	}
      /* FALLTHROUGH */
    case ghdl_rtik_type_e32:
    case ghdl_rtik_type_i32:
    case ghdl_rtik_type_i64:
    case ghdl_rtik_type_p32:
    case ghdl_rtik_type_p64:
      flags = HIST_STRING|HIST_REAL;
      if (HIST_STRING == 0)
	{
	  if (!warned)
	    fprintf (stderr, "warning: do not compile with STRICT_VCD\n");
	  warned = 1;
	  return;
	}
      break;
    case ghdl_rtik_type_f64:
      flags = HIST_REAL;
      break;
    default:
      fprintf (stderr, "ghw:add_history: unhandled kind %d\n",
	       sig->type->kind);
      return;
    }

  if(!n->curr)
    	{
      	he=histent_calloc();
      	he->flags = flags;
      	he->time=-1;
      	he->v.h_vector=NULL;

      	n->head.next=he;
      	n->curr=he;
      	n->head.time = -2;
    	}

  he=histent_calloc();
  he->flags = flags;
  he->time=h->snap_time;

  switch (sig_type->kind)
    {
    case ghdl_rtik_type_b2:
      if (sig_type->en.wkt == ghw_wkt_bit)
	he->v.h_val = sig->val->b2 == 0 ? AN_0 : AN_1;
      else
        {
	he->v.h_vector = (char *)sig->type->en.lits[sig->val->b2];
	is_vector = 1;
        }
      break;
    case ghdl_rtik_type_e8:
      if (xlat_1164 && sig_type->en.wkt == ghw_wkt_std_ulogic)
	{
	  /* Res: 0->0, 1->X, 2->Z, 3->1 */
	  static const char map_su2vlg[9] = {
	    /* U */ AN_U, /* X */ AN_X, /* 0 */ AN_0, /* 1 */ AN_1,
	    /* Z */ AN_Z, /* W */ AN_W, /* L */ AN_L, /* H */ AN_H,
	    /* - */ AN_DASH
	  };
	  he->v.h_val = map_su2vlg[sig->val->e8];
	}
      else
        {
	he->v.h_vector = (char *)sig_type->en.lits[sig->val->e8];
	is_vector = 1;
        }
      break;
    case ghdl_rtik_type_f64:
      {
	double *d = malloc_2(sizeof (double));
	*d = sig->val->f64;
	he->v.h_vector = (char *)d;
        is_vector = 1;
      }
      break;
    case ghdl_rtik_type_i32:
    case ghdl_rtik_type_p32:
      sprintf (asbuf, "%d", sig->val->i32);
      he->v.h_vector = strdup(asbuf);    
      is_vector = 1;
      break;
    case ghdl_rtik_type_i64:
    case ghdl_rtik_type_p64:
      sprintf (asbuf, TTFormat, sig->val->i64);
      he->v.h_vector = strdup(asbuf);    
      is_vector = 1;
      break;
    default:
      abort ();
    }

    /* deglitch */
    if(n->curr->time == he->time)
	{
        num_glitches++;
	if(!(n->curr->flags&HIST_GLITCH))
        	{
                n->curr->flags|=HIST_GLITCH;    /* set the glitch flag */
                num_glitch_regions++;
                }

        if(is_vector)
                {
                if(n->curr->v.h_vector &&
                   sig_type->kind != ghdl_rtik_type_b2 &&
                   sig_type->kind != ghdl_rtik_type_e8) free_2(n->curr->v.h_vector);
                n->curr->v.h_vector = he->v.h_vector;
                /* can't free up this "he" because of block allocation so assume it's dead */
                }
		else
		{
		n->curr->v.h_val = he->v.h_val;
		}
	return;
	}
	else /* look for duplicate dumps of same value at adjacent times */
	{
	if(!is_vector)
		{
		if(n->curr->v.h_val == he->v.h_val)
			{
			return;
	                /* can't free up this "he" because of block allocation so assume it's dead */
			}
		}
	}

  n->curr->next=he;
  n->curr=he;
}

static void
add_tail (struct ghw_handler *h)
{
  int i;
  TimeType j;
 
  for (j = 1; j>=0 ; j--) /* add two endcaps */
  for (i = 0; i < h->nbr_sigs; i++)
    {
      struct ghw_sig *sig = &h->sigs[i];
      struct Node *n = nxp[i];
      struct HistEnt *he;
      
      if (sig->type == NULL || n == NULL || !n->curr)
	continue;

      /* Copy the last one.  */
      he = histent_calloc();
      *he = *n->curr;
      he->time = MAX_HISTENT_TIME - j;
      he->next = NULL;

      /* Append.  */
      n->curr->next=he;
      n->curr=he;
    }
}

static void
read_traces (struct ghw_handler *h)
{
  int *list;
  int i;
  enum ghw_res res;
  
  list = malloc_2((numfacs + 1) * sizeof (int));
  
  while (1)
    {
      res = ghw_read_sm_hdr (h, list);
      switch (res)
	{
	case ghw_res_error:
	case ghw_res_eof:
	  free_2(list);
	  return;
	case ghw_res_ok:
	case ghw_res_other:
	  break;
	case ghw_res_snapshot:
	  if (h->snap_time > max_time)
	    max_time = h->snap_time;
	  /* printf ("Time is "TTFormat"\n", h->snap_time); */

	  for (i = 0; i < h->nbr_sigs; i++)
	    add_history (h, nxp[i], i);
	  break;
	case ghw_res_cycle:
	  while (1)
	    {
	      int sig;

	      /* printf ("Time is "TTFormat"\n", h->snap_time); */
	      if (h->snap_time < LLDescriptor(9223372036854775807))
		{
		  if (h->snap_time > max_time)
		    max_time = h->snap_time;
		  
		  for (i = 0; (sig = list[i]) != 0; i++)
		    add_history (h, nxp[sig], sig);
		}
	      res = ghw_read_cycle_next (h);
	      if (res != 1)
		break;
	      res = ghw_read_cycle_cont (h, list);
	      if (res < 0)
		break;
	    }
	  if (res < 0)
	    break;
	  res = ghw_read_cycle_end (h);
	  if (res < 0)
	    break;
	  break;
	}
    }
}

/*******************************************************************************/

TimeType
ghw_main(char *fname)
{
  struct ghw_handler handle;
  int i;
  int rc;

  if(!hier_was_explicitly_set) /* set default hierarchy split char */
    {
      hier_delimeter='.';
    }

 handle.flag_verbose = 0;
 if ((rc=ghw_open (&handle, fname)) < 0)
   {
     fprintf (stderr, "Error opening ghw file '%s', rc=%d.\n", fname, rc);
     exit(1);
   }

 time_scale = 1;
 time_dimension = 'f';

 if (ghw_read_base (&handle) < 0)
   {
     fprintf (stderr, "Error in ghw file '%s'.\n", fname);
     exit(1);
   }

 min_time = 0;
 max_time = 0;

 nbr_sig_ref = 0;

 nxp =(struct Node **)calloc_2(handle.nbr_sigs, sizeof(struct Node *));
 for(i=0;i<handle.nbr_sigs;i++)
	{
        nxp[i] = (struct Node *)calloc_2(1,sizeof(struct Node));
	}

 treeroot = build_hierarchy (&handle, handle.hie);
 /* GHW does not contains a 'top' name.
    FIXME: should use basename of the file.  */

 create_facs (&handle);
 read_traces (&handle);
 add_tail (&handle);
 
 set_fac_name (&handle);
 free(nxp); nxp = NULL;

 /* fix up names on aliased nodes via cloning... */
 for(i=0;i<numfacs;i++)
	{
	if(strcmp(facs[i]->name, facs[i]->n->nname))
		{
		struct Node *n = malloc(sizeof(struct Node));
		memcpy(n, facs[i]->n, sizeof(struct Node));
		facs[i]->n = n;
		n->nname = facs[i]->name;
		}
	}

 /* treeroot->name = "top"; */
 {
 const char *base_hier = "top";

 struct tree *t = calloc_2(1, sizeof(struct tree) + strlen(base_hier));
 memcpy(t, treeroot, sizeof(struct tree));
 strcpy(t->name, base_hier);
 free_2(treeroot);
 treeroot = t;
 }

 ghw_close (&handle);

 rechain_facs();	/* vectorize bitblasted nets */
 ghw_sortfacs();	/* sort nets as ghw is unsorted ... also fix hier tree (it should really be built *after* facs are sorted!) */

#if 0
 treedebug(treeroot,"");
 facs_debug();
#endif

  is_ghw = 1;

  fprintf(stderr, "["TTFormat"] start time.\n["TTFormat"] end time.\n", min_time*time_scale, max_time*time_scale);
  if(num_glitches) fprintf(stderr, "Warning: encountered %d glitch%s across %d glitch region%s.\n",
                num_glitches, (num_glitches!=1)?"es":"",
                num_glitch_regions, (num_glitch_regions!=1)?"s":"");

  return max_time;
}

/*******************************************************************************/

/*
 * $Id$
 * $Log$
 */

