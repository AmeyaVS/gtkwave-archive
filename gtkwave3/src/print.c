/*
 * Copyright (c) Tony Bybell 1999-2008. 
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version. 
 */

/*
 * This module has been re-implemented by Udi Finkelstein. Since it is no
 * longer a PostScript-only module, it had been renamed "print.c". 
 *
 * Much of the code has been "C++"-ized in style, yet written in C. We use
 * classes, virtual functions, class members, and "this" pointers written in
 * C. 
 */


/* AIX may need this for alloca to work */
#if defined _AIX
#pragma alloca
#endif

#include "globals.h"
#include <config.h>
#include "currenttime.h"
#include "analyzer.h"
#include "symbol.h"
#include "bsearch.h"
#include "wavealloca.h"
#include "debug.h"
#include "strace.h"
#include "print.h"


/**********************************************
 * this is essentially wavewindow.c's rendering
 * engine modified to produce postscript
 **********************************************/

#define WAVE_COURIER_SCALE_FAC 1.6	/* more or less is the correct
					 * pixel->ps scale mapping  */


/* 
 * PostScript device specific operations
 */
gtk_print_device ps_print_device = {
  ps_header,
  ps_trailer,
  ps_signal_init,
  ps_setgray,
  ps_draw_line,
  ps_draw_box,
  ps_draw_string
};

/*
 * MIF device specific operations
 */
gtk_print_device mif_print_device = {
  mif_header,
  mif_trailer,
  mif_signal_init,
  mif_setgray,
  mif_draw_line,
  mif_draw_box,
  mif_draw_string
};



/**************************************************************************
 * Shorthand routins                                                      *
 *                                                                        *
 * These routines call the specific operations through the device pointer *
 **************************************************************************/
void
pr_header (pr_context * prc)
{
  (*prc->gpd->gpd_header) (prc);
}

void
pr_trailer (pr_context * prc)
{
  (*prc->gpd->gpd_trailer) (prc);
}

void
pr_signal_init (pr_context * prc)
{
  (*prc->gpd->gpd_signal_init) (prc);
}

void
pr_setgray (pr_context * prc, gdouble gray)
{
  (*prc->gpd->gpd_setgray) (prc, gray);
}

void
pr_draw_line (pr_context * prc, gdouble _x1, gdouble _y1, gdouble x2,
	      gdouble y2)
{
  (*prc->gpd->gpd_draw_line) (prc, _x1, _y1, x2, y2);
}

void
pr_draw_box (pr_context * prc, gdouble _x1, gdouble _y1, gdouble x2, gdouble y2)
{
  (*prc->gpd->gpd_draw_box) (prc, _x1, _y1, x2, y2);
}

void
pr_draw_string (pr_context * prc, int x, int y, char *str, int xsize,
		int ysize)
{
  (*prc->gpd->gpd_draw_string) (prc, x, y, str, xsize, ysize);
}


/*************************************************************************
 *                    PostScript specific routines                       *
 *************************************************************************/

/*
 * Set current gray level, with 0.0 being white, and 1.0 being black. 
 */
void
ps_setgray (pr_context * prc, gdouble gray)
{
  fprintf (prc->handle, "%f setgray\n", gray);
}

/*
 * Create a rectangular path 
 */
void
ps_box (pr_context * prc, gdouble t_x1, gdouble t_y1, gdouble tx2, gdouble ty2)
{
  fprintf (prc->handle, "%f %f %f %f box\n", t_y1, t_x1, ty2, tx2);
}

/*
 * Draw a box 
 */
void
ps_draw_box (pr_context * prc, gdouble _x1, gdouble _y1, gdouble x2, gdouble y2)
{
  gdouble t_x1, t_y1, tx2, ty2;

  t_x1 = _x1 * prc->xscale;
  t_y1 = _y1 * prc->yscale;
  tx2 = x2 * prc->xscale;
  ty2 = y2 * prc->yscale;

  ps_box (prc, t_x1, t_y1, tx2, ty2);
  fprintf (prc->handle, "fill\n");
}

void
ps_signal_init (pr_context * prc)
{

  fprintf (prc->handle, "grestore\n" "gsave\n");

  if (prc->fullpage)
    {
      ps_setgray (prc, 0.0);
      ps_box (prc, prc->MinX - 1, prc->MinY - 2, prc->MaxX + 1,
	      prc->MaxY + 2);
      fprintf (prc->handle, "stroke\n");
      ps_setgray (prc, 0.5);
      ps_box (prc, prc->MinX, prc->MinY - 1, prc->MaxX, prc->MaxY + 1);
      fprintf (prc->handle, "clip\n");
    }
  fprintf (prc->handle,
	   "%d %d translate\n" "1 1 scale\n" "0.5 setlinewidth\n" "stroke\n",
	   prc->MinY, prc->MinX);
}

void
ps_header (pr_context * prc)
{
  gdouble ps_skip;

  ps_skip =
    prc->MinX + (prc->MaxX -
		 prc->MinX) *
    (((gdouble) GLOBALS->pr_signal_fill_width_print_c_1) / prc->xtotal);

  fprintf (prc->handle,
	   "%%!PS-Adobe-2.0 EPSF-1.2\n"
	   "%%%%BoundingBox: %d %d %d %d\n"
	   "/box { %% stack: _x1 _y1 x2 y2\n"
	   "\tnewpath\n"
	   "\t2 copy moveto %% x2 y2\n"
	   "\t3 index 1 index lineto %% _x1 y2\n"
	   "\t3 index 3 index lineto %% _x1 _y1\n"
	   "\t1 index 3 index lineto %% x2 _y1\n"
	   "\tpop pop pop pop\n"
	   "\tclosepath\n"
	   "} def\n"
	   "/l { %% stack: _x1 _y1 x2 y2\n"
	   "\tnewpath moveto lineto closepath stroke\n"
	   "} def\n", prc->MinY, prc->MinX,
	   (int) (prc->fullpage ? prc->MaxY : GLOBALS->ybound_print_c_1),
	   prc->MaxX);

  if (!prc->fullpage)
    {
      ps_box (prc, prc->MinX - 1, prc->MinY - 1, prc->MaxX,
	      GLOBALS->ybound_print_c_1);
      fprintf (prc->handle, "clip\n");
    }
  fprintf (prc->handle,
	   "/Courier findfont\n"
	   "10 scalefont\n" "setfont\n" "2 setlinecap\n" "gsave\n"
	   "1 1 scale\n" "0.5 setlinewidth\n" "stroke\n");
  ps_setgray (prc, 0.75);
  ps_box (prc, ps_skip, prc->MinY - 1, prc->MaxX, prc->MaxY + 1);
  fprintf (prc->handle, "clip\n" "%d %f translate stroke\n", prc->MinY,
	   ps_skip);
}


void
ps_draw_line (pr_context * prc, gdouble _x1, gdouble _y1, gdouble x2,
	      gdouble y2)
{
  gdouble t_x1, t_y1, tx2, ty2;

  if (_x1 < -1.0)
    _x1 = -1.0;
  if (x2 < -1.0)
    x2 = -1.0;
  if (_x1 > 10000.0)
    _x1 = 10000.0;
  if (x2 > 10000.0)
    x2 = 10000.0;

  t_x1 = _x1 * prc->xscale;
  t_y1 = _y1 * prc->yscale;
  tx2 = x2 * prc->xscale;
  ty2 = y2 * prc->yscale;

  fprintf (prc->handle, "%f %f %f %f l\n", t_y1, t_x1, ty2, tx2);
}


void
ps_draw_string (pr_context * prc, int x, int y, char *str, int xsize,
		int ysize)
{
  int i;
  int len;

  len = strlen (str);
  if (!len)
    return;

  fprintf (prc->handle, "save\n%f %f moveto\n%f %f scale\n90 rotate\n(",
	   (y - 1) * prc->yscale, x * prc->xscale,
	   (ysize * prc->yscale) * WAVE_COURIER_SCALE_FAC / 10.0,
	   (xsize * prc->xscale) / len * WAVE_COURIER_SCALE_FAC / 10.0);

  for (i = 0; i < len; i++)
    {
      char ch;

      ch = str[i];
      if (ch < 32)
	{
	  ch = 32;		/* fix out of range signed chars */
	}
      else if ((ch == '(') || (ch == ')') || (ch == '\\'))
	{
	  fprintf (prc->handle, "\\");	/* escape parens or esc */
	}
      fprintf (prc->handle, "%c", ch);
    }

  fprintf (prc->handle, ") show\n" "restore\n");
}


void
ps_trailer (pr_context * prc)
{
  fprintf (prc->handle, "grestore showpage\n");
}

/*************************************************************************
 *                    MIF specific routines                              *
 *************************************************************************/

/*
 * Generic maint functions missing in gcc 
 */
#ifndef _MSC_VER
static gdouble
maxdbl (gdouble a, gdouble b)
{
  return (a > b ? a : b);
}

static gdouble
mindbl (gdouble a, gdouble b)
{
  return (a < b ? a : b);
}
#endif


/*
 * Set current gray level, with 0.0 being white, and 1.0 being black. 
 */
void
mif_setgray (pr_context * prc, gdouble gray)
{
  prc->gray = gray;
}

/*
 * Set current gray level, with 0.0 being white, and 1.0 being black. 
 */
void
mif_translate (pr_context * prc, gdouble x, gdouble y)
{
  prc->tr_x = x;
  prc->tr_y = y;
}

/*
 * Draw an empty box 
 */
void
mif_box (pr_context * prc, gdouble _x1, gdouble _y1, gdouble x2, gdouble y2)
{
  fprintf (prc->handle,
	   " <Rectangle\n"
	   "   <GroupID 1>\n"
	   "   <Fill 15>\n"
	   "   <Pen 0>\n"
	   "   <PenWidth 0.5 pt>\n"
	   "   <Overprint No >\n"
	   "   <ObTint %d%%>\n"
	   "   <ShapeRect %d pt %d pt %d pt %d pt>\n"
	   "   <BRect %d pt %d pt %d pt %d pt>\n"
	   " > # end of Rectangle\n",
	   (int) (10000 * (1 - prc->gray)),
	   (int) (_y1), (int) (_x1),
	   abs ((int) (y2 - _y1)), abs ((int) (x2 - _x1)), (int) (_y1),
	   (int) (_x1), abs ((int) (y2 - _y1)), abs ((int) (x2 - _x1)));
}

/*
 * Draw a filled box 
 */

void
mif_draw_box (pr_context * prc, gdouble _x1, gdouble _y1, gdouble x2,
	      gdouble y2)
{
  gdouble t_x1, t_y1, tx2, ty2;
  int rx, ry, rw, rh;

  t_x1 = _x1 * prc->xscale + prc->tr_x;
  t_y1 = _y1 * prc->yscale + prc->tr_y;
  tx2 = x2 * prc->xscale + prc->tr_x;
  ty2 = y2 * prc->yscale + prc->tr_y;

  /* The exprssion below is derived from: */
  /* rx = mindbl((prc->PageX * inch - t_x1), (prc->PageX * inch - tx2)) */
  rx = (int) (prc->PageX * GLOBALS->inch_print_c_1 - maxdbl (tx2, t_x1));
  ry = (int) mindbl (t_y1, ty2);
  rw = abs ((int) (tx2 - t_x1));
  rh = abs ((int) (ty2 - t_y1));


  fprintf (prc->handle,
	   " <Rectangle\n"
	   "   <GroupID 1>\n"
	   "   <Fill 0>\n"
	   "   <Pen 0>\n"
	   "   <PenWidth 0.5 pt>\n"
	   "   <Overprint No >\n"
	   "   <ObTint %d%%>\n"
	   "   <ShapeRect %d pt %d pt %d pt %d pt>\n"
	   "   <BRect %d pt %d pt %d pt %d pt>\n"
	   " > # end of Rectangle\n", (int) (10000 * (1 - prc->gray)), ry, rx,
	   rh, rw, rx, ry, rh, rw);
}

