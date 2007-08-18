#ifndef BROWSE_WAVELINK_H
#define BROWSE_WAVELINK_H

/* kill the GLOBALS_H header inclusion as it's not needed here */
#define GLOBALS_H

#include "../../src/ae2.h"
#include "../../src/debug.h"
#include "../../src/helpers/vzt_read.h"
#include "../../src/helpers/lxt2_read.h"

extern struct vzt_rd_trace  *vzt;
extern struct lxt2_rd_trace *lx2;

#ifdef AET2_IS_PRESENT
extern AE2_HANDLE ae2;
#endif

#endif

/*
 * $Id$
 * $Log$
 * Revision 1.1.1.1  2007/05/30 04:25:38  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:10  gtkwave
 * initial release
 *
 */

