/*
 * Copyright (c) Tony Bybell and Concept Engineering GmbH 2008.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <config.h>
#include "globals.h"
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#if WAVE_USE_GTK2
#include <glib/gconvert.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "gtk12compat.h"
#include "analyzer.h"
#include "tree.h"
#include "symbol.h"
#include "vcd.h"
#include "lx2.h"
#include "busy.h"
#include "debug.h"
#include "hierpack.h"
#include "menu.h"
#include "tcl_helper.h"

#if !defined __MINGW32__ && !defined _MSC_VER
#include <sys/types.h>
#include <unistd.h>
#endif

#ifdef HAVE_TCL_H
#include <tcl.h>
#endif

#ifdef _MSC_VER
#define strcasecmp _stricmp
#endif


/*----------------------------------------------------------------------
 * tclBackslash -- Figure out how to handle a backslash sequence in tcl list.
 *
 * Results:
 *      The return value is the character that should be substituted
 *      in place of the backslash sequence that starts at src.  If
 *      readPtr isn't NULL then it is filled in with a count of the
 *      number of characters in the backslash sequence.
 *----------------------------------------------------------------------
 */
static char tclBackslash(const char* src, int* readPtr) {
    const char* p = src+1;
    char result;
    int count = 2;

    switch (*p) {
	/*
	 * Note: in the conversions below, use absolute values (e.g.,
	 * 0xa) rather than symbolic values (e.g. \n) that get converted
	 * by the compiler.  It's possible that compilers on some
	 * platforms will do the symbolic conversions differently, which
	 * could result in non-portable Tcl scripts.
	 */
	case 'a': result = 0x7; break;
	case 'b': result = 0x8; break;
	case 'f': result = 0xc; break;
	case 'n': result = 0xa; break;
	case 'r': result = 0xd; break;
	case 't': result = 0x9; break;
	case 'v': result = 0xb; break;
	case 'x':
	    if (isxdigit(p[1])) {
		char* end;

		result = (char)strtoul(p+1, &end, 16);
		count = end - src;
	    } else {
		count = 2;
		result = 'x';
	    }
	    break;
	case '\n':
	    do {
		p++;
	    } while ((*p == ' ') || (*p == '\t'));
	    result = ' ';
	    count = p - src;
	    break;
	case 0:
	    result = '\\';
	    count = 1;
	    break;
	default:
	    /* Check for an octal number \oo?o? */
	    if (isdigit(*p)) {
		result = *p - '0';
		p++;
		if (!isdigit(*p)) break;

		count = 3;
		result = (result << 3) + (*p - '0');
		p++;
		if (!isdigit(*p)) break;

		count = 4;
		result = (result << 3) + (*p - '0');
		break;
	    }
	    result = *p;
	    count = 2;
	    break;
    }
    if (readPtr) *readPtr = count;
    return result;
}


/*----------------------------------------------------------------------
 * tclFindElement -- locate the first (or next) element in the list.
 *
 * Results:
 *  The return value is normally 1 (OK), which means that the
 *  element was successfully located.  If 0 (ERROR) is returned
 *  it means that list didn't have proper tcl list structure
 *  (no detailed error message).
 *
 *  If 1 (OK) is returned, then *elementPtr will be set to point
 *  to the first element of list, and *nextPtr will be set to point
 *  to the character just after any white space following the last
 *  character that's part of the element.  If this is the last argument
 *  in the list, then *nextPtr will point to the NULL character at the
 *  end of list.  If sizePtr is non-NULL, *sizePtr is filled in with
 *  the number of characters in the element.  If the element is in
 *  braces, then *elementPtr will point to the character after the
 *  opening brace and *sizePtr will not include either of the braces.
 *  If there isn't an element in the list, *sizePtr will be zero, and
 *  both *elementPtr and *termPtr will refer to the null character at
 *  the end of list.  Note:  this procedure does NOT collapse backslash
 *  sequences.
 *----------------------------------------------------------------------
 */
static int tclFindElement(const char* list, const char** elementPtr, 
			  const char** nextPtr, int* sizePtr, int *bracePtr) {
    register const char *p;
    int openBraces = 0;
    int inQuotes = 0;
    int size;

    /*
     * Skim off leading white space and check for an opening brace or
     * quote.
     */
    while (isspace(*list)) list++;

    if (*list == '{') {			/* } */
	openBraces = 1;
	list++;
    } else if (*list == '"') {
	inQuotes = 1;
	list++;
    }
    if (bracePtr) *bracePtr = openBraces;

    /*
     * Find the end of the element (either a space or a close brace or
     * the end of the string).
     */
    for (p=list; 1; p++) {
	switch (*p) {

	    /*
	     * Open brace: don't treat specially unless the element is
	     * in braces.  In this case, keep a nesting count.
	     */
	    case '{':
		if (openBraces) openBraces++;
		break;

	    /*
	     * Close brace: if element is in braces, keep nesting
	     * count and quit when the last close brace is seen.
	     */
	    case '}':
		if (openBraces == 1) {
		    size = p - list;
		    p++;
		    if (isspace(*p) || (*p == 0)) goto done;

		    /* list element in braces followed by garbage instead of
		     * space
		     */
		    return 0/*ERROR*/;
		} else if (openBraces) {
		    openBraces--;
		}
		break;

	    /*
	     * Backslash:  skip over everything up to the end of the
	     * backslash sequence.
	     */
	    case '\\': {
		int siz;

		tclBackslash(p, &siz);
		p += siz - 1;
		break;
	    }

	    /*
	     * Space: ignore if element is in braces or quotes;  otherwise
	     * terminate element.
	     */
	    case ' ':
	    case '\f':
	    case '\n':
	    case '\r':
	    case '\t':
	    case '\v':
		if ((openBraces == 0) && !inQuotes) {
		    size = p - list;
		    goto done;
		}
		break;

	    /*
	     * Double-quote:  if element is in quotes then terminate it.
	     */
	    case '"':
		if (inQuotes) {
		    size = p-list;
		    p++;
		    if (isspace(*p) || (*p == 0)) goto done;

		    /* list element in quotes followed by garbage instead of
		     * space
		     */
		    return 0/*ERROR*/;
		}
		break;

	    /*
	     * End of list:  terminate element.
	     */
	    case 0:
		if (openBraces || inQuotes) {
		    /* unmatched open brace or quote in list */
		    return 0/*ERROR*/;
		}
		size = p - list;
		goto done;
	}
    }

  done:
    while (isspace(*p)) p++;

    *elementPtr = list;
    *nextPtr = p;
    if (sizePtr) *sizePtr = size;
    return 1/*OK*/;
}


/*----------------------------------------------------------------------
 * tclCopyAndCollapse -- Copy a string and eliminate any backslashes that
 *			 aren't in braces.
 *
 * Results:
 *  Count characters get copied from src to dst. Along the way, if
 *  backslash sequences are found outside braces, the backslashes are
 *  eliminated in the copy. After scanning count chars from source, a
 *  null character is placed at the end of dst.
 *----------------------------------------------------------------------
 */
static void tclCopyAndCollapse(int count, const char *src, char *dst)
{
    register char c;
    int numRead;

    for (c = *src; count > 0; src++, c = *src, count--) {
	if (c == '\\') {
	    *dst = tclBackslash(src, &numRead);
	    dst++;
	    src += numRead-1;
	    count -= numRead-1;
	} else {
	    *dst = c;
	    dst++;
	}
    }
    *dst = 0;
}


/* ----------------------------------------------------------------------------
 * zSplitTclList - Splits a list up into its constituent fields.
 *
 * Results:
 *	The return value is a pointer to a list of element points,
 *	which means that the list was successfully split up.
 *	If NULL is returned, it means that "list" didn't have proper tcl list
 *	structure (we don't return an error message about the details).
 *
 *	This procedure does allocate a single memory block
 *	by calling malloc to store both, the the argv pointer array and
 *	the extracted list elements.  The returned element
 *	pointer array must be freed by calling free_2().
 *
 *	*argcPtr will get filled in with the number of valid elements
 *	in the array.  Note: *argcPtr is only modified if the procedure
 *	returns not NULL.
 * ----------------------------------------------------------------------------
 */
static char** zSplitTclList(const char* list, int* argcPtr) {
    char** argv;
    const char* l;
    register char* p;
    int size, i, ok, elSize, brace;
    const char *element;

    /*
     * Figure out how much space to allocate.  There must be enough
     * space for both the array of pointers and also for a copy of
     * the list.  To estimate the number of pointers needed, count
     * the number of space characters in the list.
     */
    for (size = 1, l = list; *l != 0; l++) {
	if (isspace(*l)) size++;
    }
    size++;				/* Leave space for final NULL */

    i = (size * sizeof(char*)) + (l - list) + 1;
    argv = malloc_2(i);

    for (i = 0, p = (char*) (argv+size); *list != 0;  i++) {
	ok = tclFindElement(list, &element, &list, &elSize, &brace);

	if (!ok) {
	    free_2(argv);
	    return NULL;
	}
	if (*element == 0) break;

	if (i >= size) {
	    free_2(argv);
	    /* internal error in zSplitTclList */
	    return NULL;
	}
	argv[i] = p;
	if (brace) {
	    strncpy(p, element, elSize);
	    p += elSize;
	    *p = 0;
	    p++;
	} else {
	    tclCopyAndCollapse(elSize, element, p);
	    p += elSize+1;
	}
    }
    argv[i] = NULL;
    *argcPtr = i;
    return argv;
}


