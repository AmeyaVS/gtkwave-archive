/*
 * Copyright (c) Tony Bybell 2008.     
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef WAVE_HIERPACK_H
#define WAVE_HIERPACK_H

#include "globals.h"

void create_hier_array(void);
void free_hier_tree(void);

char *hier_decompress(char *n);
char *hier_decompress_flagged(char *n, int *was_packed);

char *hier_compress(char *name, int add_missing_pfx_to_tree, int *was_packed);

enum { HIERPACK_DO_NOT_ADD = 0, HIERPACK_ADD };

#endif

/*
 * $Id$
 * $Log$
 * Revision 1.3  2010/03/01 05:16:26  gtkwave
 * move compressed hier tree traversal to hierpack
 *
 * Revision 1.2  2008/07/18 17:29:50  gtkwave
 * adding cvs headers
 *
 * Revision 1.1  2008/07/18 17:27:01  gtkwave
 * adding hierpack code   
 *
 */

