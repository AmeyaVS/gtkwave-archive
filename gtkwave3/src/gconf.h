/*
 * Copyright (c) Tony Bybell 2012.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef WAVE_GCONF_H
#define WAVE_GCONF_H

#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef WAVE_HAVE_GCONF

#include <gconf/gconf-client.h>

#endif


#define WAVE_GCONF_DIR "/com.geda.gtkwave"
/*                      12345678901234567 */
#define WAVE_GCONF_DIR_LEN (17)

int wave_rpc_id;

void wave_gconf_init(int argc, char **argv);
gboolean wave_gconf_client_set_string(const gchar *key, const gchar *val);

#endif
