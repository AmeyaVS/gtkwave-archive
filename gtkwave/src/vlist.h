/* 
 * Copyright (c) Tony Bybell 2006.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

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

extern int vlist_compression_depth; /* 0-9 zlib value, -1 disables */


struct vlist_t *vlist_create(unsigned int elem_siz, unsigned int elem_start_cnt);
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
 */

