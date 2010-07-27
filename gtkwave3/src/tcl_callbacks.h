/*
 * Copyright (c) Tony Bybell 2010.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef WAVE_TCLCB_H
#define WAVE_TCLCB_H

#include <config.h>

#ifdef HAVE_LIBTCL

#include <tcl.h>
#include <tk.h>
#include "debug.h"

#define WAVE_TCL_LIST_ELEMENT			(TCL_LIST_ELEMENT)
#define WAVE_TCL_APPEND_VALUE			(TCL_APPEND_VALUE)

#else

#define WAVE_TCL_LIST_ELEMENT			(0)
#define WAVE_TCL_APPEND_VALUE			(0)

#endif


#define WAVE_TCLCB_FLAGS_NONE			(WAVE_TCL_LIST_ELEMENT)
#define WAVE_TCLCB_FLAGS_APPEND			(WAVE_TCL_LIST_ELEMENT|TCL_APPEND_VALUE)

/* ################################################################ */

#define WAVE_TCLCB_CURRENT_ACTIVE_TAB		"gtkwave::cbCurrentActiveTab"
#define WAVE_TCLCB_CURRENT_ACTIVE_TAB_FLAGS	WAVE_TCLCB_FLAGS_NONE

#define WAVE_TCLCB_QUIT_PROGRAM			"gtkwave::cbQuitProgram"
#define WAVE_TCLCB_QUIT_PROGRAM_FLAGS		WAVE_TCLCB_FLAGS_NONE

#define WAVE_TCLCB_CLOSE_TAB_NUMBER		"gtkwave::cbCloseTabNumber"
#define WAVE_TCLCB_CLOSE_TAB_NUMBER_FLAGS	WAVE_TCLCB_FLAGS_NONE

#define WAVE_TCLCB_RELOAD			"gtkwave::cbReload"
#define WAVE_TCLCB_RELOAD_FLAGS			WAVE_TCLCB_FLAGS_NONE

#define WAVE_TCLCB_TREE_SELECT			"gtkwave::cbTreeSelect"
#define WAVE_TCLCB_TREE_SELECT_FLAGS		WAVE_TCLCB_FLAGS_NONE

#define WAVE_TCLCB_TREE_UNSELECT		"gtkwave::cbTreeUnselect"
#define WAVE_TCLCB_TREE_UNSELECT_FLAGS		WAVE_TCLCB_FLAGS_NONE

#define WAVE_TCLCB_OPEN_TRACE_GROUP		"gtkwave::cbOpenTraceGroup"
#define WAVE_TCLCB_OPEN_TRACE_GROUP_FLAGS	WAVE_TCLCB_FLAGS_NONE

#define WAVE_TCLCB_CLOSE_TRACE_GROUP		"gtkwave::cbCloseTraceGroup"
#define WAVE_TCLCB_CLOSE_TRACE_GROUP_FLAGS	WAVE_TCLCB_FLAGS_NONE

/* ################################################################ */

#endif
