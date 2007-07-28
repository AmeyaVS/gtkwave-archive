/* 
 * Copyright (c) Tony Bybell 1999.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef WAVE_RC_H
#define WAVE_RC_H

struct rc_entry
{
char *name;
int (*func)(char *);
};

void read_rc_file(char *override_rc);
int get_rgb_from_name(char *str);
GdkGC *get_gc_from_name(char *str);

extern int rc_line_no;

#endif

/*
 * $Id$
 * $Log$
 * Revision 1.2  2007/07/23 23:13:08  gtkwave
 * adds for color tags in filtered trace data
 *
 * Revision 1.1.1.1  2007/05/30 04:28:00  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:17  gtkwave
 * initial release
 *
 */

