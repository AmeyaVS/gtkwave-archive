/* 
 * Copyright (c) Tony Bybell 1999-2007.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <config.h>
#include "gtk12compat.h"
#include "currenttime.h"
#include "pixmaps.h"
#include "symbol.h"
#include "bsearch.h"
#include "color.h"
#include "rc.h"
#include "strace.h"
#include "debug.h"
#include "main.h"

#if !defined _ISOC99_SOURCE
#define _ISOC99_SOURCE 1
#endif
#include <math.h>

static void rendertimebar(void);
static void draw_hptr_trace(Trptr t, hptr h, int which, int dodraw, int kill_grid);
static void draw_hptr_trace_vector(Trptr t, hptr h, int which);
static void draw_vptr_trace(Trptr t, vptr v, int which);
static void rendertraces(void);
static void rendertimes(void);

static gint m1x, m2x;

char signalwindow_width_dirty=1;	/* prime it up for the 1st one at least.. */
char enable_ghost_marker=1;
char enable_horiz_grid=1;
char enable_vert_grid=1;
char use_big_fonts=0, use_nonprop_fonts=0;
char do_resize_signals=~0;
char constant_marker_update=0;
char use_roundcaps=0;
char show_base=~0;
char wave_scrolling=~0;
int vector_padding=4;
static int in_button_press=0;
char left_justify_sigs=0;
char zoom_pow10_snap=0;
int cursor_snap=0;

gfloat old_wvalue=-1.0;

struct blackout_region_t *blackout_regions = NULL;
TimeType zoom=0, scale=1, nsperframe=1;
gdouble pixelsperframe=1;
gdouble hashstep=1.0;
static TimeType prevtim=-1;

gdouble pxns=1.0, nspx=1.0;
gdouble zoombase=2.0;

Trptr topmost_trace=NULL;
int waveheight=1, wavecrosspiece;
int wavewidth=1;
GdkFont *wavefont=NULL;
GdkFont *wavefont_smaller=NULL;

GtkWidget *wavearea=NULL;
static GtkWidget *vscroll=NULL;
static GtkWidget *hscroll=NULL;
static GdkPixmap *wavepixmap = NULL;

GtkObject *wave_vslider=NULL, *wave_hslider=NULL;
TimeType named_markers[26];

static char made_gc_contexts=0;

static GdkGC    *gc_back  =NULL;
static GdkGC    *gc_baseline=NULL;
static GdkGC    *gc_grid  =NULL;
static GdkGC    *gc_time  =NULL;
static GdkGC    *gc_timeb =NULL;
static GdkGC	*gc_value =NULL;                 
static GdkGC	*gc_low   =NULL;
static GdkGC    *gc_high  =NULL;
static GdkGC    *gc_trans =NULL;
static GdkGC    *gc_mid   =NULL;
static GdkGC    *gc_xfill =NULL;
static GdkGC    *gc_x     =NULL;
static GdkGC    *gc_vbox  =NULL;
static GdkGC    *gc_vtrans=NULL;
static GdkGC    *gc_mark  =NULL;
static GdkGC    *gc_umark =NULL;

static GdkGC    *gc_0     =NULL;
static GdkGC    *gc_1     =NULL;
static GdkGC    *gc_ufill =NULL;
static GdkGC    *gc_u     =NULL;
static GdkGC    *gc_wfill =NULL;
static GdkGC    *gc_w     =NULL;
static GdkGC    *gc_dashfill =NULL;
static GdkGC    *gc_dash  =NULL;

static char made_sgc_contexts=0;

GdkGC	*gc_white =NULL;
GdkGC	*gc_black =NULL;
GdkGC	*gc_ltgray=NULL;
GdkGC	*gc_normal=NULL;
GdkGC	*gc_mdgray=NULL;
GdkGC	*gc_dkgray=NULL;
GdkGC	*gc_dkblue=NULL;

static const GdkModifierType   bmask[4]= {0, GDK_BUTTON1_MASK, 0, GDK_BUTTON3_MASK };		        /* button 1, 3 press/rel encodings */
static const GdkModifierType m_bmask[4]= {0, GDK_BUTTON1_MOTION_MASK, 0, GDK_BUTTON3_MOTION_MASK };	/* button 1, 3 motion encodings */

static char fill_in_smaller_rgb_areas = 0; /* only set when '?' encountered and triggers a GC creation */

/******************************************************************/

static void update_dual()
{
if(dual_ctx && !dual_race_lock)
        {
        dual_ctx[dual_id].zoom = tims.zoom;
        dual_ctx[dual_id].marker = tims.marker;
        dual_ctx[dual_id].baseline = tims.baseline;
        dual_ctx[dual_id].left_margin_time = tims.start;
        dual_ctx[dual_id].use_new_times = 1;
        }
}

/******************************************************************/

/*
 * gtk_draw_line() acceleration for win32 by doing draw combining
 */
#if defined __MINGW32__ || defined _MSC_VER

#define WAVE_SEG_BUF_CNT 1024

static int seg_trans_cnt=0, seg_low_cnt=0, seg_high_cnt=0, seg_mid_cnt=0;
static GdkSegment seg_trans[WAVE_SEG_BUF_CNT], seg_low[WAVE_SEG_BUF_CNT], seg_high[WAVE_SEG_BUF_CNT], seg_mid[WAVE_SEG_BUF_CNT];


static void wave_gdk_draw_line(GdkDrawable *drawable, GdkGC *gc, gint x1, gint y1, gint x2, gint y2)
{
GdkSegment *seg;
int *seg_cnt;

if(gc==gc_trans) 	{ seg = seg_trans; seg_cnt = &seg_trans_cnt; }
else if(gc==gc_low) 	{ seg = seg_low; seg_cnt = &seg_low_cnt; }
else if(gc==gc_high) 	{ seg = seg_high; seg_cnt = &seg_high_cnt; }
else if(gc==gc_mid) 	{ seg = seg_mid; seg_cnt = &seg_mid_cnt; }
else 			{ gdk_draw_line(drawable, gc, x1, y1, x2, y2); return; }

seg[*seg_cnt].x1 = x1;
seg[*seg_cnt].y1 = y1;
seg[*seg_cnt].x2 = x2;
seg[*seg_cnt].y2 = y2;
(*seg_cnt)++;
if(*seg_cnt == WAVE_SEG_BUF_CNT)
	{
	gdk_draw_segments(drawable, gc, seg, *seg_cnt);
	*seg_cnt = 0;
	}
}

static void wave_gdk_draw_line_flush(GdkDrawable *drawable)
{
if(seg_mid)
	{
	gdk_draw_segments(drawable, gc_mid, seg_mid, seg_mid_cnt);
	seg_mid_cnt = 0;
	}

if(seg_high)
	{
	gdk_draw_segments(drawable, gc_high, seg_high, seg_high_cnt);
	seg_high_cnt = 0;
	}

if(seg_low)
	{
	gdk_draw_segments(drawable, gc_low, seg_low, seg_low_cnt);
	seg_low_cnt = 0;
	}

if(seg_trans)
	{
	gdk_draw_segments(drawable, gc_trans, seg_trans, seg_trans_cnt);
	seg_trans_cnt = 0;
	}
}

#else

/* completely unnecessary for linux */

#define wave_gdk_draw_line(a,b,c,d,e,f) gdk_draw_line(a,b,c,d,e,f)
#define wave_gdk_draw_line_flush(x)

#endif

/******************************************************************/


/*
 * aldec-like "snap" feature...
 */
TimeType cook_markertime(TimeType marker, gint x, gint y)
{
int i, num_traces_displayable;                
Trptr t = NULL;
TimeType lft, rgh;
char lftinv, rghinv;
gdouble xlft, xrgh;
gdouble xlftd, xrghd;
TimeType closest_named = MAX_HISTENT_TIME;
int closest_which = -1;
gint xold = x, yold = y;

if(!cursor_snap) return(marker);

/* potential snapping to a named marker time */
for(i=0;i<26;i++)
	{
	if(named_markers[i] != -1)
		{
		TimeType dlt;

		if((named_markers[i]>=tims.start)&&(named_markers[i]<=tims.end)&&(named_markers[i]<=tims.last))
			{
			if(marker < named_markers[i])
				{
				dlt = named_markers[i] - marker;			
				}
				else
				{		
				dlt = marker - named_markers[i];			
				}
			
			if(dlt < closest_named)
				{
				closest_named = dlt;
				closest_which = i;
				}
			}
		}
	}

num_traces_displayable=wavearea->allocation.height/(fontheight);
num_traces_displayable--;   /* for the time trace that is always there */

y-=fontheight;
if(y<0) y=0;
y/=fontheight;		    /* y now indicates the trace in question */
if(y>num_traces_displayable) y=num_traces_displayable;

t=topmost_trace;
for(i=0;i<y;i++)
	{
	if(!t) goto bot;
	t=GiveNextTrace(t);
	}

if(!t) goto bot;
if((t->flags&(TR_BLANK|TR_EXCLUDE)))
	{
	t = NULL;
	goto bot;
	}

if(t->flags & TR_ANALOG_BLANK_STRETCH)	/* seek to real analog trace is present... */
	{
	while((t) && (t = t->t_prev))
		{
		if(!(t->flags & TR_ANALOG_BLANK_STRETCH))
			{
			if(t->flags & TR_ANALOGMASK)
				{
				break; /* found it */
				}
				else
				{
				t = NULL;
				}
			}
		}
	}
if(!t) goto bot;

if(t->vector)
	{
	vptr v = bsearch_vector(t->n.vec, marker - t->shift);
	vptr v2 = v ? v->next : NULL;

	if((!v)||(!v2)) goto bot;	/* should never happen */

	lft = v->time;
	rgh = v2->time;
	}
	else
	{
	hptr h = bsearch_node(t->n.nd, marker - t->shift);
	hptr h2 = h ? h->next : NULL;

	if((!h)||(!h2)) goto bot;	/* should never happen */

	lft = h->time;
	rgh = h2->time;
	}


lftinv = (lft < (tims.start - t->shift))||(lft >= (tims.end - t->shift))||(lft >= (tims.last - t->shift));
rghinv = (rgh < (tims.start - t->shift))||(rgh >= (tims.end - t->shift))||(rgh >= (tims.last - t->shift));

xlft = (lft + t->shift - tims.start) * pxns;
xrgh = (rgh + t->shift - tims.start) * pxns;

xlftd = xlft - x;
if(xlftd<(gdouble)0.0) xlftd = ((gdouble)0.0) - xlftd;

xrghd = xrgh - x;
if(xrghd<(gdouble)0.0) xrghd = ((gdouble)0.0) - xrghd;

if(xlftd<=xrghd)
	{
	if((!lftinv)&&(xlftd<=cursor_snap)) 
		{
		if(closest_which >= 0)
		        {
        		if((closest_named * pxns) < xlftd)
                		{
                		marker = named_markers[closest_which];
				goto xit;
                		}
			}

		marker = lft + t->shift;
		goto xit;
		}
	}
	else
	{
	if((!rghinv)&&(xrghd<=cursor_snap)) 
		{
		if(closest_which >= 0)
		        {
        		if((closest_named * pxns) < xrghd)
                		{
                		marker = named_markers[closest_which];
				goto xit;
                		}
			}

		marker = rgh + t->shift;
		goto xit;
		}
	}

bot:
if(closest_which >= 0)
	{
	if((closest_named * pxns) <= cursor_snap)
		{
		marker = named_markers[closest_which];
		}
	}

xit:
move_mouseover(t, xold, yold, marker);
return(marker);
}


