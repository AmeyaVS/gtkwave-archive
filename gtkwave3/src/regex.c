/*
 * Copyright (c) Tony Bybell 1999-2004.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <config.h>

#ifdef _AIX
#ifndef _GNUC_
#include <sys/types.h>
#include <stdlib.h>
#define REGEX_MALLOC
#ifndef STDC_HEADERS
#define STDC_HEADERS
#endif
#endif
#endif

#ifdef __CYGWIN__
#include <sys/types.h>
#include <stdlib.h>
#define REGEX_MAY_COMPILE
#include "gnu_regex.c"
#else
#if defined __linux__
#include <sys/types.h>
#include <stdlib.h>
#include <regex.h>
#else			/* or for any other compiler that doesn't support POSIX.2 regexs properly like xlc or vc++ */
#ifdef __MINGW32__
#define bcopy(a,b,c) memcpy((b),(a),(c))
#define bzero(a,b) memset((a),0,(b))
#define bcmp(a,b,c) memcmp((a),(b),(c))
#endif
#ifdef _MSC_VER
#include <malloc.h> 
#define STDC_HEADERS  
#define alloca _alloca  /* AIX doesn't like this */
#endif
#define REGEX_MAY_COMPILE
#include "gnu_regex.c"
#endif
#endif

#include "regex_wave.h"
#include "debug.h"

static regex_t preg[WAVE_REGEX_TOTAL];
static int regex_ok[WAVE_REGEX_TOTAL]; /* automatically zeroed as static */

/*
 * compile a regular expression into a regex_t and
 * dealloc any previously valid ones
 */
int wave_regex_compile(char *regex, int which)
{
int comp_rc;

if(regex_ok[which]) { regfree(&preg[which]); } /* free previous regex_t ancillary data if valid */
comp_rc=regcomp(&preg[which], regex, REG_ICASE|REG_NOSUB);
return(regex_ok[which]=(comp_rc)?0:1);
}


/*
 * do match
 */
int wave_regex_match(char *str, int which)
{
int rc;

if(regex_ok[which])
	{
	rc = regexec(&preg[which], str, 0, NULL, 0);
	}
	else
	{
	rc = 1; /* fail on malformed regex */
	}

return((rc)?0:1);
}


/*
 * compile a regular expression and return the pointer to
 * the regex_t if valid else return NULL
 */
void *wave_regex_alloc_compile(char *regex)
{
regex_t *mreg = (regex_t *)malloc_2(sizeof(regex_t));
int comp_rc=regcomp(mreg, regex, REG_ICASE|REG_NOSUB);

if(comp_rc)
	{
	free_2(mreg);
	mreg=NULL;
	}

return((void *)mreg);
}


/*
 * do match
 */
int wave_regex_alloc_match(void *mreg, char *str)
{
int rc=regexec((regex_t *)mreg, str, 0, NULL, 0);

return((rc)?0:1);
}


/* 
 * free it
 */
void wave_regex_alloc_free(void *pnt)
{
regex_t *mreg = (regex_t *)pnt;

if(mreg)
	{
	regfree(mreg);
	free_2(mreg);
	}
}

/*
 * $Id$
 * $Log$
 * Revision 1.2  2007/04/20 02:08:17  gtkwave
 * initial release
 *
 */

