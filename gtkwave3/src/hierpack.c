/*
 * Copyright (c) Tony Bybell 2008.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include "hierpack.h"

char *hier_decompress(char *n)
{
char *recon;

if(*n & 0x80)
        {
        unsigned char varbuff[5];
        int idx = 0;
        int lhs, rhs, tot;
        unsigned int idxval = 0;
        while(*n & 0x80)
                {
                varbuff[idx] = *n & 0x7f;
                n++;
                idx++;
                }
        while(--idx >= 0)
                {
                idxval <<= 7;
                idxval |= (unsigned int)varbuff[idx];
                }

        lhs = strlen(GLOBALS->pfx_hier_array[idxval]);
        rhs = strlen(n);
        tot = lhs + rhs + 1;
        recon = malloc_2(tot);
        strcpy(recon, GLOBALS->pfx_hier_array[idxval]);
        strcpy(recon + lhs, n);
        }
	else
	{
	recon = n;
	}

return(recon);
}


char *hier_decompress_flagged(char *n, int *was_packed)
{
if(n)
	{
	*was_packed = ((*n & 0x80) != 0);
	return(hier_decompress(n));
	}
	else
	{
	return(n);
	}
}


char *hier_compress(char *name, int add_missing_pfx_to_tree, int *was_packed)
{
char *pnt = name;
char *last_h = NULL;
int curr_pfx_len;
JRB node;

*was_packed = 0;

while(*pnt)
	{
	if(*pnt == GLOBALS->hier_delimeter) last_h = pnt;
	pnt++;
	}
pnt = name;

if(last_h)
	{
	unsigned char varidx[5];
	unsigned int shiftval;
	unsigned int shiftidx;
	unsigned int shiftmasked;
	unsigned int pfxlen, sfxlen;

	if(!GLOBALS->hier_pfx) 
		{
		GLOBALS->hier_pfx = make_jrb();
		GLOBALS->prev_hier_pfx = strdup_2("");
		GLOBALS->prev_hier_pfx_len = 0;
		}
	curr_pfx_len = last_h - pnt + 1;
	*last_h = 0;
	if(GLOBALS->prev_hier_pfx_len && (GLOBALS->prev_hier_pfx_len == curr_pfx_len) && !strcmp(GLOBALS->prev_hier_pfx, pnt))
		{
		/* pfx matches so reuse */
		}
		else
		{
		node = jrb_find_str(GLOBALS->hier_pfx, pnt);
		if(!node)
			{
			Jval val;
			val.ui = GLOBALS->prev_hier_pfx_cnt = GLOBALS->hier_pfx_cnt;
			if(add_missing_pfx_to_tree)
				{
				node = jrb_insert_str(GLOBALS->hier_pfx, strdup_2(pnt), val);
				GLOBALS->hier_pfx_cnt++;
				/* printf("Add %d: '%s' %d\n", node->val.ui, node->key.s, GLOBALS->hier_pfx_cnt); */
				}
			}
			else
			{
			GLOBALS->prev_hier_pfx_cnt = node->val.ui;
			}
		}

	shiftval = GLOBALS->prev_hier_pfx_cnt;
	shiftidx = 0;

	for(shiftidx=0;;shiftidx++)
		{	
		shiftmasked = (shiftval & 0x7f) | 0x80; /* 0x80 = hier idx marker */
		shiftval >>= 7;
		if(!shiftval)
			{
			varidx[shiftidx] = shiftmasked;
			shiftidx++;
			break;
			}
		varidx[shiftidx] = shiftmasked;		
		}

	pfxlen = shiftidx;
	sfxlen = strlen(last_h+1);
	if(curr_pfx_len > pfxlen)
		{
		static int savecnt = 0;
		char *namereplace = malloc_2(pfxlen + 1 + sfxlen + 1);
		memcpy(namereplace, varidx, pfxlen);
		namereplace[pfxlen] = GLOBALS->hier_delimeter;
		strcpy(namereplace+pfxlen+1, last_h+1);
		*was_packed = 1;
		name = namereplace;
		savecnt += curr_pfx_len+1-pfxlen;
		}
		else
		{
		*last_h = GLOBALS->hier_delimeter;
		}
	}
	else 
	{
	/* no leading hier */
	}

return(name);
}
