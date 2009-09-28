/* 
 * Copyright (c) Yiftach Tzori 2009.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */
#include <config.h>
#include "globals.h"
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <string.h>
#include <ctype.h>
#include "gtk12compat.h"
#include "analyzer.h"
#include "tree.h"
#include "symbol.h"
#include "vcd.h"
#include "lx2.h"
#include "busy.h"
#include "debug.h"
#include "hierpack.h"
#include "tcl_helper.h"
#include "tcl_support_commands.h"

/* **
 * Search for a tree node that is associated with the hierarchical path
 * return pointer to this node or NULL
 */
GtkCTreeNode *SST_find_node_by_path(GtkCTreeRow *root, char *path) {
  char *s = strdup_2(path) ;
  char *p = s ;
  char *p1 ;
  GtkCTreeRow *gctr = root ;
  GtkCTreeNode *node = gctr->parent ;
  struct tree *t ;
  while (gctr)  {
    if ((p1 = strchr(p, '.'))) 
      *p1 = '\0' ;
    t = (struct tree *)(gctr->row.data) ;
    while (strcmp(t->name, p)) { /* name mis-match */
      if (!(node = gctr->sibling)) { /* no more siblings */
	gctr = NULL ;
	break ;
      } else {
	gctr = GTK_CTREE_ROW(node);
	t = (struct tree *)(gctr->row.data) ;
      }
    }
    if (gctr) {			/* normal exit from the above */
      if(!p1) {						    /* last in chain */
		/* node = gctr ; */
	break ;
      } else {			/* keep going down the hierarchy */
	if (!(node = gctr->children))
	  break ;
	else {
	  gctr = GTK_CTREE_ROW(gctr->children) ;
	  p = p1 + 1 ;
	}
      }
    }
  }
  free_2(s) ;
  return node ;
}

/* **
 * Open the hierarchy tree, starting from 'node' up to the root
 */
int SST_open_path(GtkCTree *ctree, GtkCTreeNode *node) {
  GtkCTreeRow *row ;
  for(row = GTK_CTREE_ROW(node) ; row->parent; row = GTK_CTREE_ROW(row->parent)) {
    gtk_ctree_expand(ctree, row->parent);
  }
  return 0 ;
}

/* **
 * Main function called by gtkwavetcl_forceOpenTreeNode
 * Inputs:
 *   char *name :: hierachical path to open
 * Output:
 *   One of: 
 *     SST_NODE_FOUND - if path is in the dump file
 *     SST_NODE_NOT_EXIST - is path is not in the dump
 *     SST_TREE_NOT_EXIST - is Tree widget does not exist
 * Side effects:
 *    If  path is in the dump then its tree is opened and scrolled
 *    to be it to display. Node is selected and associated signals
 *    are displayed.
 *    No change in any other case
  */

int SST_open_node(char *name) {
  int rv ;
   GtkCTree *ctree = GLOBALS->ctree_main;
   if (ctree) {
     GtkCTreeRow *gctr = GTK_CTREE_ROW(GLOBALS->any_tree_node);
     for(GTK_CTREE_ROW(GLOBALS->any_tree_node); gctr->parent;
	 gctr = GTK_CTREE_ROW(gctr->parent)) ;
     GtkCTreeNode *target_node ;
     if ((target_node = SST_find_node_by_path(gctr, name))) {
       struct tree *t ;
       GtkWidget *sig_view;
       rv = SST_NODE_FOUND ;
       gtk_ctree_collapse_recursive(ctree, gctr->parent) ;
       SST_open_path(ctree, target_node) ;
       gtk_ctree_node_moveto(ctree, target_node, 0, 0.5, 0.5);
       gtk_ctree_select(ctree, target_node);
       gctr = GTK_CTREE_ROW(target_node) ;
       t = (struct tree *)(gctr->row.data) ;
       GLOBALS->sig_root_treesearch_gtk2_c_1 = t->child;
       fill_sig_store ();
     } else {
       rv = SST_NODE_NOT_EXIST ;
     }
   } else {
     rv = SST_TREE_NOT_EXIST ;
   }
   return rv ;
}
/* ===== Double link lists */
llist_p *llist_new(long v, ll_elem_type type, int arg) {
  llist_p *p = (llist_p *)malloc_2(sizeof(llist_p)) ;
  p->next = p->prev = NULL ;
  switch(type) {
  case LL_INT: p->u.i = (int)v ; break ;
  case LL_UINT: p->u.u = (unsigned int)v ; break ;
  case LL_CHAR: p->u.c = (char)v ; break ;
  case LL_SHORT: p->u.s = (short)v ; break ;
  case LL_STR: 
    if(arg == -1)
      p->u.str = strdup_2((char *)v) ;
    else {
      p->u.str = (char *)malloc_2(arg) ;
      strncpy(p->u.str, (char *)v, arg) ; 
      p->u.str[arg] = '\0' ;
    }
    break ;
  case LL_VOID_P: p->u.p = (void *)v ; break ;
  }
  return p ;
}

llist_p *llist_append(llist_p *head, llist_p *elem, llist_p **tail) {
  llist_p *p ;
  if (*tail) {
    p = tail[0] ;
    p->next = elem ;
    elem->prev = p ;
    tail[0] = elem ;
  } else {
    if (head) {
      for(p = head ; p->next; p = p->next) ;
      p->next = elem ;
      elem ->prev = p ;
    } else {
      head = elem ;
    }
  }
  return head ;
}

