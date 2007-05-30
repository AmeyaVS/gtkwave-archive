/*
 * Copyright (c) Tony Bybell 1999-2001
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */
#include <config.h>
#include "gtk12compat.h"
#include "currenttime.h"

char *fontname_signals=NULL;
char *fontname_waves=NULL;

void load_all_fonts(void)
{
if((fontname_signals)&&(strlen(fontname_signals)))
	{
        signalfont=gdk_font_load(fontname_signals);
	}
	else
	{
	if(use_big_fonts)
	        {
	        if(!use_nonprop_fonts) 
	                {
	                signalfont=gdk_font_load("-*-times-*-r-*-*-15-*-*-*-*-*-*-*");
	                }
	                else
	                {
#ifdef __CYGWIN__
	                signalfont=gdk_font_load("-misc-fixed-*-*-*-*-15-*-*-*-*-*-*-*");
#else
	                signalfont=gdk_font_load("-*-courier-*-r-*-*-15-*-*-*-*-*-*-*");
			if(!signalfont) signalfont=gdk_font_load("-misc-fixed-*-*-*-*-15-*-*-*-*-*-*-*");
#endif
	                }
	        }
	        else
	        {
	        if(use_nonprop_fonts)
	                {   
#ifdef __CYGWIN__
	                signalfont=gdk_font_load("-misc-fixed-*-*-*-*-14-*-*-*-*-*-*-*");
#else
	                signalfont=gdk_font_load("-*-courier-*-r-*-*-14-*-*-*-*-*-*-*");
			if(!signalfont) signalfont=gdk_font_load("-misc-fixed-*-*-*-*-14-*-*-*-*-*-*-*");
#endif
	                }
	        }
	}

if(!signalfont)
        {  
#if WAVE_USE_GTK2
        signalfont=gdk_font_load("-*-courier-*-r-*-*-14-*-*-*-*-*-*-*");
	if(!signalfont) signalfont=gdk_font_load("-misc-fixed-*-*-*-*-14-*-*-*-*-*-*-*");
	if(!signalfont) { fprintf(stderr, "Could not load signalfont courier 14 or misc-fixed 14, exiting!\n"); exit(255); }
#else
	signalfont=wavearea->style->font;
#endif
        }

fontheight=(signalfont->ascent+signalfont->descent)+4;

if((fontname_waves)&&(strlen(fontname_waves)))
	{
        wavefont=wavefont_smaller=gdk_font_load(fontname_waves);
	}
	else
	{
#ifndef __CYGWIN__
	if(use_big_fonts)
	        {
	        wavefont=gdk_font_load("-*-courier-*-r-*-*-14-*-*-*-*-*-*-*");
		if(!wavefont) wavefont=gdk_font_load("-misc-fixed-*-*-*-*-14-*-*-*-*-*-*-*");

	        wavefont_smaller=gdk_font_load("-*-courier-*-r-*-*-10-*-*-*-*-*-*-*");
		if(!wavefont_smaller) wavefont_smaller=gdk_font_load("-misc-fixed-*-*-*-*-10-*-*-*-*-*-*-*");
	        }
	        else
	        {
	        wavefont=wavefont_smaller=gdk_font_load("-*-courier-*-r-*-*-10-*-*-*-*-*-*-*");
		if(!wavefont) wavefont=wavefont_smaller=gdk_font_load("-misc-fixed-*-*-*-*-10-*-*-*-*-*-*-*");
	        }
#else
	if(use_big_fonts)
	        {
                wavefont=gdk_font_load("-misc-fixed-*-*-*-*-14-*-*-*-*-*-*-*");
                wavefont_smaller=gdk_font_load("-misc-fixed-*-*-*-*-10-*-*-*-*-*-*-*");
	        }
	        else
	        {
                wavefont=wavefont_smaller=gdk_font_load("-misc-fixed-*-*-*-*-10-*-*-*-*-*-*-*");
	        }
#endif
	}

if(!wavefont)
        {  
#if WAVE_USE_GTK2
        wavefont=wavefont_smaller=gdk_font_load("-*-courier-*-r-*-*-10-*-*-*-*-*-*-*");
	if(!wavefont) wavefont=wavefont_smaller=gdk_font_load("-misc-fixed-*-*-*-*-10-*-*-*-*-*-*-*");
	if(!wavefont) { fprintf(stderr, "Could not load wavefont courier 10 or misc-fixed 10, exiting!\n"); exit(255); }
#else
	wavefont=wavefont_smaller=wavearea->style->font;
#endif
        }


if(signalfont->ascent<wavefont->ascent)
	{
	fprintf(stderr, "Signalfont is smaller than wavefont.  Exiting!\n");
	exit(1);
	}

if(signalfont->ascent>100)
	{
	fprintf(stderr, "Fonts are too big!  Try fonts with a smaller size.  Exiting!\n");
	exit(1);
	}

wavecrosspiece=wavefont->ascent+1;
}

/*
 * $Id$
 * $Log$
 * Revision 1.2  2007/04/20 02:08:11  gtkwave
 * initial release
 *
 */

