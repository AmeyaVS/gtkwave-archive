#include"globals.h"/* 
 * Copyright (c) Tony Bybell 1999-2004.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <config.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <gtk/gtk.h>


#include "rc.h"
#include "rgb.h"
#include "color.h"
#include "currenttime.h"

#ifdef _MSC_VER
#define strcasecmp _stricmp
#endif


//C_ARRAY_SIZE is (sizeof(c_red)/sizeof(unsigned char))

static GdkGC *rgb_contexts[sizeof(c_red)/sizeof(unsigned char)];

static int compar(const void *v1, const void *v2)
{
return((int)strcasecmp((char *)v1, *(char **)v2));
}


GdkGC *get_gc_from_name(char *str)
{
char **match;   
int offset, rgb;

if((match=(char **)bsearch((void *)str, (void *)cnames, sizeof(c_red)/sizeof(unsigned char), sizeof(char *), compar)))
	{
	offset=match-cnames;
	rgb=((int)c_red[offset]<<16)|((int)c_grn[offset]<<8)|((int)c_blu[offset]);

	if(!GLOBALS.rgb_contexts_rgb_c_1[offset]) GLOBALS.rgb_contexts_rgb_c_1[offset] = alloc_color(GLOBALS.wavearea, rgb, GLOBALS.wavearea->style->white_gc);

	return(GLOBALS.rgb_contexts_rgb_c_1[offset]);
	}

return(NULL);
}

int get_rgb_from_name(char *str)
{
char **match;
int offset, rgb;

if((match=(char **)bsearch((void *)str, (void *)cnames, sizeof(c_red)/sizeof(unsigned char), sizeof(char *), compar)))
	{
	offset=match-cnames;
	rgb=((int)c_red[offset]<<16)|((int)c_grn[offset]<<8)|((int)c_blu[offset]);
	return(rgb);
	}
	else
	{
	unsigned char *pnt=(unsigned char *)str;
	int l=strlen(str);
	unsigned char ch;
	int i, rc;

	for(i=0;i<l;i++)
		{
		ch=*(pnt++);
		if     (((ch>='0')&&(ch<='9')) ||
			((ch>='a')&&(ch<='f')) ||
			((ch>='A')&&(ch<='F'))) continue;
			else
			{
			#if defined _MSC_VER || defined __MINGW32__
			fprintf(stderr, "** gtkwave.ini (line %d): '%s' is an unknown color value; ignoring.\n", rc_line_no, str);
			#else
			fprintf(stderr, "** .gtkwaverc (line %d): '%s' is an unknown color value; ignoring.\n", GLOBALS.rc_line_no, str);
			#endif
			return(~0);
			}
		}

	sscanf(str,"%x",&rc);
	return(rc&0x00ffffff);
	}
}

/*
 * $Id$
 * $Log$
 * Revision 1.1.1.1.2.3  2007/07/31 03:18:01  kermin
 * Merge Complete - I hope
 *
 * Revision 1.1.1.1.2.2  2007/07/28 19:50:40  kermin
 * Merged in the main line
 *
 * Revision 1.2  2007/07/23 23:13:08  gtkwave
 * adds for color tags in filtered trace data
 *
 * Revision 1.1.1.1  2007/05/30 04:27:58  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:17  gtkwave
 * initial release
 *
 */

