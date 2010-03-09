/* 
 * Copyright (c) Tony Bybell 1999-2010
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */


/*
 * debug.c 01feb99ajb
 * malloc debugs added on 13jul99ajb
 * malloc tracking added on 05aug07ajb for 3.1 series
 */
#include <config.h>
#include "globals.h"
#include "debug.h"
#ifdef _WAVE_HAVE_JUDY
#include <Judy.h>
#endif

#undef free_2

#ifdef _WAVE_HAVE_JUDY
void free_outstanding(void)
{
Pvoid_t  PJArray = (Pvoid_t)GLOBALS->alloc2_chain;
PPvoid_t PPValue;
Word_t Index;
JError_t JError;
int ctr = 0;

#ifdef DEBUG_PRINTF
printf("\n*** cleanup ***\n");
printf("Freeing %d chunks\n", GLOBALS->outstanding);
system("date");
#endif

Index = 0;
for (PPValue = JudyLFirst(PJArray, &Index, &JError); PPValue != (PPvoid_t) NULL; PPValue = JudyLNext(PJArray, &Index, &JError))
	{
	free((void *)Index);
	ctr++;
	}
JudyLFreeArray(&PJArray, &JError);

GLOBALS->alloc2_chain = NULL;
GLOBALS->outstanding = 0;

#ifdef DEBUG_PRINTF
printf("Freed %d chunks\n", ctr);
system("date");
#endif
}
#else
void free_outstanding(void)
{
void **t = (void **)GLOBALS->alloc2_chain;
void **t2;
int ctr = 0;

#ifdef DEBUG_PRINTF
printf("\n*** cleanup ***\n");
printf("Freeing %d chunks\n", GLOBALS->outstanding);
system("date");
#endif

while(t)
	{
	t2 = (void **) *(t+1);
	free(t);
	t = t2;
	ctr++;
	}

GLOBALS->alloc2_chain = NULL;
GLOBALS->outstanding = 0;

#ifdef DEBUG_PRINTF
printf("Freed %d chunks\n", ctr);
system("date");
#endif
}
#endif

/*
 * wrapped malloc family...
 */
#ifdef _WAVE_HAVE_JUDY
void *malloc_2(size_t size)
{
void *ret;

ret=malloc(size);
if(ret)  
        {
	JError_t JError;
	PPvoid_t PPValue = JudyLIns ((Pvoid_t)&GLOBALS->alloc2_chain, (Word_t)ret, &JError);

        GLOBALS->outstanding++;
        
        return(ret);
        }   
        else
        {
        fprintf(stderr, "FATAL ERROR : Out of memory, sorry.\n");
        exit(1);
        }
}
#else
void *malloc_2(size_t size)
{
void *ret;

ret=malloc(size + 2*sizeof(void *));
if(ret)
	{
	void **ret2 = (void **)ret;
	*(ret2+0) = NULL;
	*(ret2+1) = GLOBALS->alloc2_chain;
	if(GLOBALS->alloc2_chain)
		{
		*(GLOBALS->alloc2_chain+0) = ret2;
		}
	GLOBALS->alloc2_chain = ret2;

	GLOBALS->outstanding++;

	return((char *)ret + 2*sizeof(void *));
	}
	else
	{
	fprintf(stderr, "FATAL ERROR : Out of memory, sorry.\n");
	exit(1);
	}
}
#endif

#ifdef _WAVE_HAVE_JUDY
void *realloc_2(void *ptr, size_t size)
{
void *ret=realloc(ptr, size);

if(ret!=ptr)
	{
	JError_t JError1, JError2;
	int delstat = JudyLDel ((Pvoid_t)&GLOBALS->alloc2_chain, (Word_t)ptr, &JError1);
	PPvoid_t PPValue2 = JudyLIns ((Pvoid_t)&GLOBALS->alloc2_chain, (Word_t)ret, &JError2);
	}        

if(ret)
        {
        return(ret);
        }
        else
        {
        fprintf(stderr, "FATAL ERROR : Out of memory, sorry.\n");
        exit(1);
        }
}
#else
void *realloc_2(void *ptr, size_t size)
{
void *ret;

void **ret2 = ((void **)ptr) - 2;
void **prv = (void **)*(ret2+0);
void **nxt = (void **)*(ret2+1);
                 
if(prv)
	{
        *(prv+1) = nxt;
        }
        else
        {
        GLOBALS->alloc2_chain = nxt;
        }
        
if(nxt)
	{
        *(nxt+0) = prv;
        }

ret=realloc((char *)ptr - 2*sizeof(void *), size + 2*sizeof(void *));

ret2 = (void **)ret;
*(ret2+0) = NULL;
*(ret2+1) = GLOBALS->alloc2_chain;
if(GLOBALS->alloc2_chain)
	{
	*(GLOBALS->alloc2_chain+0) = ret2;
	}
GLOBALS->alloc2_chain = ret2;

if(ret)
	{
	return((char *)ret + 2*sizeof(void *));
	}
	else
	{
	fprintf(stderr, "FATAL ERROR : Out of memory, sorry.\n");
	exit(1);
	}
}
#endif

