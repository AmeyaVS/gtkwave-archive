/*
 * Copyright (c) Tony Bybell 2011.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <config.h>
#include "globals.h"
#include "tree_component.h"

#ifdef _WAVE_HAVE_JUDY

void iter_through_comp_name_table(void)
{
Pvoid_t  PJArray = GLOBALS->comp_name_judy;
PPvoid_t PPValue;

if(GLOBALS->comp_name_judy)
	{
	char *mem = malloc_2(GLOBALS->comp_name_total_stringmem);
	char **idx = GLOBALS->comp_name_idx = calloc_2(GLOBALS->comp_name_serial, sizeof(char *));
	char *Index = calloc_2(GLOBALS->comp_name_longest + 1, sizeof(char));
	char *pnt = mem;

	for (PPValue  = JudySLFirst (PJArray, (uint8_t *)Index, PJE0);
	         PPValue != (PPvoid_t) NULL;
	         PPValue  = JudySLNext  (PJArray, (uint8_t *)Index, PJE0))
	    {
		int slen = strlen(Index);

		memcpy(pnt, Index, slen+1);
	        idx[(*(char **)PPValue) - ((char *)NULL)] = pnt;
		pnt += (slen + 1);
	    }
	
	free_2(Index);
	JudySLFreeArray(&GLOBALS->comp_name_judy, PJE0);
	GLOBALS->comp_name_judy = NULL;
	}
}


int add_to_comp_name_table(const char *s)
{
PPvoid_t PPValue = JudySLGet(GLOBALS->comp_name_judy, (uint8_t *)s, PJE0);
int slen;

if(PPValue)
	{
        return((*(char **)PPValue) - ((char *)NULL) + 1);
        }

slen = strlen(s);
GLOBALS->comp_name_total_stringmem += (slen + 1);

if(slen > GLOBALS->comp_name_longest)
	{
	GLOBALS->comp_name_longest = slen;
	}

PPValue = JudySLIns(&GLOBALS->comp_name_judy, (uint8_t *)s, PJE0);
*((char **)PPValue) = ((char *)NULL) + GLOBALS->comp_name_serial;

return(++GLOBALS->comp_name_serial);	/* always nonzero */
}

#else

int add_to_comp_name_table(const char *s)	/* always zero = don't add */
{
return(0);
}

void iter_through_comp_name_table(void)
{
/* nothing */
}

#endif

/*
 * $Id$
 * $Log$
 */
