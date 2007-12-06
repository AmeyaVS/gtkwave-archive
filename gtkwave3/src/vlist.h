/* 
 * Copyright (c) Tony Bybell 2006.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include "globals.h"

#ifndef WAVE_VLIST_H
#define WAVE_VLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"

struct vlist_t
{
struct vlist_t *next;
unsigned int siz;
unsigned int offs;
unsigned int elem_siz;
};


void vlist_init_spillfile(void);
void vlist_kill_spillfile(void);

struct vlist_t *vlist_create(unsigned int elem_siz);
void vlist_destroy(struct vlist_t *v);
void *vlist_alloc(struct vlist_t **v, int compressable);
unsigned int vlist_size(struct vlist_t *v);
void *vlist_locate(struct vlist_t *v, unsigned int idx);
void vlist_freeze(struct vlist_t **v);
void vlist_uncompress(struct vlist_t **v);

#endif

/*
 * $Id$
 * $Log$
 * Revision 1.3  2007/11/30 01:31:23  gtkwave
 * added vlist memory spill to disk code + fixed vcdload status bar on > 2GB
 *
 * Revision 1.2  2007/08/26 21:35:46  gtkwave
 * integrated global context management from SystemOfCode2007 branch
 *
 * Revision 1.1.1.1.2.2  2007/08/25 19:43:46  gtkwave
 * header cleanups
 *
 * Revision 1.1.1.1.2.1  2007/08/05 02:27:28  kermin
 * Semi working global struct
 *
 * Revision 1.1.1.1  2007/05/30 04:27:22  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:18  gtkwave
 * initial release
 *
 */