void
mif_signal_init (pr_context * prc)
{
  if (prc->fullpage)
    {
      mif_setgray (prc, 0.0);
      mif_box (prc, prc->MinX - 1, prc->MinY - 2, prc->MaxX + 1,
	       prc->MaxY + 2);
      mif_setgray (prc, 0.5);
      mif_box (prc, prc->MinX, prc->MinY - 1, prc->MaxX, prc->MaxY + 1);
    }
  mif_translate (prc, prc->MinX, prc->MinY);
}

void
mif_header (pr_context * prc)
{
  gdouble modified_skip;
  gdouble mif_skip;

  mif_skip =
    (prc->MaxX -
     prc->MinX) * (((gdouble) GLOBALS->pr_signal_fill_width_print_c_1) /
		   prc->xtotal);
  modified_skip = prc->MinX + mif_skip;

  fprintf (prc->handle,
	   "<MIFFile 5.00> # Generated by GTKWave\n"
	   "               # MIF support by Udi Finkelstein <udif@usa.net>\n"
	   "<Page\n");
  mif_translate (prc, modified_skip, prc->MinY);
}


void
mif_draw_line (pr_context * prc, gdouble _x1, gdouble _y1, gdouble x2,
	       gdouble y2)
{
  gdouble t_x1, t_y1, tx2, ty2;

  if (_x1 < -1.0)
    _x1 = -1.0;
  if (x2 < -1.0)
    x2 = -1.0;
  if (_x1 > 10000.0)
    _x1 = 10000.0;
  if (x2 > 10000.0)
    x2 = 10000.0;

  t_x1 = _x1 * prc->xscale + prc->tr_x;
  t_y1 = _y1 * prc->yscale + prc->tr_y;
  tx2 = x2 * prc->xscale + prc->tr_x;
  ty2 = y2 * prc->yscale + prc->tr_y;

  fprintf (prc->handle,
	   " <PolyLine\n"
	   "   <GroupID 1>\n"
	   "   <Pen 0>\n"
	   "   <PenWidth 0.5 pt>\n"
	   "   <ObTint %d%%>\n"
	   "   <Overprint No >\n"
	   "   <NumPoints 4>\n"
	   "   <Point  %d pt %d pt>\n"
	   "   <Point  %d pt %d pt>\n"
	   " > # end of PolyLine\n",
	   (int) (10000 * (1 - prc->gray)),
	   (int) (t_y1), (int) (prc->PageX * GLOBALS->inch_print_c_1 - t_x1),
	   (int) (ty2), (int) (prc->PageX * GLOBALS->inch_print_c_1 - tx2));
}


void
mif_draw_string (pr_context * prc, int x, int y, char *str, int xsize,
		 int ysize)
{
  int len;
  gdouble tx, ty;
  gdouble stretchx, stretchy;

  if (x < -1.0)
    x = -1.0;
  if (x > 10000.0)
    x = 10000.0;

  tx = x * prc->xscale + prc->tr_x;
  ty = y * prc->yscale + prc->tr_y;

  len = strlen (str);
  if (!len)
    return;

  stretchy = (ysize * (1.52 * prc->yscale));
  stretchx =
    (xsize / (len * stretchy)) * prc->xscale * WAVE_COURIER_SCALE_FAC *
    100.00;

  fprintf (prc->handle,
	   " <TextLine\n"
	   "  <Angle 90.0>\n"
	   "  <TLOrigin  %d pt %d pt>\n"
	   "  <TLAlignment Left>\n"
	   "  <Font\n"
	   "   <FTag `'>\n"
	   "   <FSize %3.0f pt>\n"
	   "   <FPairKern Yes>\n"
	   "   <FStretch %3.2f%%>\n"
	   "   <FFamily `Courier'>\n"
	   "   <FEncoding `FrameRoman'>\n"
	   "   <FLocked No>\n"
	   "  > # end of Font\n"
	   "  <String `%s'>\n"
	   " > # end of TextLine\n", (int) (ty),
	   (int) (prc->PageX * GLOBALS->inch_print_c_1 - tx), stretchy,
	   stretchx, str);
}


void
mif_trailer (pr_context * prc)
{
  fprintf (prc->handle,
	   " <Group\n" "  <ID 1>\n" "  <Overprint No>\n" " > # end of Group\n"
	   "> # end of Page\n");
}

/**********************************************/

/*
 * Initialize print related constants 
 */
static void
pr_wave_init (pr_context * prc)
{
  int wh = GLOBALS->waveheight;
  int yheight = 33 * GLOBALS->fontheight;

  prc->MinX = prc->LM * GLOBALS->inch_print_c_1;
  prc->MaxX = (prc->PageX - prc->RM) * GLOBALS->inch_print_c_1;
  prc->MinY = prc->BM * GLOBALS->inch_print_c_1;
  prc->MaxY = (prc->PageY - prc->TM) * GLOBALS->inch_print_c_1;

  if (!prc->fullpage)
    {
      if (wh < 2 * GLOBALS->fontheight)
	wh = 2 * GLOBALS->fontheight;
    }
  yheight = (wh < yheight) ? yheight : wh;
  yheight = yheight - (yheight % GLOBALS->fontheight);

  if (!prc->fullpage)
    {
      GLOBALS->ybound_print_c_1 =
	((prc->MaxY - prc->MinY) / ((gdouble) yheight)) * ((gdouble) (wh -
								      (wh %
								       GLOBALS->
								       fontheight)))
	+ prc->MinY;
    }
  prc->xtotal =
    (gdouble) (GLOBALS->wavewidth + GLOBALS->pr_signal_fill_width_print_c_1);
  prc->xscale = ((prc->MaxX - prc->MinX) / prc->xtotal);
  prc->yscale = (prc->MaxY - prc->MinY) / ((gdouble) yheight);

}

static int
ps_MaxSignalLength (void)
{
  Trptr t;
  int len = 0, maxlen = 0, numchars = 0;
  int vlen = 0;
  int i, trwhich, trtarget, num_traces_displayable;
  GtkAdjustment *sadj;
  char sbuf[128];
  int bufxlen;
  int bufclen;

  GLOBALS->ps_nummaxchars_print_c_1 = 7;	/* allows a good spacing if 60 pixel default
						 * is used */

  sadj = GTK_ADJUSTMENT (GLOBALS->wave_vslider);
  trtarget = (int) (sadj->value);

  t = GLOBALS->traces.first;
  trwhich = 0;
  while (t)
    {
      if ((trwhich < trtarget) && (GiveNextTrace (t)))
	{
	  trwhich++;
	  t = GiveNextTrace (t);
	}
      else
	{
	  break;
	}
    }

  num_traces_displayable =
    GLOBALS->signalarea->allocation.height / GLOBALS->fontheight;

  for (i = 0; (i < num_traces_displayable) && (t); i++)
    {

      sbuf[0] = 0;
      bufxlen = bufclen = 0;
      if ((GLOBALS->shift_timebase = t->shift))
	{
	  sbuf[0] = '(';
	  reformat_time (sbuf + 1, t->shift, GLOBALS->time_dimension);
	  strcpy (sbuf + (bufclen = strlen (sbuf + 1) + 1), ")");
	  bufclen++;
	  bufxlen = font_engine_string_measure (GLOBALS->signalfont, sbuf);
	}

      if ((!t->vector) && (t->n.nd) && (t->n.nd->array_height))
	{
	  bufclen +=
	    sprintf (sbuf + strlen (sbuf), "{%d}", t->n.nd->this_row);
	  bufxlen = font_engine_string_measure (GLOBALS->signalfont, sbuf);
	}

      if (t->flags & (TR_BLANK | TR_ANALOG_BLANK_STRETCH))
	{
	  if (t->name)
	    {
	      len =
		font_engine_string_measure (GLOBALS->signalfont, t->name) + bufxlen;
	      numchars = strlen (t->name) + bufclen;

	      if (len > maxlen)
		maxlen = len;
	      if (numchars > GLOBALS->ps_nummaxchars_print_c_1)
		GLOBALS->ps_nummaxchars_print_c_1 = numchars;
	    }
	}
      else if (t->name)
	{
	  len = font_engine_string_measure (GLOBALS->signalfont, t->name) + bufxlen;
	  numchars = strlen (t->name) + bufclen;
	  if ((GLOBALS->tims.marker != -1) && (!(t->flags & TR_EXCLUDE)))
	    {
	      t->asciitime = GLOBALS->tims.marker;
	      if (t->asciivalue)
		free_2 (t->asciivalue);

	      if (t->vector)
		{
		  char *str, *str2;
		  vptr v;

		  v = bsearch_vector (t->n.vec, GLOBALS->tims.marker);
		  str = convert_ascii (t, v);
		  if (str)
		    {
		      int slen;

		      str2 = (char *) malloc_2 (strlen (str) + 2);
		      *str2 = '=';
		      strcpy (str2 + 1, str);
		      free_2 (str);

		      t->asciivalue = str2;
		      if ((slen = strlen (str2)) > GLOBALS->ps_maxveclen)
			{
			  str2[GLOBALS->ps_maxveclen] = 0;
			  str2[GLOBALS->ps_maxveclen - 1] = '+';
			  vlen =
			    font_engine_string_measure (GLOBALS->signalfont, str2);
			  numchars += GLOBALS->ps_maxveclen;
			}
		      else
			{
			  vlen =
			    font_engine_string_measure (GLOBALS->signalfont, str2);
			  numchars += slen;
			}
		    }
		  else
		    {
		      vlen = 0;
		      t->asciivalue = NULL;
		    }
		}
	      else
		{
		  char *str;
		  hptr h_ptr;

		  if ((h_ptr = bsearch_node (t->n.nd, GLOBALS->tims.marker)))
		    {
		      if (!t->n.nd->ext)
			{
			  str = (char *) calloc_2 (1, 3 * sizeof (char));
			  str[0] = '=';
			  if (t->flags & TR_INVERT)
			    {
			      str[1] = AN_STR_INV[h_ptr->v.h_val];
			    }
			  else
			    {
			      str[1] = AN_STR[h_ptr->v.h_val];
			    }
			  t->asciivalue = str;
			  vlen =
			    font_engine_string_measure (GLOBALS->signalfont, str);
			  numchars += 2;
			}
		      else
			{
			  char *str2;

			  if (h_ptr->flags & HIST_REAL)
			    {
			      if (!(h_ptr->flags & HIST_STRING))
				{
				  str =
				    convert_ascii_real ((double *) h_ptr->v.
							h_vector);
				}
			      else
				{
				  str =
				    convert_ascii_string ((char *) h_ptr->v.
							  h_vector);
				}
			    }
			  else
			    {
			      str = convert_ascii_vec (t, h_ptr->v.h_vector);
			    }
			  if (str)
			    {
			      int slen;

			      str2 = (char *) malloc_2 (strlen (str) + 2);
			      *str2 = '=';
			      strcpy (str2 + 1, str);
			      free_2 (str);

			      t->asciivalue = str2;
			      if ((slen =
				   strlen (str2)) > GLOBALS->ps_maxveclen)
				{
				  str2[GLOBALS->ps_maxveclen] = 0;
				  str2[GLOBALS->ps_maxveclen - 1] = '+';
				  vlen =
				    font_engine_string_measure (GLOBALS->signalfont,
							str2);
				  numchars += GLOBALS->ps_maxveclen;
				}
			      else
				{
				  vlen =
				    font_engine_string_measure (GLOBALS->signalfont,
							str2);
				  numchars += slen;
				}
			    }
			  else
			    {
			      vlen = 0;
			      t->asciivalue = NULL;
			    }
			}
		    }
		  else
		    {
		      vlen = 0;
		      t->asciivalue = NULL;
		    }
		}

	      len += vlen;
	    }
	  if (len > maxlen)
	    maxlen = len;
	  if (numchars > GLOBALS->ps_nummaxchars_print_c_1)
	    GLOBALS->ps_nummaxchars_print_c_1 = numchars;
	}
      t = GiveNextTrace (t);
    }

  maxlen += 6;			/* endcap padding */
  if (maxlen < 60)
    maxlen = 60;

  return maxlen;
}

