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

/* set of functions to support TCL interface */

/* **
 * Search for a tree node that is associated with the hierarchical path
 * return pointer to this node or NULL
 */
GtkCTreeNode *SST_find_node_by_path(GtkCTreeRow *root, char *path) {
  char *s = strdup(path) ;
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
     GtkCTreeRow *gctr;
     GtkCTreeNode *target_node;
     for(gctr = GTK_CTREE_ROW(GLOBALS->any_tree_node); gctr->parent; gctr = GTK_CTREE_ROW(gctr->parent)) ;
     if ((target_node = SST_find_node_by_path(gctr, name))) {
       struct tree *t ;
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

/* 
 * $Id$
 * $Log$
 * Revision 1.1  2009/09/20 21:43:35  gtkwave
 * created
 *  
 */