/*----------------------------------------------------------------------
 * tclScanElement -- scan a tcl list string to see what needs to be done.
 *
 *  This procedure is a companion procedure to tclConvertElement.
 *
 * Results:
 *  The return value is an overestimate of the number of characters
 *  that will be needed by tclConvertElement to produce a valid
 *  list element from string.  The word at *flagPtr is filled in
 *  with a value needed by tclConvertElement when doing the actual
 *  conversion.
 *
 *
 * This procedure and tclConvertElement together do two things:
 *
 * 1. They produce a proper list, one that will yield back the
 * argument strings when evaluated or when disassembled with
 * zSplitTclList.  This is the most important thing.
 * 
 * 2. They try to produce legible output, which means minimizing the
 * use of backslashes (using braces instead).  However, there are
 * some situations where backslashes must be used (e.g. an element
 * like "{abc": the leading brace will have to be backslashed.  For
 * each element, one of three things must be done:
 *
 * (a) Use the element as-is (it doesn't contain anything special
 * characters).  This is the most desirable option.
 *
 * (b) Enclose the element in braces, but leave the contents alone.
 * This happens if the element contains embedded space, or if it
 * contains characters with special interpretation ($, [, ;, or \),
 * or if it starts with a brace or double-quote, or if there are
 * no characters in the element.
 *
 * (c) Don't enclose the element in braces, but add backslashes to
 * prevent special interpretation of special characters.  This is a
 * last resort used when the argument would normally fall under case
 * (b) but contains unmatched braces.  It also occurs if the last
 * character of the argument is a backslash or if the element contains
 * a backslash followed by newline.
 *
 * The procedure figures out how many bytes will be needed to store
 * the result (actually, it overestimates).  It also collects information
 * about the element in the form of a flags word.
 *----------------------------------------------------------------------
 */
#define DONT_USE_BRACES  1
#define USE_BRACES       2
#define BRACES_UNMATCHED 4

static int tclScanElement(const char* string, int* flagPtr) {
    register const char *p;
    int nestingLevel = 0;
    int flags = 0;

    if (string == NULL) string = "";

    p = string;
    if ((*p == '{') || (*p == '"') || (*p == 0)) {	/* } */
	flags |= USE_BRACES;
    }
    for ( ; *p != 0; p++) {
	switch (*p) {
	    case '{':
		nestingLevel++;
		break;
	    case '}':
		nestingLevel--;
		if (nestingLevel < 0) {
		    flags |= DONT_USE_BRACES | BRACES_UNMATCHED;
		}
		break;
	    case '[':
	    case '$':
	    case ';':
	    case ' ':
	    case '\f':
	    case '\r':
	    case '\t':
	    case '\v':
		flags |= USE_BRACES;
		break;
	    case '\n':		/* lld: dont want line breaks inside a list */
		flags |= DONT_USE_BRACES;
		break;
	    case '\\':
		if ((p[1] == 0) || (p[1] == '\n')) {
		    flags = DONT_USE_BRACES | BRACES_UNMATCHED;
		} else {
		    int size;

		    tclBackslash(p, &size);
		    p += size-1;
		    flags |= USE_BRACES;
		}
		break;
	}
    }
    if (nestingLevel != 0) {
	flags = DONT_USE_BRACES | BRACES_UNMATCHED;
    }
    *flagPtr = flags;

    /* Allow enough space to backslash every character plus leave
     * two spaces for braces.
     */
    return 2*(p-string) + 2;
}


/*----------------------------------------------------------------------
 *
 * tclConvertElement - convert a string into a list element
 *
 *  This is a companion procedure to tclScanElement.  Given the
 *  information produced by tclScanElement, this procedure converts
 *  a string to a list element equal to that string.
 *
 *  See the comment block at tclScanElement above for details of how this
 *  works.
 *
 * Results:
 *  Information is copied to *dst in the form of a list element
 *  identical to src (i.e. if zSplitTclList is applied to dst it
 *  will produce a string identical to src).  The return value is
 *  a count of the number of characters copied (not including the
 *  terminating NULL character).
 *----------------------------------------------------------------------
 */
static int tclConvertElement(const char* src, char* dst, int flags)
{
    register char *p = dst;

    if ((src == NULL) || (*src == 0)) {
	p[0] = '{';
	p[1] = '}';
	p[2] = 0;
	return 2;
    }
    if ((flags & USE_BRACES) && !(flags & DONT_USE_BRACES)) {
	*p = '{';
	p++;
	for ( ; *src != 0; src++, p++) {
	    *p = *src;
	}
	*p = '}';
	p++;
    } else {
	if (*src == '{') {		/* } */
	    /* Can't have a leading brace unless the whole element is
	     * enclosed in braces.  Add a backslash before the brace.
	     * Furthermore, this may destroy the balance between open
	     * and close braces, so set BRACES_UNMATCHED.
	     */
	    p[0] = '\\';
	    p[1] = '{';			/* } */
	    p += 2;
	    src++;
	    flags |= BRACES_UNMATCHED;
	}
	for (; *src != 0 ; src++) {
	    switch (*src) {
		case ']':
		case '[':
		case '$':
		case ';':
		case ' ':
		case '\\':
		case '"':
		    *p = '\\';
		    p++;
		    break;
		case '{':
		case '}':
		    /* It may not seem necessary to backslash braces, but
		     * it is.  The reason for this is that the resulting
		     * list element may actually be an element of a sub-list
		     * enclosed in braces, so there may be a brace mismatch
		     * if the braces aren't backslashed.
		     */
		    if (flags & BRACES_UNMATCHED) {
			*p = '\\';
			p++;
		    }
		    break;
		case '\f':
		    *p = '\\';
		    p++;
		    *p = 'f';
		    p++;
		    continue;
		case '\n':
		    *p = '\\';
		    p++;
		    *p = 'n';
		    p++;
		    continue;
		case '\r':
		    *p = '\\';
		    p++;
		    *p = 'r';
		    p++;
		    continue;
		case '\t':
		    *p = '\\';
		    p++;
		    *p = 't';
		    p++;
		    continue;
		case '\v':
		    *p = '\\';
		    p++;
		    *p = 'v';
		    p++;
		    continue;
	    }
	    *p = *src;
	    p++;
	}
    }
    *p = '\0';
    return p-dst;
}


/* ============================================================================
 * zMergeTclList - Creates a tcl list from a set of element strings.
 *
 *	Given a collection of strings, merge them together into a
 *	single string that has proper Tcl list structured (i.e.
 *	zSplitTclList may be used to retrieve strings equal to the
 *	original elements).
 *	The merged list is stored in dynamically-allocated memory.
 *
 * Results:
 *      The return value is the address of a dynamically-allocated string.
 * ============================================================================
 */
static char* zMergeTclList(int argc, const char** argv) {
    enum  {LOCAL_SIZE = 20};
    int   localFlags[LOCAL_SIZE];
    int*  flagPtr;
    int   numChars;
    int   i;
    char* result;
    char* dst;

    /* Pass 1: estimate space, gather flags */
    if (argc <= LOCAL_SIZE) flagPtr = localFlags;
    else                    flagPtr = malloc_2(argc*sizeof(int));
    numChars = 1;

    for (i=0; i<argc; i++) numChars += tclScanElement(argv[i], &flagPtr[i]) + 1;

    result = malloc_2(numChars);

    /* Pass two: copy into the result area */
    dst = result;
    for (i = 0; i < argc; i++) {
	numChars = tclConvertElement(argv[i], dst, flagPtr[i]);
	dst += numChars;
	*dst = ' ';
	dst++;
    }
    if (dst == result) *dst = 0;
    else                dst[-1] = 0;

    if (flagPtr != localFlags) free_2(flagPtr);
    return result;
}


/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */
/* XXX functions for data coming into gtkwave XXX */
/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */


/* ----------------------------------------------------------------------------
 * determine_trace_from_y - finds trace under the marker
 *
 * Results:
 *      Returns Trptr which corresponds to the mouse pointer y position.
 * ----------------------------------------------------------------------------
 */     

static Trptr determine_trace_from_y(void)
{
Trptr t;
int trwhich, trtarget;
GdkModifierType state;
gdouble x, y;
#ifdef WAVE_USE_GTK2
gint xi, yi;
#else
GdkEventMotion event[1];
event[0].deviceid = GDK_CORE_POINTER;
#endif


if(GLOBALS->dnd_tgt_on_signalarea_treesearch_gtk2_c_1)
	{
	WAVE_GDK_GET_POINTER(GLOBALS->signalarea->window, &x, &y, &xi, &yi, &state);
	WAVE_GDK_GET_POINTER_COPY;

	if((x<0)||(y<0)||(x>=GLOBALS->signalarea->allocation.width)||(y>=GLOBALS->signalarea->allocation.height)) return(NULL);
	}
else
if(GLOBALS->dnd_tgt_on_wavearea_treesearch_gtk2_c_1)
	{
	WAVE_GDK_GET_POINTER(GLOBALS->wavearea->window, &x, &y, &xi, &yi, &state);
	WAVE_GDK_GET_POINTER_COPY;

	if((x<0)||(y<0)||(x>=GLOBALS->wavearea->allocation.width)||(y>=GLOBALS->wavearea->allocation.height)) return(NULL);
	}
else
	{
	return(NULL);
	}

if((t=GLOBALS->traces.first))
        {       
        while(t)
                {
                t->flags&=~TR_HIGHLIGHT;
                t=t->t_next;
                }
        signalarea_configure_event(GLOBALS->signalarea, NULL);
        wavearea_configure_event(GLOBALS->wavearea, NULL);
	}

trtarget = ((int)y / (int)GLOBALS->fontheight) - 2; 
if(trtarget < 0) 
	{
	return(NULL);
	}
	else
	{
	t=GLOBALS->topmost_trace;
	}

trwhich=0;
while(t)
	{
        if((trwhich<trtarget)&&(GiveNextTrace(t)))
        	{
                trwhich++;
                t=GiveNextTrace(t);
                }
                else
                {
                break;
                }
	}

return(t);
}


/* ----------------------------------------------------------------------------
 * check_gtkwave_directive_from_tcl_list - parses tcl list for any gtkwave 
 * directives
 *
 * Results:
 *      Returns decomposed list or NULL if not applicable.  Number of items
 *      is passed back through pointer in l.
 * ----------------------------------------------------------------------------
 */

static char **check_gtkwave_directive_from_tcl_list(char *s, int *l)
{
char** elem = NULL; 
                         
elem = zSplitTclList(s, l);
                 
if(elem)  
        {
        if(strcmp("gtkwave", elem[0]))
		{
		free_2(elem);
		elem = NULL;
		}
	}
return(elem);
}