/**********************************************/

static void
pr_renderhash (pr_context * prc, int x, TimeType tim)
{
  TimeType rborder;
  gdouble dx;
  gdouble hashoffset;
  int fhminus2;
  int rhs;
  int iter = 0;

  fhminus2 = GLOBALS->fontheight - 2;

  pr_setgray (prc, 0.75);

  pr_draw_line (prc, x, 0,
		x, ((!GLOBALS->timearray) && (GLOBALS->display_grid)
		    && (GLOBALS->enable_vert_grid)) ? GLOBALS->
		liney_max : fhminus2);

  if (tim == GLOBALS->tims.last)
    return;

  rborder = (GLOBALS->tims.last - GLOBALS->tims.start) * GLOBALS->pxns;
  DEBUG (printf ("Rborder: %lld, Wavewidth: %d\n", rborder, wavewidth));

  if (rborder > GLOBALS->wavewidth)
    rborder = GLOBALS->wavewidth;
  if ((rhs = x + GLOBALS->pixelsperframe) > rborder)
    rhs = rborder;
  pr_draw_line (prc, x, GLOBALS->wavecrosspiece, rhs,
		GLOBALS->wavecrosspiece);


  dx = x + (hashoffset = GLOBALS->hashstep);
  x = dx;

  while ((hashoffset < GLOBALS->pixelsperframe) && (x <= rhs) && (iter < 10))
    {
      pr_draw_line (prc, x, GLOBALS->wavecrosspiece, x, fhminus2);

      hashoffset += GLOBALS->hashstep;
      dx = dx + GLOBALS->hashstep;
      if ((GLOBALS->pixelsperframe != 200) || (GLOBALS->hashstep != 10.0))
	iter++;			/* fix any roundoff errors */
      x = dx;
    }

}


static void
pr_renderblackout (pr_context * prc)
{
  gfloat pageinc;
  TimeType lhs, rhs, lclip, rclip;
  struct blackout_region_t *bt = GLOBALS->blackout_regions;

  if (bt)
    {
      pageinc = (gfloat) (((gdouble) GLOBALS->wavewidth) * GLOBALS->nspx);
      lhs = GLOBALS->tims.start;
      rhs = pageinc + lhs;

      while (bt)
	{
	  if((bt->bend < lhs) || (bt->bstart > rhs))
	    {
  	      /* nothing */
	    }
	    else
	    {
	      lclip = bt->bstart;
	      rclip = bt->bend;

	      if (lclip < lhs)
		lclip = lhs;
	      else if (lclip > rhs)
		lclip = rhs;

	      if (rclip < lhs)
		rclip = lhs;

	      lclip -= lhs;
	      rclip -= lhs;
              if(rclip>((GLOBALS->wavewidth+1)*GLOBALS->nspx)) 
                rclip = (GLOBALS->wavewidth+1)*(GLOBALS->nspx);

	      pr_setgray (prc, 0.80);
	      pr_draw_box (prc,
			   (((gdouble) lclip) * GLOBALS->pxns),
			   GLOBALS->fontheight,
			   (((gdouble) (rclip)) * GLOBALS->pxns),
			   GLOBALS->waveheight - GLOBALS->fontheight);
	    }

	  bt = bt->next;
	}
    }
}


static void
pr_rendertimes (pr_context * prc)
{
  int lastx = -1000;		/* arbitrary */
  TimeType tim, rem;
  int x, len, lenhalf;
  char timebuff[32];

  pr_renderblackout (prc);

  tim = GLOBALS->tims.start;
  GLOBALS->tims.end =
    GLOBALS->tims.start + (((gdouble) GLOBALS->wavewidth) * GLOBALS->nspx);

	/**********/
  if (GLOBALS->timearray)
    {
      int pos, pos2;
      TimeType *t, tm;
      int y = GLOBALS->fontheight + 2;
      int oldx = -1;

      pos = bsearch_timechain (GLOBALS->tims.start);
    top:
      if ((pos >= 0) && (pos < GLOBALS->timearray_size))
	{
	  pr_setgray (prc, 0.90);
	  t = GLOBALS->timearray + pos;
	  for (; pos < GLOBALS->timearray_size; t++, pos++)
	    {
	      tm = *t;
	      if (tm >= GLOBALS->tims.start)
		{
		  if (tm <= GLOBALS->tims.end)
		    {
		      x = (tm - GLOBALS->tims.start) * GLOBALS->pxns;
		      if (oldx == x)
			{
			  pos2 =
			    bsearch_timechain (GLOBALS->tims.start +
					       (((gdouble) (x + 1)) *
						GLOBALS->nspx));
			  if (pos2 > pos)
			    {
			      pos = pos2;
			      goto top;
			    }
			  else
			    continue;
			}
		      oldx = x;
		      pr_draw_line (prc, x, y, x, GLOBALS->liney_max);
		    }
		  else
		    {
		      break;
		    }
		}
	    }
	}
    }
	/**********/

  DEBUG (printf
	 ("Ruler Start time: " TTFormat ", Finish time: " TTFormat "\n",
	  tims.start, tims.end));

  x = 0;
  if (tim)
    {
      rem = tim % GLOBALS->nsperframe;
      if (rem)
	{
	  tim = tim - GLOBALS->nsperframe - rem;
	  x =
	    -GLOBALS->pixelsperframe -
	    ((rem * GLOBALS->pixelsperframe) / GLOBALS->nsperframe);
	}
    }
  for (;;)
    {
      pr_renderhash (prc, x, tim);
      if (tim)
	{
	  reformat_time (timebuff, time_trunc (tim), GLOBALS->time_dimension);
	}
      else
	{
	  strcpy (timebuff, "0");
	}

      len = font_engine_string_measure (GLOBALS->wavefont, timebuff) >> 1;
      lenhalf = len >> 1;

      if ((prc->gpd == &ps_print_device)
	  || ((x - lenhalf >= 0) && (x + lenhalf < GLOBALS->wavewidth)))
	{
	  pr_setgray (prc, 0.0);
	  if ((x - lenhalf >= lastx) || (GLOBALS->pixelsperframe >= 200))
	    {
	      pr_draw_string (prc, x - lenhalf, GLOBALS->wavefont->ascent + 1,
			      timebuff, len, GLOBALS->wavefont->ascent);
	    }
	}
      lastx = x + lenhalf;

      tim += GLOBALS->nsperframe;
      x += GLOBALS->pixelsperframe;
      if ((x >= GLOBALS->wavewidth) || (tim > GLOBALS->tims.last))
	break;
    }
}

/*************************************************/

static void
pr_draw_named_markers (pr_context * prc)
{
  gdouble pixstep;
  gint xl, y;
  int i;
  TimeType t;

  for (i = 0; i < 26; i++)
    {
      if ((t = GLOBALS->named_markers[i]) != -1)
	{
	  if ((t >= GLOBALS->tims.start) && (t <= GLOBALS->tims.last)
	      && (t <= GLOBALS->tims.end))
	    {

	      /* this needs to be here rather than outside the loop as gcc does some
	         optimizations that cause it to calculate slightly different from the marker if it's not here */
	      pixstep =
		((gdouble) GLOBALS->nsperframe) /
		((gdouble) GLOBALS->pixelsperframe);

	      xl = ((gdouble) (t - GLOBALS->tims.start)) / pixstep;	/* snap to integer */
	      if ((xl >= 0) && (xl < GLOBALS->wavewidth))
		{
		  char nbuff[2];
		  int xsize;
		  nbuff[0] = 'A' + i;
		  nbuff[1] = 0x00;

		  pr_setgray (prc, 0.40);
		  for (y = GLOBALS->fontheight - 1;
		       y <= GLOBALS->liney_max - 5; y += 8)
		    {
		      pr_draw_line (prc, xl, y, xl, y + 5);
		    }

		  if((!GLOBALS->marker_names[i])||(!GLOBALS->marker_names[i][0]))
			{
		  	xsize = font_engine_string_measure (GLOBALS->wavefont, nbuff);
		  	pr_setgray (prc, 0.00);
		  	pr_draw_string (prc,
				  xl - (xsize >> 1) + 1,
				  GLOBALS->fontheight - 1, nbuff,
				  xsize,
				  (prc->gpd ==
				   &ps_print_device) ? GLOBALS->wavefont->
				  ascent / 2 : GLOBALS->wavefont->ascent);
			}
			else
			{
		  	xsize = font_engine_string_measure (GLOBALS->wavefont, GLOBALS->marker_names[i]);
		  	pr_setgray (prc, 0.00);
		  	pr_draw_string (prc,
				  xl - (xsize >> 1) + 1,
				  GLOBALS->fontheight - 1, GLOBALS->marker_names[i],
				  xsize,
				  (prc->gpd ==
				   &ps_print_device) ? GLOBALS->wavefont->
				  ascent / 2 : GLOBALS->wavefont->ascent);
			}
		}
	    }
	}
    }
}


static void
pr_draw_marker (pr_context * prc)
{
  gdouble pixstep;
  gint xl;

  if (GLOBALS->tims.baseline != -1)
    {
      if ((GLOBALS->tims.baseline >= GLOBALS->tims.start)
	  && (GLOBALS->tims.baseline <= GLOBALS->tims.last)
	  && (GLOBALS->tims.baseline <= GLOBALS->tims.end))
	{
	  pixstep =
	    ((gdouble) GLOBALS->nsperframe) /
	    ((gdouble) GLOBALS->pixelsperframe);
	  xl = ((gdouble) (GLOBALS->tims.baseline - GLOBALS->tims.start)) / pixstep;	/* snap to integer */
	  if ((xl >= 0) && (xl < GLOBALS->wavewidth))
	    {
	      pr_setgray (prc, 0.65);
	      pr_draw_line (prc, xl, GLOBALS->fontheight - 1, xl,
			    GLOBALS->liney_max);
	    }
	}
    }

  if (GLOBALS->tims.marker != -1)
    {
      if ((GLOBALS->tims.marker >= GLOBALS->tims.start)
	  && (GLOBALS->tims.marker <= GLOBALS->tims.last)
	  && (GLOBALS->tims.marker <= GLOBALS->tims.end))
	{
	  pixstep =
	    ((gdouble) GLOBALS->nsperframe) /
	    ((gdouble) GLOBALS->pixelsperframe);
	  xl = ((gdouble) (GLOBALS->tims.marker - GLOBALS->tims.start)) / pixstep;	/* snap to integer */
	  if ((xl >= 0) && (xl < GLOBALS->wavewidth))
	    {
	      pr_setgray (prc, 0.40);
	      pr_draw_line (prc, xl, GLOBALS->fontheight - 1, xl,
			    GLOBALS->liney_max);
	    }
	}
    }
}

