/* 
 * Copyright (c) Tony Bybell 1999-2007.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <config.h>
#include <gtk/gtk.h>
#include "currenttime.h"
#include "symbol.h"

char is_vcd=0, partial_vcd=0;

char use_maxtime_display=1;
char use_frequency_delta=0;
static GtkWidget *max_or_marker_label=NULL;
static GtkWidget *base_or_curtime_label=NULL;
   
static TimeType cached_currenttimeval=0;
TimeType currenttime=0;
TimeType max_time=0;
TimeType min_time=-1;
char display_grid=~0;       /* default to displaying grid */
TimeType time_scale=1;      /* multiplier is 1, 10, 100   */
char time_dimension='n';    /* nsec, psec, etc...         */
static char *time_prefix=" munpf";
static GtkWidget *maxtimewid;
static GtkWidget *curtimewid;
static char *maxtext;
static char *curtext;
static char *maxtime_label_text="Maximum Time";
static char *marker_label_text ="Marker Time";


void update_maxmarker_labels(void)
{
if(use_maxtime_display) 
	{
	gtk_label_set(GTK_LABEL(max_or_marker_label),maxtime_label_text);
	update_maxtime(max_time);
	}
	else
	{
	gtk_label_set(GTK_LABEL(max_or_marker_label),marker_label_text);
	update_markertime(tims.marker);
	}
}

TimeType unformat_time(const char *buf, char dim)
{
TimeType rval;
const char *pnt;
char *offs=NULL, *doffs;
char ch;
int i, ich, delta;

rval=atoi_64(buf);
if((pnt=atoi_cont_ptr))
	{
	while((ch=*(pnt++)))
		{
		if((ch==' ')||(ch=='\t')) continue;

		ich=tolower((int)ch);		
		if(ich=='s') ich=' ';	/* as in plain vanilla seconds */

		offs=strchr(time_prefix, ich);
		break;
		}
	}

if(!offs) return(rval);
doffs=strchr(time_prefix, (int)dim);
if(!doffs) return(rval); /* should *never* happen */

delta= (doffs-time_prefix) - (offs-time_prefix);
if(delta<0)
	{
	for(i=delta;i<0;i++)
		{
		rval=rval/1000;
		}
	}
	else
	{
	for(i=0;i<delta;i++)
		{
		rval=rval*1000;
		}
	}

return(rval);
}

void reformat_time(char *buf, TimeType val, char dim)
{
char *pnt;
int i, offset;

pnt=strchr(time_prefix, (int)dim);
if(pnt) { offset=pnt-time_prefix; } else offset=0;

for(i=offset; i>0; i--)
	{
	if(val%1000) break;
	val=val/1000;
	}

if(i)
	{
	sprintf(buf, TTFormat" %cs", val, time_prefix[i]);
	}
	else
	{
	sprintf(buf, TTFormat" sec", val);
	}
}


void reformat_time_as_frequency(char *buf, TimeType val, char dim)
{
char *pnt;
int offset;
double k;
static const double negpow[] = { 1.0, 1.0e-3, 1.0e-6, 1.0e-9, 1.0e-12, 1.0e-15 };

pnt=strchr(time_prefix, (int)dim);
if(pnt) { offset=pnt-time_prefix; } else offset=0;

if(val)
	{
	k = 1 / ((double)val * negpow[offset]);

	sprintf(buf, "%e Hz", k);
	}
	else
	{
	strcpy(buf, "-- Hz");
	}

}


void reformat_time_blackout(char *buf, TimeType val, char dim)
{
char *pnt;
int i, offset;
struct blackout_region_t *bt = blackout_regions;
char blackout = ' ';

while(bt)
	{
	if((val>=bt->bstart)&&(val<bt->bend))
		{
		blackout = '*';
		break;
		}

	bt=bt->next;
	}

pnt=strchr(time_prefix, (int)dim);
if(pnt) { offset=pnt-time_prefix; } else offset=0;

for(i=offset; i>0; i--)
	{
	if(val%1000) break;
	val=val/1000;
	}

if(i)
	{
	sprintf(buf, TTFormat"%c%cs", val, blackout, time_prefix[i]);
	}
	else
	{
	sprintf(buf, TTFormat"%csec", val, blackout);
	}
}


void update_markertime(TimeType val)
{
#if !defined _MSC_VER && !defined __MINGW32__ 
if(anno_ctx)
	{
	if(val >= 0)
		{
		anno_ctx->marker_set = 0;	/* avoid race on update */
		anno_ctx->marker = val / time_scale;
		reformat_time(anno_ctx->time_string, val, time_dimension);

		anno_ctx->marker_set = 1;
		}
		else
		{
		anno_ctx->marker_set = 0;
		}
	}
#endif

if(!use_maxtime_display)
	{
	if(val>=0)
		{
		if(tims.baseline>=0)
			{
			val-=tims.baseline; /* do delta instead */
			*maxtext='B';
			if(val>=0)
				{
				*(maxtext+1)='+';
				if(use_frequency_delta)
					{
					reformat_time_as_frequency(maxtext+2, val, time_dimension);
					}
					else
					{
					reformat_time(maxtext+2, val, time_dimension);
					}
				}
				else
				{
				if(use_frequency_delta)
					{
					reformat_time_as_frequency(maxtext+1, val, time_dimension);
					}
					else
					{
					reformat_time(maxtext+1, val, time_dimension);
					}
				}
			}
		else if(tims.lmbcache>=0) 
			{
			val-=tims.lmbcache; /* do delta instead */

			if(use_frequency_delta)
				{
				reformat_time_as_frequency(maxtext, val, time_dimension);
				}
			else
				{
				if(val>=0)
					{
					*maxtext='+';
					reformat_time(maxtext+1, val, time_dimension);
					}
					else
					{
					reformat_time(maxtext, val, time_dimension);
					}
				}
			}
		else
			{
			reformat_time(maxtext, val, time_dimension);
			}
		}
		else
		{
		sprintf(maxtext, "--");
		}

	gtk_label_set(GTK_LABEL(maxtimewid), maxtext);
	}
}