/* ----------------------------------------------------------------------------
 * make_net_name_from_tcl_list - creates gtkwave-style name from tcl list entry
 *
 * Results:
 *      Returns suitable name or NULL if not applicable.
 * ----------------------------------------------------------------------------
 */     

static char *make_net_name_from_tcl_list(char *s, char **unescaped_str)
{
char *s_new = NULL;
char *s_new2 = NULL;
int len;
int i,l;
char** elem = NULL;
char *pnt, *pnt2;
int esc = 0;

elem = zSplitTclList(s, &l);

if(elem)
	{
	if((!strcmp("net", elem[0])) || (!strcmp("netBus", elem[0])))
		{
		char delim_str[2] = { GLOBALS->hier_delimeter, 0 };

		len = 0;
		for(i=1;i<l;i++)
			{
			len += (strlen(elem[i])) + 1;
			}

		s_new = calloc_2(1, len);
		for(i=1;i<l;i++)
			{
			strcat(s_new, elem[i]);
			if(i!=(l-1)) strcat(s_new, delim_str);
			}

		free_2(elem);
		}
		else
		{
		free_2(elem);
		return(NULL);
		}

	pnt = s_new;
	while(*pnt)
		{
		if( (!isalnum(*pnt)) && (!isspace(*pnt)) && (*pnt != GLOBALS->hier_delimeter) )
			{
			esc++;
			}
		pnt++;
		}

	if(esc)
		{
		s_new2 = calloc_2(1, len + esc);
		pnt = s_new;
		pnt2 = s_new2;
		while(*pnt)
			{
			if( (!isalnum(*pnt)) && (!isspace(*pnt)) && (*pnt != GLOBALS->hier_delimeter) )
				{
				*(pnt2++) = '\\';
				}

			*(pnt2++) = *(pnt++);
			}
		*unescaped_str = s_new;
		/* free_2(s_new); */
		s_new = s_new2;				
		}
		else
		{
		*unescaped_str = s_new;
		}
	}

return(s_new);
}


/* ----------------------------------------------------------------------------
 * process_tcl_list - given a tcl list, inserts traces into viewer
 *
 * Results:
 *      Inserts traces if found in dumpfile, returns number of traces inserted
 * ----------------------------------------------------------------------------
 */     

int process_tcl_list(char *s)
{
char *s_new = NULL;
char *this_regex = "\\(\\[.*\\]\\)*$";
char *entry_suffixed;
int c, i, ii;
char **list;
char **s_new_list;
char **most_recent_lbrack_list;
int *match_idx_list;
int *match_type_list;
Trptr t = NULL;
int found = 0;
int lbrack_adj;
int net_processing_is_off = 0;
int unesc_len;
int curr_srch_idx = 0;
char *unescaped_str = NULL;

list = zSplitTclList(s, &c);
if(!list)
	{
	return(0);
	}
s_new_list = calloc_2(c, sizeof(char *));
match_idx_list = calloc_2(c, sizeof(int *));
match_type_list = calloc_2(c, sizeof(int *));
most_recent_lbrack_list = calloc_2(c, sizeof(char *));

GLOBALS->default_flags=TR_RJUSTIFY;

for(ii=0;ii<c;ii++)
	{
	s_new = make_net_name_from_tcl_list(list[ii], &unescaped_str);
	if(s_new)
		{
		if(net_processing_is_off) continue;
		}
	else
		{
		int ngl;
		char **gdirect = check_gtkwave_directive_from_tcl_list(list[ii], &ngl);
		if(gdirect)
			{
			switch(ngl)
				{
				case 3:
				 	if(!strcmp(gdirect[1], "PID"))
						{
						pid_t pid = atoi(gdirect[2]);
						if(pid == getpid())	/* block self-drags in XEmbed */
							{
							free_2(gdirect);
							goto cleanup;
							}
						}
					 else if(!strcmp(gdirect[1], "NET"))
						{
						net_processing_is_off = !strcmp(gdirect[2], "OFF");
						}
					 else if(!strcmp(gdirect[1], "SAVELIST"))
						{
						int is;
						for(is = 0; is < 4; is++)
							{
							char *pnt = gdirect[2];
							char *nxt_hd = pnt;

							if(is == 1)
								{
								if(found)
									{
							                if(GLOBALS->is_lx2)
										{
										lx2_import_masked();
										}

									t = determine_trace_from_y();
									if(t)
										{
										t->flags |=  TR_HIGHLIGHT;
										}
	
									memcpy(&GLOBALS->tcache_treesearch_gtk2_c_2,&GLOBALS->traces,sizeof(Traces));
									GLOBALS->traces.total=0;
									GLOBALS->traces.first=GLOBALS->traces.last=NULL;
	
									continue;
									}
									else
									{
									goto cleanup;
									}
								}
							else
							if(is==3)
								{
								goto paste_routine;
								}
							else /* (is == 0) or (is == 2) */
							for(;;)
								{
								if(*pnt == 0)
									{
									if(!(*nxt_hd))
										{
										break;
										}
	
									if((!is)&&(GLOBALS->is_lx2)) { parsewavline_lx2(nxt_hd, 0); found++; } else { parsewavline(nxt_hd, 0); }
									break;
									}
								else
								if(*pnt == '\n')
									{
									*pnt = 0;
									if((!is)&&(GLOBALS->is_lx2)) { parsewavline_lx2(nxt_hd, 0); found++; } else { parsewavline(nxt_hd, 0); }
									*pnt = '\n';
									nxt_hd = pnt+1;
									pnt++;
									}
								else
									{
									pnt++;
									}
								}
							}
						}
					 break;

				default: break;
				}

			free_2(gdirect);
			}

		continue;
		}
	s_new_list[ii] = s_new;

	lbrack_adj = 0;
	most_recent_lbrack_list[ii] = strrchr(s_new, '[');
	if((most_recent_lbrack_list[ii])&&(most_recent_lbrack_list[ii] != s_new))
		{
		char *chp = most_recent_lbrack_list[ii]-1;
		if(*chp == '\\')
			{
			most_recent_lbrack_list[ii] = chp;
			lbrack_adj = 1;
			}
		}


	unesc_len = strlen(unescaped_str);
	for(i=0;i<GLOBALS->numfacs;i++)
	        {
	        int was_packed;
	        char *hfacname = NULL;
	                                 
       		hfacname = hier_decompress_flagged(GLOBALS->facs[curr_srch_idx]->name, &was_packed);

	        if(!strncmp(unescaped_str, hfacname, unesc_len)) 
	                {
			int hfacname_len = strlen(hfacname);
			if((unesc_len == hfacname_len) || ((hfacname_len > unesc_len) && (hfacname[unesc_len] == '[')))
				{
				found++;
				match_idx_list[ii] = curr_srch_idx;
				match_type_list[ii] = 1; /* match was on normal search */
			        if(was_packed) { free_2(hfacname); }
				if(s_new != unescaped_str) { free_2(unescaped_str); }
				goto import;
		                }
			}

		curr_srch_idx++;
		if(curr_srch_idx == GLOBALS->numfacs) curr_srch_idx = 0; /* optimization for rtlbrowse as names should be in order */
		        
	        if(was_packed) { free_2(hfacname); }
	        }

	if(s_new != unescaped_str) { free_2(unescaped_str); }

	entry_suffixed=wave_alloca(2+strlen(s_new)+strlen(this_regex)+1);
	*entry_suffixed=0x00;
	strcpy(entry_suffixed, "\\<");
	strcat(entry_suffixed,s_new);
	strcat(entry_suffixed,this_regex);

	wave_regex_compile(entry_suffixed, WAVE_REGEX_DND); 
	for(i=0;i<GLOBALS->numfacs;i++)
	        {
	        int was_packed;
	        char *hfacname = NULL;
	                                 
       		hfacname = hier_decompress_flagged(GLOBALS->facs[i]->name, &was_packed);

	        if(wave_regex_match(hfacname, WAVE_REGEX_DND)) 
	                {
			found++;
			match_idx_list[ii] = i;
			match_type_list[ii] = 1; /* match was on normal search */
		        if(was_packed) { free_2(hfacname); }
			goto import;
	                }
		        
	        if(was_packed) { free_2(hfacname); }
	        }

	if(most_recent_lbrack_list[ii])
		{
		*most_recent_lbrack_list[ii] = 0;

		entry_suffixed=wave_alloca(2+strlen(s_new)+strlen(this_regex)+1);
		*entry_suffixed=0x00;
		strcpy(entry_suffixed, "\\<");
		strcat(entry_suffixed,s_new);
		strcat(entry_suffixed,this_regex);

		wave_regex_compile(entry_suffixed, WAVE_REGEX_DND); 
		for(i=0;i<GLOBALS->numfacs;i++)
		        {
		        int was_packed;
		        char *hfacname = NULL;
		                                 
        		hfacname = hier_decompress_flagged(GLOBALS->facs[i]->name, &was_packed);
	
		        if(wave_regex_match(hfacname, WAVE_REGEX_DND)) 
		                {
				found++;
				match_idx_list[ii] = i;
				match_type_list[ii] = 2+lbrack_adj; /* match was on lbrack removal */
			        if(was_packed) { free_2(hfacname); }
				goto import;
		                }
			        
		        if(was_packed) { free_2(hfacname); }
		        }
		}

	import:
	if(match_type_list[ii])
		{
		struct symbol *s = GLOBALS->facs[match_idx_list[ii]];

		if(GLOBALS->is_lx2)
			{
                        lx2_set_fac_process_mask(s->n);
			}
		}
	}

if(!found) goto cleanup;

if(GLOBALS->is_lx2)
	{
	lx2_import_masked();
	}

t = determine_trace_from_y();
if(t)
	{
	t->flags |=  TR_HIGHLIGHT;
	}

memcpy(&GLOBALS->tcache_treesearch_gtk2_c_2,&GLOBALS->traces,sizeof(Traces));
GLOBALS->traces.total=0;
GLOBALS->traces.first=GLOBALS->traces.last=NULL;

for(ii=0;ii<c;ii++)
	{
	if(match_type_list[ii])
		{
		struct symbol *s = GLOBALS->facs[match_idx_list[ii]];

		if((match_type_list[ii] >= 2)&&(s->n->ext))
			{
			nptr nexp;
			int bit = atoi(most_recent_lbrack_list[ii]+1 + (match_type_list[ii] == 3)); /* == 3 for adjustment when lbrack is escaped */
			int which, cnt;
			
			if(s->n->ext->lsi > s->n->ext->msi)
				{
				for(which=0,cnt=s->n->ext->lsi ; cnt>=s->n->ext->msi ; cnt--,which++)
					{
					if(cnt==bit) break;
					}
				}
				else
				{
				for(which=0,cnt=s->n->ext->msi ; cnt>=s->n->ext->lsi ; cnt--,which++)
					{
					if(cnt==bit) break;
					}
				}

			nexp = ExtractNodeSingleBit(s->n, which);
			*most_recent_lbrack_list[ii] = '[';
	                if(nexp)
	                        {
	                        AddNode(nexp, NULL);
	                        }
				else
				{
				AddNodeUnroll(s->n, NULL);
				}
			}
			else
			{
			AddNodeUnroll(s->n, NULL);
			}
		}
	}

paste_routine:

GLOBALS->default_flags=TR_RJUSTIFY;

GLOBALS->traces.buffercount=GLOBALS->traces.total;
GLOBALS->traces.buffer=GLOBALS->traces.first;
GLOBALS->traces.bufferlast=GLOBALS->traces.last;
GLOBALS->traces.first=GLOBALS->tcache_treesearch_gtk2_c_2.first;
GLOBALS->traces.last=GLOBALS->tcache_treesearch_gtk2_c_2.last;
GLOBALS->traces.total=GLOBALS->tcache_treesearch_gtk2_c_2.total;
                                
if(t)
	{
	PasteBuffer();
	}
	else
	{	
	PrependBuffer();
	}

GLOBALS->traces.buffercount=GLOBALS->tcache_treesearch_gtk2_c_2.buffercount;
GLOBALS->traces.buffer=GLOBALS->tcache_treesearch_gtk2_c_2.buffer;
GLOBALS->traces.bufferlast=GLOBALS->tcache_treesearch_gtk2_c_2.bufferlast;

if(t)
	{
	t->flags &= ~TR_HIGHLIGHT;
	}

cleanup:
for(ii=0;ii<c;ii++)
	{
	if(s_new_list[ii]) free_2(s_new_list[ii]);
	}
free_2(s_new_list);
free_2(match_idx_list);
free_2(match_type_list);
free_2(most_recent_lbrack_list);
free_2(list);

return(found);
}