/*************************************************/

/*
 * draw single traces and use this for rendering the grid lines for
 * "excluded" traces 
 */
static void
pr_draw_hptr_trace (pr_context * prc, Trptr t, hptr h, int which, int dodraw,
		    int kill_grid)
{
  TimeType _x0, _x1, newtime;
  int _y0, _y1, yu, liney, ytext, ysiz;
  TimeType tim, h2tim;
  hptr h2, h3;
  char hval, h2val;
  char identifier_str[2];

  GLOBALS->tims.start -= GLOBALS->shift_timebase;
  GLOBALS->tims.end -= GLOBALS->shift_timebase;

  liney = ((which + 2) * GLOBALS->fontheight) - 2;
  if (t && (t->flags & TR_INVERT))
    {
      _y0 = ((which + 1) * GLOBALS->fontheight) + 2;
      _y1 = liney - 2;
    }
  else
    {
      _y1 = ((which + 1) * GLOBALS->fontheight) + 2;
      _y0 = liney - 2;
    }

  yu = (_y0 + _y1) / 2;
  ytext = yu - (GLOBALS->wavefont->ascent / 2) + GLOBALS->wavefont->ascent;
  ysiz = GLOBALS->wavefont->ascent - 1;

  if ((GLOBALS->display_grid) && (GLOBALS->enable_horiz_grid) && (!kill_grid))
    {
      pr_setgray (prc, 0.75);
      pr_draw_line (prc,
		    (GLOBALS->tims.start <
		     GLOBALS->tims.first) ? (GLOBALS->tims.first -
					     GLOBALS->tims.start) *
		    GLOBALS->pxns : 0, liney,
		    (GLOBALS->tims.last <=
		     GLOBALS->tims.end) ? (GLOBALS->tims.last -
					   GLOBALS->tims.start) *
		    GLOBALS->pxns : GLOBALS->wavewidth - 1, liney);
    }
  pr_setgray (prc, 0.0);
  if ((h) && (GLOBALS->tims.start == h->time))
    {
      pr_draw_line (prc, 0, _y0, 0, _y1);
    }
  if (dodraw)
    for (;;)
      {
	if (!h)
	  break;
	tim = (h->time);
	if ((tim > GLOBALS->tims.end) || (tim > GLOBALS->tims.last))
	  break;

	_x0 = (tim - GLOBALS->tims.start) * GLOBALS->pxns;
	if (_x0 < -1)
	  {
	    _x0 = -1;
	  }
	else if (_x0 > GLOBALS->wavewidth)
	  {
	    break;
	  }
	h2 = h->next;
	if (!h2)
	  break;
	h2tim = tim = (h2->time);
	if (tim > GLOBALS->tims.last)
	  tim = GLOBALS->tims.last;
	else if (tim > GLOBALS->tims.end + 1)
	  tim = GLOBALS->tims.end + 1;
	_x1 = (tim - GLOBALS->tims.start) * GLOBALS->pxns;
	if (_x1 < -1)
	  {
	    _x1 = -1;
	  }
	else if (_x1 > GLOBALS->wavewidth)
	  {
	    _x1 = GLOBALS->wavewidth;
	  }
	if (_x0 != _x1)
	  {
	    hval = h->v.h_val;
	    h2val = h2->v.h_val;

	    switch (hval)
	      {
	      case AN_0:	/* 0 */
	      case AN_L:	/* 0 */
		pr_draw_line (prc, _x0, _y0, _x1, _y0);

		if (h2tim <= GLOBALS->tims.end)
		  switch (h2val)
		    {
		    case AN_0:
		    case AN_L:
		      break;

		    case AN_Z:
		      pr_draw_line (prc, _x1, _y0, _x1, yu);
		      break;
		    default:
		      pr_draw_line (prc, _x1, _y0, _x1, _y1);
		      break;
		    }
		break;

	      case AN_X:	/* X */
	      case AN_W:	/* X */
	      case AN_U:	/* X */
	      case AN_DASH:	/* X */
		pr_setgray (prc, 0.70);
		pr_draw_box (prc, _x0, _y0, _x1, _y1);
		pr_setgray (prc, 0.0);

		identifier_str[1] = 0;
		switch (hval)
		  {
		  case AN_X:
		    identifier_str[0] = 0;
		    break;
		  case AN_W:
		    identifier_str[0] = 'W';
		    break;
		  case AN_U:
		    identifier_str[0] = 'U';
		    break;
		  default:
		    identifier_str[0] = '-';
		    break;
		  }

		if (identifier_str[0])
		  {
		    int _x0_new = (_x0 >= 0) ? _x0 : 0;
		    int width;
		    int pixlen = 0;

		    if ((width = _x1 - _x0_new) > GLOBALS->vector_padding)
		      {
			if ((_x1 >= GLOBALS->wavewidth)
			    ||
			    ((pixlen =
			      font_engine_string_measure (GLOBALS->wavefont,
						  identifier_str)) +
			     GLOBALS->vector_padding <= width))
			  {
			    pr_draw_string (prc, _x0 + 2, ytext,
					    identifier_str, pixlen, ysiz);
			  }
		      }
		  }

		if (_x0 >= 0)
		  pr_draw_line (prc, _x0, _y0, _x0, _y1);

		pr_draw_line (prc, _x0, _y0, _x1, _y0);
		pr_draw_line (prc, _x0, _y1, _x1, _y1);
		if (h2tim <= GLOBALS->tims.end)
		  pr_draw_line (prc, _x1, _y0, _x1, _y1);
		break;

	      case AN_Z:	/* Z */
		pr_draw_line (prc, _x0, yu, _x1, yu);
		if (h2tim <= GLOBALS->tims.end)
		  switch (h2val)
		    {
		    case AN_0:
		    case AN_L:
		      pr_draw_line (prc, _x1, yu, _x1, _y0);
		      break;
		    case AN_1:
		    case AN_H:
		      pr_draw_line (prc, _x1, yu, _x1, _y1);
		      break;
		    default:
		      pr_draw_line (prc, _x1, _y0, _x1, _y1);
		      break;
		    }
		break;

	      case AN_1:	/* 1 */
	      case AN_H:	/* 1 */
		pr_draw_line (prc, _x0, _y1, _x1, _y1);
		if (h2tim <= GLOBALS->tims.end)
		  switch (h2val)
		    {
		    case AN_1:
		    case AN_H:
		      break;

		    case AN_0:
		    case AN_L:
		      pr_draw_line (prc, _x1, _y1, _x1, _y0);
		      break;
		    case AN_Z:
		      pr_draw_line (prc, _x1, _y1, _x1, yu);
		      break;
		    default:
		      pr_draw_line (prc, _x1, _y0, _x1, _y1);
		      break;
		    }
		break;

	      default:
		break;
	      }
	  }
	else
	  {
	    pr_draw_line (prc, _x1, _y0, _x1, _y1);
	    newtime = (((gdouble) (_x1 + WAVE_OPT_SKIP)) * GLOBALS->nspx) + GLOBALS->tims.start + GLOBALS->shift_timebase;	/* skip to next pixel */
	    h3 = bsearch_node (t->n.nd, newtime);
	    if (h3->time > h->time)
	      {
		h = h3;
		continue;
	      }
	  }

	h = h->next;
      }

  GLOBALS->tims.start += GLOBALS->shift_timebase;
  GLOBALS->tims.end += GLOBALS->shift_timebase;
}


/*
 * draw hptr vectors (integer+real) 
 */