static void draw_named_markers(void)
{
gdouble pixstep;
gint xl, y;
int i;
TimeType t;

for(i=0;i<26;i++)
{
if((t=named_markers[i])!=-1)
	{
	if((t>=tims.start)&&(t<=tims.last)
		&&(t<=tims.end))
		{
		/* this needs to be here rather than outside the loop as gcc does some
		   optimizations that cause it to calculate slightly different from the marker if it's not here */
		pixstep=((gdouble)nsperframe)/((gdouble)pixelsperframe);

		xl=((gdouble)(t-tims.start))/pixstep;     /* snap to integer */
		if((xl>=0)&&(xl<wavewidth))
			{
			char nbuff[2];
			nbuff[0]='A'+i; nbuff[1]=0x00;

			for(y=fontheight-1;y<=waveheight-1;y+=8)
				{
				gdk_draw_line(wavepixmap,
					gc_mark,
        	        		xl, y, xl, y+5);
				}

			gdk_draw_string(wavepixmap, wavefont_smaller,
				gc_mark,
				xl-(gdk_string_measure(wavefont_smaller, nbuff)>>1), 
				fontheight-2, nbuff);
			}
		}
	}
}
}


static void sync_marker(void)
{
if((tims.prevmarker==-1)&&(tims.marker!=-1))
	{
	signalwindow_width_dirty=1;
	}
	else
if((tims.marker==-1)&&(tims.prevmarker!=-1))
	{
	signalwindow_width_dirty=1;
	}
tims.prevmarker=tims.marker;
}


static void draw_marker(void)
{
gdouble pixstep;
gint xl;

if(!wavearea->window) return;

m1x=m2x=-1;

if(tims.baseline>=0)
	{
	if((tims.baseline>=tims.start)&&(tims.baseline<=tims.last)
		&&(tims.baseline<=tims.end))
		{
		pixstep=((gdouble)nsperframe)/((gdouble)pixelsperframe);
		xl=((gdouble)(tims.baseline-tims.start))/pixstep;     /* snap to integer */
		if((xl>=0)&&(xl<wavewidth))
			{
			gdk_draw_line(wavearea->window,
				gc_baseline,
                		xl, fontheight-1, xl, waveheight-1);
			}
		}
	}

if(tims.marker>=0)
	{
	if((tims.marker>=tims.start)&&(tims.marker<=tims.last)
		&&(tims.marker<=tims.end))
		{
		pixstep=((gdouble)nsperframe)/((gdouble)pixelsperframe);
		xl=((gdouble)(tims.marker-tims.start))/pixstep;     /* snap to integer */
		if((xl>=0)&&(xl<wavewidth))
			{
			gdk_draw_line(wavearea->window,
				gc_umark,
                		xl, fontheight-1, xl, waveheight-1);
			m1x=xl;
			}
		}
	}


if((enable_ghost_marker)&&(in_button_press)&&(tims.lmbcache>=0))
	{
	if((tims.lmbcache>=tims.start)&&(tims.lmbcache<=tims.last)
		&&(tims.lmbcache<=tims.end))
		{
		pixstep=((gdouble)nsperframe)/((gdouble)pixelsperframe);
		xl=((gdouble)(tims.lmbcache-tims.start))/pixstep;     /* snap to integer */
		if((xl>=0)&&(xl<wavewidth))
			{
			gdk_draw_line(wavearea->window,
				gc_umark,
                		xl, fontheight-1, xl, waveheight-1);
			m2x=xl;
			}
		}
	}

if(m1x>m2x)		/* ensure m1x <= m2x for partitioned refresh */
	{
	gint t;

	t=m1x;
	m1x=m2x;
	m2x=t;
	}

if(m1x==-1) m1x=m2x;	/* make both markers same if no ghost marker or v.v. */

update_dual();
}


static void draw_marker_partitions(void)
{
draw_marker();

if(m1x==m2x)
	{
	gdk_draw_pixmap(wavearea->window, wavearea->style->fg_gc[GTK_WIDGET_STATE(wavearea)],
	wavepixmap, m1x, 0, m1x, 0, 1, fontheight-2);

	if(m1x<0)
		{
		gdk_draw_pixmap(wavearea->window, wavearea->style->fg_gc[GTK_WIDGET_STATE(wavearea)],
		wavepixmap, 0, 0, 0, 0, wavewidth, waveheight);
		}
		else
		{
		if(m1x==0)
			{
			gdk_draw_pixmap(wavearea->window, wavearea->style->fg_gc[GTK_WIDGET_STATE(wavearea)],
			wavepixmap, 1, 0, 1, 0, wavewidth-1, waveheight);
			}
		else
		if(m1x==wavewidth-1)
			{

			gdk_draw_pixmap(wavearea->window, wavearea->style->fg_gc[GTK_WIDGET_STATE(wavearea)],
			wavepixmap, 0, 0, 0, 0, wavewidth-1, waveheight);
			}
		else
			{
			gdk_draw_pixmap(wavearea->window, wavearea->style->fg_gc[GTK_WIDGET_STATE(wavearea)],
			wavepixmap, 0, 0, 0, 0, m1x, waveheight);
			gdk_draw_pixmap(wavearea->window, wavearea->style->fg_gc[GTK_WIDGET_STATE(wavearea)],
			wavepixmap, m1x+1, 0, m1x+1, 0, wavewidth-m1x-1, waveheight);
			}
		}
	}
	else
	{
	gdk_draw_pixmap(wavearea->window, wavearea->style->fg_gc[GTK_WIDGET_STATE(wavearea)],
	wavepixmap, m1x, 0, m1x, 0, 1, fontheight-2);
	gdk_draw_pixmap(wavearea->window, wavearea->style->fg_gc[GTK_WIDGET_STATE(wavearea)],
	wavepixmap, m2x, 0, m2x, 0, 1, fontheight-2);

	if(m1x>0)
		{
		gdk_draw_pixmap(wavearea->window, wavearea->style->fg_gc[GTK_WIDGET_STATE(wavearea)],
		wavepixmap, 0, 0, 0, 0, m1x, waveheight);
		}

	if(m2x-m1x>1)
		{
		gdk_draw_pixmap(wavearea->window, wavearea->style->fg_gc[GTK_WIDGET_STATE(wavearea)],
		wavepixmap, m1x+1, 0, m1x+1, 0, m2x-m1x-1, waveheight);
		}

	if(m2x!=wavewidth-1)
		{
		gdk_draw_pixmap(wavearea->window, wavearea->style->fg_gc[GTK_WIDGET_STATE(wavearea)],
		wavepixmap, m2x+1, 0, m2x+1, 0, wavewidth-m2x-1, waveheight);
		}
	}

/* keep baseline from getting obliterated */
if(tims.baseline>=0)
	{
	draw_marker();
	}
}

static void renderblackout(void)
{
gfloat pageinc;
TimeType lhs, rhs, lclip, rclip;
struct blackout_region_t *bt = blackout_regions;

if(bt)
	{
	pageinc=(gfloat)(((gdouble)wavewidth)*nspx);
	lhs = tims.start;
	rhs = pageinc + lhs;

	while(bt)
		{
		if( ((bt->bstart <= lhs) && (bt->bend >= lhs)) || ((bt->bstart >= lhs) && (bt->bstart <= rhs)) )
			{
			lclip = bt->bstart; rclip = bt->bend;

			if(lclip < lhs) lclip = lhs;
				else if (lclip > rhs) lclip = rhs;

			if(rclip < lhs) rclip = lhs;

			lclip -= lhs;
			rclip -= lhs;
			if(rclip>(wavewidth+1)) rclip = wavewidth+1;
			
			gdk_draw_rectangle(wavepixmap, gc_xfill, TRUE, 
				(((gdouble)lclip)*pxns), fontheight,
				(((gdouble)(rclip-lclip))*pxns), waveheight-fontheight);
			}

		bt=bt->next;
		}
	}
}

static void     
service_hslider(GtkWidget *text, gpointer data)
{
DEBUG(printf("Wave HSlider Moved\n"));

if((wavepixmap)&&(wavewidth>1))
	{
	GtkAdjustment *hadj;

	hadj=GTK_ADJUSTMENT(wave_hslider);

	if(!tims.timecache)
		{
		tims.start=time_trunc(hadj->value);
		}
		else
		{
		tims.start=time_trunc(tims.timecache);
		tims.timecache=0;	/* reset */
		}

	if(tims.start<tims.first) tims.start=tims.first;
		else if(tims.start>tims.last) tims.start=tims.last;

	tims.laststart=tims.start;

	gdk_draw_rectangle(wavepixmap, gc_back, TRUE, 0, 0,
		wavewidth, waveheight);
	rendertimebar();
	}
}

static void     
service_vslider(GtkWidget *text, gpointer data)
{
GtkAdjustment *sadj, *hadj;
int trtarget;
int xsrc;

if(signalpixmap)
	{
	hadj=GTK_ADJUSTMENT(signal_hslider);
	sadj=GTK_ADJUSTMENT(wave_vslider);
	xsrc=(gint)hadj->value;

	trtarget=(int)(sadj->value);
	DEBUG(printf("Wave VSlider Moved to %d\n",trtarget));

		gdk_draw_rectangle(signalpixmap, 
			gc_ltgray, TRUE, 0, 0,
	            	signal_fill_width, signalarea->allocation.height);
	
		sync_marker();
		RenderSigs(trtarget,(old_wvalue==sadj->value)?0:1);

	old_wvalue=sadj->value;

		draw_named_markers();
		gdk_draw_pixmap(signalarea->window, signalarea->style->fg_gc[GTK_WIDGET_STATE(signalarea)],
	                signalpixmap,
			xsrc, 0,
			0, 0,
	                signalarea->allocation.width, signalarea->allocation.height);
		draw_marker();
	}
}

void button_press_release_common(void)
{
MaxSignalLength();
gdk_draw_rectangle(signalpixmap, 
	gc_ltgray, TRUE, 0, 0,
        signal_fill_width, signalarea->allocation.height);
sync_marker();
RenderSigs((int)(GTK_ADJUSTMENT(wave_vslider)->value),0);
gdk_draw_pixmap(signalarea->window, signalarea->style->fg_gc[GTK_WIDGET_STATE(signalarea)],
      	signalpixmap,
	(gint)(GTK_ADJUSTMENT(signal_hslider)->value), 0,
	0, 0,
        signalarea->allocation.width, signalarea->allocation.height);
}

static void button_motion_common(gint xin, gint yin, int pressrel, int is_button_2) 
{
gdouble x,offset,pixstep;
TimeType newcurr;

if(xin<0) xin=0;
if(xin>(wavewidth-1)) xin=(wavewidth-1);

x=xin;	/* for pix time calc */

pixstep=((gdouble)nsperframe)/((gdouble)pixelsperframe);
newcurr=(TimeType)(offset=((gdouble)tims.start)+(x*pixstep));

if(offset-newcurr>0.5)	/* round to nearest integer ns */
	{
	newcurr++;
	}

if(newcurr>tims.last) 		/* sanity checking */
	{
	newcurr=tims.last;
	}
if(newcurr>tims.end)
	{
	newcurr=tims.end;
	}
if(newcurr<tims.start)
	{
	newcurr=tims.start;
	}

if(!is_button_2)
	{
	update_markertime(tims.marker=cook_markertime(time_trunc(newcurr), xin, yin));
	if(tims.lmbcache<0) tims.lmbcache=time_trunc(newcurr);

	draw_marker_partitions();

	if((pressrel)||(constant_marker_update)) 
		{
		button_press_release_common();
		}
	}
	else
	{
	tims.baseline = ((tims.baseline<0)||(is_button_2<0)) ? cook_markertime(time_trunc(newcurr), xin, yin) : -1;
	update_basetime(tims.baseline);
	update_markertime(tims.marker);
	wavearea_configure_event(wavearea, NULL);
	}
}