/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */
/* XXX functions for data exiting from gtkwave XXX */
/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */


/* ----------------------------------------------------------------------------
 * make_gtkwave_pid - generates gtkwave pid (necessary when using twinwave as
 * the XEmbed protocol seems to be dropping the source widget for drags which
 * causes drops to occur twice), this should only need to be called by
 * add_dnd_from_signal_window().
 *
 * Results:
 *      generated tcl list string containing gtkwave PID for drop filtering
 * ----------------------------------------------------------------------------
 */

static char *make_gtkwave_pid(void)
{
#if !defined __MINGW32__ && !defined _MSC_VER
char pidstr[128];

sprintf(pidstr, "{gtkwave PID %d} ", getpid());

return(strdup_2(pidstr));
#else
return(NULL);
#endif
}


/* ----------------------------------------------------------------------------
 * make_single_tcl_list_name - generates tcl name from a gtkwave one
 *
 * Results:
 *      generated tcl list string
 * ----------------------------------------------------------------------------
 */

static char *make_single_tcl_list_name(char *s)
{
char *rpnt = NULL;
char *pnt, *pnt2;
int delim_cnt = 0;
char *lbrack=NULL, *colon=NULL, *rbrack=NULL;
const char **names = NULL;
char *tcllist = NULL;
int tcllist_len;
int names_idx = 0;
char is_bus = 0;

if(s)
	{
	int len = strlen(s);
	char *s2 = wave_alloca(len+1);
	
	strcpy(s2, s);
	pnt = s2;
	while(*pnt)
		{
		if(*pnt == GLOBALS->hier_delimeter)
			{
			delim_cnt++;
			}
		else if(*pnt == '[') { lbrack = pnt; }
		else if(*pnt == ':') { colon  = pnt; }
		else if(*pnt == ']') { rbrack = pnt; }

		pnt++;
		}

	if(lbrack && colon && rbrack && ((colon-lbrack)>0) && ((rbrack - colon)>0) && ((rbrack-lbrack)>0))
		{
		is_bus = 1;
		*lbrack = 0;
		len = lbrack - s2;
		}

	names = calloc_2(delim_cnt+1, sizeof(char *));
	pnt = s2;
	names[0] = pnt;
	while(*pnt)
		{
		if(*pnt == GLOBALS->hier_delimeter)
			{
			*pnt = 0;
			names_idx++;
			pnt++;
			if(*pnt) { names[names_idx] = pnt; }
			}
			else
			{
			pnt++;
			}
		}

	
	tcllist = zMergeTclList(delim_cnt+1, names);
	tcllist_len = strlen(tcllist);
	free_2(names);

	if(is_bus)
		{
		len = 8 + strlen(tcllist) + 1 + 1 + 1; /* "{netBus ...} " + trailing null char */

		pnt = s2;
		rpnt = malloc_2(len+1);
		strcpy(rpnt, "{netBus ");
		pnt2 = rpnt + 8;
		}
		else
		{
		len = 5 + strlen(tcllist) + 1 + 1 + 1; /* "{net ...} " + trailing null char */

		pnt = s2;
		rpnt = malloc_2(len+1);
		strcpy(rpnt, "{net ");
		pnt2 = rpnt + 5;
		}

	strcpy(pnt2, tcllist);
	strcpy(pnt2 + tcllist_len, "} ");

	free_2(tcllist);
	}

return(rpnt);
}


/* ----------------------------------------------------------------------------
 * add_dnd_from_searchbox - generates tcl names from selected searchbox ones
 *
 * Results:
 *      tcl list containing all generated names
 * ----------------------------------------------------------------------------
 */

char *add_dnd_from_searchbox(void)
{
int i;
char *one_entry = NULL, *mult_entry = NULL;
unsigned int mult_len = 0;

for(i=0;i<GLOBALS->num_rows_search_c_2;i++)
        {
        int len;
        struct symbol *s, *t;
        s=(struct symbol *)gtk_clist_get_row_data(GTK_CLIST(GLOBALS->clist_search_c_3), i);
        if(s->selected)
                {
                if((!s->vec_root)||(!GLOBALS->autocoalesce))
                        {
			one_entry = make_single_tcl_list_name(s->n->nname);
			WAVE_OE_ME
                        }
                        else
                        {
                        len=0;
                        t=s->vec_root;
                        while(t)
                                {
				one_entry = make_single_tcl_list_name(t->n->nname);
				WAVE_OE_ME

                                if(t->selected)
                                        {
                                        if(len) t->selected=0;
                                        }
                                len++;
                                t=t->vec_chain;
                                }
                        }
                }
        }
return(mult_entry);
}


/* ----------------------------------------------------------------------------
 * add_dnd_from_signal_window - generates tcl names from selected sigwin ones
 *
 * Results:
 *      tcl list containing all generated names
 * ----------------------------------------------------------------------------
 */

char *add_dnd_from_signal_window(void)
{
Trptr t;
char *one_entry = NULL, *mult_entry = NULL;
unsigned int mult_len = 0;
char *netoff = "{gtkwave NET OFF} ";

t=GLOBALS->traces.first;
while(t)
	{
        if( (!(t->flags&(TR_BLANK|TR_ANALOG_BLANK_STRETCH))) && (t->flags & TR_HIGHLIGHT) )
		{
                if(t->vector)
                        {
                        int i;
                        nptr *nodes;
                                        
                        nodes=t->n.vec->bits->nodes;
                        for(i=0;i<t->n.vec->nbits;i++)
                                {
                                if(nodes[i]->expansion)
                                        {
		                        int which, cnt;
					int bit = nodes[i]->expansion->parentbit;
					nptr n = nodes[i]->expansion->parent;
					char *str = append_array_row(n);
					char *p = strrchr(str, '[');
					if(p) { *p = 0; }					
              
                        		if(n->ext->lsi > n->ext->msi)
                                		{
                                		for(which=0,cnt=n->ext->lsi ; cnt>=n->ext->msi ; cnt--,which++)
                                        		{
                                        		if(cnt==bit) break;
                                        		}
                                		}
                                		else
                                		{
                                		for(which=0,cnt=n->ext->msi ; cnt>=n->ext->lsi ; cnt--,which++)
                                        		{
                                        		if(cnt==bit) break;
                                        		}
                                		}   

					sprintf(str+strlen(str), "[%d]", which);
					if(!mult_entry) { one_entry = make_gtkwave_pid(); WAVE_OE_ME one_entry = strdup_2(netoff); WAVE_OE_ME }
					one_entry = make_single_tcl_list_name(str);
					WAVE_OE_ME
                                        }
                                        else
                                        {
					if(!mult_entry) { one_entry = make_gtkwave_pid(); WAVE_OE_ME one_entry = strdup_2(netoff); WAVE_OE_ME}
					one_entry = make_single_tcl_list_name(append_array_row(nodes[i]));
					WAVE_OE_ME
                                        }
                                }
                        }
			else
			{
			if(t->n.nd->expansion)
				{
	                        int which, cnt;
				int bit = t->n.nd->expansion->parentbit;
				nptr n = t->n.nd->expansion->parent;
				char *str = append_array_row(n);
				char *p = strrchr(str, '[');
				if(p) { *p = 0; }					
              
                       		if(n->ext->lsi > n->ext->msi)
                               		{
                               		for(which=0,cnt=n->ext->lsi ; cnt>=n->ext->msi ; cnt--,which++)
                                       		{
                                       		if(cnt==bit) break;
                                       		}
                               		}
                               		else
                               		{
                               		for(which=0,cnt=n->ext->msi ; cnt>=n->ext->lsi ; cnt--,which++)
                                       		{
                                       		if(cnt==bit) break;
                                       		}
                               		}   

				sprintf(str+strlen(str), "[%d]", which);
				if(!mult_entry) { one_entry = make_gtkwave_pid(); WAVE_OE_ME one_entry = strdup_2(netoff); WAVE_OE_ME}
				one_entry = make_single_tcl_list_name(str);
				WAVE_OE_ME
				}
				else
				{
				if(!mult_entry) { one_entry = make_gtkwave_pid(); WAVE_OE_ME one_entry = strdup_2(netoff); WAVE_OE_ME}
				one_entry = make_single_tcl_list_name(append_array_row(t->n.nd));
				WAVE_OE_ME
				}
			}
		}
		else
		{
		one_entry = strdup_2(netoff); WAVE_OE_ME
		}
	t = t->t_next;
	}

return(mult_entry);
}


