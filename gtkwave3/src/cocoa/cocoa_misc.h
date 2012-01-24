/*
 * Copyright (c) Tony Bybell 2012.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef __COCOA_MISC_H__
#define __COCOA_MISC_H__

#ifdef WAVE_COCOA_GTK
#import <Cocoa/Cocoa.h>
#endif
#include <gtk/gtk.h>

char *gtk_file_req_bridge(const char *title, const char *fpath, const char *pattn, int is_writemode);

#endif
