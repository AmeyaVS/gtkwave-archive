/* 
 * Copyright (c) Tony Bybell 2005.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include "globals.h"

#ifndef GHW_H
#define GHW_H

#include <limits.h>
#include "ghwlib.h"
#include "tree.h"
#include "vcd.h"

#define WAVE_GHW_DUMMYFACNAME "!!__(dummy)__!!"

TimeType ghw_main(char *fname);
int strand_pnt(char *s);

#endif

/*
 * $Id$
 * $Log$
 * Revision 1.1.1.1.2.1  2007/08/05 02:27:20  kermin
 * Semi working global struct
 *
 * Revision 1.1.1.1  2007/05/30 04:27:22  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:12  gtkwave
 * initial release
 *
 */