static void
pr_draw_hptr_trace_vector_analog (pr_context * prc, Trptr t, hptr h,
				  int which, int num_extension)
{
  TimeType _x0, _x1, newtime;
  int _y0, _y1, yu, liney, ytext, yt0, yt1;
  TimeType tim, h2tim;
  hptr h2, h3;
  int endcnt = 0;
  int ysiz;
  int type;
  int lasttype = -1;
  double mynan = strtod ("NaN", NULL);
  double tmin = mynan, tmax = mynan, tv, tv2;
  int is_nan = 0, is_nan2 = 0, is_inf = 0, is_inf2 = 0;
  int any_infs = 0, any_infp = 0, any_infm = 0;

  liney = ((which + 2 + num_extension) * GLOBALS->fontheight) - 2;
  _y1 = ((which + 1) * GLOBALS->fontheight) + 2;
  _y0 = liney - 2;
  yu = (_y0 + _y1) / 2;
  ytext = yu - (GLOBALS->wavefont->ascent / 2) + GLOBALS->wavefont->ascent;

  ysiz = GLOBALS->wavefont->ascent - 1;
  if (ysiz < 1)
    ysiz = 1;

  if (t->flags & TR_ANALOG_FULLSCALE)	/* otherwise use dynamic */
    {
      if (!t->minmax_valid)
	{
	  h3 = &t->n.nd->head;
	  for (;;)
	    {
	      if (!h3)
		break;

	      if ((h3->time >= GLOBALS->tims.first)
		  && (h3->time <= GLOBALS->tims.last))
		{
		  tv = mynan;
		  if (h3->flags & HIST_REAL)
		    {
		      if (!(h3->flags & HIST_STRING) && h3->v.h_vector)
			tv = *(double *) h3->v.h_vector;
		    }
		  else
		    {
		      if (h3->time <= GLOBALS->tims.last)
			tv = convert_real_vec (t, h3->v.h_vector);
		    }
		  if (!isnan (tv) && !isinf (tv))
		    {
		      if (isnan (tmin) || tv < tmin)
			tmin = tv;
		      if (isnan (tmax) || tv > tmax)
			tmax = tv;
		    }
		  else if (isinf (tv))
		    {
		      any_infs = 1;

		      if (tv > 0)
			{
			  any_infp = 1;
			}
		      else
			{
			  any_infm = 1;
			}
		    }
		}
	      h3 = h3->next;
	    }

	  if (isnan (tmin) || isnan (tmax))
	    tmin = tmax = 0;

	  if (any_infs)
	    {
	      double tdelta = (tmax - tmin) * WAVE_INF_SCALING;

	      if (any_infp)
		tmax = tmax + tdelta;
	      if (any_infm)
		tmin = tmin - tdelta;
	    }

	  if ((tmax - tmin) < 1e-20)
	    {
	      tmax = 1;
	      tmin -= 0.5 * (_y1 - _y0);
	    }
	  else
	    {
	      tmax = (_y1 - _y0) / (tmax - tmin);
	    }

	  t->minmax_valid = 1;
	  t->d_minval = tmin;
	  t->d_maxval = tmax;
	}
      else
	{
	  tmin = t->d_minval;
	  tmax = t->d_maxval;
	}
    }
  else
    {
      h3 = h;
      for (;;)
	{
	  if (!h3)
	    break;
	  tim = h3->time;
	  if (tim > GLOBALS->tims.end)
	    {
	      endcnt++;
	      if (endcnt == 2)
		break;
	    }
	  if (tim > GLOBALS->tims.last)
	    break;
	  _x0 = (tim - GLOBALS->tims.start) * GLOBALS->pxns;
	  if ((_x0 > GLOBALS->wavewidth) && (endcnt == 2))
	    break;

	  tv = mynan;
	  if (h3->flags & HIST_REAL)
	    {
	      if (!(h3->flags & HIST_STRING) && h3->v.h_vector)
		tv = *(double *) h3->v.h_vector;
	    }
	  else
	    {
	      if (h3->time <= GLOBALS->tims.last)
		tv = convert_real_vec (t, h3->v.h_vector);
	    }
	  if (!isnan (tv) && !isinf (tv))
	    {
	      if (isnan (tmin) || tv < tmin)
		tmin = tv;
	      if (isnan (tmax) || tv > tmax)
		tmax = tv;
	    }
	  else if (isinf (tv))
	    {
	      any_infs = 1;
	      if (tv > 0)
		{
		  any_infp = 1;
		}
	      else
		{
		  any_infm = 1;
		}
	    }

	  h3 = h3->next;
	}
      if (isnan (tmin) || isnan (tmax))
	tmin = tmax = 0;

      if (any_infs)
	{
	  double tdelta = (tmax - tmin) * WAVE_INF_SCALING;

	  if (any_infp)
	    tmax = tmax + tdelta;
	  if (any_infm)
	    tmin = tmin - tdelta;
	}

      if ((tmax - tmin) < 1e-20)
	{
	  tmax = 1;
	  tmin -= 0.5 * (_y1 - _y0);
	}
      else
	{
	  tmax = (_y1 - _y0) / (tmax - tmin);
	}
    }

  pr_setgray (prc, 0.0);
  for (;;)
    {
      if (!h)
	break;
      tim = (h->time);
      if ((tim > GLOBALS->tims.end) || (tim > GLOBALS->tims.last))
	break;

      _x0 = (tim - GLOBALS->tims.start) * GLOBALS->pxns;
      /*
         if (_x0 < -1)
         _x0 = -1;
         else if (_x0 > GLOBALS->wavewidth)
         break;
       */

      h2 = h->next;
      if (!h2)
	break;
      h2tim = tim = (h2->time);
      if (tim > GLOBALS->tims.last)
	tim = GLOBALS->tims.last;
      /* else if (tim > GLOBALS->tims.end + 1)
         tim = GLOBALS->tims.end + 1; */
      _x1 = (tim - GLOBALS->tims.start) * GLOBALS->pxns;
      /*
         if (_x1 < -1)
         _x1 = -1;
         else if (_x1 > GLOBALS->wavewidth)
         _x1 = GLOBALS->wavewidth;
       */

      /* draw trans */
      type =
	(!(h->flags & (HIST_REAL | HIST_STRING))) ? vtype (t,
							   h->v.
							   h_vector) : AN_0;
      tv = tv2 = mynan;
      if (h->flags & HIST_REAL)
	{
	  if (!(h->flags & HIST_STRING) && h->v.h_vector)
	    tv = *(double *) h->v.h_vector;
	}
      else
	{
	  if (h->time <= GLOBALS->tims.last)
	    tv = convert_real_vec (t, h->v.h_vector);
	}
      if (h2->flags & HIST_REAL)
	{
	  if (!(h2->flags & HIST_STRING) && h2->v.h_vector)
	    tv2 = *(double *) h2->v.h_vector;
	}
      else
	{
	  if (h2->time <= GLOBALS->tims.last)
	    tv2 = convert_real_vec (t, h2->v.h_vector);
	}

      if ((is_inf = isinf (tv)))
	{
	  if (tv < 0)
	    {
	      yt0 = _y0;
	    }
	  else
	    {
	      yt0 = _y1;
	    }
	}
      else if ((is_nan = isnan (tv)))
	{
	  yt0 = yu;
	}
      else
	{
	  yt0 = _y0 + (tv - tmin) * tmax;
	}

      if ((is_inf2 = isinf (tv2)))
	{
	  if (tv2 < 0)
	    {
	      yt1 = _y0;
	    }
	  else
	    {
	      yt1 = _y1;
	    }
	}
      else if ((is_nan2 = isnan (tv2)))
	{
	  yt1 = yu;
	}
      else
	{
	  yt1 = _y0 + (tv2 - tmin) * tmax;
	}

      if (_x0 != _x1)
	{
	  if (h->next)
	    {
	      if (h->next->time > GLOBALS->max_time)
		{
		  yt1 = yt0;
		}
	    }

	  if ((is_nan2) && (h2tim > GLOBALS->max_time))
	    is_nan2 = 0;

	  if (is_nan || is_nan2)
	    {
	      if (is_nan)
		{
		  pr_setgray (prc, 0.70);
		  pr_draw_box (prc, _x0, _y1, _x1, _y0);
		  pr_setgray (prc, 0.0);

		  if ((t->
		       flags & (TR_ANALOG_INTERPOLATED | TR_ANALOG_STEP)) ==
		      (TR_ANALOG_INTERPOLATED | TR_ANALOG_STEP))
		    {
		      pr_draw_line (prc, _x1 - 1, yt1, _x1 + 1, yt1);
		      pr_draw_line (prc, _x1, yt1 - 1, _x1, yt1 + 1);

		      pr_draw_line (prc, _x0 - 1, _y0, _x0 + 1, _y0);
		      pr_draw_line (prc, _x0, _y0 - 1, _x0, _y0 + 1);

		      pr_draw_line (prc, _x0 - 1, _y1, _x0 + 1, _y1);
		      pr_draw_line (prc, _x0, _y1 - 1, _x0, _y1 + 1);
		    }
		}
	      if (is_nan2)
		{
		  pr_draw_line (prc, _x0, yt0, _x1, yt0);

		  if ((t->
		       flags & (TR_ANALOG_INTERPOLATED | TR_ANALOG_STEP)) ==
		      (TR_ANALOG_INTERPOLATED | TR_ANALOG_STEP))
		    {
		      pr_setgray (prc, 0.70);
		      pr_draw_line (prc, _x1, _y1, _x1, _y0);
		      pr_setgray (prc, 0.0);

		      pr_draw_line (prc, _x1 - 1, _y0, _x1 + 1, _y0);
		      pr_draw_line (prc, _x1, _y0 - 1, _x1, _y0 + 1);

		      pr_draw_line (prc, _x1 - 1, _y1, _x1 + 1, _y1);
		      pr_draw_line (prc, _x1, _y1 - 1, _x1, _y1 + 1);
		    }
		}
	    }
	  else if (t->flags & TR_ANALOG_INTERPOLATED && !is_inf && !is_inf2)
	    {
	      pr_draw_line (prc, _x0, yt0, _x1, yt1);
	      if (t->flags & TR_ANALOG_STEP)
		{
		  pr_draw_line (prc, _x0 - 1, yt0, _x0 + 1, yt0);
		  pr_draw_line (prc, _x0, yt0 - 1, _x0, yt0 + 1);
		}
	    }
	  else			/* if (t->flags & TR_ANALOG_STEP) */
	    {
	      pr_draw_line (prc, _x0, yt0, _x1, yt0);
	      pr_draw_line (prc, _x1, yt0, _x1, yt1);

	      if ((t->flags & (TR_ANALOG_INTERPOLATED | TR_ANALOG_STEP)) ==
		  (TR_ANALOG_INTERPOLATED | TR_ANALOG_STEP))
		{
		  pr_draw_line (prc, _x0 - 1, yt0, _x0 + 1, yt0);
		  pr_draw_line (prc, _x0, yt0 - 1, _x0, yt0 + 1);
		}
	    }
	}
      else
	{
	  newtime = (((gdouble) (_x1 + WAVE_OPT_SKIP)) * GLOBALS->nspx) + GLOBALS->tims.start + GLOBALS->shift_timebase;	/* skip to next pixel */
	  h3 = bsearch_node (t->n.nd, newtime);
	  if (h3->time > h->time)
	    {
	      h = h3;
	      lasttype = type;
	      continue;
	    }
	}

      h = h->next;
      lasttype = type;
    }

  GLOBALS->tims.start += GLOBALS->shift_timebase;
  GLOBALS->tims.end += GLOBALS->shift_timebase;
}

