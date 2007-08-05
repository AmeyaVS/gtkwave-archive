#include"globals.h"/* 
 * Copyright (c) Tony Bybell 1999.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */


/*
 * debug.c 01feb99ajb
 * malloc debugs added on 13jul99ajb
 */
#include <config.h>
#include "debug.h"

#undef free_2

#ifdef DEBUG_MALLOC	/* normally this should be undefined..this is *only* for finding stray allocations/frees */
	static struct memchunk *mem=NULL;
	static size_t mem_total=0;
	static int mem_chunks=0;

	static void mem_addnode(void *ptr, size_t size)
	{
	struct memchunk *m;
	
	m=(struct memchunk *)malloc(sizeof(struct memchunk));
	m->ptr=ptr;
	m->size=size;
	m->next=mem;
	
	mem=m;
	mem_total+=size;
	mem_chunks++;
	
	fprintf(stderr,"mem_addnode:  TC:%05d TOT:%010d PNT:%010p LEN:+%d\n",mem_chunks,mem_total,ptr,size);
	}
	
	static void mem_freenode(void *ptr)
	{
	struct memchunk *m, *mprev=NULL;
	m=mem;
	
	while(m)
		{
		if(m->ptr==ptr)
			{
			if(mprev)
				{
				mprev->next=m->next;
				}
				else
				{
				mem=m->next;
				}
	
			mem_total=mem_total-m->size;
			mem_chunks--;
			fprintf(stderr,"mem_freenode: TC:%05d TOT:%010d PNT:%010p LEN:-%d\n",mem_chunks,mem_total,ptr,m->size);
			free(m);
			return;
			}
		mprev=m;
		m=m->next;
		}
	
	fprintf(stderr,"mem_freenode: PNT:%010p *INVALID*\n",ptr);
	sleep(1);
	}
#endif


/*
 * wrapped malloc family...
 */
void *malloc_2(size_t size)
{
void *ret;
ret=malloc(size);
if(ret)
	{
	DEBUG_M(mem_addnode(ret,size));
	return(ret);
	}
	else
	{
	fprintf(stderr, "FATAL ERROR : Out of memory, sorry.\n");
	exit(1);
	}
}

void *realloc_2(void *ptr, size_t size)
{
void *ret;
ret=realloc(ptr, size);
if(ret)
	{
	DEBUG_M(mem_freenode(ptr));
	DEBUG_M(mem_addnode(ret,size));
	return(ret);
	}
	else
	{
	fprintf(stderr, "FATAL ERROR : Out of memory, sorry.\n");
	exit(1);
	}
}

void *calloc_2(size_t nmemb, size_t size)
{
void *ret;
ret=calloc(nmemb, size);
if(ret)
	{
	DEBUG_M(mem_addnode(ret, nmemb*size));
	return(ret);
	}
	else
	{
	fprintf(stderr, "FATAL ERROR: Out of memory, sorry.\n");
	exit(1);
	}
}


#ifdef DEBUG_MALLOC_LINES
void free_2(void *ptr, char *filename, int lineno)
{
if(ptr)
	{
	DEBUG_M(mem_freenode(ptr));
	free(ptr);
	}
	else
	{
	fprintf(stderr, "WARNING: Attempt to free NULL pointer caught: \"%s\", line %d.\n", filename, lineno);
	}
}
#else
void free_2(void *ptr)
{
if(ptr)
	{
	DEBUG_M(mem_freenode(ptr));
	free(ptr);
	}
	else
	{
	fprintf(stderr, "WARNING: Attempt to free NULL pointer caught.\n");
	}
}
#endif


char *strdup_2(const char *s)
{
char *s2 = NULL;

if(s)
	{
	int len = strlen(s);
	s2 = malloc_2(len+1);
	strcpy(s2, s); 
	}

return(s2);
}


/*
 * atoi 64-bit version..
 * y/on     default to '1'
 * n/nonnum default to '0'
 */

TimeType atoi_64(const char *str)
{
TimeType val=0;
unsigned char ch, nflag=0;

GLOBALS.atoi_cont_ptr=NULL;

switch(*str)
	{
	case 'y':
	case 'Y':
		return(LLDescriptor(1));

	case 'o':
	case 'O':
		str++;
		ch=*str;
		if((ch=='n')||(ch=='N'))
			return(LLDescriptor(1));
		else	return(LLDescriptor(0));

	case 'n':
	case 'N':
		return(LLDescriptor(0));
		break;

	default:
		break;
	}

while((ch=*(str++)))
	{
	if((ch>='0')&&(ch<='9'))
		{
		val=(val*10+(ch&15));
		}
	else
	if((ch=='-')&&(val==0)&&(!nflag))
		{
		nflag=1;
		}
	else
	if(val)
		{
		GLOBALS.atoi_cont_ptr=str-1;
		break;
		}
	}
return(nflag?(-val):val);
}


/*
 * wrapped tooltips
 */

void gtk_tooltips_set_tip_2(GtkTooltips *tooltips, GtkWidget *widget, 
	const gchar *tip_text, const gchar *tip_private)
{
if(!GLOBALS.disable_tooltips)
	{
	gtk_tooltips_set_tip(tooltips, widget, tip_text, tip_private);
	}
}


void gtk_tooltips_set_delay_2(GtkTooltips *tooltips, guint delay)
{
if(!GLOBALS.disable_tooltips)
	{
	gtk_tooltips_set_delay(tooltips, delay);
	}
}


GtkTooltips* gtk_tooltips_new_2(void)
{
if(!GLOBALS.disable_tooltips)
	{
	return(gtk_tooltips_new());
	}
	else
	{
	return(NULL);
	}
}


char *tmpnam_2(char *s, int *fd)
{
#if defined _MSC_VER || defined __MINGW32__

*fd = -1;
return(tmpnam(s));

#else

char *backpath = "gtkwaveXXXXXX";
char *tmpspace;
int len = strlen(P_tmpdir);
int i;

unsigned char slash = '/';
for(i=0;i<len;i++)
	{
	if((P_tmpdir[i] == '\\') || (P_tmpdir[i] == '/'))
		{
		slash = P_tmpdir[i];
		break;
		}
	}

tmpspace = malloc_2(len + 1 + strlen(backpath) + 1);
sprintf(tmpspace, "%s%c%s", P_tmpdir, slash, backpath);
*fd = mkstemp(tmpspace);
if(*fd<0)
	{
	fprintf(stderr, "tmpnam_2() could not create tempfile, exiting.\n");
	perror("Why");
	exit(255);
	}

return(tmpspace);

#endif
}

/*
 * $Id$
 * $Log$
 * Revision 1.2  2007/06/01 21:13:41  gtkwave
 * regenerate configure for cygwin with proper flags, add missing files
 *
 * Revision 1.1.1.1  2007/05/30 04:27:23  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:11  gtkwave
 * initial release
 *
 */