/* ----------------------------------------------------------------------------
 * sig_selection_foreach_dnd - generates tcl names from iterated clist ones
 *
 * Results:
 *      tcl list containing all generated names coalesced back into *data
 * ----------------------------------------------------------------------------
 */

#if WAVE_USE_GTK2
static void
sig_selection_foreach_dnd
                      (GtkTreeModel *model,
                       GtkTreePath *path,
                       GtkTreeIter *iter,
                       gpointer data)
{
  struct tree *sel;
  int i;
  int low, high;
  struct iter_dnd_strings *it;
  char *one_entry, *mult_entry;
  unsigned int mult_len;
  enum { NAME_COLUMN, TREE_COLUMN, N_COLUMNS };

  it = (struct iter_dnd_strings *)data;
  one_entry = it->one_entry;
  mult_entry = it->mult_entry;
  mult_len = it->mult_len;

  /* Get the tree.  */
  gtk_tree_model_get (model, iter, TREE_COLUMN, &sel, -1);
 
  if(!sel) return;

  low = fetchlow(sel)->which;
  high = fetchhigh(sel)->which;
 
  /* If signals are vectors, iterate through them if so.  */
  for(i=low;i<=high;i++)
        {
        struct symbol *s;
        s=GLOBALS->facs[i];
        if((s->vec_root)&&(GLOBALS->autocoalesce))
                {
		struct symbol *t = s->vec_root;
                while(t)
			{
                        one_entry = make_single_tcl_list_name(t->n->nname);
                        WAVE_OE_ME
                        t=t->vec_chain;
                        }
                }
		else
		{		
                one_entry = make_single_tcl_list_name(s->n->nname);
                WAVE_OE_ME
		}
        }

  it->one_entry = one_entry;
  it->mult_entry = mult_entry;
  it->mult_len = mult_len;
}
#endif

/* ----------------------------------------------------------------------------
 * add_dnd_from_tree_window - generates tcl names from selected tree clist ones
 *
 * Results:
 *      tcl list containing all generated names
 * ----------------------------------------------------------------------------
 */

char *add_dnd_from_tree_window(void)
{
#if WAVE_USE_GTK2
struct iter_dnd_strings it;

memset(&it, 0, sizeof(struct iter_dnd_strings));
gtk_tree_selection_selected_foreach(GLOBALS->sig_selection_treesearch_gtk2_c_1, &sig_selection_foreach_dnd, (gpointer)&it);

return(it.mult_entry);
#else
return(NULL);
#endif
}


/* ----------------------------------------------------------------------------
 * make_message - printf() which mallocs into a string
 *
 * Results:
 *      dynamically allocated string
 * ----------------------------------------------------------------------------
 */

static char *make_message (const char *fmt, ...)
{
  /* Guess we need no more than 100 bytes. */
  int n, size = 100;
  char *p, *np;
  va_list ap;

  if ((p = malloc_2(size)) == NULL)
    return NULL;

  while (1)
    {
      /* Try to print in the allocated space. */
      va_start (ap, fmt);
      n = vsnprintf (p, size, fmt, ap);
      va_end (ap);
      /* If that worked, return the string. */
      if (n > -1 && n < size)
	return p;
      /* Else try again with more space. */
      if (n > -1)		/* glibc 2.1 */
	size = n + 1;		/* precisely what is needed */
      else			/* glibc 2.0 */
	size *= 2;		/* twice the old size */
      if ((np = realloc_2(p, size)) == NULL)
	{
	  free (p);
	  return NULL;
	}
      else
	{
	  p = np;
	}
    }
}


/* ----------------------------------------------------------------------------
 * emit_gtkwave_savefile_formatted_entries_in_tcl_list - performs as named
 *
 * Results:
 *      tcl list which mimics a gtkwave save file for cut and paste entries
 *      which is later iteratively run through the normal gtkwave save file 
 *      loader parsewavline() on the distant end.  the reason this is 
 *      necessary is in order to pass attribute and concatenation information
 *      along to the distant end.
 * ----------------------------------------------------------------------------
 */

char *emit_gtkwave_savefile_formatted_entries_in_tcl_list(void) {
	char *one_entry, *mult_entry = NULL;
	unsigned int mult_len = 0;
	unsigned int prev_flags = 0;

	Trptr t;
	unsigned int def=0;
	TimeType prevshift=LLDescriptor(0);
	char is_first = 1;
	char collapsed_state = 0, flag_skip;

	t=GLOBALS->traces.first;
	while(t)
		{
		flag_skip = 0;

		if((t->flags & (TR_HIGHLIGHT|TR_COLLAPSED)) == (TR_HIGHLIGHT|TR_COLLAPSED)) 
			{
			collapsed_state = 1;
			}
		else
		if((t->flags & TR_BLANK) && collapsed_state)
			{
			collapsed_state = 0;
			}
		else
		if(!(t->flags & TR_HIGHLIGHT))
			{
			if((prev_flags & TR_ANALOGMASK) && (t->flags &TR_ANALOG_BLANK_STRETCH))
				{
				flag_skip = 1;
				}
			else
				{
				t = t->t_next;
				continue;
				}
			}

		if((t->flags!=def)||(is_first))
			{
			is_first = 0;
			if((t->flags & TR_PTRANSLATED) && (!t->p_filter)) t->flags &= (~TR_PTRANSLATED);
			if((t->flags & TR_FTRANSLATED) && (!t->f_filter)) t->flags &= (~TR_FTRANSLATED);
			one_entry = make_message("@%x\n",(def=t->flags) & ~TR_HIGHLIGHT);
			WAVE_OE_ME
			if(!flag_skip) prev_flags = def;
			}

		if((t->shift)||((prevshift)&&(!t->shift)))
			{
			one_entry = make_message(">"TTFormat"\n", t->shift);
			WAVE_OE_ME
			}
		prevshift=t->shift;

		if(!(t->flags&(TR_BLANK|TR_ANALOG_BLANK_STRETCH)))	
			{
			if(t->flags & TR_FTRANSLATED)
				{
				if(t->f_filter && GLOBALS->filesel_filter[t->f_filter])
					{
					one_entry = make_message("^%d %s\n", t->f_filter, GLOBALS->filesel_filter[t->f_filter]);
					WAVE_OE_ME
					}
					else
					{
					one_entry = make_message("^%d %s\n", 0, "disabled");
					WAVE_OE_ME
					}
				}
			else
			if(t->flags & TR_PTRANSLATED)
				{
				if(t->p_filter && GLOBALS->procsel_filter[t->p_filter])
					{
					one_entry = make_message("^>%d %s\n", t->p_filter, GLOBALS->procsel_filter[t->p_filter]);
					WAVE_OE_ME
					}
					else
					{
					one_entry = make_message("^>%d %s\n", 0, "disabled");
					WAVE_OE_ME
					}
				}

			if(t->vector)
				{
				int i;
				nptr *nodes;
				bptr bits = t->n.vec->bits;
				baptr ba = bits ? bits->attribs : NULL;

				one_entry = make_message("%c%s", ba ? ':' : '#', t->name);
				WAVE_OE_ME

				nodes=t->n.vec->bits->nodes;
				for(i=0;i<t->n.vec->nbits;i++)
					{
					if(nodes[i]->expansion)
						{
						one_entry = make_message(" (%d)%s",nodes[i]->expansion->parentbit, append_array_row(nodes[i]->expansion->parent));
						WAVE_OE_ME
						}
						else
						{
						one_entry = make_message(" %s",append_array_row(nodes[i]));
						WAVE_OE_ME
						}
					if(ba)
						{
						one_entry = make_message(" "TTFormat" %x", ba[i].shift, ba[i].flags);
						WAVE_OE_ME
						}
					}

				one_entry = make_message("\n");
				WAVE_OE_ME
				}
				else
				{
				if(t->is_alias)
					{
					if(t->n.nd->expansion)
						{
						one_entry = make_message("+%s (%d)%s\n",t->name+2,t->n.nd->expansion->parentbit, append_array_row(t->n.nd->expansion->parent));
						WAVE_OE_ME
						}
						else
						{
						one_entry = make_message("+%s %s\n",t->name+2,append_array_row(t->n.nd));
						WAVE_OE_ME
						}
					}
					else
					{
					if(t->n.nd->expansion)
						{
						one_entry = make_message("(%d)%s\n",t->n.nd->expansion->parentbit, append_array_row(t->n.nd->expansion->parent));
						WAVE_OE_ME
						}
						else
						{
						one_entry = make_message("%s\n",append_array_row(t->n.nd));
						WAVE_OE_ME
						}
					}
				}
			}
			else
			{
			if(!t->name) { one_entry = make_message("-\n"); WAVE_OE_ME }
			else { one_entry = make_message("-%s\n",t->name); WAVE_OE_ME }
			}

		t=t->t_next;
		}

if(mult_entry)
	{
	const char *hdr = "{gtkwave SAVELIST ";
	int hdr_len = strlen(hdr);
	const char *av[1] = { mult_entry };
	char *zm = zMergeTclList(1, av);
	int zm_len = strlen(zm);

	free_2(mult_entry);

	mult_entry = malloc_2(hdr_len + zm_len + 2 + 1);
	memcpy(mult_entry, hdr, hdr_len);
	memcpy(mult_entry + hdr_len, zm, zm_len);
	strcpy(mult_entry + hdr_len + zm_len, "} ");

	free_2(zm);
	}

return(mult_entry);
}