static void
pr_draw_hptr_trace_vector (pr_context * prc, Trptr t, hptr h, int which)
{
  TimeType _x0, _x1, newtime, width;
  int _y0, _y1, yu, liney, ytext;
  TimeType tim, h2tim;
  hptr h2, h3;
  char *ascii = NULL;
  int pixlen, ysiz;
  int type;
  int lasttype = -1;

  GLOBALS->tims.start -= GLOBALS->shift_timebase;
  GLOBALS->tims.end -= GLOBALS->shift_timebase;

  liney = ((which + 2) * GLOBALS->fontheight) - 2;
  _y1 = ((which + 1) * GLOBALS->fontheight) + 2;
  _y0 = liney - 2;
  yu = (_y0 + _y1) / 2;
  ytext = yu - (GLOBALS->wavefont->ascent / 2) + GLOBALS->wavefont->ascent;

  ysiz = GLOBALS->wavefont->ascent - 1;
  if (ysiz < 1)
    ysiz = 1;


  if ((GLOBALS->display_grid) && (GLOBALS->enable_horiz_grid))
    {
      if ((t->flags & TR_ANALOGMASK) && (t->t_next)
	  && (t->t_next->flags & TR_ANALOG_BLANK_STRETCH))
	{
	}
      else
	{
	  pr_setgray (prc, 0.75);
	  pr_draw_line (prc,
			(GLOBALS->tims.start <
			 GLOBALS->tims.first) ? (GLOBALS->tims.first -
						 GLOBALS->tims.start) *
			GLOBALS->pxns : 0, liney,
			(GLOBALS->tims.last <=
			 GLOBALS->tims.end) ? (GLOBALS->tims.last -
					       GLOBALS->tims.start) *
			GLOBALS->pxns : GLOBALS->wavewidth - 1, liney);
	}
    }

  if ((t->flags & TR_ANALOGMASK)
      && (!(h->flags & HIST_STRING) || !(h->flags & HIST_REAL)))
    {
      Trptr te = t->t_next;
      int ext = 0;
      int num_traces_displayable =
	GLOBALS->signalarea->allocation.height / GLOBALS->fontheight;

      while (te)
	{
	  if (te->flags & TR_ANALOG_BLANK_STRETCH)
	    {
	      ext++;
	      te = te->t_next;
	    }
	  else
	    {
	      break;
	    }
	}

      if (which + ext > num_traces_displayable - 2)
	{
	  ext = num_traces_displayable - which - 2;
	  if (ext < 0)
	    ext = 0;		/* just in case of a one-off */
	}

      pr_draw_hptr_trace_vector_analog (prc, t, h, which, ext);
      return;
    }

  pr_setgray (prc, 0.0);
  for (;;)
    {
      if (!h)
	break;
      tim = (h->time);
      if ((tim > GLOBALS->tims.end) || (tim > GLOBALS->tims.last))
	break;

      _x0 = (tim - GLOBALS->tims.start) * GLOBALS->pxns;
      if (_x0 < -1)
	_x0 = -1;
      else if (_x0 > GLOBALS->wavewidth)
	break;

      h2 = h->next;
      if (!h2)
	break;
      h2tim = tim = (h2->time);
      if (tim > GLOBALS->tims.last)
	tim = GLOBALS->tims.last;
      else if (tim > GLOBALS->tims.end + 1)
	tim = GLOBALS->tims.end + 1;
      _x1 = (tim - GLOBALS->tims.start) * GLOBALS->pxns;

      if (_x1 < -1)
	_x1 = -1;
      else if (_x1 > GLOBALS->wavewidth)
	_x1 = GLOBALS->wavewidth;

      /* draw trans */
      type =
	(!(h->flags & (HIST_REAL | HIST_STRING))) ? vtype (t,
							   h->v.
							   h_vector) : AN_0;
      if (_x0 > -1)
	{
	  if (GLOBALS->use_roundcaps)
	    {
	      if (type == AN_Z)
		{
		  if (lasttype != -1)
		    {
		      pr_draw_line (prc, _x0 - 1, _y0, _x0, yu);
		      pr_draw_line (prc, _x0, yu, _x0 - 1, _y1);
		    }
		}
	      else if (lasttype == AN_Z)
		{
		  pr_draw_line (prc, _x0 + 1, _y0, _x0, yu);
		  pr_draw_line (prc, _x0, yu, _x0 + 1, _y1);
		}
	      else
		{
		  if (lasttype != type)
		    {
		      pr_draw_line (prc, _x0 - 1, _y0, _x0, yu);
		      pr_draw_line (prc, _x0, yu, _x0 - 1, _y1);
		      pr_draw_line (prc, _x0 + 1, _y0, _x0, yu);
		      pr_draw_line (prc, _x0, yu, _x0 + 1, _y1);
		    }
		  else
		    {
		      pr_draw_line (prc, _x0 - 2, _y0, _x0 + 2, _y1);
		      pr_draw_line (prc, _x0 + 2, _y0, _x0 - 2, _y1);
		    }
		}
	    }
	  else
	    {
	      pr_draw_line (prc, _x0, _y0, _x0, _y1);
	    }
	}

      if (_x0 != _x1)
	{
	  if (type == AN_Z)
	    {
	      if (GLOBALS->use_roundcaps)
		{
		  pr_draw_line (prc, _x0 + 1, yu, _x1 - 1, yu);
		}
	      else
		{
		  pr_draw_line (prc, _x0, yu, _x1, yu);
		}
	    }
	  else
	    {

	      if (GLOBALS->use_roundcaps)
		{
		  pr_draw_line (prc, _x0 + 2, _y0, _x1 - 2, _y0);
		  pr_draw_line (prc, _x0 + 2, _y1, _x1 - 2, _y1);
		}
	      else
		{
		  pr_draw_line (prc, _x0, _y0, _x1, _y0);
		  pr_draw_line (prc, _x0, _y1, _x1, _y1);
		}


	      if (_x0 < 0)
		_x0 = 0;		/* fixup left margin */

	      width = ((prc->gpd == &ps_print_device) || (_x1 < GLOBALS->wavewidth)) ? _x1 - _x0 : GLOBALS->wavewidth - _x0;	/* truncate render
																 * window for non-ps */

	      if (width > GLOBALS->vector_padding)
		{
		  if (h->flags & HIST_REAL)
		    {
		      if (!(h->flags & HIST_STRING))
			{
			  ascii =
			    convert_ascii_real ((double *) h->v.h_vector);
			}
		      else
			{
			  ascii =
			    convert_ascii_string ((char *) h->v.h_vector);
			}
		    }
		  else
		    {
		      ascii = convert_ascii_vec (t, h->v.h_vector);
		    }


		  if (((pixlen =
			font_engine_string_measure (GLOBALS->wavefont,
					    ascii)) +
		       GLOBALS->vector_padding <= width)
		      || ((_x1 >= GLOBALS->wavewidth)
			  && (prc->gpd == &ps_print_device)))
		    {
		      pr_draw_string (prc, _x0 + 2, ytext, ascii, pixlen,
				      ysiz);
		    }
		  else
		    {
		      char *mod;

		      mod =
			bsearch_trunc (ascii,
				       width - GLOBALS->vector_padding);
		      if (mod)
			{
			  *mod = '+';
			  *(mod + 1) = 0;
			  pr_draw_string (prc, _x0 + 2, ytext, ascii,
					  GLOBALS->maxlen_trunc, ysiz);
			}
		    }
		}
	    }
	}
      else
	{
	  newtime = (((gdouble) (_x1 + WAVE_OPT_SKIP)) * GLOBALS->nspx) + GLOBALS->tims.start + GLOBALS->shift_timebase;	/* skip to next pixel */
	  h3 = bsearch_node (t->n.nd, newtime);
	  if (h3->time > h->time)
	    {
	      h = h3;
	      lasttype = type;
	      continue;
	    }
	}

      if (ascii)
	{
	  free_2 (ascii);
	  ascii = NULL;
	}
      h = h->next;
      lasttype = type;
    }

  GLOBALS->tims.start += GLOBALS->shift_timebase;
  GLOBALS->tims.end += GLOBALS->shift_timebase;
}


/*
 * draw vector traces 
 */
static void
pr_draw_vptr_trace_analog (pr_context * prc, Trptr t, vptr v, int which,
			   int num_extension)
{
  TimeType _x0, _x1, newtime;
  int _y0, _y1, yu, liney, ytext, yt0, yt1;
  TimeType tim, h2tim;
  vptr h, h2, h3;
  int endcnt = 0;
  int ysiz;
  int type;
  int lasttype = -1;
  double mynan = strtod ("NaN", NULL);
  double tmin = mynan, tmax = mynan, tv, tv2;
  int is_nan = 0, is_nan2 = 0, is_inf = 0, is_inf2 = 0;
  int any_infs = 0, any_infp = 0, any_infm = 0;

  h = v;
  liney = ((which + 2 + num_extension) * GLOBALS->fontheight) - 2;
  _y1 = ((which + 1) * GLOBALS->fontheight) + 2;
  _y0 = liney - 2;
  yu = (_y0 + _y1) / 2;
  ytext = yu - (GLOBALS->wavefont->ascent / 2) + GLOBALS->wavefont->ascent;

  ysiz = GLOBALS->wavefont->ascent - 1;
  if (ysiz < 1)
    ysiz = 1;

  if (t->flags & TR_ANALOG_FULLSCALE)	/* otherwise use dynamic */
    {
      if (!t->minmax_valid)
	{
	  h3 = t->n.vec->vectors[0];
	  for (;;)
	    {
	      if (!h3)
		break;

	      if ((h3->time >= GLOBALS->tims.first)
		  && (h3->time <= GLOBALS->tims.last))
		{
		  tv = mynan;

		  tv = convert_real (t, h3);
		  if (!isnan (tv) && !isinf (tv))
		    {
		      if (isnan (tmin) || tv < tmin)
			tmin = tv;
		      if (isnan (tmax) || tv > tmax)
			tmax = tv;
		    }
		  else if (isinf (tv))
		    {
		      any_infs = 1;

		      if (tv > 0)
			{
			  any_infp = 1;
			}
		      else
			{
			  any_infm = 1;
			}
		    }
		}

	      h3 = h3->next;
	    }

	  if (isnan (tmin) || isnan (tmax))
	    tmin = tmax = 0;

	  if (any_infs)
	    {
	      double tdelta = (tmax - tmin) * WAVE_INF_SCALING;

	      if (any_infp)
		tmax = tmax + tdelta;
	      if (any_infm)
		tmin = tmin - tdelta;
	    }

	  if ((tmax - tmin) < 1e-20)
	    {
	      tmax = 1;
	      tmin -= 0.5 * (_y1 - _y0);
	    }
	  else
	    {
	      tmax = (_y1 - _y0) / (tmax - tmin);
	    }

	  t->minmax_valid = 1;
	  t->d_minval = tmin;
	  t->d_maxval = tmax;
	}
      else
	{
	  tmin = t->d_minval;
	  tmax = t->d_maxval;
	}
    }
  else
    {
      h3 = h;
      for (;;)
	{
	  if (!h3)
	    break;
	  tim = h3->time;

	  if (tim > GLOBALS->tims.end)
	    {
	      endcnt++;
	      if (endcnt == 2)
		break;
	    }
	  if (tim > GLOBALS->tims.last)
	    break;

	  _x0 = (tim - GLOBALS->tims.start) * GLOBALS->pxns;
	  if ((_x0 > GLOBALS->wavewidth) && (endcnt == 2))
	    {
	      break;
	    }

	  tv = convert_real (t, h3);
	  if (!isnan (tv) && !isinf (tv))
	    {
	      if (isnan (tmin) || tv < tmin)
		tmin = tv;
	      if (isnan (tmax) || tv > tmax)
		tmax = tv;
	    }
	  else if (isinf (tv))
	    {
	      any_infs = 1;
	      if (tv > 0)
		{
		  any_infp = 1;
		}
	      else
		{
		  any_infm = 1;
		}
	    }

	  h3 = h3->next;
	}
      if (isnan (tmin) || isnan (tmax))
	tmin = tmax = 0;

      if (any_infs)
	{
	  double tdelta = (tmax - tmin) * WAVE_INF_SCALING;

	  if (any_infp)
	    tmax = tmax + tdelta;
	  if (any_infm)
	    tmin = tmin - tdelta;
	}

      if ((tmax - tmin) < 1e-20)
	{
	  tmax = 1;
	  tmin -= 0.5 * (_y1 - _y0);
	}
      else
	{
	  tmax = (_y1 - _y0) / (tmax - tmin);
	}
    }

  pr_setgray (prc, 0.0);
  for (;;)
    {
      if (!h)
	break;
      tim = (h->time);
      if ((tim > GLOBALS->tims.end) || (tim > GLOBALS->tims.last))
	break;

      _x0 = (tim - GLOBALS->tims.start) * GLOBALS->pxns;
      /*
         if (_x0 < -1)
         _x0 = -1;
         else if (_x0 > GLOBALS->wavewidth)
         break;
       */

      h2 = h->next;
      if (!h2)
	break;
      h2tim = tim = (h2->time);
      if (tim > GLOBALS->tims.last)
	tim = GLOBALS->tims.last;
      /* else if (tim > GLOBALS->tims.end + 1)
         tim = GLOBALS->tims.end + 1; */
      _x1 = (tim - GLOBALS->tims.start) * GLOBALS->pxns;
      /*
         if (_x1 < -1)
         _x1 = -1;
         else if (_x1 > GLOBALS->wavewidth)
         _x1 = GLOBALS->wavewidth;
       */

      /* draw trans */
      type = vtype2 (t, h);
      tv = convert_real (t, h);
      tv2 = convert_real (t, h2);

      if ((is_inf = isinf (tv)))
	{
	  if (tv < 0)
	    {
	      yt0 = _y0;
	    }
	  else
	    {
	      yt0 = _y1;
	    }
	}
      else if ((is_nan = isnan (tv)))
	{
	  yt0 = yu;
	}
      else
	{
	  yt0 = _y0 + (tv - tmin) * tmax;
	}

      if ((is_inf2 = isinf (tv2)))
	{
	  if (tv2 < 0)
	    {
	      yt1 = _y0;
	    }
	  else
	    {
	      yt1 = _y1;
	    }
	}
      else if ((is_nan2 = isnan (tv2)))
	{
	  yt1 = yu;
	}
      else
	{
	  yt1 = _y0 + (tv2 - tmin) * tmax;
	}

      if (_x0 != _x1)
	{
	  if (h->next)
	    {
	      if (h->next->time > GLOBALS->max_time)
		{
		  yt1 = yt0;
		}
	    }

	  if ((is_nan2) && (h2tim > GLOBALS->max_time))
	    is_nan2 = 0;

	  if (is_nan || is_nan2)
	    {
	      if (is_nan)
		{
		  pr_setgray (prc, 0.70);
		  pr_draw_box (prc, _x0, _y1, _x1, _y0);
		  pr_setgray (prc, 0.0);

		  if ((t->
		       flags & (TR_ANALOG_INTERPOLATED | TR_ANALOG_STEP)) ==
		      (TR_ANALOG_INTERPOLATED | TR_ANALOG_STEP))
		    {
		      pr_draw_line (prc, _x1 - 1, yt1, _x1 + 1, yt1);
		      pr_draw_line (prc, _x1, yt1 - 1, _x1, yt1 + 1);

		      pr_draw_line (prc, _x0 - 1, _y0, _x0 + 1, _y0);
		      pr_draw_line (prc, _x0, _y0 - 1, _x0, _y0 + 1);

		      pr_draw_line (prc, _x0 - 1, _y1, _x0 + 1, _y1);
		      pr_draw_line (prc, _x0, _y1 - 1, _x0, _y1 + 1);
		    }
		}
	      if (is_nan2)
		{
		  pr_draw_line (prc, _x0, yt0, _x1, yt0);

		  if ((t->
		       flags & (TR_ANALOG_INTERPOLATED | TR_ANALOG_STEP)) ==
		      (TR_ANALOG_INTERPOLATED | TR_ANALOG_STEP))
		    {
		      pr_setgray (prc, 0.70);
		      pr_draw_line (prc, _x1, _y1, _x1, _y0);
		      pr_setgray (prc, 0.0);

		      pr_draw_line (prc, _x1 - 1, _y0, _x1 + 1, _y0);
		      pr_draw_line (prc, _x1, _y0 - 1, _x1, _y0 + 1);

		      pr_draw_line (prc, _x1 - 1, _y1, _x1 + 1, _y1);
		      pr_draw_line (prc, _x1, _y1 - 1, _x1, _y1 + 1);
		    }
		}
	    }
	  else if ((t->flags & TR_ANALOG_INTERPOLATED) && !is_inf && !is_inf2)
	    {
	      pr_draw_line (prc, _x0, yt0, _x1, yt1);
	      if (t->flags & TR_ANALOG_STEP)
		{
		  pr_draw_line (prc, _x0 - 1, yt0, _x0 + 1, yt0);
		  pr_draw_line (prc, _x0, yt0 - 1, _x0, yt0 + 1);
		}
	    }
	  else			/* if (t->flags & TR_ANALOG_STEP) */
	    {
	      pr_draw_line (prc, _x0, yt0, _x1, yt0);
	      pr_draw_line (prc, _x1, yt0, _x1, yt1);

	      if ((t->flags & (TR_ANALOG_INTERPOLATED | TR_ANALOG_STEP)) ==
		  (TR_ANALOG_INTERPOLATED | TR_ANALOG_STEP))
		{
		  pr_draw_line (prc, _x0 - 1, yt0, _x0 + 1, yt0);
		  pr_draw_line (prc, _x0, yt0 - 1, _x0, yt0 + 1);
		}
	    }
	}
      else
	{
	  newtime = (((gdouble) (_x1 + WAVE_OPT_SKIP)) * GLOBALS->nspx) + GLOBALS->tims.start + GLOBALS->shift_timebase;	/* skip to next pixel */
	  h3 = bsearch_vector (t->n.vec, newtime);
	  if (h3->time > h->time)
	    {
	      h = h3;
	      lasttype = type;
	      continue;
	    }
	}

      h = h->next;
      lasttype = type;
    }

  GLOBALS->tims.start += GLOBALS->shift_timebase;
  GLOBALS->tims.end += GLOBALS->shift_timebase;
}

