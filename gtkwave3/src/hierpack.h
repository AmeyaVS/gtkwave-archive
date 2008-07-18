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

char *hier_decompress(char *n);
char *hier_decompress_flagged(char *n, int *was_packed);

char *hier_compress(char *name, int add_missing_pfx_to_tree, int *was_packed);

enum { HIERPACK_DO_NOT_ADD = 0, HIERPACK_ADD };

#endif