void update_basetime(TimeType val)
{
if(val>=0)
	{
	gtk_label_set(GTK_LABEL(base_or_curtime_label), "Base Marker");
	reformat_time(curtext, val, time_dimension);
	}
	else
	{
	gtk_label_set(GTK_LABEL(base_or_curtime_label), "Current Time");
	reformat_time_blackout(curtext, cached_currenttimeval, time_dimension);
	}

gtk_label_set(GTK_LABEL(curtimewid), curtext);
}


void update_maxtime(TimeType val)
{
max_time=val;

if(use_maxtime_display)
	{
	reformat_time(maxtext, val, time_dimension);
	gtk_label_set(GTK_LABEL(maxtimewid), maxtext);
	}
}


void update_currenttime(TimeType val)
{
cached_currenttimeval = val;

if(tims.baseline<0)
	{
	currenttime=val;
	reformat_time_blackout(curtext, val, time_dimension);
	gtk_label_set(GTK_LABEL(curtimewid), curtext);
	}
}

   
/* Create an entry box */
GtkWidget *
create_time_box(void)
{
GtkWidget *mainbox;
GtkWidget *eventbox;

max_or_marker_label=(use_maxtime_display)
	? gtk_label_new(maxtime_label_text)
	: gtk_label_new(marker_label_text);

maxtext=(char *)malloc_2(40);
if(use_maxtime_display)
	{
	reformat_time(maxtext, max_time, time_dimension);
	}
	else
	{
	sprintf(maxtext,"--");
	}

maxtimewid=gtk_label_new(maxtext);

curtext=(char *)malloc_2(40);
if(tims.baseline<0)
	{
	base_or_curtime_label=gtk_label_new("Current Time");
	reformat_time(curtext, (currenttime=min_time), time_dimension);
	curtimewid=gtk_label_new(curtext);
	}
	else
	{
	base_or_curtime_label=gtk_label_new("Base Marker");
	reformat_time(curtext, tims.baseline, time_dimension);
	curtimewid=gtk_label_new(curtext);
	}

mainbox=gtk_vbox_new(FALSE, 0);
eventbox=gtk_event_box_new();
gtk_container_add(GTK_CONTAINER(eventbox), mainbox);

gtk_box_pack_start(GTK_BOX(mainbox), max_or_marker_label, TRUE, FALSE, 0);
gtk_widget_show(max_or_marker_label);
gtk_box_pack_start(GTK_BOX(mainbox), maxtimewid, TRUE, FALSE, 0);
gtk_widget_show(maxtimewid);

gtk_box_pack_start(GTK_BOX(mainbox), base_or_curtime_label, TRUE, FALSE, 0);
gtk_widget_show(base_or_curtime_label);
gtk_box_pack_start(GTK_BOX(mainbox), curtimewid, TRUE, FALSE, 0);
gtk_widget_show(curtimewid);
gtk_widget_show(mainbox);   

return(eventbox);
}
   

static TimeType time_trunc_val=1;
char use_full_precision=0;


TimeType time_trunc(TimeType t)
{
if(!use_full_precision)
if(time_trunc_val!=1)
	{
	t=t/time_trunc_val;
	t=t*time_trunc_val;
	if(t<tims.first) t=tims.first;
	}
 
return(t);
}

void time_trunc_set(void)
{
gdouble gcompar=1e15;
TimeType compar=LLDescriptor(1000000000000000);

for(;compar!=1;compar=compar/10,gcompar=gcompar/((gdouble)10.0))
	{
	if(nspx>=gcompar)
		{
		time_trunc_val=compar;
		return;
		}
        }
 
time_trunc_val=1;
}


/*
 * called by lxt/lxt2/vzt reader inits
 */
void exponent_to_time_scale(signed char scale)
{
switch(scale)
        {
        case 0:         time_dimension = 's'; break;

        case -1:        time_scale = LLDescriptor(100); time_dimension = 'm'; break;
        case -2:        time_scale = LLDescriptor(10);
        case -3:                                        time_dimension = 'm'; break;

        case -4:        time_scale = LLDescriptor(100); time_dimension = 'u'; break;
        case -5:        time_scale = LLDescriptor(10);
        case -6:                                        time_dimension = 'u'; break;

        case -10:       time_scale = LLDescriptor(100); time_dimension = 'p'; break;
        case -11:       time_scale = LLDescriptor(10);
        case -12:                                       time_dimension = 'p'; break;

        case -13:       time_scale = LLDescriptor(100); time_dimension = 'f'; break;
        case -14:       time_scale = LLDescriptor(10);
        case -15:                                       time_dimension = 'f'; break;

        case -7:        time_scale = LLDescriptor(100); time_dimension = 'n'; break;
        case -8:        time_scale = LLDescriptor(10);
        case -9:
        default:                                        time_dimension = 'n'; break;
        }
}

/*
 * $Id$
 * $Log$
 * Revision 1.2  2007/04/20 02:08:11  gtkwave
 * initial release
 *
 */

