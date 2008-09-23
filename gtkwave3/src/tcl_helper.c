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
#include <stdio.h>
#include <stdlib.h>
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
 *	pointer array must be freed by calling free().
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


/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */
/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */
/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */

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
#endif

if(GLOBALS->dnd_tgt_on_signalarea_treesearch_gtk2_c_1)
	{
	WAVE_GDK_GET_POINTER(GLOBALS->signalarea->window, &x, &y, &xi, &yi, &state);
	WAVE_GDK_GET_POINTER_COPY;

	if((x<0)||(y<0)||(x>=GLOBALS->signalarea->allocation.width)||(y>=GLOBALS->signalarea->allocation.height)) return;
	}
else
if(GLOBALS->dnd_tgt_on_wavearea_treesearch_gtk2_c_1)
	{
	WAVE_GDK_GET_POINTER(GLOBALS->wavearea->window, &x, &y, &xi, &yi, &state);
	WAVE_GDK_GET_POINTER_COPY;

	if((x<0)||(y<0)||(x>=GLOBALS->wavearea->allocation.width)||(y>=GLOBALS->wavearea->allocation.height)) return;
	}
else
	{
	return;
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
 * make_net_name_from_tcl_list - creates gtkwave-style name from tcl list entry
 *
 * Results:
 *      Returns suitable name or NULL if not applicable.
 * ----------------------------------------------------------------------------
 */     

static char *make_net_name_from_tcl_list(char *s)
{
char *s_new = NULL;
int len;
int i,l;
char** elem = NULL;

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

		s_new = calloc(1, len);
		for(i=1;i<l;i++)
			{
			strcat(s_new, elem[i]);
			if(i!=(l-1)) strcat(s_new, delim_str);
			}
		}

	free_2(elem);
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
char *most_recent_lbrack = NULL;
char *this_regex = "\\(\\[.*\\]\\)*$";
char *entry_suffixed;
int c, i, ii;
char **list;
char **s_new_list;
char **most_recent_lbrack_list;
int *match_idx_list;
int *match_type_list;
char delim_str[2] = { GLOBALS->hier_delimeter, 0 };
Trptr t;
int found = 0;

list = zSplitTclList(s, &c);
if(!list)
	{
	return;
	}
s_new_list = calloc_2(c, sizeof(char *));
match_idx_list = calloc_2(c, sizeof(int *));
match_type_list = calloc_2(c, sizeof(int *));
most_recent_lbrack_list = calloc_2(c, sizeof(char *));

for(ii=0;ii<c;ii++)
	{
	s_new = make_net_name_from_tcl_list(list[ii]);
	if(!s_new) continue;
	s_new_list[ii] = s_new;

	most_recent_lbrack_list[ii] = strrchr(s_new, '[');

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
				match_type_list[ii] = 2; /* match was on lbrack removal */
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

		if((match_type_list[ii] == 2)&&(s->n->ext))
			{
			nptr nexp;
			int bit = atoi(most_recent_lbrack_list[ii]+1);
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
free_2(s_new_list);
free_2(match_idx_list);
free_2(match_type_list);
free_2(most_recent_lbrack_list);
free_2(list);

return(found);
}

/*
 * $Id$
 * $Log$
 *
 */