static gint motion_notify_event(GtkWidget *widget, GdkEventMotion *event)
{
gdouble x, y, pixstep, offset;
GdkModifierType state;
TimeType newcurr;
int scrolled;

#ifdef WAVE_USE_GTK2
gint xi, yi;
#endif

int dummy_x, dummy_y;
get_window_xypos(&dummy_x, &dummy_y);

if(event->is_hint)   
        {
	WAVE_GDK_GET_POINTER(event->window, &x, &y, &xi, &yi, &state);
	WAVE_GDK_GET_POINTER_COPY;
        }
        else
        {
        x = event->x;
        y = event->y;
        state = event->state;
        }

do
	{
	scrolled=0;
	if(state&bmask[in_button_press]) /* needed for retargeting in AIX/X11 */
		{
		if(x<0)
			{ 
			if(wave_scrolling)
			if(tims.start>tims.first)
				{
				if(nsperframe<10) 
					{
					tims.start-=nsperframe;
					}
					else
					{
					tims.start-=(nsperframe/10);
					}
				if(tims.start<tims.first) tims.start=tims.first;
				GTK_ADJUSTMENT(wave_hslider)->value=
					tims.marker=time_trunc(tims.timecache=tims.start);
	
				gtk_signal_emit_by_name (GTK_OBJECT (GTK_ADJUSTMENT(wave_hslider)), "changed");
				gtk_signal_emit_by_name (GTK_OBJECT (GTK_ADJUSTMENT(wave_hslider)), "value_changed");
				scrolled=1;
				}	
			x=0; 
			}
		else
		if(x>wavewidth) 
			{ 
			if(wave_scrolling)
			if(tims.start!=tims.last)
				{
				gfloat pageinc;
		
				pageinc=(gfloat)(((gdouble)wavewidth)*nspx);
	
				if(nsperframe<10) 
					{
					tims.start+=nsperframe;
					}
					else
					{
					tims.start+=(nsperframe/10);
					}
	
				if(tims.start>tims.last-pageinc+1) tims.start=time_trunc(tims.last-pageinc+1);
				if(tims.start<tims.first) tims.start=tims.first;
	
				tims.marker=time_trunc(tims.start+pageinc);
				if(tims.marker>tims.last) tims.marker=tims.last;
		
				GTK_ADJUSTMENT(wave_hslider)->value=tims.timecache=tims.start;
	
				gtk_signal_emit_by_name (GTK_OBJECT (GTK_ADJUSTMENT(wave_hslider)), "changed");
				gtk_signal_emit_by_name (GTK_OBJECT (GTK_ADJUSTMENT(wave_hslider)), "value_changed");
				scrolled=1;
				}
			x=wavewidth-1; 
			}
		}	
	else if((state&GDK_BUTTON2_MASK)&&(tims.baseline>=0))
		{
		button_motion_common(x,y,0,-1); /* neg one says don't clear tims.baseline */
		}

	pixstep=((gdouble)nsperframe)/((gdouble)pixelsperframe);
	newcurr=tims.start+(offset=x*pixstep);
	if((offset-((int)offset))>0.5)  /* round to nearest integer ns */
	        {
	        newcurr++;
	        }
	
	if(newcurr>tims.last) newcurr=tims.last;
	
	if(newcurr!=prevtim)
		{
		update_currenttime(time_trunc(newcurr));
		prevtim=newcurr;
		}
	
	if(state&bmask[in_button_press])
		{
		button_motion_common(x,y,0,0);
		}

	/* warp selected signals if CTRL is pressed */
        if((event->state & GDK_CONTROL_MASK)&&(state&GDK_BUTTON1_MASK))
		{
	  	int warp = 0;
          	Trptr t = traces.first;
		TimeType gt, delta;

          	while ( t )
          		{
            		if ( t->flags & TR_HIGHLIGHT )
            			{
	      			warp++;

				if(!t->shift_drag_valid)
					{
					t->shift_drag = t->shift;
					t->shift_drag_valid = 1;
					}

              			gt = t->shift_drag + (tims.marker - tims.lmbcache);

		        	if(gt<0)
        		        	{
		                	delta=tims.first-tims.last;
		                	if(gt<delta) gt=delta;
		                	}
		       		else 
				if(gt>0)
		                	{
		                	delta=tims.last-tims.first;
		                	if(gt>delta) gt=delta;
		                	}
				t->shift = gt;
            			}

            		t = t->t_next;
          		}

	  	if( warp )
	  		{
/* commented out to reduce on visual noise...

            		signalwindow_width_dirty = 1;
            		MaxSignalLength(  );

...commented out to reduce on visual noise */

            		signalarea_configure_event( signalarea, NULL );
            		wavearea_configure_event( wavearea, NULL );
          		}
		}

	if(scrolled)	/* make sure counters up top update.. */
		{
		while(gtk_events_pending())
			{
			gtk_main_iteration();
			}
		}

	WAVE_GDK_GET_POINTER(event->window, &x, &y, &xi, &yi, &state);
	WAVE_GDK_GET_POINTER_COPY;

	} while((scrolled)&&(state&bmask[in_button_press]));

return(TRUE);
}

#ifdef WAVE_USE_GTK2
static        gint
scroll_event( GtkWidget * widget, GdkEventScroll * event )
{
  DEBUG(printf("Mouse Scroll Event\n"));
  switch ( event->direction )
  {
    case GDK_SCROLL_UP:
      if ( event->state & GDK_CONTROL_MASK )
        service_left_shift(NULL, 0);
      else if ( event->state & GDK_MOD1_MASK )
	service_zoom_out(NULL, 0);
      else
        service_left_page(NULL, 0);
      break;
    case GDK_SCROLL_DOWN:
      if ( event->state & GDK_CONTROL_MASK )
        service_right_shift(NULL, 0);
      else if ( event->state & GDK_MOD1_MASK )
	service_zoom_in(NULL, 0);
      else
        service_right_page(NULL, 0);
      break;

    default:
      break;
  }
  return(TRUE);
}
#endif

static gint button_press_event(GtkWidget *widget, GdkEventButton *event)
{
if((event->button==1)||((event->button==3)&&(!in_button_press)))
	{
	in_button_press=event->button;
	
	DEBUG(printf("Button Press Event\n"));
	button_motion_common(event->x,event->y,1,0);
	tims.timecache=tims.start;

	gdk_pointer_grab(widget->window, FALSE,
		m_bmask[in_button_press] | 				/* key up on motion for button pressed ONLY */
		GDK_POINTER_MOTION_HINT_MASK |
	      	GDK_BUTTON_RELEASE_MASK, NULL, NULL, event->time);
	
	if ((event->state & GDK_CONTROL_MASK) && (event->button==1))
		{
		Trptr t = traces.first;

		while(t)
			{
			if((t->flags & TR_HIGHLIGHT)&&(!t->shift_drag_valid))			
				{
				t->shift_drag = t->shift; /* cache old value */
				t->shift_drag_valid = 1;
				}
			t=t->t_next;
			}
		}
	}
else
if(event->button==2)
	{
	button_motion_common(event->x,event->y,1,1);
	}

return(TRUE);
}

static gint button_release_event(GtkWidget *widget, GdkEventButton *event)
{
if((event->button)&&(event->button==in_button_press))
	{
	in_button_press=0;

	DEBUG(printf("Button Release Event\n"));
	button_motion_common(event->x,event->y,1,0);

	/* warp selected signals if CTRL is pressed */
        if((event->button==1))
		{
	  	int warp = 0;
          	Trptr t = traces.first;

		if(event->state & GDK_CONTROL_MASK)
			{	
			TimeType gt, delta;

	          	while ( t )
	          		{
	            		if ( t->flags & TR_HIGHLIGHT )
	            			{
		      			warp++;
	              			gt = (t->shift_drag_valid ? t-> shift_drag : t->shift) + (tims.marker - tims.lmbcache);
	
			        	if(gt<0)
	        		        	{
			                	delta=tims.first-tims.last;
			                	if(gt<delta) gt=delta;
			                	}
			       		else 
					if(gt>0)
			                	{
			                	delta=tims.last-tims.first;
			                	if(gt>delta) gt=delta;
			                	}
					t->shift = gt;
	
	              			t->flags &= ( ~TR_HIGHLIGHT );
	            			}
	
				t->shift_drag_valid = 0;
	            		t = t->t_next;
	          		}
			}
			else	/* back out warp and keep highlighting */
			{
			while(t)
				{
				if(t->shift_drag_valid)
					{
					t->shift = t->shift_drag;
					t->shift_drag_valid = 0;
					warp++;
					}
				t=t->t_next;
				}
			}

	  	if( warp )
	  		{
            		signalwindow_width_dirty = 1;
            		MaxSignalLength(  );
            		signalarea_configure_event( signalarea, NULL );
            		wavearea_configure_event( wavearea, NULL );
          		}
		}

	tims.timecache=tims.start;

	gdk_pointer_ungrab(event->time);

	if(event->button==3)	/* oh yeah, dragzoooooooom! */
		{
		service_dragzoom(tims.lmbcache, tims.marker);
		}

	tims.lmbcache=-1;
	update_markertime(time_trunc(tims.marker));
	}

move_mouseover(NULL, 0, 0, LLDescriptor(0));
tims.timecache=0;
return(TRUE);
}


void make_sigarea_gcs(GtkWidget *signalarea)
{
if(!made_sgc_contexts)
	{
	gc_white = alloc_color(signalarea, color_white, signalarea->style->white_gc);
	gc_black = alloc_color(signalarea, color_black, signalarea->style->black_gc);
	gc_ltgray= alloc_color(signalarea, color_ltgray, signalarea->style->bg_gc[GTK_STATE_PRELIGHT]);
	gc_normal= alloc_color(signalarea, color_normal, signalarea->style->bg_gc[GTK_STATE_NORMAL]);
	gc_mdgray= alloc_color(signalarea, color_mdgray, signalarea->style->bg_gc[GTK_STATE_INSENSITIVE]);
	gc_dkgray= alloc_color(signalarea, color_dkgray, signalarea->style->bg_gc[GTK_STATE_ACTIVE]);
	gc_dkblue= alloc_color(signalarea, color_dkblue, signalarea->style->bg_gc[GTK_STATE_SELECTED]);

	made_sgc_contexts=~0;
	}
}

gint wavearea_configure_event(GtkWidget *widget, GdkEventConfigure *event)
{
DEBUG(printf("WaveWin Configure Event h: %d, w: %d\n",widget->allocation.height,
		widget->allocation.width));

if(wavepixmap)
	{
	if((wavewidth!=widget->allocation.width)||(waveheight!=widget->allocation.height))
		{
		gdk_pixmap_unref(wavepixmap);
		wavepixmap=gdk_pixmap_new(widget->window, wavewidth=widget->allocation.width,
			waveheight=widget->allocation.height, -1);
		}
	old_wvalue=-1.0;
	}
	else
	{
	wavepixmap=gdk_pixmap_new(widget->window, wavewidth=widget->allocation.width,
		waveheight=widget->allocation.height, -1);
	}

if(!made_gc_contexts)
	{
	gc_back   = alloc_color(wavearea, color_back, wavearea->style->white_gc);    
	gc_baseline = alloc_color(wavearea, color_baseline, wavearea->style->bg_gc[GTK_STATE_SELECTED]);    
	gc_grid   = alloc_color(wavearea, color_grid, wavearea->style->bg_gc[GTK_STATE_PRELIGHT]);
	gc_time   = alloc_color(wavearea, color_time, wavearea->style->black_gc);
	gc_timeb  = alloc_color(wavearea, color_timeb, wavearea->style->bg_gc[GTK_STATE_ACTIVE]);
	gc_value  = alloc_color(wavearea, color_value, wavearea->style->black_gc);
	gc_low    = alloc_color(wavearea, color_low, wavearea->style->black_gc);    
	gc_high   = alloc_color(wavearea, color_high, wavearea->style->black_gc);    
	gc_trans  = alloc_color(wavearea, color_trans, wavearea->style->black_gc);    
	gc_mid    = alloc_color(wavearea, color_mid, wavearea->style->black_gc);    
	gc_xfill  = alloc_color(wavearea, color_xfill, wavearea->style->bg_gc[GTK_STATE_PRELIGHT]);
	gc_x      = alloc_color(wavearea, color_x, wavearea->style->black_gc);
	gc_vbox   = alloc_color(wavearea, color_vbox, wavearea->style->black_gc);
	gc_vtrans = alloc_color(wavearea, color_vtrans, wavearea->style->black_gc);
	gc_mark   = alloc_color(wavearea, color_mark, wavearea->style->bg_gc[GTK_STATE_SELECTED]);
	gc_umark  = alloc_color(wavearea, color_umark, wavearea->style->bg_gc[GTK_STATE_SELECTED]);

        gc_0      = alloc_color(wavearea, color_0, wavearea->style->black_gc);
        gc_1      = alloc_color(wavearea, color_1, wavearea->style->black_gc);
        gc_ufill  = alloc_color(wavearea, color_ufill, wavearea->style->bg_gc[GTK_STATE_PRELIGHT]);
        gc_u      = alloc_color(wavearea, color_u, wavearea->style->black_gc);
        gc_wfill  = alloc_color(wavearea, color_wfill, wavearea->style->bg_gc[GTK_STATE_PRELIGHT]);
        gc_w      = alloc_color(wavearea, color_w, wavearea->style->black_gc);
        gc_dashfill= alloc_color(wavearea, color_dashfill, wavearea->style->bg_gc[GTK_STATE_PRELIGHT]);
        gc_dash   = alloc_color(wavearea, color_dash, wavearea->style->black_gc);

	made_gc_contexts=~0;
	}

if(wavewidth>1)
	{
	if(!do_initial_zoom_fit)
		{
		calczoom(tims.zoom);
		fix_wavehadj();
		gtk_signal_emit_by_name (GTK_OBJECT (GTK_ADJUSTMENT(wave_hslider)), "value_changed"); /* force zoom update */ 
		gtk_signal_emit_by_name (GTK_OBJECT (GTK_ADJUSTMENT(wave_hslider)), "changed"); /* force zoom update */
		}
		else
		{
		do_initial_zoom_fit=0;
		service_zoom_fit(NULL,NULL);
		}	
	}

/* tims.timecache=tims.laststart; */
return(TRUE);
}