static void
pr_draw_vptr_trace (pr_context * prc, Trptr t, vptr v, int which)
{
  TimeType _x0, _x1, newtime, width;
  int _y0, _y1, yu, liney, ytext;
  TimeType tim, h2tim;
  vptr h, h2, h3;
  char *ascii = NULL;
  int pixlen, ysiz;
  int type;
  int lasttype = -1;

  GLOBALS->tims.start -= GLOBALS->shift_timebase;
  GLOBALS->tims.end -= GLOBALS->shift_timebase;

  h = v;
  liney = ((which + 2) * GLOBALS->fontheight) - 2;
  _y1 = ((which + 1) * GLOBALS->fontheight) + 2;
  _y0 = liney - 2;
  yu = (_y0 + _y1) / 2;
  ytext = yu - (GLOBALS->wavefont->ascent / 2) + GLOBALS->wavefont->ascent;

  ysiz = GLOBALS->wavefont->ascent - 1;
  if (ysiz < 1)
    ysiz = 1;

  if ((GLOBALS->display_grid) && (GLOBALS->enable_horiz_grid))
    {
      if ((t->flags & TR_ANALOGMASK) && (t->t_next)
	  && (t->t_next->flags & TR_ANALOG_BLANK_STRETCH))
	{
	}
      else
	{
	  pr_setgray (prc, 0.75);
	  pr_draw_line (prc,
			(GLOBALS->tims.start <
			 GLOBALS->tims.first) ? (GLOBALS->tims.first -
						 GLOBALS->tims.start) *
			GLOBALS->pxns : 0, liney,
			(GLOBALS->tims.last <=
			 GLOBALS->tims.end) ? (GLOBALS->tims.last -
					       GLOBALS->tims.start) *
			GLOBALS->pxns : GLOBALS->wavewidth - 1, liney);
	}
    }

  if (t->flags & TR_ANALOGMASK)
    {
      Trptr te = t->t_next;
      int ext = 0;

      while (te)
	{
	  if (te->flags & TR_ANALOG_BLANK_STRETCH)
	    {
	      ext++;
	      te = te->t_next;
	    }
	  else
	    {
	      break;
	    }
	}

      pr_draw_vptr_trace_analog (prc, t, v, which, ext);
      return;
    }

  pr_setgray (prc, 0.0);
  for (;;)
    {
      if (!h)
	break;
      tim = (h->time);
      if ((tim > GLOBALS->tims.end) || (tim > GLOBALS->tims.last))
	break;

      _x0 = (tim - GLOBALS->tims.start) * GLOBALS->pxns;
      if (_x0 < -1)
	_x0 = -1;
      else if (_x0 > GLOBALS->wavewidth)
	break;

      h2 = h->next;
      if (!h2)
	break;
      h2tim = tim = (h2->time);
      if (tim > GLOBALS->tims.last)
	tim = GLOBALS->tims.last;
      else if (tim > GLOBALS->tims.end + 1)
	tim = GLOBALS->tims.end + 1;
      _x1 = (tim - GLOBALS->tims.start) * GLOBALS->pxns;
      if (_x1 < -1)
	_x1 = -1;
      else if (_x1 > GLOBALS->wavewidth)
	_x1 = GLOBALS->wavewidth;

      /* draw trans */
      type = vtype2 (t, h);
      if (_x0 > -1)
	{
	  if (GLOBALS->use_roundcaps)
	    {
	      if (type == 2)
		{
		  if (lasttype != -1)
		    {
		      pr_draw_line (prc, _x0 - 1, _y0, _x0, yu);
		      pr_draw_line (prc, _x0, yu, _x0 - 1, _y1);
		    }
		}
	      else if (lasttype == 2)
		{
		  pr_draw_line (prc, _x0 + 1, _y0, _x0, yu);
		  pr_draw_line (prc, _x0, yu, _x0 + 1, _y1);
		}
	      else
		{
		  if (lasttype != type)
		    {
		      pr_draw_line (prc, _x0 - 1, _y0, _x0, yu);
		      pr_draw_line (prc, _x0, yu, _x0 - 1, _y1);
		      pr_draw_line (prc, _x0 + 1, _y0, _x0, yu);
		      pr_draw_line (prc, _x0, yu, _x0 + 1, _y1);
		    }
		  else
		    {
		      pr_draw_line (prc, _x0 - 2, _y0, _x0 + 2, _y1);
		      pr_draw_line (prc, _x0 + 2, _y0, _x0 - 2, _y1);
		    }
		}
	    }
	  else
	    {
	      pr_draw_line (prc, _x0, _y0, _x0, _y1);
	    }
	}

      if (_x0 != _x1)
	{
	  if (type == 2)
	    {
	      if (GLOBALS->use_roundcaps)
		{
		  pr_draw_line (prc, _x0 + 1, yu, _x1 - 1, yu);
		}
	      else
		{
		  pr_draw_line (prc, _x0, yu, _x1, yu);
		}
	    }
	  else
	    {

	      if (GLOBALS->use_roundcaps)
		{
		  pr_draw_line (prc, _x0 + 2, _y0, _x1 - 2, _y0);
		  pr_draw_line (prc, _x0 + 2, _y1, _x1 - 2, _y1);
		}
	      else
		{
		  pr_draw_line (prc, _x0, _y0, _x1, _y0);
		  pr_draw_line (prc, _x0, _y1, _x1, _y1);
		}

	      if (_x0 < 0)
		_x0 = 0;		/* fixup left margin */

	      width = ((prc->gpd == &ps_print_device) || (_x1 < GLOBALS->wavewidth)) ? _x1 - _x0 : GLOBALS->wavewidth - _x0;	/* truncate render
																 * window for non-ps */

	      if (width > GLOBALS->vector_padding)
		{
		  ascii = convert_ascii (t, h);

		  if (((pixlen =
			font_engine_string_measure (GLOBALS->wavefont,
					    ascii)) +
		       GLOBALS->vector_padding <= width)
		      || ((_x1 >= GLOBALS->wavewidth)
			  && (prc->gpd == &ps_print_device)))
		    {
		      pr_draw_string (prc, _x0 + 2, ytext, ascii, pixlen,
				      ysiz);
		    }
		  else
		    {
		      char *mod;

		      mod =
			bsearch_trunc (ascii,
				       width - GLOBALS->vector_padding);
		      if (mod)
			{
			  *mod = '+';
			  *(mod + 1) = 0;

			  pr_draw_string (prc, _x0 + 2, ytext, ascii,
					  GLOBALS->maxlen_trunc, ysiz);
			}
		    }
		}
	    }
	}
      else
	{
	  newtime = (((gdouble) (_x1 + WAVE_OPT_SKIP)) * GLOBALS->nspx) + GLOBALS->tims.start + GLOBALS->shift_timebase;	/* skip to next pixel */
	  h3 = bsearch_vector (t->n.vec, newtime);
	  if (h3->time > h->time)
	    {
	      h = h3;
	      lasttype = type;
	      continue;
	    }
	}

      if (ascii)
	{
	  free_2 (ascii);
	  ascii = NULL;
	}
      h = h->next;
      lasttype = type;
    }

  GLOBALS->tims.start += GLOBALS->shift_timebase;
  GLOBALS->tims.end += GLOBALS->shift_timebase;
}