#ifdef _WAVE_HAVE_JUDY
void *calloc_2_into_context(struct Global *g, size_t nmemb, size_t size)
{
void *ret;

ret=calloc(nmemb, size);
if(ret)
	{
	JError_t JError;
	PPvoid_t PPValue = JudyLIns ((Pvoid_t)&g->alloc2_chain, (Word_t)ret, &JError);

	g->outstanding++;

	return(ret);
	}
	else
	{
	fprintf(stderr, "FATAL ERROR: Out of memory, sorry.\n");
	exit(1);
	}
}
#else
void *calloc_2_into_context(struct Global *g, size_t nmemb, size_t size)
{
void *ret;

ret=calloc(1, (nmemb * size) + 2*sizeof(void *));
if(ret)
	{
	void **ret2 = (void **)ret;
	*(ret2+0) = NULL;
	*(ret2+1) = g->alloc2_chain;
	if(g->alloc2_chain)
		{
		*(g->alloc2_chain+0) = ret2;
		}
	g->alloc2_chain = ret2;

	g->outstanding++;

	return((char *)ret + 2*sizeof(void *));
	}
	else
	{
	fprintf(stderr, "FATAL ERROR: Out of memory, sorry.\n");
	exit(1);
	}
}
#endif

void *calloc_2(size_t nmemb, size_t size)
{
return(calloc_2_into_context(GLOBALS, nmemb, size));
}


#ifdef _WAVE_HAVE_JUDY
void free_2(void *ptr)
{
if(ptr)
	{
	JError_t JError;
	int delstat = JudyLDel ((Pvoid_t)&GLOBALS->alloc2_chain, (Word_t)ptr, &JError);

	if(delstat)
		{
		GLOBALS->outstanding--;
		free(ptr);
		}
		else
		{
		printf("JUDYMEM | free to non-malloc'd address %p blocked\n", ptr);
		}
	}
	else
	{
	fprintf(stderr, "WARNING: Attempt to free NULL pointer caught.\n");
	}
}
#else
void free_2(void *ptr)
{
if(ptr)
	{
	void **ret2 = ((void **)ptr) - 2;
	void **prv = (void **)*(ret2+0);
	void **nxt = (void **)*(ret2+1);

	if(prv)
		{
		*(prv+1) = nxt;
		}	
		else
		{
		GLOBALS->alloc2_chain = nxt;
		}

	if(nxt)
		{
		*(nxt+0) = prv;
		}

	GLOBALS->outstanding--;

	free((char *)ptr - 2*sizeof(void *));
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

char *strdup_2s(const char *s)
{
char *s2 = NULL;

if(s)
	{
	int len = strlen(s);
	s2 = malloc_2(len+2);
	strcpy(s2, s); 
	strcat(s2+len, " ");
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

GLOBALS->atoi_cont_ptr=NULL;

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
		GLOBALS->atoi_cont_ptr=str-1;
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
if(!GLOBALS->disable_tooltips)
	{
	gtk_tooltips_set_tip(tooltips, widget, tip_text, tip_private);
	}
}


void gtk_tooltips_set_delay_2(GtkTooltips *tooltips, guint delay)
{
if(!GLOBALS->disable_tooltips)
	{
	gtk_tooltips_set_delay(tooltips, delay);
	}
}


GtkTooltips* gtk_tooltips_new_2(void)
{
if(!GLOBALS->disable_tooltips)
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
 * Revision 1.9  2009/03/25 09:20:26  gtkwave
 * fixing reloader crashes in vcd_build_symbols if times is zero
 *
 * Revision 1.8  2009/01/20 06:11:48  gtkwave
 * added gtkwave::getDisplayedSignals command
 *
 * Revision 1.7  2007/09/23 18:33:54  gtkwave
 * warnings cleanups from sun compile
 *
 * Revision 1.6  2007/09/18 16:17:06  gtkwave
 * hiding of debug messages
 *
 * Revision 1.5  2007/09/09 20:10:30  gtkwave
 * preliminary support for tabbed viewing of multiple waveforms
 *
 * Revision 1.4  2007/08/31 22:42:43  gtkwave
 * 3.1.0 RC3 updates
 *
 * Revision 1.3  2007/08/26 21:35:40  gtkwave
 * integrated global context management from SystemOfCode2007 branch
 *
 * Revision 1.2.2.6  2007/08/25 19:43:45  gtkwave
 * header cleanups
 *
 * Revision 1.2.2.5  2007/08/23 02:47:32  gtkwave
 * updating of reload debug messages
 *
 * Revision 1.2.2.4  2007/08/07 04:54:59  gtkwave
 * slight modifications to global initialization scheme
 *
 * Revision 1.2.2.3  2007/08/07 03:18:54  kermin
 * Changed to pointer based GLOBAL structure and added initialization function
 *
 * Revision 1.2.2.2  2007/08/06 03:50:45  gtkwave
 * globals support for ae2, gtk1, cygwin, mingw.  also cleaned up some machine
 * generated structs, etc.
 *
 * Revision 1.1.1.1  2007/05/30 04:27:23  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:11  gtkwave
 * initial release
 *
 */

