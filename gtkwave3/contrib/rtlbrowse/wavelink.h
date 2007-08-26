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
 * Revision 1.1.1.1.2.1  2007/08/18 21:51:56  gtkwave
 * widget destroys and teardown of file formats which use external loaders
 * and are outside of malloc_2/free_2 control
 *
 * Revision 1.1.1.1  2007/05/30 04:25:38  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:10  gtkwave
 * initial release
 *
 */

