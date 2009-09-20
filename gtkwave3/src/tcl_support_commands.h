/* 
 * Copyright (c) Yiftach Tzori 2009.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */
#ifndef WAVE_TCL_SUPPORT_CMDS_H
#define WAVE_TCL_SUPPORT_CMDS_H

#include <gtk/gtk.h>

GtkCTreeNode *SST_find_node_by_path(GtkCTreeRow *root, char *path);
int SST_open_path(GtkCTree *ctree, GtkCTreeNode *node);
void fill_sig_store (void);
int SST_open_node(char *name);

#define SST_NODE_FOUND 0
#define SST_NODE_NOT_EXIST 1
#define SST_TREE_NOT_EXIST -1

#endif

/*
 * $Id$
 * $Log$
 */

