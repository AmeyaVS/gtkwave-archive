#include"globals.h"/*
 * Copyright (c) Tony Bybell 2005.     
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef WAVE_TRANSLATE_H
#define WAVE_TRANSLATE_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "fgetdynamic.h"
#include "debug.h"

/*
 * char splay
 */
typedef struct xl_tree_node xl_Tree;
struct xl_tree_node {
    xl_Tree *left, *right;
    char *item;
    char *trans;
};


#define FILE_FILTER_MAX 1024



xl_Tree * xl_splay (char *i, xl_Tree * t);
void trans_searchbox(char *title);
void init_filetrans_data(void);
void install_file_filter(int which);
void set_current_translate_file(char *name);

#endif

/*
 * $Id$
 * $Log$
 * Revision 1.1.1.1  2007/05/30 04:27:21  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:17  gtkwave
 * initial release
 *
 */

