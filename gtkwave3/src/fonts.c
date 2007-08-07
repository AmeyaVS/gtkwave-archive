/*
 * Copyright (c) Tony Bybell 1999-2001
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include "globals.h"
#include <config.h>
#include "gtk12compat.h"
#include "currenttime.h"


void load_all_fonts(void)
{
if((GLOBALS->fontname_signals)&&(strlen(GLOBALS->fontname_signals)))
	{
        GLOBALS->signalfont=gdk_font_load(GLOBALS->fontname_signals);
	}
	else
	{
	if(GLOBALS->use_big_fonts)
	        {
	        if(!GLOBALS->use_nonprop_fonts) 
	                {
	                GLOBALS->signalfont=gdk_font_load("-*-times-*-r-*-*-15-*-*-*-*-*-*-*");
	                }
	                else
	                {
#ifdef __CYGWIN__
	                GLOBALS->signalfont=gdk_font_load("-misc-fixed-*-*-*-*-15-*-*-*-*-*-*-*");
#else
	                GLOBALS->signalfont=gdk_font_load("-*-courier-*-r-*-*-15-*-*-*-*-*-*-*");
			if(!GLOBALS->signalfont) GLOBALS->signalfont=gdk_font_load("-misc-fixed-*-*-*-*-15-*-*-*-*-*-*-*");
#endif
	                }
	        }
	        else
	        {
	        if(GLOBALS->use_nonprop_fonts)
	                {   
#ifdef __CYGWIN__
	                GLOBALS->signalfont=gdk_font_load("-misc-fixed-*-*-*-*-14-*-*-*-*-*-*-*");
#else
	                GLOBALS->signalfont=gdk_font_load("-*-courier-*-r-*-*-14-*-*-*-*-*-*-*");
			if(!GLOBALS->signalfont) GLOBALS->signalfont=gdk_font_load("-misc-fixed-*-*-*-*-14-*-*-*-*-*-*-*");
#endif
	                }
	        }
	}

if(!GLOBALS->signalfont)
        {  
#if WAVE_USE_GTK2
        GLOBALS->signalfont=gdk_font_load("-*-courier-*-r-*-*-14-*-*-*-*-*-*-*");
	if(!GLOBALS->signalfont) GLOBALS->signalfont=gdk_font_load("-misc-fixed-*-*-*-*-14-*-*-*-*-*-*-*");
	if(!GLOBALS->signalfont) { fprintf(stderr, "Could not load signalfont courier 14 or misc-fixed 14, exiting!\n"); exit(255); }
#else
	GLOBALS->signalfont=GLOBALS->wavearea->style->font;
#endif
        }

GLOBALS->fontheight=(GLOBALS->signalfont->ascent+GLOBALS->signalfont->descent)+4;

if((GLOBALS->fontname_waves)&&(strlen(GLOBALS->fontname_waves)))
	{
        GLOBALS->wavefont=GLOBALS->wavefont_smaller=gdk_font_load(GLOBALS->fontname_waves);
	}
	else
	{
#ifndef __CYGWIN__
	if(GLOBALS->use_big_fonts)
	        {
	        GLOBALS->wavefont=gdk_font_load("-*-courier-*-r-*-*-14-*-*-*-*-*-*-*");
		if(!GLOBALS->wavefont) GLOBALS->wavefont=gdk_font_load("-misc-fixed-*-*-*-*-14-*-*-*-*-*-*-*");

	        GLOBALS->wavefont_smaller=gdk_font_load("-*-courier-*-r-*-*-10-*-*-*-*-*-*-*");
		if(!GLOBALS->wavefont_smaller) GLOBALS->wavefont_smaller=gdk_font_load("-misc-fixed-*-*-*-*-10-*-*-*-*-*-*-*");
	        }
	        else
	        {
	        GLOBALS->wavefont=GLOBALS->wavefont_smaller=gdk_font_load("-*-courier-*-r-*-*-10-*-*-*-*-*-*-*");
		if(!GLOBALS->wavefont) GLOBALS->wavefont=GLOBALS->wavefont_smaller=gdk_font_load("-misc-fixed-*-*-*-*-10-*-*-*-*-*-*-*");
	        }
#else
	if(GLOBALS->use_big_fonts)
	        {
                GLOBALS->wavefont=gdk_font_load("-misc-fixed-*-*-*-*-14-*-*-*-*-*-*-*");
                GLOBALS->wavefont_smaller=gdk_font_load("-misc-fixed-*-*-*-*-10-*-*-*-*-*-*-*");
	        }
	        else
	        {
                GLOBALS->wavefont=GLOBALS->wavefont_smaller=gdk_font_load("-misc-fixed-*-*-*-*-10-*-*-*-*-*-*-*");
	        }
#endif
	}

if(!GLOBALS->wavefont)
        {  
#if WAVE_USE_GTK2
        GLOBALS->wavefont=GLOBALS->wavefont_smaller=gdk_font_load("-*-courier-*-r-*-*-10-*-*-*-*-*-*-*");
	if(!GLOBALS->wavefont) GLOBALS->wavefont=GLOBALS->wavefont_smaller=gdk_font_load("-misc-fixed-*-*-*-*-10-*-*-*-*-*-*-*");
	if(!GLOBALS->wavefont) { fprintf(stderr, "Could not load wavefont courier 10 or misc-fixed 10, exiting!\n"); exit(255); }
#else
	GLOBALS->wavefont=GLOBALS->wavefont_smaller=GLOBALS->wavearea->style->font;
#endif
        }


if(GLOBALS->signalfont->ascent<GLOBALS->wavefont->ascent)
	{
	fprintf(stderr, "Signalfont is smaller than wavefont.  Exiting!\n");
	exit(1);
	}

if(GLOBALS->signalfont->ascent>100)
	{
	fprintf(stderr, "Fonts are too big!  Try fonts with a smaller size.  Exiting!\n");
	exit(1);
	}

GLOBALS->wavecrosspiece=GLOBALS->wavefont->ascent+1;
}

/*
 * $Id$
 * $Log$
 * Revision 1.1.1.1.2.2  2007/08/06 03:50:46  gtkwave
 * globals support for ae2, gtk1, cygwin, mingw.  also cleaned up some machine
 * generated structs, etc.
 *
 * Revision 1.1.1.1.2.1  2007/08/05 02:27:19  kermin
 * Semi working global struct
 *
 * Revision 1.1.1.1  2007/05/30 04:27:28  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:11  gtkwave
 * initial release
 *
 */

