/* 
 * Copyright (c) Tony Bybell 2001-2004
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <config.h>
#include "globals.h"
#include <stdio.h>

#ifndef _MSC_VER
#include <unistd.h>
#ifndef __MINGW32__ 
#include <sys/mman.h>
#endif
#else
#include <windows.h>
#endif


#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "symbol.h"
#include "vcd.h"
#include "lxt.h"
#include "bsearch.h"





/*
 * Generic hash function for symbol names...
 */
int hash(char *s)
{
char *p;
char ch;
unsigned int h=0, h2=0, pos=0, g;
for(p=s;*p;p++)
        {
	ch=*p;
	h2<<=3;
	h2-=((unsigned int)ch+(pos++));		/* this handles stranded vectors quite well.. */

        h=(h<<4)+ch;
        if((g=h&0xf0000000))
                {
                h=h^(g>>24);
                h=h^g;
                }   
        }

h^=h2;						/* combine the two hashes */
GLOBALS.hashcache=h%SYMPRIME;
return(GLOBALS.hashcache);
}


/*
 * add symbol to table.  no duplicate checking
 * is necessary as aet's are "correct."
 */
struct symbol *symadd(char *name, int hv)
{
struct symbol *s;

s=(struct symbol *)calloc_2(1,sizeof(struct symbol));
strcpy(s->name=(char *)malloc_2(strlen(name)+1),name);
s->next=GLOBALS.sym[hv];
GLOBALS.sym[hv]=s;
return(s);
}

struct symbol *symadd_name_exists(char *name, int hv)
{
struct symbol *s;

s=(struct symbol *)calloc_2(1,sizeof(struct symbol));
s->name = name;
s->next=GLOBALS.sym[hv];
GLOBALS.sym[hv]=s;
return(s);
}

/*
 * find a slot already in the table...
 */
struct symbol *symfind(char *s, unsigned int *rows_return)
{
int hv;
struct symbol *temp;

if(!GLOBALS.facs_are_sorted)
	{
	hv=hash(s);
	if(!(temp=GLOBALS.sym[hv])) return(NULL); /* no hash entry, add here wanted to add */
	
	while(temp)
	        {
	        if(!strcmp(temp->name,s))
	                {
	                return(temp); /* in table already */    
	                }
	        if(!temp->next) break;
	        temp=temp->next;
	        }
	
	return(NULL); /* not found, add here if you want to add*/
	}
	else	/* no sense hashing if the facs table is built */
	{	
	DEBUG(printf("BSEARCH: %s\n",s));
	return(bsearch_facs(s, rows_return));
	}
}

/*
 * $Id$
 * $Log$
 * Revision 1.1.1.1.2.1  2007/08/05 02:27:23  kermin
 * Semi working global struct
 *
 * Revision 1.1.1.1  2007/05/30 04:27:26  gtkwave
 * Imported sources
 *
 * Revision 1.3  2007/05/28 00:55:06  gtkwave
 * added support for arrays as a first class dumpfile datatype
 *
 * Revision 1.2  2007/04/20 02:08:17  gtkwave
 * initial release
 *
 */

