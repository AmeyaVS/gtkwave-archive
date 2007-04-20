#ifndef BROWSE_WAVELINK_H
#define BROWSE_WAVELINK_H

#include "../../src/debug.h"
#include "../../src/helpers/vzt_read.h"
#include "../../src/helpers/lxt2_read.h"
#include "../../src/ae2.h"

extern struct vzt_rd_trace  *vzt;
extern struct lxt2_rd_trace *lx2;

#ifdef AET2_IS_PRESENT
extern AE2_HANDLE ae2;
#endif

#endif
