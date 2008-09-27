/*
 * Copyright (c) Tony Bybell and Concept Engineering GmbH 2008.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef WAVE_TCLHELPER_H
#define WAVE_TCLHELPER_H

#define WAVE_OE_ME \
	if(one_entry) \
		{ \
		if(!mult_entry) \
			{ \
			mult_entry = one_entry; \
			mult_len = strlen(mult_entry); \
			} \
			else \
			{ \
			int sing_len = strlen(one_entry); \
			mult_entry = realloc_2(mult_entry, mult_len + sing_len + 1); \
			strcpy(mult_entry + mult_len, one_entry); \
			mult_len += sing_len; \
			free_2(one_entry); \
			} \
		}

struct iter_dnd_strings 
	{
	char *one_entry;
	char *mult_entry;
	int mult_len;
	};

int process_tcl_list(char *s);
char *add_dnd_from_searchbox(void);
char *add_dnd_from_signal_window(void);
char *add_dnd_from_tree_window(void);

#endif

/* 
 * $Id$
 * $Log$
 * Revision 1.2  2008/09/25 01:41:36  gtkwave
 * drag from tree clist window into external process
 *
 * Revision 1.1  2008/09/25 01:31:29  gtkwave
 * file creation
 *
 */