/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */
/* XXX functions for URL (not TCL list) handling XXX */
/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */

/* ----------------------------------------------------------------------------
 * process_url_file - examines filename and performs appropriate side-effect
 *
 * Results:
 *      Loads save file, new dump file, or stems file viewer
 * ----------------------------------------------------------------------------
 */

static int process_url_file(char *s)
{
int rc = 0;
char *pnt = s;
char *dot = NULL, *dot2 = NULL;
int ftype = 0;

while(*pnt)
	{
	if(*pnt == '.') { dot2 = dot; dot = pnt; }
	pnt++;
	}

if(dot)
	{
	if(!strcasecmp("sav", dot+1))
		{
		ftype = 1;
		}
	else
	if(!strcasecmp("stems", dot+1))
		{
		ftype = 2;
		}
	else /* detect dumpfile type */
	if	(
		(!strcasecmp("vcd", dot+1)) ||
		(!strcasecmp("dmp", dot+1)) ||
		(!strcasecmp("lxt", dot+1)) ||
		(!strcasecmp("lx2", dot+1)) ||
		(!strcasecmp("vzt", dot+1)) ||
		(!strcasecmp("ghw", dot+1)) ||
		(!strcasecmp("aet", dot+1)) ||
		(!strcasecmp("ae2", dot+1))
		)
		{
		ftype = 3;
		}
	else
	if(dot2)
		{
		if	(
			(!strcasecmp("ghw.gz", dot2+1))  ||
			(!strcasecmp("ghw.bz2", dot2+1)) ||
			(!strcasecmp("ghw.bz2", dot2+1)) ||
			(!strcasecmp("vcd.gz", dot2+1))  ||
			(!strcasecmp("vcd.zip", dot2+1)) 
			)
			{
			ftype = 3;
			}
		}
	}
	else
	{
	FILE *f = fopen(s, "rb");
	if(f)
		{
		int ch0 = getc(f);
		int ch1 = getc(f);

		if(ch0 == EOF) { ch0 = ch1 = 0; }
		else
		if(ch1 == EOF) { ch1 = 0; }

		if((ch0 == '+') && (ch1 == '+'))
			{
			ftype = 2; /* stems file */
			}
		else 
		if(ch0 == '[')
			{
			ftype = 1; /* save file */
			}

		fclose(f);
		}
	}


switch(ftype)
	{
	case 1:
		GLOBALS->fileselbox_text = &GLOBALS->filesel_writesave;
	        GLOBALS->filesel_ok=1;
        	if(*GLOBALS->fileselbox_text) free_2(*GLOBALS->fileselbox_text);
        	*GLOBALS->fileselbox_text=(char *)strdup_2(s);

		GLOBALS->block_xy_update = 1;
		read_save_helper(s);
		GLOBALS->block_xy_update = 0;
		rc = 1;
		break;

	case 2:
		GLOBALS->fileselbox_text = &GLOBALS->stems_name;
	        GLOBALS->filesel_ok=1;
        	if(*GLOBALS->fileselbox_text) free_2(*GLOBALS->fileselbox_text);
        	*GLOBALS->fileselbox_text=(char *)strdup_2(s);

		menu_read_stems_cleanup(NULL, NULL);
		rc = 1;
		break;

	case 3:
		GLOBALS->fileselbox_text = &GLOBALS->filesel_newviewer_menu_c_1;
	        GLOBALS->filesel_ok=1;
        	if(*GLOBALS->fileselbox_text) free_2(*GLOBALS->fileselbox_text);
        	*GLOBALS->fileselbox_text=(char *)strdup_2(s);

		menu_new_viewer_tab_cleanup(NULL, NULL);
		rc = 1;
		break;

	default:
		break;
	}

return(rc);
}


/* ----------------------------------------------------------------------------
 * process_url_list - examines list of URLs and 
 *
 * Results:
 *      Indicates if any URLs were processed
 * ----------------------------------------------------------------------------
 */

int process_url_list(char *s)
{
int is_url = 0;
#if WAVE_USE_GTK2
char pch = 0;

char *nxt_hd = s;
char *pnt = s;
char *path;

if(*pnt == '{') return(0); /* exit early if tcl list */

for(;;)
	{
	if(*pnt == 0)
		{
		if(!(*nxt_hd))
			{
			break;
			}

		path = g_filename_from_uri(nxt_hd, NULL, NULL);
		if(path) { is_url += process_url_file(path); }
		break;
		}
	else
	if((*pnt == '\n')||(*pnt == '\r'))
		{
		if((pch != '\n') && (pch != '\r'))
			{
			char sav = *pnt;
			*pnt = 0;

			path = g_filename_from_uri(nxt_hd, NULL, NULL);
			if(path) { is_url += process_url_file(path); }
			*pnt = sav;
			}
		pch = *pnt;
		nxt_hd = pnt+1;
		pnt++;
		}
	else
		{
		pch = *pnt;
		pnt++;
		}
	}

#endif
return(is_url);
}

/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */
/* XXX functions for embedding TCL interpreter XXX */
/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */

#if HAVE_TCL_H

static int gtkwavetcl_nop(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
/* nothing, this is simply to call gtk's main loop */
gtkwave_gtk_main_iteration();
return(TCL_OK);
}

static int gtkwavetcl_printInteger(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[], int intVal)
{
Tcl_Obj *aobj;
char reportString[33];

sprintf(reportString, "%d", intVal);

aobj = Tcl_NewStringObj(reportString, -1); 
Tcl_SetObjResult(interp, aobj);

return(TCL_OK);
}

static int gtkwavetcl_printTimeType(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[], TimeType ttVal)
{
Tcl_Obj *aobj;
char reportString[65];

sprintf(reportString, TTFormat, ttVal);

aobj = Tcl_NewStringObj(reportString, -1); 
Tcl_SetObjResult(interp, aobj);

return(TCL_OK);
}

static int gtkwavetcl_printDouble(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[], double dVal)
{
Tcl_Obj *aobj;
char reportString[65];

sprintf(reportString, "%e", dVal);

aobj = Tcl_NewStringObj(reportString, -1); 
Tcl_SetObjResult(interp, aobj);

return(TCL_OK);
}

static int gtkwavetcl_printString(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[], char *reportString)
{
Tcl_Obj *aobj;

aobj = Tcl_NewStringObj(reportString, -1); 
Tcl_SetObjResult(interp, aobj);

return(TCL_OK);
}

/* tcl interface functions */

static int gtkwavetcl_getNumFacs(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
int value = GLOBALS->numfacs;
return(gtkwavetcl_printInteger(clientData, interp, objc, objv, value));
}

static int gtkwavetcl_getLongestName(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
int value = GLOBALS->longestname;
return(gtkwavetcl_printInteger(clientData, interp, objc, objv, value));
}

static int gtkwavetcl_getFacName(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
Tcl_Obj *aobj;

if(objc == 2)
	{
	char *s = Tcl_GetString(objv[1]);
	int which = atoi(s);

	if((which >= 0) && (which < GLOBALS->numfacs))
		{
		int was_packed;
        	char *hfacname = NULL;

        	hfacname = hier_decompress_flagged(GLOBALS->facs[which]->name, &was_packed);

		aobj = Tcl_NewStringObj(hfacname, -1); 
		Tcl_SetObjResult(interp, aobj);
		if(was_packed) free_2(hfacname);
		}
	}

return(TCL_OK);
}

static int gtkwavetcl_getMinTime(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
TimeType value = GLOBALS->min_time;
return(gtkwavetcl_printTimeType(clientData, interp, objc, objv, value));
}

static int gtkwavetcl_getMaxTime(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
TimeType value = GLOBALS->max_time;
return(gtkwavetcl_printTimeType(clientData, interp, objc, objv, value));
}

static int gtkwavetcl_getTimeDimension(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
Tcl_Obj *aobj;
char reportString[2] = { GLOBALS->time_dimension, 0 };

aobj = Tcl_NewStringObj(reportString, -1);
Tcl_SetObjResult(interp, aobj);

return(TCL_OK);
}

static int gtkwavetcl_getBaselineMarker(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
TimeType value = GLOBALS->tims.baseline;
return(gtkwavetcl_printTimeType(clientData, interp, objc, objv, value));
}

static int gtkwavetcl_getMarker(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
TimeType value = GLOBALS->tims.marker;
return(gtkwavetcl_printTimeType(clientData, interp, objc, objv, value));
}

static int gtkwavetcl_getWindowStartTime(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
TimeType value = GLOBALS->tims.start;
return(gtkwavetcl_printTimeType(clientData, interp, objc, objv, value));
}

static int gtkwavetcl_getWindowEndTime(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
TimeType value = GLOBALS->tims.end;
return(gtkwavetcl_printTimeType(clientData, interp, objc, objv, value));
}

static int gtkwavetcl_getDumpType(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
Tcl_Obj *aobj;
char *reportString = "UNKNOWN";

if(GLOBALS->is_vcd) 
        {
        if(GLOBALS->partial_vcd)
                {
                reportString = "PVCD";
                }  
                else
                {
                reportString = "VCD";
                }
        }
else
if(GLOBALS->is_lxt)
        {
	reportString = "LXT";
        }
else
if(GLOBALS->is_ghw)
        {
	reportString = "GHW";
        }
else
if(GLOBALS->is_lx2)
        {
        switch(GLOBALS->is_lx2)
                {
                case LXT2_IS_LXT2: reportString = "LXT2"; break;
                case LXT2_IS_AET2: reportString = "AET2"; break;
                case LXT2_IS_VZT:  reportString = "VZT"; break;
                case LXT2_IS_VLIST:reportString = "VCD"; break;
                }
        }

aobj = Tcl_NewStringObj(reportString, -1);
Tcl_SetObjResult(interp, aobj);

return(TCL_OK);
}