llist_p *llist_remove_last(llist_p *head, llist_p **tail, ll_elem_type type, void *f() ) {
  if (head) {
    llist_p *p = tail[0] ;
    switch(type) {
    case LL_STR: free_2(p->u.str) ; break ;
    case LL_VOID_P: 
      if (f)
	f(p->u.p) ; 
      break ;
    }
    if (p->prev) {
      tail[0] = p->prev ;
    } else {
      head = tail[0] = NULL ;
    }
    free_2(p) ;
  }
  return head ;
}

void llist_free(llist_p *head, ll_elem_type type, void *f()) {
  llist_p *p = head, *p1 ;
  while(p) {
    p1 = p->next ;
    switch(type) {
    case LL_STR: free_2(p->u.str) ; break ;
    case LL_VOID_P: 
      if (f)
	f(p->u.p) ; 
      break ;
    }
    free_2(p) ;
    p = p1 ;
  }
}
/* ===================================================== */
static Trptr find_first_highlighted_trace(void)
{
Trptr t=GLOBALS->traces.first;
while(t)
        {
        if(t->flags&TR_HIGHLIGHT)
                {  
		if(!(t->flags&(TR_BLANK|TR_ANALOG_BLANK_STRETCH)))
			{
			break;
			}
                }
        t=t->t_next;
        }

return(t);
}

llist_p *signal_change_list(char *sig_name, int dir, int start_time, 
		       int end_time, int max_elements) {
  llist_p *l0_head = NULL, *l0_tail = NULL, *l1_head = NULL,*l_elem, *lp ;
  Trptr t = NULL ;
  if(!sig_name) {
    t = (Trptr)find_first_highlighted_trace();
  } else {
    /* case of sig name, find the representing Trptr structure */
  }
  if (t) {			/* we have a signal */
    /* create a list of value change structs (hptrs or vptrs */
    int nelem = 0, bw = -1 ;
    int tstart = (dir == STRACE_FORWARD) ? start_time : end_time ;
    int tend = (dir == STRACE_FORWARD) ? end_time : start_time ;
    if (!t->vector) {
      hptr h, h1;
      int len = 0  ;
      if(t->n.nd->ext)
	bw = abs(t->n.nd->ext->msi - t->n.nd->ext->lsi) + 1 ;
      h = bsearch_node(t->n.nd, tstart) ;
      for(h1 = h; h1; h1 = h1->next) {
	if (h1->time <= tend) {
	  if (len++ < max_elements) {
	    l_elem = llist_new((long)h1, LL_VOID_P, -1) ;
	    l0_head = llist_append(l0_head, l_elem, &l0_tail) ;
	    if(!l0_tail) l0_tail = l0_head ;
	  } else {
	    if(dir == STRACE_FORWARD)
	      break ;
	    else {
	      l_elem = l0_head ;
	      l0_head = l0_head->next ;
	      l0_head->prev = NULL ;
	      l_elem->u.p = (void *)h1 ;
	      l_elem->next = NULL ;
	      l_elem->prev = l0_tail ;
	      l0_tail->next = l_elem ;
	      l0_tail = l_elem ;
	    }
	  }
	}
      }
    } else {
      vptr v, v1;
      v = bsearch_vector(t->n.vec, tstart) ;
      for(v1 = v; v1; v1 = v1->next) {
	if (v1->time <= tend) {
	  l_elem = llist_new((long)v1, LL_VOID_P, -1) ;
	  l0_head = llist_append(l0_head, l_elem, &l0_tail) ;
	  if(!l0_tail) l0_tail = l0_head ;
	}
      }
    }
    lp = (start_time < end_time) ? l0_head : l0_tail ;
    llist_p *l1_tail = NULL ;
    char *s ;
    hptr h_ptr ;
    /* now create a linked list of time,value.. */
    while (lp && (nelem++ < max_elements)) {
      l_elem = llist_new((long)((t->vector) ? ((vptr)lp->u.p)->time:
				((hptr)lp->u.p)->time), LL_INT, -1) ;
      l1_head = llist_append(l1_head, l_elem, &l1_tail) ;
      if(!l1_tail) l1_tail = l1_head ;
      if(t->vector == 0) {
	if(!t->n.nd->ext) {	/* really single bit */
	  switch(((hptr)lp->u.p)->v.h_val) {
	  case AN_0: l_elem = llist_new((long)"0", LL_STR, -1) ; break ;
	  case AN_1: l_elem = llist_new((long)"1", LL_STR, -1) ; break ;
	  case AN_X: l_elem = llist_new((long)"x", LL_STR, -1) ; break ;
	  case AN_Z: l_elem = llist_new((long)"z", LL_STR, -1) ; break ;
	  }
	} else {		/* this is still an array */
	  h_ptr = (hptr)lp->u.p ;
	  if(h_ptr->flags&HIST_REAL) {
	    if(!(h_ptr->flags&HIST_STRING)) {
	      s=convert_ascii_real((double *)h_ptr->v.h_vector);
	    } else {
	      s=convert_ascii_string((char *)h_ptr->v.h_vector);
	    }
	  } else {
	    s=convert_ascii_vec(t,h_ptr->v.h_vector);
	  }
	  if(s) {
	    l_elem = llist_new((long)s, LL_STR, -1) ;
	  } else {
	    l1_head = llist_remove_last(l1_head, &l1_tail, LL_INT, NULL) ;
	  }
	}
      } else {
	l_elem = llist_new((long)convert_ascii(t, (vptr)lp->u.p), LL_STR, -1) ;
      }
      l1_head = llist_append(l1_head, l_elem, &l1_tail) ;
      lp = (start_time < end_time) ? lp->next : lp->prev ;
    }
    llist_free(l0_head, LL_VOID_P, NULL) ;
  }
  return l1_head ;
}
