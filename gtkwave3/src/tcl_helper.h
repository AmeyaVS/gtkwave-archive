/*
 * Copyright (c) Tony Bybell and Concept Engineering GmbH 2008-2009.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef WAVE_TCLHELPER_H
#define WAVE_TCLHELPER_H

#include <config.h>

#ifdef HAVE_LIBTCL

#include <tcl.h>

typedef struct
        {
        const char *cmdstr;
        int (*func)();
        } tcl_cmdstruct;

extern tcl_cmdstruct gtkwave_commands[];

#endif

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

int process_url_list(char *s);
int process_tcl_list(char *s, gboolean track_mouse_y);
char *add_dnd_from_searchbox(void);
char *add_dnd_from_signal_window(void);
char *add_traces_from_signal_window(gboolean is_from_tcl_command);
char *add_dnd_from_tree_window(void);
char *emit_gtkwave_savefile_formatted_entries_in_tcl_list(Trptr trhead, gboolean use_tcl_mode);

char* zMergeTclList(int argc, const char** argv);
char** zSplitTclList(const char* list, int* argcPtr);
char *make_single_tcl_list_name(char *s, char *opt_value, int promote_to_bus);
void make_tcl_interpreter(char *argv[]);

#endif

/* 
 * $Id$
 * $Log$
 * Revision 1.14  2009/01/20 06:11:48  gtkwave
 * added gtkwave::getDisplayedSignals command
 *
 * Revision 1.13  2009/01/02 06:24:28  gtkwave
 * bumped copyright to 2009
 *
 * Revision 1.12  2009/01/02 06:01:51  gtkwave
 * added getArgv for tcl commands
 *
 * Revision 1.11  2008/11/25 18:07:32  gtkwave
 * added cut copy paste functionality that survives reload and can do
 * multiple pastes on the same cut buffer
 *
 * Revision 1.10  2008/11/24 02:55:10  gtkwave
 * use TCL_INCLUDE_SPEC to fix ubuntu compiles
 *
 * Revision 1.9  2008/11/17 16:49:38  gtkwave
 * convert net object to netBus when encountering stranded bits in
 * signal search and tree search window
 *
 * Revision 1.8  2008/10/26 02:36:06  gtkwave
 * added netValue and netBusValue tcl list values from sigwin drag
 *
 * Revision 1.7  2008/10/17 18:05:27  gtkwave
 * split tcl command extensions out into their own separate file
 *
 * Revision 1.6  2008/10/13 22:16:52  gtkwave
 * tcl interpreter integration
 *
 * Revision 1.5  2008/10/02 00:52:25  gtkwave
 * added dnd of external filetypes into viewer
 *
 * Revision 1.4  2008/09/29 22:46:39  gtkwave
 * complex dnd handling with gtkwave trace attributes
 *
 * Revision 1.3  2008/09/27 05:05:05  gtkwave
 * removed unnecessary sing_len struct item
 *
 * Revision 1.2  2008/09/25 01:41:36  gtkwave
 * drag from tree clist window into external process
 *
 * Revision 1.1  2008/09/25 01:31:29  gtkwave
 * file creation
 *
 */