static gint expose_event(GtkWidget *widget, GdkEventExpose *event)
{
gdk_draw_pixmap(widget->window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
		wavepixmap, 
		event->area.x, event->area.y,
		event->area.x, event->area.y,
		event->area.width, event->area.height);
draw_marker();

return(FALSE);
}

GtkWidget *
create_wavewindow(void)
{
GtkWidget *table;
GtkWidget *frame;
GtkAdjustment *hadj, *vadj;

table = gtk_table_new(10, 10, FALSE);
wavearea=gtk_drawing_area_new();
gtk_widget_show(wavearea);

gtk_widget_set_events(wavearea,
#ifdef WAVE_USE_GTK2
		GDK_SCROLL_MASK |
#endif
                GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK | 
                GDK_BUTTON_RELEASE_MASK |
                GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK
                );

gtk_signal_connect(GTK_OBJECT(wavearea), "configure_event",
                        GTK_SIGNAL_FUNC(wavearea_configure_event), NULL);
gtk_signal_connect(GTK_OBJECT(wavearea), "expose_event",
                        GTK_SIGNAL_FUNC(expose_event), NULL);
gtk_signal_connect(GTK_OBJECT(wavearea), "motion_notify_event",
                        GTK_SIGNAL_FUNC(motion_notify_event), NULL);
gtk_signal_connect(GTK_OBJECT(wavearea), "button_press_event",
                        GTK_SIGNAL_FUNC(button_press_event), NULL);
gtk_signal_connect(GTK_OBJECT(wavearea), "button_release_event",
                        GTK_SIGNAL_FUNC(button_release_event), NULL);

#ifdef WAVE_USE_GTK2
gtk_signal_connect(GTK_OBJECT(wavearea), "scroll_event",
                        GTK_SIGNAL_FUNC(scroll_event), NULL);
GTK_WIDGET_SET_FLAGS( wavearea, GTK_CAN_FOCUS );
#endif

gtk_table_attach (GTK_TABLE (table), wavearea, 0, 9, 0, 9,
                        GTK_FILL | GTK_EXPAND,
                        GTK_FILL | GTK_EXPAND, 3, 2);

wave_vslider=gtk_adjustment_new(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
vadj=GTK_ADJUSTMENT(wave_vslider);
gtk_signal_connect(GTK_OBJECT(wave_vslider), "value_changed",
                        GTK_SIGNAL_FUNC(service_vslider), NULL);
vscroll=gtk_vscrollbar_new(vadj);
/* GTK_WIDGET_SET_FLAGS(vscroll, GTK_CAN_FOCUS); */
gtk_widget_show(vscroll);
gtk_table_attach (GTK_TABLE (table), vscroll, 9, 10, 0, 9,
                        GTK_FILL,
                        GTK_FILL | GTK_SHRINK, 3, 3);

wave_hslider=gtk_adjustment_new(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
hadj=GTK_ADJUSTMENT(wave_hslider);
gtk_signal_connect(GTK_OBJECT(wave_hslider), "value_changed",
                        GTK_SIGNAL_FUNC(service_hslider), NULL);
hscroll=gtk_hscrollbar_new(hadj);
/* GTK_WIDGET_SET_FLAGS(hscroll, GTK_CAN_FOCUS); */
gtk_widget_show(hscroll);

gtk_table_attach (GTK_TABLE (table), hscroll, 0, 9, 9, 10,
                        GTK_FILL,
                        GTK_FILL | GTK_SHRINK, 3, 4);
gtk_widget_show(table);

frame=gtk_frame_new("Waves");
gtk_container_border_width(GTK_CONTAINER(frame),2);

gtk_container_add(GTK_CONTAINER(frame),table);
return(frame);
}
   

/**********************************************/

void RenderSigs(int trtarget, int update_waves)
{
Trptr t;
int i, trwhich;
int num_traces_displayable;
GtkAdjustment *hadj;
int xsrc;

hadj=GTK_ADJUSTMENT(signal_hslider);
xsrc=(gint)hadj->value;

num_traces_displayable=signalarea->allocation.height/(fontheight);
num_traces_displayable--;   /* for the time trace that is always there */

gdk_draw_rectangle(signalpixmap, gc_dkgray, TRUE, 0, -1, signal_fill_width, fontheight); 
gdk_draw_line(signalpixmap, gc_white, 0, fontheight-1, signal_fill_width-1, fontheight-1);
gdk_draw_string(signalpixmap, signalfont, gc_black, 3+xsrc, fontheight-4, "Time");

t=traces.first;
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

topmost_trace=t;
if(t)
        {
        for(i=0;(i<num_traces_displayable)&&(t);i++)
                {
		RenderSig(t, i, 0);
                t=GiveNextTrace(t);
                }
        }

if((wavepixmap)&&(update_waves))
	{
	gdk_draw_rectangle(wavepixmap, gc_back, TRUE, 0, fontheight-1,
	                        wavewidth, waveheight-fontheight+1);

	if(display_grid) rendertimes();
	rendertraces();
	}
}


int RenderSig(Trptr t, int i, int dobackground)
{
int texty, liney;
int retval;
char buf[128];
int bufxlen = 0;

buf[0] = 0;

UpdateSigValue(t); /* in case it's stale on nonprop */
if((t->name)&&(t->shift))
	{
	buf[0]='`';
	reformat_time(buf+1, t->shift, time_dimension);
	strcpy(buf+strlen(buf+1)+1,"\'");
	bufxlen=gdk_string_measure(signalfont, buf);
	}

if((!t->vector)&&(t->n.nd)&&(t->n.nd->array_height))
	{
	sprintf(buf + strlen(buf), "{%d}", t->n.nd->this_row);
	bufxlen=gdk_string_measure(signalfont, buf);
	}

liney=((i+2)*fontheight)-2;
texty=liney-(signalfont->descent);

retval=liney-fontheight+1;

if(!(t->flags&TR_HIGHLIGHT)) 
	{
	if(dobackground)	/* for the highlight routines in signalwindow.c */
		{
		if(dobackground==2)
			{
			gdk_draw_rectangle(signalpixmap, gc_normal, TRUE, 
				0, retval,
		            	signal_fill_width, fontheight-1);
			}
			else
			{
			gdk_draw_rectangle(signalpixmap, gc_ltgray, TRUE, 
				0, retval,
		            	signal_fill_width, fontheight-1);
			}
		}

	gdk_draw_line(signalpixmap, 
		gc_white,
		0, liney,
		signal_fill_width-1, liney);

	if(!(t->flags&(TR_BLANK|TR_ANALOG_BLANK_STRETCH)))
		{
		if(t->name)
			{
			if(bufxlen)
				{
				int baselen=gdk_string_measure(signalfont, t->name);
				int combined=baselen+bufxlen;

				gdk_draw_string(signalpixmap,
					signalfont,
				        gc_black,
				        left_justify_sigs?3:3+max_signal_name_pixel_width-
						combined, 
					texty,
				        t->name);
				gdk_draw_string(signalpixmap,
					signalfont,
				        gc_black,
				        left_justify_sigs?3+baselen:3+max_signal_name_pixel_width-
						bufxlen, 
					texty,
				        buf);
				}
				else
				{
				gdk_draw_string(signalpixmap,
					signalfont,
				        gc_black,
				        left_justify_sigs?3:3+max_signal_name_pixel_width-
						gdk_string_measure(signalfont, t->name), 
					texty,
				        t->name);
				}
			}

		if((t->asciivalue)&&(!(t->flags&TR_EXCLUDE)))
			gdk_draw_string(signalpixmap,
				signalfont,
	        		gc_black,
	        		max_signal_name_pixel_width+6,
				texty,
	        		t->asciivalue);
		}
		else
		{
		if(t->name)
			{
			GdkGC *comment_color = (t->flags&TR_COLLAPSED) ? gc_mdgray : gc_dkblue;

			if(bufxlen)
				{
				int baselen=gdk_string_measure(signalfont, t->name);
				int combined=baselen+bufxlen;

				gdk_draw_string(signalpixmap,
					signalfont,
				        comment_color,
				        left_justify_sigs?3:3+max_signal_name_pixel_width-
						combined, 
					texty,
				        t->name);
				gdk_draw_string(signalpixmap,
					signalfont,
				        comment_color,
				        left_justify_sigs?3+baselen:3+max_signal_name_pixel_width-
						bufxlen, 
					texty,
				        buf);
				}
				else
				{
				gdk_draw_string(signalpixmap,
					signalfont,
				        comment_color,
				        left_justify_sigs?3:3+max_signal_name_pixel_width-
						gdk_string_measure(signalfont, t->name), 
					texty,
				        t->name);
				}
			}
		}
	}
	else
	{
	gdk_draw_rectangle(signalpixmap, gc_dkblue, TRUE, 
		0, retval,
            	signal_fill_width, fontheight-1);
	gdk_draw_line(signalpixmap, 
		gc_white,
		0, liney,
		signal_fill_width-1, liney);

	if(!(t->flags&(TR_BLANK|TR_ANALOG_BLANK_STRETCH)))
		{
		if(t->name)
			{
			if(bufxlen)
				{
				int baselen=gdk_string_measure(signalfont, t->name);
				int combined=baselen+bufxlen;

				gdk_draw_string(signalpixmap,
					signalfont,
				        gc_white,
				        left_justify_sigs?3:3+max_signal_name_pixel_width-
						combined, 
					texty,
				        t->name);
				gdk_draw_string(signalpixmap,
					signalfont,
				        gc_white,
				        left_justify_sigs?3+baselen:3+max_signal_name_pixel_width-
						bufxlen, 
					texty,
				        buf);
				}
				else
				{
				gdk_draw_string(signalpixmap,
					signalfont,
				        gc_white,
				        left_justify_sigs?3:3+max_signal_name_pixel_width-
						gdk_string_measure(signalfont, t->name), 
					texty,
				        t->name);
				}
			}

		if((t->asciivalue)&&(!(t->flags&TR_EXCLUDE)))
			gdk_draw_string(signalpixmap,
		      	signalfont,
		       	gc_white,
		        max_signal_name_pixel_width+6,
			texty,
		        t->asciivalue);
		}
		else
		{
		if(t->name)
		gdk_draw_string(signalpixmap,
		      	signalfont,
		        (dobackground==2)?gc_ltgray:gc_dkgray,
		        left_justify_sigs?3:3+max_signal_name_pixel_width-
				gdk_string_measure(signalfont, t->name), 
			texty,
  		        t->name);
		}
	} 

return(retval);
}

/***************************************************************************/

void MaxSignalLength(void)
{
Trptr t;
int len=0,maxlen=0;
int vlen=0, vmaxlen=0;
char buf[128];
int bufxlen;

DEBUG(printf("signalwindow_width_dirty: %d\n",signalwindow_width_dirty));

if((!signalwindow_width_dirty)&&(use_nonprop_fonts)) return;

signalwindow_width_dirty=0;

t=traces.first;
while(t)
{
if(t->flags&(TR_BLANK|TR_ANALOG_BLANK_STRETCH))	/* for "comment" style blank traces */
	{
	if(t->name)
		{
		len=gdk_string_measure(signalfont, t->name);
		if(len>maxlen) maxlen=len;
		}
	t=GiveNextTrace(t);
	}
else
if(t->name)
	{
	bufxlen = 0;
	buf[0] = 0;

	if((shift_timebase=t->shift))
        	{
        	buf[0]='`';
        	reformat_time(buf+1, t->shift, time_dimension);
        	strcpy(buf+strlen(buf+1)+1,"\'");
        	bufxlen=gdk_string_measure(signalfont, buf);
        	}

	if((!t->vector)&&(t->n.nd)&&(t->n.nd->array_height))
		{
		sprintf(buf + strlen(buf), "{%d}", t->n.nd->this_row);
		bufxlen=gdk_string_measure(signalfont, buf);
		}

	len=gdk_string_measure(signalfont, t->name)+bufxlen;
	if(len>maxlen) maxlen=len;

	if((tims.marker!=-1)&&(!(t->flags&TR_EXCLUDE)))
		{
		t->asciitime=tims.marker;
		if(t->asciivalue) free_2(t->asciivalue);

		if(t->vector)
			{
			char *str, *str2;
			vptr v;

                        v=bsearch_vector(t->n.vec,tims.marker);
                        str=convert_ascii(t,v);
			if(str)
				{
				str2=(char *)malloc_2(strlen(str)+2);
				*str2='=';
				strcpy(str2+1,str);
				free_2(str);

				vlen=gdk_string_measure(signalfont,str2);
				t->asciivalue=str2;
				}
				else
				{
				vlen=0;
				t->asciivalue=NULL;
				}

			}
			else
			{
			char *str;
			hptr h_ptr;
			if((h_ptr=bsearch_node(t->n.nd,tims.marker)))
				{
				if(!t->n.nd->ext)
					{
					str=(char *)calloc_2(1,3*sizeof(char));
					str[0]='=';
					if(t->flags&TR_INVERT)
						{
						str[1]=AN_STR_INV[h_ptr->v.h_val];
						}
						else
						{
						str[1]=AN_STR[h_ptr->v.h_val];
						}
					t->asciivalue=str;
					vlen=gdk_string_measure(signalfont,str);
					}
					else
					{
					char *str2;

					if(h_ptr->flags&HIST_REAL)
						{
						if(!(h_ptr->flags&HIST_STRING))
							{
							str=convert_ascii_real((double *)h_ptr->v.h_vector);
							}
							else
							{
							str=convert_ascii_string((char *)h_ptr->v.h_vector);
							}
						}
						else
						{
		                        	str=convert_ascii_vec(t,h_ptr->v.h_vector);
						}

					if(str)
						{
						str2=(char *)malloc_2(strlen(str)+2);
						*str2='=';
						strcpy(str2+1,str);

						free_2(str); 

						vlen=gdk_string_measure(signalfont,str2);
						t->asciivalue=str2;
						}
						else
						{
						vlen=0;
						t->asciivalue=NULL;
						}
					}
				}
				else
				{
				vlen=0;
				t->asciivalue=NULL;
				}
			}

		if(vlen>vmaxlen)
			{
			vmaxlen=vlen;
			}
		}

	t=GiveNextTrace(t);
	}
	else
	{
	t=GiveNextTrace(t);
	}
}

max_signal_name_pixel_width = maxlen;
signal_pixmap_width=maxlen+6; 		/* 2 * 3 pixel pad */
if(tims.marker!=-1)
	{
	signal_pixmap_width+=(vmaxlen+6);
	}

if(signal_pixmap_width<60) signal_pixmap_width=60;

if(!in_button_press)
	{
	if(!do_resize_signals)
		{
		int os;
		os=48;
		gtk_widget_set_usize(GTK_WIDGET(signalwindow), 
				os+30, -1);
		}
	else
	if((do_resize_signals)&&(signalwindow))
		{
		int oldusize;
	
		oldusize=signalwindow->allocation.width;
		if(oldusize!=max_signal_name_pixel_width)
			{ /* keep signalwindow from expanding arbitrarily large */
#ifdef WAVE_USE_GTK2
			int wx, wy;
			get_window_size(&wx, &wy);

			if((3*max_signal_name_pixel_width) < (2*wx))	/* 2/3 width max */
#else
			if((3*max_signal_name_pixel_width) < (2*(wavewidth + signalwindow->allocation.width)))
#endif
				{
				int os;
				os=max_signal_name_pixel_width;
				os=(os<48)?48:os;
				gtk_widget_set_usize(GTK_WIDGET(signalwindow), 
						os+30, -1);
				}
				else
				{
				int os;
				os=48;
				gtk_widget_set_usize(GTK_WIDGET(signalwindow), 
						os+30, -1);
				}
			}
		}
	}
}
/***************************************************************************/

void UpdateSigValue(Trptr t)
{
if(!t) return;
if((t->asciivalue)&&(t->asciitime==tims.marker))return;

if((t->name)&&(!(t->flags&(TR_BLANK|TR_ANALOG_BLANK_STRETCH))))
	{
	shift_timebase=t->shift;
	DEBUG(printf("UpdateSigValue: %s\n",t->name));

	if((tims.marker!=-1)&&(!(t->flags&TR_EXCLUDE)))
		{
		t->asciitime=tims.marker;
		if(t->asciivalue) free_2(t->asciivalue);

		if(t->vector)
			{
			char *str, *str2;
			vptr v;

                        v=bsearch_vector(t->n.vec,tims.marker);
                        str=convert_ascii(t,v);
			if(str)
				{
				str2=(char *)malloc_2(strlen(str)+2);
				*str2='=';
				strcpy(str2+1,str);
				free_2(str);

				t->asciivalue=str2;
				}
				else
				{
				t->asciivalue=NULL;
				}

			}
			else
			{
			char *str;
			hptr h_ptr;
			if((h_ptr=bsearch_node(t->n.nd,tims.marker)))
				{
				if(!t->n.nd->ext)
					{
					str=(char *)calloc_2(1,3*sizeof(char));
					str[0]='=';
					if(t->flags&TR_INVERT)
						{
						str[1]=AN_STR_INV[h_ptr->v.h_val];
						}
						else
						{
						str[1]=AN_STR[h_ptr->v.h_val];
						}
					t->asciivalue=str;
					}
					else
					{
					char *str2;

					if(h_ptr->flags&HIST_REAL)
						{
						if(!(h_ptr->flags&HIST_STRING))
							{
							str=convert_ascii_real((double *)h_ptr->v.h_vector);
							}
							else
							{
							str=convert_ascii_string((char *)h_ptr->v.h_vector);
							}
						}
						else
						{
		                        	str=convert_ascii_vec(t,h_ptr->v.h_vector);
						}

					if(str)
						{
						str2=(char *)malloc_2(strlen(str)+2);
						*str2='=';
						strcpy(str2+1,str);
						free_2(str);

						t->asciivalue=str2;
						}
						else
						{
						t->asciivalue=NULL;
						}
					}
				}
				else
				{
				t->asciivalue=NULL;
				}
			}
		}
	}
}

/***************************************************************************/

void calczoom(gdouble z0)
{
gdouble ppf, frame;
ppf=((gdouble)(pixelsperframe=200));
frame=pow(zoombase,-z0);

if(frame>((gdouble)MAX_HISTENT_TIME/(gdouble)4.0))
	{
	nsperframe=((gdouble)MAX_HISTENT_TIME/(gdouble)4.0);
	}
	else
	if(frame<(gdouble)1.0)
	{
	nsperframe=1.0;
	}
	else
	{
	nsperframe=frame;
	}

hashstep=10.0;

if(zoom_pow10_snap)
if(nsperframe>10.0)
	{
	TimeType nsperframe2;
	gdouble p=10.0;
	gdouble scale;
	int l;
	l=(int)((log(nsperframe)/log(p))+0.5);	/* nearest power of 10 */
	nsperframe2=pow(p, (gdouble)l);

	scale = (gdouble)nsperframe2 / (gdouble)nsperframe;
	ppf *= scale;
	pixelsperframe = ppf;

	nsperframe = nsperframe2;
	hashstep = ppf / 10.0;
	}

nspx=nsperframe/ppf;
pxns=ppf/nsperframe;

time_trunc_set();	/* map nspx to rounding value */

DEBUG(printf("Zoom: %e Pixelsperframe: %lld, nsperframe: %e\n",z0, pixelsperframe,(float)nsperframe));
}

static void renderhash(int x, TimeType tim)
{
TimeType rborder;
int fhminus2;
int rhs;
gdouble dx;
gdouble hashoffset;
int iter = 0;

fhminus2=fontheight-2;

gdk_draw_line(wavepixmap, 
		gc_grid,
		x, 0,
		x, ((!timearray)&&(display_grid)&&(enable_vert_grid))?waveheight:fhminus2);

if(tim==tims.last) return;

rborder=(tims.last-tims.start)*pxns;
DEBUG(printf("Rborder: %lld, Wavewidth: %d\n", rborder, wavewidth));
if(rborder>wavewidth) rborder=wavewidth;
if((rhs=x+pixelsperframe)>rborder) rhs=rborder;

gdk_draw_line(wavepixmap, 
		gc_grid,
		x, wavecrosspiece,
		rhs, wavecrosspiece);

dx = x + (hashoffset=hashstep);
x  = dx;

while((hashoffset<pixelsperframe)&&(x<=rhs)&&(iter<9))
	{
	gdk_draw_line(wavepixmap, 
		gc_grid,
		x, wavecrosspiece,
		x, fhminus2);

	hashoffset+=hashstep;
	dx=dx+hashstep;
	if((pixelsperframe!=200)||(hashstep!=10.0)) iter++; /* fix any roundoff errors */
	x = dx;
	} 

}

static void rendertimes(void)
{
int lastx = -1000; /* arbitrary */
int x, lenhalf;
TimeType tim, rem;
char timebuff[32];
char prevover=0;
gdouble realx;

renderblackout();

tim=tims.start;
tims.end=tims.start+(((gdouble)wavewidth)*nspx);

/***********/
if(timearray)
	{
	int pos, pos2;
	TimeType *t, tm;
	int y=fontheight+2;
	int oldx=-1;

	pos=bsearch_timechain(tims.start);
	top:
	if((pos>=0)&&(pos<timearray_size))
		{
		t=timearray+pos;
		for(;pos<timearray_size;t++, pos++)
			{
			tm=*t;
			if(tm>=tims.start)
				{
				if(tm<=tims.end)
					{
					x=(tm-tims.start)*pxns;
					if(oldx==x) 
						{
						pos2=bsearch_timechain(tims.start+(((gdouble)(x+1))*nspx));
						if(pos2>pos) { pos=pos2; goto top; } else continue;
						}
					oldx=x;
					gdk_draw_line(wavepixmap, gc_grid, x, y, x, waveheight);
					}
					else
					{
					break;
					}
				}
			}
		}
	}
/***********/


DEBUG(printf("Ruler Start time: "TTFormat", Finish time: "TTFormat"\n",tims.start, tims.end));

x=0;
realx=0;
if(tim)
	{
	rem=tim%((TimeType)nsperframe);
	if(rem)
		{
		tim=tim-nsperframe-rem;
		x=-pixelsperframe-((rem*pixelsperframe)/nsperframe);
		realx=-pixelsperframe-((rem*pixelsperframe)/nsperframe);
		}
	}

for(;;)
	{
	renderhash(realx, tim);

	if(tim)
		{
		reformat_time(timebuff, time_trunc(tim), time_dimension);
		}
		else
		{
		strcpy(timebuff, "0");
		}

	lenhalf=gdk_string_measure(wavefont, timebuff) >> 1;

	if((x-lenhalf >= lastx) || (pixelsperframe >= 200))
		{
		gdk_draw_string(wavepixmap,
			wavefont,
		       	gc_time,
			x-lenhalf, wavefont->ascent-1,
		        timebuff);

		lastx = x+lenhalf;
		}

	tim+=nsperframe;
	x+=pixelsperframe;
	realx+=pixelsperframe;
	if((prevover)||(tim>tims.last)) break;
	if(x>=wavewidth) prevover=1;
	}
}

/***************************************************************************/

static void rendertimebar(void)
{
gdk_draw_rectangle(wavepixmap, gc_timeb, TRUE,
		0, -1, wavewidth, fontheight); 
rendertimes();
rendertraces();

update_dual();
}

static void rendertraces(void)
{
if(!topmost_trace)
	{
	topmost_trace=traces.first;
	}

if(topmost_trace)
	{
	Trptr t;
	hptr h;
	vptr v;
	int i, num_traces_displayable;

	num_traces_displayable=wavearea->allocation.height/(fontheight);
	num_traces_displayable--;   /* for the time trace that is always there */

	t=topmost_trace;
	for(i=0;((i<num_traces_displayable)&&(t));i++)
		{
		if(!(t->flags&(TR_EXCLUDE|TR_BLANK|TR_ANALOG_BLANK_STRETCH)))
			{
			shift_timebase=t->shift;
			if(!t->vector)
				{
				h=bsearch_node(t->n.nd, tims.start);
				DEBUG(printf("Bit Trace: %s, %s\n", t->name, t->n.nd->nname));
				DEBUG(printf("Start time: "TTFormat", Histent time: "TTFormat"\n", tims.start,(h->time+shift_timebase)));

				if(!t->n.nd->ext)
					{
					draw_hptr_trace(t,h,i,1,0);
					}
					else
					{
					draw_hptr_trace_vector(t,h,i);
					}
				}
				else
				{
				v=bsearch_vector(t->n.vec, tims.start);
				DEBUG(printf("Vector Trace: %s, %s\n", t->name, t->n.vec->name));
				DEBUG(printf("Start time: "TTFormat", Vectorent time: "TTFormat"\n", tims.start,(v->time+shift_timebase)));
				draw_vptr_trace(t,v,i);
				}
			}
			else
			{
			int kill_dodraw_grid = t->flags & TR_ANALOG_BLANK_STRETCH;

			if(kill_dodraw_grid)
				{
				if(!(t->t_next)) 
					{
					kill_dodraw_grid = 0;
					}
				else
				if(!(t->t_next->flags & TR_ANALOG_BLANK_STRETCH))
					{
					kill_dodraw_grid = 0;
					}
				}
			
			draw_hptr_trace(NULL,NULL,i,0,kill_dodraw_grid);
			}
		t=GiveNextTrace(t);
		}
	}


draw_named_markers();
draw_marker_partitions();
}


/*
 * draw single traces and use this for rendering the grid lines
 * for "excluded" traces
 */
static void draw_hptr_trace(Trptr t, hptr h, int which, int dodraw, int kill_grid)
{
TimeType x0, x1, newtime;
int y0, y1, yu, liney, ytext;
TimeType tim, h2tim;
hptr h2, h3;
char hval, h2val, invert;
GdkGC    *c;
GdkGC    *gcx, *gcxf;
char identifier_str[2];

tims.start-=shift_timebase;
tims.end-=shift_timebase;

liney=((which+2)*fontheight)-2;
if((t)&&(t->flags&TR_INVERT))
	{
	y0=((which+1)*fontheight)+2;	
	y1=liney-2;
	invert=1;
	}
	else
	{
	y1=((which+1)*fontheight)+2;	
	y0=liney-2;
	invert=0;
	}
yu=(y0+y1)/2;
ytext=yu-(wavefont->ascent/2)+wavefont->ascent;


if((display_grid)&&(enable_horiz_grid)&&(!kill_grid))
	{
	gdk_draw_line(wavepixmap, 
		gc_grid,
		(tims.start<tims.first)?(tims.first-tims.start)*pxns:0, liney,
		(tims.last<=tims.end)?(tims.last-tims.start)*pxns:wavewidth-1, liney);
	}

if((h)&&(tims.start==h->time))
	if (h->v.h_val != AN_Z) {

	switch(h->v.h_val)
		{
		case AN_X:	c = gc_x; break;
		case AN_U:	c = gc_u; break;
		case AN_W:	c = gc_w; break;
		case AN_DASH:	c = gc_dash; break;
		default:	c = (h->v.h_val == AN_X) ? gc_x: gc_trans;
		}
	gdk_draw_line(wavepixmap, c, 0, y0, 0, y1);
	}

if(dodraw)
for(;;)
{
if(!h) break;
tim=(h->time);
if((tim>tims.end)||(tim>tims.last)) break;

x0=(tim - tims.start) * pxns;
if(x0<-1) 
	{ 
	x0=-1; 
	}
	else
if(x0>wavewidth)
	{
	break;
	}

h2=h->next;
if(!h2) break;
h2tim=tim=(h2->time);
if(tim>tims.last) tim=tims.last;
	else if(tim>tims.end+1) tim=tims.end+1;
x1=(tim - tims.start) * pxns;
if(x1<-1) 
	{ 
	x1=-1; 
	}
	else
if(x1>wavewidth)
	{
	x1=wavewidth;
	}

if(x0!=x1)
	{
	hval=h->v.h_val;
	h2val=h2->v.h_val;

	switch(h2val)
		{
		case AN_X:	c = gc_x; break;
		case AN_U:	c = gc_u; break;
		case AN_W:	c = gc_w; break;
		case AN_DASH:	c = gc_dash; break;
		default:	c = (hval == AN_X) ? gc_x: gc_trans;
		}

	switch(hval)
		{
		case AN_0:	/* 0 */
		case AN_L:	/* L */
		wave_gdk_draw_line(wavepixmap, 
			(hval==AN_0) ? gc_0 : gc_low,
			x0, y0,
			x1, y0);

		if(h2tim<=tims.end)
		switch(h2val)
			{
			case AN_0:
			case AN_L:	break;

			case AN_Z:	wave_gdk_draw_line(wavepixmap, c, x1, y0, x1, yu); break;
			default:	wave_gdk_draw_line(wavepixmap, c, x1, y0, x1, y1); break;
			}
		break;
	
		case AN_X: /* X */
		case AN_W: /* W */
		case AN_U: /* U */
		case AN_DASH: /* - */

		identifier_str[1] = 0;
		switch(hval)
			{
			case AN_X:	c = gcx = gc_x; gcxf = gc_xfill; identifier_str[0] = 0; break;
			case AN_W:	c = gcx = gc_w; gcxf = gc_wfill; identifier_str[0] = 'W'; break;
			case AN_U:	c = gcx = gc_u; gcxf = gc_ufill; identifier_str[0] = 'U'; break;
			default:	c = gcx = gc_dash; gcxf = gc_dashfill; identifier_str[0] = '-'; break;
			}

		if(invert)
			{
			gdk_draw_rectangle(wavepixmap, 
				gcx, TRUE,
				x0+1, y0, x1-x0, y1-y0+1); 
			}
			else
			{
			gdk_draw_rectangle(wavepixmap, 
				gcxf, TRUE,
				x0+1, y1, x1-x0, y0-y1+1); 
			}

		if(identifier_str[0])
			{
			int x0_new = (x0>=0) ? x0 : 0;
			int width;

			if((width=x1-x0_new)>vector_padding)
				{
				if((x1>=wavewidth)||(gdk_string_measure(wavefont, identifier_str)+vector_padding<=width))
					{
		                        gdk_draw_string(wavepixmap,
		                                wavefont,  
		                                gc_value,  
		                                x0+2,ytext,
		                                identifier_str);
					}
				}
			}

		wave_gdk_draw_line(wavepixmap, 
			gcx,
			x0, y0,
			x1, y0);
		wave_gdk_draw_line(wavepixmap, 
			gcx,
			x0, y1,
			x1, y1);
		if(h2tim<=tims.end) wave_gdk_draw_line(wavepixmap, c, x1, y0, x1, y1);
		break;
		
		case AN_Z: /* Z */
		wave_gdk_draw_line(wavepixmap, 
			gc_mid,
			x0, yu,
			x1, yu);
		if(h2tim<=tims.end)
		switch(h2val)
			{
			case AN_0:
			case AN_L: 	
					wave_gdk_draw_line(wavepixmap, c, x1, yu, x1, y0); break;
			case AN_1:
			case AN_H:
					wave_gdk_draw_line(wavepixmap, c, x1, yu, x1, y1); break;
			default:	wave_gdk_draw_line(wavepixmap, c, x1, y0, x1, y1); break;
			}
		break;
		
		case AN_1: /* 1 */
		case AN_H: /* 1 */
		wave_gdk_draw_line(wavepixmap, 
			(hval==AN_1) ? gc_1 : gc_high,
			x0, y1,
			x1, y1);
		if(h2tim<=tims.end)
		switch(h2val)
			{
			case AN_1:
			case AN_H:	break;

			case AN_0: 	
			case AN_L:
					wave_gdk_draw_line(wavepixmap, c, x1, y1, x1, y0); break;
			case AN_Z:	wave_gdk_draw_line(wavepixmap, c, x1, y1, x1, yu); break;
			default:	wave_gdk_draw_line(wavepixmap, c, x1, y0, x1, y1); break;
			}
		break;

		default:
		break;
		}
	}
	else
	{
	wave_gdk_draw_line(wavepixmap, gc_trans, x1, y0, x1, y1);		
	newtime=(((gdouble)(x1+WAVE_OPT_SKIP))*nspx)+tims.start+shift_timebase;	/* skip to next pixel */
	h3=bsearch_node(t->n.nd,newtime);
	if(h3->time>h->time)
		{
		h=h3;
		continue;
		}
	}

h=h->next;
}

wave_gdk_draw_line_flush(wavepixmap); /* clear out state */

tims.start+=shift_timebase;
tims.end+=shift_timebase;
}

/********************************************************************************************************/

static void draw_hptr_trace_vector_analog(Trptr t, hptr h, int which, int num_extension)
{
TimeType x0, x1, newtime;
int y0, y1, yu, liney, yt0, yt1;
TimeType tim, h2tim;
hptr h2, h3;
int endcnt = 0;
int type;
int lasttype=-1;
GdkGC    *c;
double mynan = strtod("NaN", NULL);
double tmin = mynan, tmax = mynan, tv, tv2;

liney=((which+2+num_extension)*fontheight)-2;
y1=((which+1)*fontheight)+2;	
y0=liney-2;
yu=(y0+y1)/2;

h2 = h;
for(;;)
{
if(!h2) break;
tim=(h2->time);
if(tim>tims.end) { endcnt++; if(endcnt==2) break; }
if(tim>tims.last) break;
x0=(tim - tims.start) * pxns;
if((x0>wavewidth)&&(endcnt==2))
	{
	break;
	}
h3=h2;
h2 = h2->next;
if (!h2) break;
tim=(h2->time);
x1=(tim - tims.start) * pxns;
if(x1<0)
	continue;
tv = mynan;
if(h3->flags&HIST_REAL)
	{
	if(!(h3->flags&HIST_STRING) && h3->v.h_vector)
		tv = *(double *)h3->v.h_vector;
	}
	else
	{
	if(h3->time <= tims.last) tv=convert_real_vec(t,h3->v.h_vector);
	}
if (!isnan(tv))
	{
	if (isnan(tmin) || tv < tmin)
		tmin = tv;
	if (isnan(tmax) || tv > tmax)
		tmax = tv;
	}
}
if (isnan(tmin) || isnan(tmax))
	tmin = tmax = 0;
if ((tmax - tmin) < 1e-20)
	{
	tmax = 1;
	tmin -= 0.5 * (y1 - y0);
	}
	else
	{
	tmax = (y1 - y0) / (tmax - tmin);
	}

h3 = NULL;
for(;;)
{
if(!h) break;
tim=(h->time);
if((tim>tims.end)||(tim>tims.last)) break;

x0=(tim - tims.start) * pxns;
if(x0<-1) 
	{ 
	x0=-1; 
	}
	else
if(x0>wavewidth)
	{
	break;
	}

h2=h->next;
if(!h2) break;
h2tim=tim=(h2->time);
if(tim>tims.last) tim=tims.last;
	else if(tim>tims.end+1) tim=tims.end+1;
x1=(tim - tims.start) * pxns;
if(x1<-1) 
	{ 
	x1=-1; 
	}
	else
if(x1>wavewidth)
	{
	x1=wavewidth;
	}

/* draw trans */
type = (!(h->flags&(HIST_REAL|HIST_STRING))) ? vtype(t,h->v.h_vector) : AN_0;
tv = tv2 = mynan;

if(h->flags&HIST_REAL)
	{
	if(!(h->flags&HIST_STRING) && h->v.h_vector)
		tv = *(double *)h->v.h_vector;
	}
	else
	{
	if(h->time <= tims.last) tv=convert_real_vec(t,h->v.h_vector);
	}

if(h2->flags&HIST_REAL)
	{
	if(!(h2->flags&HIST_STRING) && h2->v.h_vector)
		tv2 = *(double *)h2->v.h_vector;
	}
	else
	{
	if(h2->time <= tims.last) tv2=convert_real_vec(t,h2->v.h_vector);
	}

if(isnan(tv))
	{
	yt0 = yu;
	}
	else
	{
	yt0 = y0 + (tv - tmin) * tmax;
	}
if(isnan(tv2))
	{
	yt1 = yu;
	}
	else
	{
	yt1 = y0 + (tv2 - tmin) * tmax;
	}
if(x0!=x1)
	{
	if(type == AN_0) 
		{
		c = gc_vbox; 
		} 
		else 
		{
		c = gc_x;
		}
	if(t->flags & TR_ANALOG_STEP)
		{
		wave_gdk_draw_line(wavepixmap, 
			c,
			x0, yt0,
			x1, yt0);
		wave_gdk_draw_line(wavepixmap, 
			c,
			x1, yt0,
			x1, yt1);
		}
		else
		{
		wave_gdk_draw_line(wavepixmap, 
			c,
			x0, yt0,
			x1, yt1);
		}
	}
	else
	{
	newtime=(((gdouble)(x1+WAVE_OPT_SKIP))*nspx)+tims.start+shift_timebase;	/* skip to next pixel */
	h3=bsearch_node(t->n.nd,newtime);
	if(h3->time>h->time)
		{
		h=h3;
		lasttype=type;
		continue;
		}
	}

h=h->next;
lasttype=type;
}

wave_gdk_draw_line_flush(wavepixmap);

tims.start+=shift_timebase;
tims.end+=shift_timebase;
}

/*
 * draw hptr vectors (integer+real)
 */
static void draw_hptr_trace_vector(Trptr t, hptr h, int which)
{
TimeType x0, x1, newtime, width;
int y0, y1, yu, liney, ytext;
TimeType tim, h2tim;
hptr h2, h3;
char *ascii=NULL;
int type;
int lasttype=-1;
GdkGC    *c;

tims.start-=shift_timebase;
tims.end-=shift_timebase;

liney=((which+2)*fontheight)-2;
y1=((which+1)*fontheight)+2;	
y0=liney-2;
yu=(y0+y1)/2;
ytext=yu-(wavefont->ascent/2)+wavefont->ascent;

if((display_grid)&&(enable_horiz_grid))
	{
	if(!(t->flags & TR_ANALOGMASK))
		{
		gdk_draw_line(wavepixmap, 
			gc_grid,
			(tims.start<tims.first)?(tims.first-tims.start)*pxns:0, liney,
			(tims.last<=tims.end)?(tims.last-tims.start)*pxns:wavewidth-1, liney);
		}
	}

if((t->flags & TR_ANALOGMASK) && (!(h->flags&HIST_STRING) || !(h->flags&HIST_REAL)))
	{
	Trptr te = t->t_next;
	int ext = 0;

	while(te)
		{
		if(te->flags & TR_ANALOG_BLANK_STRETCH)
			{
			ext++;
			te = te->t_next;
			}
			else
			{
			break;
			}
		}	

	draw_hptr_trace_vector_analog(t, h, which, ext);
	return;
	}

color_active_in_filter = 1;

for(;;)
{
if(!h) break;
tim=(h->time);
if((tim>tims.end)||(tim>tims.last)) break;

x0=(tim - tims.start) * pxns;
if(x0<-1) 
	{ 
	x0=-1; 
	}
	else
if(x0>wavewidth)
	{
	break;
	}

h2=h->next;
if(!h2) break;
h2tim=tim=(h2->time);
if(tim>tims.last) tim=tims.last;
	else if(tim>tims.end+1) tim=tims.end+1;
x1=(tim - tims.start) * pxns;
if(x1<-1) 
	{ 
	x1=-1; 
	}
	else
if(x1>wavewidth)
	{
	x1=wavewidth;
	}

/* draw trans */
type = (!(h->flags&(HIST_REAL|HIST_STRING))) ? vtype(t,h->v.h_vector) : AN_0;
if(x0>-1) {
if(use_roundcaps)
	{
	if (type == AN_Z) {
		if (lasttype != -1) {
		wave_gdk_draw_line(wavepixmap, 
			(lasttype==AN_X? gc_x:gc_vtrans),
			x0-1, y0,
			x0,   yu);
		wave_gdk_draw_line(wavepixmap, 
			(lasttype==AN_X? gc_x:gc_vtrans),
			x0, yu,
			x0-1, y1);
		}
	} else
	if (lasttype==AN_Z) {
		wave_gdk_draw_line(wavepixmap, 
			(type==AN_X? gc_x:gc_vtrans),
			x0+1, y0,
			x0,   yu);
		wave_gdk_draw_line(wavepixmap, 
			(type==AN_X? gc_x:gc_vtrans),
			x0, yu,
			x0+1, y1);
	} else {
		if (lasttype != type) {
		wave_gdk_draw_line(wavepixmap, 
			(lasttype==AN_X? gc_x:gc_vtrans),
			x0-1, y0,
			x0,   yu);
		wave_gdk_draw_line(wavepixmap, 
			(lasttype==AN_X? gc_x:gc_vtrans),
			x0, yu,
			x0-1, y1);
		wave_gdk_draw_line(wavepixmap, 
			(type==AN_X? gc_x:gc_vtrans),
			x0+1, y0,
			x0,   yu);
		wave_gdk_draw_line(wavepixmap, 
			(type==AN_X? gc_x:gc_vtrans),
			x0, yu,
			x0+1, y1);
		} else {
	wave_gdk_draw_line(wavepixmap, 
		(type==AN_X? gc_x:gc_vtrans),
		x0-2, y0,
		x0+2, y1);
	wave_gdk_draw_line(wavepixmap, 
		(type==AN_X? gc_x:gc_vtrans),
		x0+2, y0,
		x0-2, y1);
		}
	}
	}
	else
	{
	wave_gdk_draw_line(wavepixmap, 
		(type==AN_X? gc_x:gc_vtrans),
		x0, y0,
		x0, y1);
	}
}
		
if(x0!=x1)
	{
	if (type == AN_Z) 
		{
		if(use_roundcaps)
			{
			wave_gdk_draw_line(wavepixmap, 
				gc_mid,
				x0+1, yu,
				x1-1, yu);
			} 
			else 
			{
			wave_gdk_draw_line(wavepixmap, 
				gc_mid,
				x0, yu,
				x1, yu);
			}
		} 
		else 
		{
		if(type == AN_0) 
			{
			c = gc_vbox; 
			} 
			else 
			{
			c = gc_x;
			}
	
	if(use_roundcaps)
		{
		wave_gdk_draw_line(wavepixmap, 
			c,
			x0+2, y0,
			x1-2, y0);
		wave_gdk_draw_line(wavepixmap, 
			c,
			x0+2, y1,
			x1-2, y1);
		}
		else
		{
		wave_gdk_draw_line(wavepixmap, 
			c,
			x0, y0,
			x1, y0);
		wave_gdk_draw_line(wavepixmap, 
			c,
			x0, y1,
			x1, y1);
		}

if(x0<0) x0=0;	/* fixup left margin */

	if((width=x1-x0)>vector_padding)
		{
		char *ascii2;

		if(h->flags&HIST_REAL)
			{
			if(!(h->flags&HIST_STRING))
				{
				ascii=convert_ascii_real((double *)h->v.h_vector);
				}
				else
				{
				ascii=convert_ascii_string((char *)h->v.h_vector);
				}
			}
			else
			{
			ascii=convert_ascii_vec(t,h->v.h_vector);
			}

		ascii2 = ascii;
		if(*ascii == '?')
			{
			GdkGC *cb;
			char *srch_for_color = strchr(ascii+1, '?');
			if(srch_for_color)
				{
				*srch_for_color = 0;
				cb = get_gc_from_name(ascii+1);
				if(cb)
					{	
					ascii2 =  srch_for_color + 1;
					gdk_draw_rectangle(wavepixmap, cb, TRUE, x0+1, y1+1, width-1, (y0-1) - (y1+1) + 1);
					fill_in_smaller_rgb_areas = 1;
					}
					else
					{
					*srch_for_color = '?'; /* replace name as color is a miss */
					}
				}
			}

		if((x1>=wavewidth)||(gdk_string_measure(wavefont, ascii2)+vector_padding<=width))
			{
			gdk_draw_string(wavepixmap,
				wavefont,
			       	gc_value,
				x0+2,ytext,
			        ascii2);
			}
		else
			{
			char *mod;

			mod=bsearch_trunc(ascii2,width-vector_padding);
			if(mod)
				{
				*mod='+';
				*(mod+1)=0;

				gdk_draw_string(wavepixmap,
					wavefont,
				       	gc_value,
					x0+2,ytext,
				        ascii2);
				}
			}
		}
		else if(fill_in_smaller_rgb_areas)
		{
		char *ascii2;

		if(h->flags&HIST_REAL)
			{
			if(!(h->flags&HIST_STRING))
				{
				ascii=convert_ascii_real((double *)h->v.h_vector);
				}
				else
				{
				ascii=convert_ascii_string((char *)h->v.h_vector);
				}
			}
			else
			{
			ascii=convert_ascii_vec(t,h->v.h_vector);
			}

		ascii2 = ascii;
		if(*ascii == '?')
			{
			GdkGC *cb;
			char *srch_for_color = strchr(ascii+1, '?');
			if(srch_for_color)
				{
				*srch_for_color = 0;
				cb = get_gc_from_name(ascii+1);
				if(cb)
					{	
					ascii2 =  srch_for_color + 1;
					gdk_draw_rectangle(wavepixmap, cb, TRUE, x0, y1+1, width, (y0-1) - (y1+1) + 1);
					}
					else
					{
					*srch_for_color = '?'; /* replace name as color is a miss */
					}
				}
			}

		}
	    }
	}
	else
	{
	newtime=(((gdouble)(x1+WAVE_OPT_SKIP))*nspx)+tims.start+shift_timebase;	/* skip to next pixel */
	h3=bsearch_node(t->n.nd,newtime);
	if(h3->time>h->time)
		{
		h=h3;
		lasttype=type;
		continue;
		}
	}

if(ascii) { free_2(ascii); ascii=NULL; }
h=h->next;
lasttype=type;
}

color_active_in_filter = 0;

wave_gdk_draw_line_flush(wavepixmap);

tims.start+=shift_timebase;
tims.end+=shift_timebase;
}

/********************************************************************************************************/

static void draw_vptr_trace_analog(Trptr t, vptr v, int which, int num_extension)
{
TimeType x0, x1, newtime;
int y0, y1, yu, liney, yt0, yt1;
TimeType tim, h2tim;
vptr h, h2, h3;
int type;
int lasttype=-1;
GdkGC    *c;
double mynan = strtod("NaN", NULL);
double tmin = mynan, tmax = mynan, tv, tv2;

h=v;
liney=((which+2+num_extension)*fontheight)-2;
y1=((which+1)*fontheight)+2;	
y0=liney-2;
yu=(y0+y1)/2;

h2 = h;
for(;;)
{
if(!h2) break;
tim=(h2->time);
if((tim>tims.end)||(tim>tims.last)) break;
x0=(tim - tims.start) * pxns;
if(x0>wavewidth)
	{
	break;
	}
h3=h2;
h2 = h2->next;
if (!h2) break;
tim=(h2->time);
x1=(tim - tims.start) * pxns;
if(x1<0)
	continue;
tv=convert_real(t,h3);
if (!isnan(tv))
	{
	if (isnan(tmin) || tv < tmin)
		tmin = tv;
	if (isnan(tmax) || tv > tmax)
		tmax = tv;
	}
}
if (isnan(tmin) || isnan(tmax))
	tmin = tmax = 0;
if ((tmax - tmin) < 1e-20)
	{
	tmax = 1;
	tmin -= 0.5 * (y1 - y0);
	}
	else
	{
	tmax = (y1 - y0) / (tmax - tmin);
	}

h3 = NULL;
for(;;)
{
if(!h) break;
tim=(h->time);
if((tim>tims.end)||(tim>tims.last)) break;

x0=(tim - tims.start) * pxns;
if(x0<-1) 
	{ 
	x0=-1; 
	}
	else
if(x0>wavewidth)
	{
	break;
	}

h2=h->next;
if(!h2) break;
h2tim=tim=(h2->time);
if(tim>tims.last) tim=tims.last;
	else if(tim>tims.end+1) tim=tims.end+1;
x1=(tim - tims.start) * pxns;
if(x1<-1) 
	{ 
	x1=-1; 
	}
	else
if(x1>wavewidth)
	{
	x1=wavewidth;
	}

/* draw trans */
type = vtype2(t,h);
tv=convert_real(t,h);
tv2=convert_real(t,h2);
if(isnan(tv))
	{
	yt0 = yu;
	}
	else
	{
	yt0 = y0 + (tv - tmin) * tmax;
	}
if(isnan(tv2))
	{
	yt1 = yu;
	}
	else
	{
	yt1 = y0 + (tv2 - tmin) * tmax;
	}
if(x0!=x1)
	{
	if(type == AN_0) 
		{
		c = gc_vbox; 
		} 
		else 
		{
		c = gc_x;
		}
	if(t->flags & TR_ANALOG_STEP)
		{
		wave_gdk_draw_line(wavepixmap, 
			c,
			x0, yt0,
			x1, yt0);
		wave_gdk_draw_line(wavepixmap, 
			c,
			x1, yt0,
			x1, yt1);
		}
		else
		{
		wave_gdk_draw_line(wavepixmap, 
			c,
			x0, yt0,
			x1, yt1);
		}
	}
	else
	{
	newtime=(((gdouble)(x1+WAVE_OPT_SKIP))*nspx)+tims.start+shift_timebase;	/* skip to next pixel */
	h3=bsearch_vector(t->n.vec,newtime);
	if(h3->time>h->time)
		{
		h=h3;
		lasttype=type;
		continue;
		}
	}

h=h->next;
lasttype=type;
}

wave_gdk_draw_line_flush(wavepixmap);

tims.start+=shift_timebase;
tims.end+=shift_timebase;
}

/*
 * draw vector traces
 */
static void draw_vptr_trace(Trptr t, vptr v, int which)
{
TimeType x0, x1, newtime, width;
int y0, y1, yu, liney, ytext;
TimeType tim, h2tim;
vptr h, h2, h3;
char *ascii=NULL;
int type;
int lasttype=-1;
GdkGC    *c;

tims.start-=shift_timebase;
tims.end-=shift_timebase;

h=v;
liney=((which+2)*fontheight)-2;
y1=((which+1)*fontheight)+2;	
y0=liney-2;
yu=(y0+y1)/2;
ytext=yu-(wavefont->ascent/2)+wavefont->ascent;

if((display_grid)&&(enable_horiz_grid))
	{
	gdk_draw_line(wavepixmap, 
		gc_grid,
		(tims.start<tims.first)?(tims.first-tims.start)*pxns:0, liney,
		(tims.last<=tims.end)?(tims.last-tims.start)*pxns:wavewidth-1, liney);
	}

if(t->flags & TR_ANALOGMASK)
	{
        Trptr te = t->t_next;
        int ext = 0;
                 
        while(te)
                {
                if(te->flags & TR_ANALOG_BLANK_STRETCH)
                        {
                        ext++;
                        te = te->t_next;
                        }
                        else
                        {
                        break;
                        }
                }

	draw_vptr_trace_analog(t, v, which, ext);
	return;
	}

color_active_in_filter = 1;

for(;;)
{
if(!h) break;
tim=(h->time);
if((tim>tims.end)||(tim>tims.last)) break;

x0=(tim - tims.start) * pxns;
if(x0<-1) 
	{ 
	x0=-1; 
	}
	else
if(x0>wavewidth)
	{
	break;
	}

h2=h->next;
if(!h2) break;
h2tim=tim=(h2->time);
if(tim>tims.last) tim=tims.last;
	else if(tim>tims.end+1) tim=tims.end+1;
x1=(tim - tims.start) * pxns;
if(x1<-1) 
	{ 
	x1=-1; 
	}
	else
if(x1>wavewidth)
	{
	x1=wavewidth;
	}

/* draw trans */
type = vtype2(t,h);

if(x0>-1) {
if(use_roundcaps)
	{
	if (type == AN_Z) 
		{
		if (lasttype != -1) 
			{
			wave_gdk_draw_line(wavepixmap, 
				(lasttype==AN_X? gc_x:gc_vtrans),
				x0-1, y0,
				x0,   yu);
			wave_gdk_draw_line(wavepixmap, 
				(lasttype==AN_X? gc_x:gc_vtrans),
				x0, yu,
				x0-1, y1);
			}
		} 
		else
		if (lasttype==AN_Z) 
			{
			wave_gdk_draw_line(wavepixmap, 
				(type==AN_X? gc_x:gc_vtrans),
				x0+1, y0,
				x0,   yu);
			wave_gdk_draw_line(wavepixmap, 
				(type==AN_X? gc_x:gc_vtrans),
				x0, yu,
				x0+1, y1);
			} 
			else 
			{
			if (lasttype != type) 
				{
				wave_gdk_draw_line(wavepixmap, 
					(lasttype==AN_X? gc_x:gc_vtrans),
					x0-1, y0,
					x0,   yu);
				wave_gdk_draw_line(wavepixmap, 
					(lasttype==AN_X? gc_x:gc_vtrans),
					x0, yu,
					x0-1, y1);
				wave_gdk_draw_line(wavepixmap, 
					(type==AN_X? gc_x:gc_vtrans),
					x0+1, y0,
					x0,   yu);
				wave_gdk_draw_line(wavepixmap, 
					(type==AN_X? gc_x:gc_vtrans),
					x0, yu,
					x0+1, y1);
				} 
				else 
				{
				wave_gdk_draw_line(wavepixmap, 
					(type==AN_X? gc_x:gc_vtrans),
					x0-2, y0,
					x0+2, y1);
				wave_gdk_draw_line(wavepixmap, 
					(type==AN_X? gc_x:gc_vtrans),
					x0+2, y0,
					x0-2, y1);
				}
			}
		}
		else
		{
		wave_gdk_draw_line(wavepixmap, 
			(type==AN_X? gc_x:gc_vtrans),
			x0, y0,
			x0, y1);
		}
}

if(x0!=x1)
	{
	if (type == AN_Z) 
		{
		if(use_roundcaps)
			{
			wave_gdk_draw_line(wavepixmap, 
				gc_mid,
				x0+1, yu,
				x1-1, yu);
			} 
			else 
			{
			wave_gdk_draw_line(wavepixmap, 
				gc_mid,
				x0, yu,
				x1, yu);
			}
		} 
		else 
		{
		if(type == AN_0) 
			{
			c = gc_vbox; 
			} 
			else 
			{
			c = gc_x;
			}
	
	if(use_roundcaps)
		{
		wave_gdk_draw_line(wavepixmap, 
			c,
			x0+2, y0,
			x1-2, y0);
		wave_gdk_draw_line(wavepixmap, 
			c,
			x0+2, y1,
			x1-2, y1);
		}
		else
		{
		wave_gdk_draw_line(wavepixmap, 
			c,
			x0, y0,
			x1, y0);
		wave_gdk_draw_line(wavepixmap, 
			c,
			x0, y1,
			x1, y1);
		}


if(x0<0) x0=0;	/* fixup left margin */

	if((width=x1-x0)>vector_padding)
		{
		char *ascii2;

		ascii=convert_ascii(t,h);

		ascii2 = ascii;
		if(*ascii == '?')
			{
			GdkGC *cb;
			char *srch_for_color = strchr(ascii+1, '?');
			if(srch_for_color)
				{
				*srch_for_color = 0;
				cb = get_gc_from_name(ascii+1);
				if(cb)
					{	
					ascii2 =  srch_for_color + 1;
					gdk_draw_rectangle(wavepixmap, cb, TRUE, x0+1, y1+1, width-1, (y0-1) - (y1+1) + 1);
					fill_in_smaller_rgb_areas = 1;
					}
					else
					{
					*srch_for_color = '?'; /* replace name as color is a miss */
					}
				}
			}

		if((x1>=wavewidth)||(gdk_string_measure(wavefont, ascii2)+vector_padding<=width))
			{
			gdk_draw_string(wavepixmap,
				wavefont,
			       	gc_value,
				x0+2,ytext,
			        ascii2);
			}
		else
			{
			char *mod;

			mod=bsearch_trunc(ascii2,width-vector_padding);
			if(mod)
				{
				*mod='+';
				*(mod+1)=0;

				gdk_draw_string(wavepixmap,
					wavefont,
				       	gc_value,
					x0+2,ytext,
				        ascii2);
				}
			}

		}
		else if(fill_in_smaller_rgb_areas)
		{
		char *ascii2;

		ascii=convert_ascii(t,h);

		ascii2 = ascii;
		if(*ascii == '?')
			{
			GdkGC *cb;
			char *srch_for_color = strchr(ascii+1, '?');
			if(srch_for_color)
				{
				*srch_for_color = 0;
				cb = get_gc_from_name(ascii+1);
				if(cb)
					{	
					ascii2 =  srch_for_color + 1;
					gdk_draw_rectangle(wavepixmap, cb, TRUE, x0, y1+1, width, (y0-1) - (y1+1) + 1);
					}
					else
					{
					*srch_for_color = '?'; /* replace name as color is a miss */
					}
				}
			}
		}
	}
	}
	else
	{
	newtime=(((gdouble)(x1+WAVE_OPT_SKIP))*nspx)+tims.start+shift_timebase;	/* skip to next pixel */
	h3=bsearch_vector(t->n.vec,newtime);
	if(h3->time>h->time)
		{
		h=h3;
		lasttype=type;
		continue;
		}
	}

if(ascii) { free_2(ascii); ascii=NULL; }
lasttype=type;
h=h->next;
}

color_active_in_filter = 0;

wave_gdk_draw_line_flush(wavepixmap);

tims.start+=shift_timebase;
tims.end+=shift_timebase;
}

/*
 * $Id$
 * $Log$
 * Revision 1.2  2007/07/23 23:13:09  gtkwave
 * adds for color tags in filtered trace data
 *
 * Revision 1.1.1.1  2007/05/30 04:28:00  gtkwave
 * Imported sources
 *
 * Revision 1.3  2007/05/28 00:55:06  gtkwave
 * added support for arrays as a first class dumpfile datatype
 *
 * Revision 1.2  2007/04/20 02:08:18  gtkwave
 * initial release
 *
 */

