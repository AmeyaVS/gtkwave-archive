/*
 * Copyright (c) Tony Bybell 2010-2014
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef WAVE_MARKERBOX_H
#define WAVE_MARKERBOX_H

/* do not go less than 26! */
#define WAVE_NUM_NAMED_MARKERS      (26)

/* 702 will go from A-Z, AA-AZ, ... , ZA-ZZ */
/* this is a bijective name similar to the columns on spreadsheets */
/* #define WAVE_NUM_NAMED_MARKERS      (702) */

void markerbox(char *title, GtkSignalFunc func);

char *make_bijective_marker_id_string(char *buf, unsigned int value);
unsigned int bijective_marker_id_string_hash(char *so);
unsigned int bijective_marker_id_string_len(char *s);

#endif