static void
pr_rendertraces (pr_context * prc)
{
  if (!GLOBALS->topmost_trace)
    {
      GLOBALS->topmost_trace = GLOBALS->traces.first;
    }
  if (GLOBALS->topmost_trace)
    {
      Trptr t;
      hptr h;
      vptr v;
      int i, num_traces_displayable;

      num_traces_displayable =
	GLOBALS->wavearea->allocation.height / (GLOBALS->fontheight);
      num_traces_displayable--;	/* for the time trace that is
				 * always there */

      t = GLOBALS->topmost_trace;
      for (i = 0; ((i < num_traces_displayable) && (t)); i++)
	{
	  if (!(t->flags & (TR_EXCLUDE | TR_BLANK | TR_ANALOG_BLANK_STRETCH)))
	    {
	      GLOBALS->shift_timebase = t->shift;
	      if (!t->vector)
		{
		  h = bsearch_node (t->n.nd, GLOBALS->tims.start);
		  DEBUG (printf
			 ("Bit Trace: %s, %s\n", t->name, t->n.nd->nname));
		  DEBUG (printf
			 ("Start time: " TTFormat ", Histent time: " TTFormat
			  "\n", tims.start, (h->time + shift_timebase)));

		  if (!t->n.nd->ext)
		    {
		      pr_draw_hptr_trace (prc, t, h, i, 1, 0);
		    }
		  else
		    {
		      pr_draw_hptr_trace_vector (prc, t, h, i);
		    }
		}
	      else
		{
		  v = bsearch_vector (t->n.vec, GLOBALS->tims.start);
		  DEBUG (printf
			 ("Vector Trace: %s, %s\n", t->name, t->n.vec->name));
		  DEBUG (printf
			 ("Start time: " TTFormat ", Vectorent time: "
			  TTFormat "\n", tims.start,
			  (v->time + shift_timebase)));
		  pr_draw_vptr_trace (prc, t, v, i);
		}
	    }
	  else
	    {
	      int kill_dodraw_grid = t->flags & TR_ANALOG_BLANK_STRETCH;

	      if (kill_dodraw_grid)
		{
		  if (!(t->t_next))
		    {
		      kill_dodraw_grid = 0;
		    }
		  else if (!(t->t_next->flags & TR_ANALOG_BLANK_STRETCH))
		    {
		      kill_dodraw_grid = 0;
		    }
		}

	      pr_draw_hptr_trace (prc, NULL, NULL, i, 0, kill_dodraw_grid);
	    }
	  t = GiveNextTrace (t);
	}
    }
  pr_draw_named_markers (prc);
  pr_draw_marker (prc);
}

/**********************************************/

static int
pr_RenderSig (pr_context * prc, Trptr t, int i)
{
  int texty, liney;
  int retval;
  char sbuf[128];
  int bufclen;

  UpdateSigValue (t);		/* in case it's stale on nonprop */

  bufclen = 0;
  sbuf[0] = 0;

  if ((t->name) && (t->shift))
    {
      sbuf[0] = '(';
      reformat_time (sbuf + 1, t->shift, GLOBALS->time_dimension);
      strcpy (sbuf + (bufclen = strlen (sbuf + 1) + 1), ")");
      bufclen++;
    }

  if ((!t->vector) && (t->n.nd) && (t->n.nd->array_height))
    {
      bufclen += sprintf (sbuf + strlen (sbuf), "{%d}", t->n.nd->this_row);
    }


  liney = ((i + 2) * GLOBALS->fontheight) - 2;

  texty = liney - (GLOBALS->signalfont->descent);

  retval = liney - GLOBALS->fontheight + 1;

  if (t->flags & TR_HIGHLIGHT)
    pr_setgray (prc, 0.75);
  else
    pr_setgray (prc, 0.95);

  pr_draw_box (prc, 2, retval + 1,
	       GLOBALS->pr_signal_fill_width_print_c_1 - 2,
	       retval + GLOBALS->fontheight - 3);
  pr_setgray (prc, 0.75);

  pr_draw_line (prc, 0, liney, GLOBALS->pr_signal_fill_width_print_c_1 - 1,
		liney);

  /* if (!(t->flags&(TR_BLANK|TR_ANALOG_BLANK_STRETCH))) */
  {
    int maxwidth = 0;

    if (t->name)
      maxwidth = strlen (t->name) + bufclen;
    if ((t->asciivalue) && (!(t->flags & TR_EXCLUDE)))
      maxwidth += strlen (t->asciivalue);
    if (maxwidth)
      {
	gdouble realwidth;
	char *buf;
	buf = wave_alloca (maxwidth + 1);
	buf[0] = 0;
	if (t->name)
	  {
	    strcpy (buf, t->name);
	    if (bufclen)
	      strcat (buf, sbuf);
	  }
	if ((t->asciivalue) && (!(t->flags & TR_EXCLUDE)))
	  strcat (buf, t->asciivalue);

	realwidth = maxwidth * GLOBALS->ps_chwidth_print_c_1;

	if (maxwidth == 0)
	  return (retval);
	pr_setgray (prc, 0.0);
	pr_draw_string (prc, 3, texty - 1, buf, realwidth,
			GLOBALS->signalfont->ascent -
			GLOBALS->signalfont->descent);
      }
  }

  return (retval);
}

static void
pr_RenderSigs (pr_context * prc, int trtarget)
{
  Trptr t;
  int i, trwhich, width;
  int num_traces_displayable;
  GtkAdjustment *hadj;
  gint xsrc;

  hadj = GTK_ADJUSTMENT (GLOBALS->signal_hslider);
  xsrc = (gint) hadj->value;

  num_traces_displayable =
    GLOBALS->signalarea->allocation.height / (GLOBALS->fontheight);
  num_traces_displayable--;	/* for the time trace that is always
				 * there */

  pr_setgray (prc, 0.75);
  pr_draw_line (prc, 0, GLOBALS->fontheight - 1,
		GLOBALS->pr_signal_fill_width_print_c_1 - 1,
		GLOBALS->fontheight - 1);
  pr_setgray (prc, 0.0);
  pr_draw_string (prc, 3, GLOBALS->fontheight, "Time",
		  (width =
		   font_engine_string_measure (GLOBALS->wavefont, "Time")) * 2,
		  GLOBALS->fontheight);

  GLOBALS->ps_chwidth_print_c_1 =
    ((gdouble) (GLOBALS->pr_signal_fill_width_print_c_1 - 6)) /
    ((gdouble) (GLOBALS->ps_nummaxchars_print_c_1));

  t = GLOBALS->traces.first;
  trwhich = 0;
  while (t)
    {
      if ((trwhich < trtarget) && (GiveNextTrace (t)))
	{
	  trwhich++;
	  t = GiveNextTrace (t);
	}
      else
	{
	  break;
	}
    }

  GLOBALS->topmost_trace = t;
  if (t)
    {
      for (i = 0; (i < num_traces_displayable) && (t); i++)
	{
	  pr_RenderSig (prc, t, i);
	  t = GiveNextTrace (t);
	}
    }
}

/**********************************************/

void
print_image (pr_context * prc)
{
  GtkAdjustment *sadj;
  int trtarget;

  if ((GLOBALS->traces.total + 1) * GLOBALS->fontheight >
      GLOBALS->wavearea->allocation.height)
    GLOBALS->liney_max = GLOBALS->wavearea->allocation.height;
  else
    GLOBALS->liney_max = (GLOBALS->traces.total + 1) * GLOBALS->fontheight;

  GLOBALS->pr_signal_fill_width_print_c_1 = ps_MaxSignalLength ();
  pr_wave_init (prc);
  pr_header (prc);
  pr_rendertimes (prc);
  pr_rendertraces (prc);

  pr_signal_init (prc);

  sadj = GTK_ADJUSTMENT (GLOBALS->wave_vslider);
  trtarget = (int) (sadj->value);
  pr_RenderSigs (prc, trtarget);

  pr_trailer (prc);
}

void
print_ps_image (FILE * wave, gdouble px, gdouble py)
{
  pr_context prc;

  prc.gpd = &ps_print_device;
  prc.PageX = px;		/* Legal page width */
  prc.PageY = py;		/* Legal page height */
  prc.LM = 1;			/* Left Margin (inch) */
  prc.RM = 1;			/* Right Margin (inch) */
  prc.BM = 1;			/* Bottom Margin (inch) */
  prc.TM = 1;			/* Top Margin (inch) */
  prc.handle = wave;
  prc.fullpage = GLOBALS->ps_fullpage;
  print_image (&prc);
}

void
print_mif_image (FILE * wave, gdouble px, gdouble py)
{
  pr_context prc;

  prc.gpd = &mif_print_device;
  prc.PageX = px;		/* Legal page width */
  prc.PageY = py;		/* Legal page height */
  prc.LM = 1;			/* Left Margin (inch) */
  prc.RM = 1;			/* Right Margin (inch) */
  prc.BM = 1;			/* Bottom Margin (inch) */
  prc.TM = 1;			/* Top Margin (inch) */
  prc.tr_x = 0;
  prc.tr_y = 0;
  prc.handle = wave;
  prc.fullpage = GLOBALS->ps_fullpage;
  print_image (&prc);
}

/*
 * $Id$
 * $Log$
 * Revision 1.18  2008/12/25 04:14:11  gtkwave
 * -Wshadow warning fixes
 *
 * Revision 1.17  2008/12/11 21:08:15  gtkwave
 * allow support for marker names which are strings
 *
 * Revision 1.16  2008/10/12 02:56:20  gtkwave
 * fix for blackout regions
 *
 * Revision 1.15  2008/02/08 02:26:36  gtkwave
 * anti-aliased font support add
 *
 * Revision 1.14  2008/01/28 23:08:50  gtkwave
 * added infinite scaling define in currenttime.h
 *
 * Revision 1.13  2008/01/27 23:54:05  gtkwave
 * right side margin NaN fix
 *
 * Revision 1.12  2008/01/27 01:21:05  gtkwave
 * make transition to/from inf/-inf a square wave when interpolated
 *
 * Revision 1.11  2008/01/26 19:13:34  gtkwave
 * replace NaN trapezoids with NaN boxes
 *
 * Revision 1.10  2008/01/26 05:02:14  gtkwave
 * added parallelogram rendering for nans
 *
 * Revision 1.9  2008/01/25 23:29:23  gtkwave
 * modify analog slightly for nan and inf handling
 *
 * Revision 1.8  2008/01/25 04:10:15  gtkwave
 * added new resizing options to menu
 *
 * Revision 1.7  2008/01/24 20:19:39  gtkwave
 * analog rendering fixes
 *
 * Revision 1.6  2008/01/23 04:49:32  gtkwave
 * more tweaking of interpolated+step mode (use snap dots)
 *
 * Revision 1.5  2008/01/23 02:05:44  gtkwave
 * added interpolated + step mode
 *
 * Revision 1.4  2008/01/22 20:11:47  gtkwave
 * track and hold experimentation
 *
 * Revision 1.3  2008/01/21 16:54:04  gtkwave
 * some analog fixes for end of screen
 *
 * Revision 1.2  2007/08/26 21:35:43  gtkwave
 * integrated global context management from SystemOfCode2007 branch
 *
 * Revision 1.1.1.1.2.3  2007/08/07 03:18:55  kermin
 * Changed to pointer based GLOBAL structure and added initialization function
 *
 * Revision 1.1.1.1.2.2  2007/08/06 03:50:48  gtkwave
 * globals support for ae2, gtk1, cygwin, mingw.  also cleaned up some machine
 * generated structs, etc.
 *
 * Revision 1.1.1.1.2.1  2007/08/05 02:27:22  kermin
 * Semi working global struct
 *
 * Revision 1.1.1.1  2007/05/30 04:27:28  gtkwave
 * Imported sources
 *
 * Revision 1.3  2007/05/28 00:55:06  gtkwave
 * added support for arrays as a first class dumpfile datatype
 *
 * Revision 1.2  2007/04/20 02:08:13  gtkwave
 * initial release
 *
 */
