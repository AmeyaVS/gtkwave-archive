/*
 * Copyright (c) Tony Bybell 2005.     
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include "globals.h"

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


#define FILE_FILTER_MAX (128)



xl_Tree * xl_splay (char *i, xl_Tree * t);
xl_Tree * xl_insert(char *i, xl_Tree * t, char *trans);
xl_Tree * xl_delete(char *i, xl_Tree * t);


void trans_searchbox(char *title);
void init_filetrans_data(void);
int install_file_filter(int which);
void set_current_translate_file(char *name);

#endif

/*
 * $Id$
 * $Log$
 * Revision 1.3  2010/03/30 18:33:27  gtkwave
 * fixed cut and paste errors from file to proc
 *
 * Revision 1.2  2007/08/26 21:35:46  gtkwave
 * integrated global context management from SystemOfCode2007 branch
 *
 * Revision 1.1.1.1.2.3  2007/08/25 19:43:46  gtkwave
 * header cleanups
 *
 * Revision 1.1.1.1.2.2  2007/08/21 22:35:40  gtkwave
 * prelim tree state merge
 *
 * Revision 1.1.1.1.2.1  2007/08/05 02:27:24  kermin
 * Semi working global struct
 *
 * Revision 1.1.1.1  2007/05/30 04:27:21  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:17  gtkwave
 * initial release
 *
 */