static int gtkwavetcl_getNamedMarker(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
Tcl_Obj *aobj;

if(objc == 2)
	{
	char *s = Tcl_GetString(objv[1]);
	int which;

	if((s[0]>='A')&&(s[0]<='Z'))
		{
		TimeType value = GLOBALS->named_markers[s[0] - 'A'];
		return(gtkwavetcl_printTimeType(clientData, interp, objc, objv, value));
		}
	else
	if((s[0]>='a')&&(s[0]<='z'))
		{
		TimeType value = GLOBALS->named_markers[s[0] - 'a'];
		return(gtkwavetcl_printTimeType(clientData, interp, objc, objv, value));
		}

	which = atoi(s);
	if((which >= 0) && (which < 26))
		{
		TimeType value = GLOBALS->named_markers[which];
		return(gtkwavetcl_printTimeType(clientData, interp, objc, objv, value));
		}
	}

return(TCL_OK);
}

static int gtkwavetcl_getWaveHeight(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
int value = GLOBALS->waveheight;
return(gtkwavetcl_printInteger(clientData, interp, objc, objv, value));
}

static int gtkwavetcl_getWaveWidth(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
int value = GLOBALS->wavewidth;
return(gtkwavetcl_printInteger(clientData, interp, objc, objv, value));
}

static int gtkwavetcl_getPixelsUnitTime(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
double value = GLOBALS->pxns;
return(gtkwavetcl_printDouble(clientData, interp, objc, objv, value));
}

static int gtkwavetcl_getUnitTimePixels(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
double value = GLOBALS->nspx;
return(gtkwavetcl_printDouble(clientData, interp, objc, objv, value));
}

static int gtkwavetcl_getZoomFactor(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
double value = GLOBALS->tims.zoom;
return(gtkwavetcl_printDouble(clientData, interp, objc, objv, value));
}

static int gtkwavetcl_getDumpFileName(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
char *value = GLOBALS->loaded_file_name;
return(gtkwavetcl_printString(clientData, interp, objc, objv, value));
}

static int gtkwavetcl_getVisibleNumTraces(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
int value = GLOBALS->traces.visible;
return(gtkwavetcl_printInteger(clientData, interp, objc, objv, value));
}

static int gtkwavetcl_getTotalNumTraces(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
int value = GLOBALS->traces.total;
return(gtkwavetcl_printInteger(clientData, interp, objc, objv, value));
}

static int gtkwavetcl_getTraceNameFromIndex(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
Tcl_Obj *aobj;

if(objc == 2)
	{
	char *s = Tcl_GetString(objv[1]);
	int which = atoi(s);

	if((which >= 0) && (which < GLOBALS->traces.total))
		{
		Trptr t = GLOBALS->traces.first;
		int i = 0;
		while(t)
			{
			if(i == which)
				{
				if(t->name)
					{
					return(gtkwavetcl_printString(clientData, interp, objc, objv, t->name));
					}
					else
					{
					break;
					}
				}

			i++;
			t = t->t_next;
			}
		}
	}

return(TCL_OK);
}

static int gtkwavetcl_getTraceFlagsFromIndex(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
Tcl_Obj *aobj;

if(objc == 2)
	{
	char *s = Tcl_GetString(objv[1]);
	int which = atoi(s);

	if((which >= 0) && (which < GLOBALS->traces.total))
		{
		Trptr t = GLOBALS->traces.first;
		int i = 0;
		while(t)
			{
			if(i == which)
				{
				return(gtkwavetcl_printInteger(clientData, interp, objc, objv, t->flags));
				}

			i++;
			t = t->t_next;
			}
		}
	}

return(TCL_OK);
}

static int gtkwavetcl_getTraceValueAtMarkerFromIndex(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
Tcl_Obj *aobj;

if(objc == 2)
	{
	char *s = Tcl_GetString(objv[1]);
	int which = atoi(s);

	if((which >= 0) && (which < GLOBALS->traces.total))
		{
		Trptr t = GLOBALS->traces.first;
		int i = 0;
		while(t)
			{
			if(i == which)
				{
				if(t->asciivalue)
					{
					char *pnt = t->asciivalue;
					if(*pnt == '=') pnt++;

					return(gtkwavetcl_printString(clientData, interp, objc, objv, pnt));
					}
					else
					{
					break;
					}
				}

			i++;
			t = t->t_next;
			}
		}
	}

return(TCL_OK);
}

static int gtkwavetcl_getHierMaxLevel(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
int value = GLOBALS->hier_max_level;
return(gtkwavetcl_printInteger(clientData, interp, objc, objv, value));
}

static int gtkwavetcl_getFontHeight(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
int value = GLOBALS->fontheight;
return(gtkwavetcl_printInteger(clientData, interp, objc, objv, value));
}

static int gtkwavetcl_getLeftJustifySigs(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
int value = (GLOBALS->left_justify_sigs != 0);
return(gtkwavetcl_printInteger(clientData, interp, objc, objv, value));
}

static int gtkwavetcl_getSaveFileName(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
char *value = GLOBALS->filesel_writesave;
if(value)
	{
	return(gtkwavetcl_printString(clientData, interp, objc, objv, value));
	}

return(TCL_OK);
}

static int gtkwavetcl_getStemsFileName(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
char *value = GLOBALS->stems_name;
if(value)
	{
	return(gtkwavetcl_printString(clientData, interp, objc, objv, value));
	}

return(TCL_OK);
}

static int gtkwavetcl_getTraceScrollbarRowValue(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
GtkAdjustment *wadj=GTK_ADJUSTMENT(GLOBALS->wave_vslider);
int value = (int)wadj->value;

return(gtkwavetcl_printInteger(clientData, interp, objc, objv, value));
}



static int gtkwavetcl_setMarker(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
if(objc == 2)
        {
        char *s = Tcl_GetString(objv[1]);
        TimeType mrk = unformat_time(s, GLOBALS->time_dimension);

	if((mrk >= GLOBALS->min_time) && (mrk <= GLOBALS->max_time))
		{
		GLOBALS->tims.marker = mrk;
		}
		else
		{
		GLOBALS->tims.marker = LLDescriptor(-1);
		}

        update_markertime(GLOBALS->tims.marker);
        GLOBALS->signalwindow_width_dirty=1;
        MaxSignalLength();
        signalarea_configure_event(GLOBALS->signalarea, NULL);
        wavearea_configure_event(GLOBALS->wavearea, NULL);

	gtkwave_gtk_main_iteration();
	}

return(TCL_OK);
}

static int gtkwavetcl_setWindowStartTime(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
if(objc == 2)
        {
        char *s = Tcl_GetString(objv[1]);

	if(s)
	        {
	        TimeType gt;
	        char update_string[128];
	        char timval[40];
	        GtkAdjustment *hadj;
	        TimeType pageinc;

	        gt=unformat_time(s, GLOBALS->time_dimension);    
          
	        if(gt<GLOBALS->tims.first) gt=GLOBALS->tims.first;
	        else if(gt>GLOBALS->tims.last) gt=GLOBALS->tims.last;
 
	        hadj=GTK_ADJUSTMENT(GLOBALS->wave_hslider);
	        hadj->value=gt;

	        pageinc=(TimeType)(((gdouble)GLOBALS->wavewidth)*GLOBALS->nspx);
	        if(gt<(GLOBALS->tims.last-pageinc+1))
	                GLOBALS->tims.timecache=gt;
	                else
	                {
	                GLOBALS->tims.timecache=GLOBALS->tims.last-pageinc+1;
	                if(GLOBALS->tims.timecache<GLOBALS->tims.first) GLOBALS->tims.timecache=GLOBALS->tims.first;
	                }
	
	        reformat_time(timval,GLOBALS->tims.timecache,GLOBALS->time_dimension);
	        
	        time_update();
	        }

        signalarea_configure_event(GLOBALS->signalarea, NULL);
        wavearea_configure_event(GLOBALS->wavearea, NULL);
	gtkwave_gtk_main_iteration();
	}

return(TCL_OK);
}

static int gtkwavetcl_setZoomFactor(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
if(objc == 2)
        {
        char *s = Tcl_GetString(objv[1]);
        float f;
         
        sscanf(s, "%f", &f);
        if(f>0.0)
                {
                f=0.0; /* in case they try to go out of range */
                }
        else
        if(f<-62.0)
                {
                f=-62.0; /* in case they try to go out of range */
                } 
                
        GLOBALS->tims.prevzoom=GLOBALS->tims.zoom; 
        GLOBALS->tims.zoom=(gdouble)f;
        calczoom(GLOBALS->tims.zoom);
        fix_wavehadj();

        gtk_signal_emit_by_name (GTK_OBJECT (GTK_ADJUSTMENT(GLOBALS->wave_hslider)), "changed");
        gtk_signal_emit_by_name (GTK_OBJECT (GTK_ADJUSTMENT(GLOBALS->wave_hslider)), "value_changed");

	gtkwave_gtk_main_iteration();
	}

return(TCL_OK);
}

static int gtkwavetcl_setLeftJustifySigs(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
if(objc == 2)
        {
        char *s = Tcl_GetString(objv[1]);
        TimeType val = atoi_64(s);
	GLOBALS->left_justify_sigs = (val != LLDescriptor(0)) ? ~0 : 0;

        MaxSignalLength();
        signalarea_configure_event(GLOBALS->signalarea, NULL);

	gtkwave_gtk_main_iteration();
	}

return(TCL_OK);
}

