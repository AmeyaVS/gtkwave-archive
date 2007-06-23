/* 
 * Copyright (c) Tony Bybell 2004.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef WAVE_AE2RDR_H
#define WAVE_AE2RDR_H

#ifndef _MSC_VER
#include <inttypes.h>
#endif
#include "vcd.h"
#include "lx2.h"

#define AET2_RDLOAD "AE2LOAD | "

struct ae2_ncycle_autosort
{
struct ae2_ncycle_autosort *next;
};

struct regex_links
{
struct regex_links *next;
void *pnt;
};


TimeType 	ae2_main(char *fname, char *skip_start, char *skip_end, char *indirect_fname);
void 		import_ae2_trace(nptr np);
void 		ae2_set_fac_process_mask(nptr np);
void 		ae2_import_masked(void);

#ifndef _MSC_VER
/*
 * texsim prototypes/libae2rw interfacing...
 */
#define	MAXFACLEN 	 65536
typedef unsigned long	 FACIDX;

struct facref
{
FACIDX s;                         /* symbol table key */
char *facname;                    /* ptr to facility name */
int offset;                       /* offset of reference */ 
int length;                       /* length of reference */  
unsigned int row;                 /* row number for arrays */
unsigned int row_high;            /* row number for arrays */
};
typedef struct facref FACREF;

typedef void* 	AE2_HANDLE;
typedef unsigned long 	AE2_SYMBOL;
typedef void 	(*AE2_SEVERROR) (const char*, ...);
typedef void 	(*AE2_MSG) (int, const char*, ...);
typedef void* 	(*AE2_ALLOC) (size_t size);
typedef void 	(*AE2_FREE) (void* ptr, size_t size);

void 		ae2_initialize(AE2_SEVERROR error_fn, AE2_MSG msg_fn, AE2_ALLOC alloc_fn, AE2_FREE free_fn);
AE2_HANDLE 	ae2_read_initialize(FILE* file);
uint64_t 	ae2_read_start_cycle(AE2_HANDLE handle);
uint64_t 	ae2_read_end_cycle(AE2_HANDLE handle);  
unsigned long 	ae2_read_num_sections(AE2_HANDLE handle);
uint64_t* 	ae2_read_ith_section_range(AE2_HANDLE handle, unsigned long section_idx);
unsigned long	ae2_read_num_symbols(AE2_HANDLE handle);
unsigned long 	ae2_read_symbol_name(AE2_HANDLE handle, unsigned long symbol_idx, char* name);
unsigned long	ae2_read_symbol_rows(AE2_HANDLE handle, unsigned long symbol_idx);
unsigned long	ae2_read_symbol_length(AE2_HANDLE handle, unsigned long symbol_idx);
unsigned long 	ae2_read_value(AE2_HANDLE handle, FACREF* fr, uint64_t cycle, char* value);
uint64_t 	ae2_read_next_value(AE2_HANDLE handle, FACREF* fr, uint64_t cycle, char* value);
void 		ae2_read_close(AE2_HANDLE handle);

unsigned long	ae2_read_num_sparse_rows(AE2_HANDLE handle, unsigned long symbol_idx, uint64_t cycle);
uint64_t 	ae2_read_ith_sparse_row(AE2_HANDLE handle, unsigned long symbol_idx, uint64_t cycle, unsigned long idx);

#endif

#endif

/*
 * $Id$
 * $Log$
 * Revision 1.1.1.1  2007/05/30 04:27:30  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 01:39:00  gtkwave
 * initial release
 *
 */