static int gtkwavetcl_setNamedMarker(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
if(objc == 3)
        {
        char *s = Tcl_GetString(objv[1]);
	int which = -1;

        if((s[0]>='A')&&(s[0]<='Z'))  
                {
                which = s[0] - 'A';
                }
        else
        if((s[0]>='a')&&(s[0]<='z'))  
                {
                which = s[0] - 'a';
                }
	else                
		{
	        which = atoi(s);
		}

        if((which >= 0) && (which < 26))
                {
	        char *t = Tcl_GetString(objv[2]);
		TimeType gt=unformat_time(t, GLOBALS->time_dimension);

                GLOBALS->named_markers[which] = gt;
	        wavearea_configure_event(GLOBALS->wavearea, NULL);
		gtkwave_gtk_main_iteration();
                } 
	}

return(TCL_OK);
}


static int gtkwavetcl_setTraceScrollbarRowValue(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
if(objc == 2)
        {
        char *s = Tcl_GetString(objv[1]);
        int target = atoi(s);
        GtkAdjustment *wadj=GTK_ADJUSTMENT(GLOBALS->wave_vslider);

        int num_traces_displayable=(GLOBALS->signalarea->allocation.height)/(GLOBALS->fontheight);
        num_traces_displayable--;   /* for the time trace that is always there */

	if(target > GLOBALS->traces.visible - num_traces_displayable) target = GLOBALS->traces.visible - num_traces_displayable;

	if(target < 0) target = 0;

	wadj->value = target;

        gtk_signal_emit_by_name (GTK_OBJECT (wadj), "changed"); /* force bar update */
        gtk_signal_emit_by_name (GTK_OBJECT (wadj), "value_changed"); /* force text update */
	gtkwave_gtk_main_iteration();
	}

return(TCL_OK);
}


typedef struct 
	{
    	const char *cmdstr;
    	int (*func)();
	} tcl_cmdstruct;

static tcl_cmdstruct gtkwave_commands[] =
	{
	{"getBaselineMarker",			gtkwavetcl_getBaselineMarker},
	{"getDumpFileName",			gtkwavetcl_getDumpFileName},
	{"getDumpType", 			gtkwavetcl_getDumpType},
	{"getFacName", 				gtkwavetcl_getFacName},
	{"getFontHeight",			gtkwavetcl_getFontHeight},
	{"getHierMaxLevel",			gtkwavetcl_getHierMaxLevel},
	{"getLeftJustifySigs",			gtkwavetcl_getLeftJustifySigs},
	{"getLongestName", 			gtkwavetcl_getLongestName},
	{"getMarker",				gtkwavetcl_getMarker},
	{"getMaxTime", 				gtkwavetcl_getMaxTime},
	{"getMinTime", 				gtkwavetcl_getMinTime},
	{"getNamedMarker", 			gtkwavetcl_getNamedMarker},
	{"getNumFacs", 				gtkwavetcl_getNumFacs},
	{"getPixelsUnitTime", 			gtkwavetcl_getPixelsUnitTime},
	{"getSaveFileName",			gtkwavetcl_getSaveFileName},
	{"getStemsFileName",			gtkwavetcl_getStemsFileName},
	{"getTimeDimension", 			gtkwavetcl_getTimeDimension},
	{"getTotalNumTraces",  			gtkwavetcl_getTotalNumTraces},
	{"getTraceFlagsFromIndex", 		gtkwavetcl_getTraceFlagsFromIndex},
	{"getTraceNameFromIndex", 		gtkwavetcl_getTraceNameFromIndex},
	{"getTraceScrollbarRowValue", 		gtkwavetcl_getTraceScrollbarRowValue},
	{"getTraceValueAtMarkerFromIndex", 	gtkwavetcl_getTraceValueAtMarkerFromIndex},
	{"getUnitTimePixels", 			gtkwavetcl_getUnitTimePixels},
	{"getVisibleNumTraces", 		gtkwavetcl_getVisibleNumTraces},
	{"getWaveHeight", 			gtkwavetcl_getWaveHeight},
	{"getWaveWidth", 			gtkwavetcl_getWaveWidth},
	{"getWindowEndTime", 			gtkwavetcl_getWindowEndTime},
	{"getWindowStartTime", 			gtkwavetcl_getWindowStartTime},
	{"getZoomFactor",			gtkwavetcl_getZoomFactor},
   	{"nop", 				gtkwavetcl_nop},
	{"setLeftJustifySigs",			gtkwavetcl_setLeftJustifySigs},
	{"setMarker",				gtkwavetcl_setMarker},
	{"setNamedMarker",			gtkwavetcl_setNamedMarker},
	{"setTraceScrollbarRowValue", 		gtkwavetcl_setTraceScrollbarRowValue},
	{"setWindowStartTime",			gtkwavetcl_setWindowStartTime},
	{"setZoomFactor",			gtkwavetcl_setZoomFactor},
   	{"", 					NULL} /* sentinel */
	};


static int menu_func(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
GtkItemFactoryEntry *ife = (GtkItemFactoryEntry *)clientData;
int i;
FILE *old_handle = GLOBALS->script_handle;
char fexit = GLOBALS->enable_fast_exit;

GLOBALS->script_handle = NULL;
GLOBALS->enable_fast_exit = 1;

if(objc > 1)
	{
	int fd = -1;
	char *fnam = tmpnam_2("tclwave", &fd);
	if(fnam)
		{
		GLOBALS->script_handle = fopen(fnam, "wb");
		for(i=1;i<objc;i++)
			{
			char *s = Tcl_GetString(objv[i]);
			fprintf(GLOBALS->script_handle,"%s\n", s);
			}
		fclose(GLOBALS->script_handle);
		GLOBALS->script_handle = fopen(fnam, "rb");
		ife->callback();
		gtkwave_gtk_main_iteration();
		unlink(fnam);
		if(fd>-1) 
			{
			free_2(fnam);
			close(fd);
			}
		GLOBALS->script_handle = NULL;
		}
	}
	else
	{
	ife->callback();
	gtkwave_gtk_main_iteration();
	}

GLOBALS->enable_fast_exit = fexit;
GLOBALS->script_handle = old_handle;
return(TCL_OK); /* signal error with rc=TCL_ERROR, Tcl_Obj *aobj = Tcl_NewStringObj(reportString, -1); Tcl_SetObjResult(interp, aobj); */
}


void make_tcl_interpreter(char *argv[])
{
int i;
char commandName[128];
GtkItemFactoryEntry *ife;
int num_menu_items;

Tcl_FindExecutable(argv[0]);

GLOBALS->interp = Tcl_CreateInterp();

if (TCL_OK != Tcl_Init(GLOBALS->interp)) 
	{
   	fprintf(stderr, "GTKWAVE | Tcl_Init error: %s\n", Tcl_GetStringResult (GLOBALS->interp));
   	exit(EXIT_FAILURE);
  	}

strcpy(commandName, "gtkwave::");

ife = retrieve_menu_items_array(&num_menu_items);
for(i=0;i<num_menu_items;i++)
	{
	if(ife[i].callback)
		{
		char *pnt = commandName + 9;
		strcpy(pnt, ife[i].path);	
		while(*pnt)
			{
			if(*pnt==' ') *pnt='_';
			pnt++;
			}
	
	      	Tcl_CreateObjCommand(GLOBALS->interp, commandName,
	                (Tcl_ObjCmdProc *)menu_func,
	                (ClientData)(ife+i), (Tcl_CmdDeleteProc *)NULL);
		}
	}


for (i = 0; gtkwave_commands[i].func != NULL; i++) 
	{
      	strcpy(commandName + 9, gtkwave_commands[i].cmdstr);

      	Tcl_CreateObjCommand(GLOBALS->interp, commandName,
                (Tcl_ObjCmdProc *)gtkwave_commands[i].func,
                (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
   	}
}

#else

void make_tcl_interpreter(char *argv[])
{
/* nothing */
}

#endif


/*
 * $Id$
 * $Log$
 * Revision 1.20  2008/10/14 20:47:53  gtkwave
 * more setXX adds for tcl
 *
 * Revision 1.19  2008/10/14 19:39:04  gtkwave
 * beginning to add setXX capability from tcl scripts
 *
 * Revision 1.18  2008/10/14 18:56:12  gtkwave
 * starting to add getXX functions called from tcl
 *
 * Revision 1.17  2008/10/14 03:32:09  gtkwave
 * starting to add non-menu commands
 *
 * Revision 1.16  2008/10/14 00:53:46  gtkwave
 * enabled tcl scripts to call existing gtkwave style scripted menu functions
 *
 * Revision 1.15  2008/10/13 22:16:52  gtkwave
 * tcl interpreter integration
 *
 * Revision 1.14  2008/10/04 21:00:08  gtkwave
 * do direct search before any attempted regex ones in list process
 *
 * Revision 1.13  2008/10/04 15:15:20  gtkwave
 * gtk1 compatibility fixes
 *
 * Revision 1.12  2008/10/02 00:52:25  gtkwave
 * added dnd of external filetypes into viewer
 *
 * Revision 1.11  2008/09/30 06:32:00  gtkwave
 * added dnd support for comment traces, collapse groups, blank traces
 *
 * Revision 1.10  2008/09/29 22:46:39  gtkwave
 * complex dnd handling with gtkwave trace attributes
 *
 * Revision 1.9  2008/09/27 19:08:39  gtkwave
 * compiler warning fixes
 *
 * Revision 1.8  2008/09/27 06:26:35  gtkwave
 * twinwave (XEmbed) fixes for self-dnd in signal window
 *
 * Revision 1.7  2008/09/27 05:05:04  gtkwave
 * removed unnecessary sing_len struct item
 *
 * Revision 1.6  2008/09/25 18:23:47  gtkwave
 * cut over to usage of zMergeTclList for list generation
 *
 * Revision 1.5  2008/09/25 01:41:35  gtkwave
 * drag from tree clist window into external process
 *
 * Revision 1.4  2008/09/24 23:41:24  gtkwave
 * drag from signal window into external process
 *
 * Revision 1.3  2008/09/24 18:54:00  gtkwave
 * drag from search widget into external processes
 *
 * Revision 1.2  2008/09/24 02:17:32  gtkwave
 * fix memory leak on recreated signal names at import end
 *
 * Revision 1.1  2008/09/23 18:22:01  gtkwave
 * file creation
 *
 */
