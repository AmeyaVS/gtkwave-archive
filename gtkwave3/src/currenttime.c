#include"globals.h"/* 
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


   


void update_maxmarker_labels(void)
{
if(GLOBALS.use_maxtime_display) 
	{
	gtk_label_set(GTK_LABEL(GLOBALS.max_or_marker_label_currenttime_c_1),GLOBALS.maxtime_label_text_currenttime_c_1);
	update_maxtime(GLOBALS.max_time);
	}
	else
	{
	gtk_label_set(GTK_LABEL(GLOBALS.max_or_marker_label_currenttime_c_1),GLOBALS.marker_label_text_currenttime_c_1);
	update_markertime(GLOBALS.tims.marker);
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
if((pnt=GLOBALS.atoi_cont_ptr))
	{
	while((ch=*(pnt++)))
		{
		if((ch==' ')||(ch=='\t')) continue;

		ich=tolower((int)ch);		
		if(ich=='s') ich=' ';	/* as in plain vanilla seconds */

		offs=strchr(GLOBALS.time_prefix_currenttime_c_1, ich);
		break;
		}
	}

if(!offs) return(rval);
doffs=strchr(GLOBALS.time_prefix_currenttime_c_1, (int)dim);
if(!doffs) return(rval); /* should *never* happen */

delta= (doffs-GLOBALS.time_prefix_currenttime_c_1) - (offs-GLOBALS.time_prefix_currenttime_c_1);
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

pnt=strchr(GLOBALS.time_prefix_currenttime_c_1, (int)dim);
if(pnt) { offset=pnt-GLOBALS.time_prefix_currenttime_c_1; } else offset=0;

for(i=offset; i>0; i--)
	{
	if(val%1000) break;
	val=val/1000;
	}

if(i)
	{
	sprintf(buf, TTFormat" %cs", val, GLOBALS.time_prefix_currenttime_c_1[i]);
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

pnt=strchr(GLOBALS.time_prefix_currenttime_c_1, (int)dim);
if(pnt) { offset=pnt-GLOBALS.time_prefix_currenttime_c_1; } else offset=0;

if(val)
	{
	k = 1 / ((double)val * GLOBALS.negpow_currenttime_c_1[offset]);

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
struct blackout_region_t *bt = GLOBALS.blackout_regions;
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

pnt=strchr(GLOBALS.time_prefix_currenttime_c_1, (int)dim);
if(pnt) { offset=pnt-GLOBALS.time_prefix_currenttime_c_1; } else offset=0;

for(i=offset; i>0; i--)
	{
	if(val%1000) break;
	val=val/1000;
	}

if(i)
	{
	sprintf(buf, TTFormat"%c%cs", val, blackout, GLOBALS.time_prefix_currenttime_c_1[i]);
	}
	else
	{
	sprintf(buf, TTFormat"%csec", val, blackout);
	}
}


void update_markertime(TimeType val)
{
#if !defined _MSC_VER && !defined __MINGW32__ 
if(GLOBALS.anno_ctx)
	{
	if(val >= 0)
		{
		GLOBALS.anno_ctx->marker_set = 0;	/* avoid race on update */
		GLOBALS.anno_ctx->marker = val / GLOBALS.time_scale;
		reformat_time(GLOBALS.anno_ctx->time_string, val, GLOBALS.time_dimension);

		GLOBALS.anno_ctx->marker_set = 1;
		}
		else
		{
		GLOBALS.anno_ctx->marker_set = 0;
		}
	}
#endif

if(!GLOBALS.use_maxtime_display)
	{
	if(val>=0)
		{
		if(GLOBALS.tims.baseline>=0)
			{
			val-=GLOBALS.tims.baseline; /* do delta instead */
			*GLOBALS.maxtext_currenttime_c_1='B';
			if(val>=0)
				{
				*(GLOBALS.maxtext_currenttime_c_1+1)='+';
				if(GLOBALS.use_frequency_delta)
					{
					reformat_time_as_frequency(GLOBALS.maxtext_currenttime_c_1+2, val, GLOBALS.time_dimension);
					}
					else
					{
					reformat_time(GLOBALS.maxtext_currenttime_c_1+2, val, GLOBALS.time_dimension);
					}
				}
				else
				{
				if(GLOBALS.use_frequency_delta)
					{
					reformat_time_as_frequency(GLOBALS.maxtext_currenttime_c_1+1, val, GLOBALS.time_dimension);
					}
					else
					{
					reformat_time(GLOBALS.maxtext_currenttime_c_1+1, val, GLOBALS.time_dimension);
					}
				}
			}
		else if(GLOBALS.tims.lmbcache>=0) 
			{
			val-=GLOBALS.tims.lmbcache; /* do delta instead */

			if(GLOBALS.use_frequency_delta)
				{
				reformat_time_as_frequency(GLOBALS.maxtext_currenttime_c_1, val, GLOBALS.time_dimension);
				}
			else
				{
				if(val>=0)
					{
					*GLOBALS.maxtext_currenttime_c_1='+';
					reformat_time(GLOBALS.maxtext_currenttime_c_1+1, val, GLOBALS.time_dimension);
					}
					else
					{
					reformat_time(GLOBALS.maxtext_currenttime_c_1, val, GLOBALS.time_dimension);
					}
				}
			}
		else
			{
			reformat_time(GLOBALS.maxtext_currenttime_c_1, val, GLOBALS.time_dimension);
			}
		}
		else
		{
		sprintf(GLOBALS.maxtext_currenttime_c_1, "--");
		}

	gtk_label_set(GTK_LABEL(GLOBALS.maxtimewid_currenttime_c_1), GLOBALS.maxtext_currenttime_c_1);
	}
}


void update_basetime(TimeType val)
{
if(val>=0)
	{
	gtk_label_set(GTK_LABEL(GLOBALS.base_or_curtime_label_currenttime_c_1), "Base Marker");
	reformat_time(GLOBALS.curtext_currenttime_c_1, val, GLOBALS.time_dimension);
	}
	else
	{
	gtk_label_set(GTK_LABEL(GLOBALS.base_or_curtime_label_currenttime_c_1), "Current Time");
	reformat_time_blackout(GLOBALS.curtext_currenttime_c_1, GLOBALS.cached_currenttimeval_currenttime_c_1, GLOBALS.time_dimension);
	}

gtk_label_set(GTK_LABEL(GLOBALS.curtimewid_currenttime_c_1), GLOBALS.curtext_currenttime_c_1);
}


void update_maxtime(TimeType val)
{
GLOBALS.max_time=val;

if(GLOBALS.use_maxtime_display)
	{
	reformat_time(GLOBALS.maxtext_currenttime_c_1, val, GLOBALS.time_dimension);
	gtk_label_set(GTK_LABEL(GLOBALS.maxtimewid_currenttime_c_1), GLOBALS.maxtext_currenttime_c_1);
	}
}


void update_currenttime(TimeType val)
{
GLOBALS.cached_currenttimeval_currenttime_c_1 = val;

if(GLOBALS.tims.baseline<0)
	{
	GLOBALS.currenttime=val;
	reformat_time_blackout(GLOBALS.curtext_currenttime_c_1, val, GLOBALS.time_dimension);
	gtk_label_set(GTK_LABEL(GLOBALS.curtimewid_currenttime_c_1), GLOBALS.curtext_currenttime_c_1);
	}
}

   
/* Create an entry box */
GtkWidget *
create_time_box(void)
{
GtkWidget *mainbox;
GtkWidget *eventbox;

GLOBALS.max_or_marker_label_currenttime_c_1=(GLOBALS.use_maxtime_display)
	? gtk_label_new(GLOBALS.maxtime_label_text_currenttime_c_1)
	: gtk_label_new(GLOBALS.marker_label_text_currenttime_c_1);

GLOBALS.maxtext_currenttime_c_1=(char *)malloc_2(40);
if(GLOBALS.use_maxtime_display)
	{
	reformat_time(GLOBALS.maxtext_currenttime_c_1, GLOBALS.max_time, GLOBALS.time_dimension);
	}
	else
	{
	sprintf(GLOBALS.maxtext_currenttime_c_1,"--");
	}

GLOBALS.maxtimewid_currenttime_c_1=gtk_label_new(GLOBALS.maxtext_currenttime_c_1);

GLOBALS.curtext_currenttime_c_1=(char *)malloc_2(40);
if(GLOBALS.tims.baseline<0)
	{
	GLOBALS.base_or_curtime_label_currenttime_c_1=gtk_label_new("Current Time");
	reformat_time(GLOBALS.curtext_currenttime_c_1, (GLOBALS.currenttime=GLOBALS.min_time), GLOBALS.time_dimension);
	GLOBALS.curtimewid_currenttime_c_1=gtk_label_new(GLOBALS.curtext_currenttime_c_1);
	}
	else
	{
	GLOBALS.base_or_curtime_label_currenttime_c_1=gtk_label_new("Base Marker");
	reformat_time(GLOBALS.curtext_currenttime_c_1, GLOBALS.tims.baseline, GLOBALS.time_dimension);
	GLOBALS.curtimewid_currenttime_c_1=gtk_label_new(GLOBALS.curtext_currenttime_c_1);
	}

mainbox=gtk_vbox_new(FALSE, 0);
eventbox=gtk_event_box_new();
gtk_container_add(GTK_CONTAINER(eventbox), mainbox);

gtk_box_pack_start(GTK_BOX(mainbox), GLOBALS.max_or_marker_label_currenttime_c_1, TRUE, FALSE, 0);
gtk_widget_show(GLOBALS.max_or_marker_label_currenttime_c_1);
gtk_box_pack_start(GTK_BOX(mainbox), GLOBALS.maxtimewid_currenttime_c_1, TRUE, FALSE, 0);
gtk_widget_show(GLOBALS.maxtimewid_currenttime_c_1);

gtk_box_pack_start(GTK_BOX(mainbox), GLOBALS.base_or_curtime_label_currenttime_c_1, TRUE, FALSE, 0);
gtk_widget_show(GLOBALS.base_or_curtime_label_currenttime_c_1);
gtk_box_pack_start(GTK_BOX(mainbox), GLOBALS.curtimewid_currenttime_c_1, TRUE, FALSE, 0);
gtk_widget_show(GLOBALS.curtimewid_currenttime_c_1);
gtk_widget_show(mainbox);   

return(eventbox);
}
   



TimeType time_trunc(TimeType t)
{
if(!GLOBALS.use_full_precision)
if(GLOBALS.time_trunc_val_currenttime_c_1!=1)
	{
	t=t/GLOBALS.time_trunc_val_currenttime_c_1;
	t=t*GLOBALS.time_trunc_val_currenttime_c_1;
	if(t<GLOBALS.tims.first) t=GLOBALS.tims.first;
	}
 
return(t);
}

void time_trunc_set(void)
{
gdouble gcompar=1e15;
TimeType compar=LLDescriptor(1000000000000000);

for(;compar!=1;compar=compar/10,gcompar=gcompar/((gdouble)10.0))
	{
	if(GLOBALS.nspx>=gcompar)
		{
		GLOBALS.time_trunc_val_currenttime_c_1=compar;
		return;
		}
        }
 
GLOBALS.time_trunc_val_currenttime_c_1=1;
}


/*
 * called by lxt/lxt2/vzt reader inits
 */
void exponent_to_time_scale(signed char scale)
{
switch(scale)
        {
        case 0:         GLOBALS.time_dimension = 's'; break;

        case -1:        GLOBALS.time_scale = LLDescriptor(100); GLOBALS.time_dimension = 'm'; break;
        case -2:        GLOBALS.time_scale = LLDescriptor(10);
        case -3:                                        GLOBALS.time_dimension = 'm'; break;

        case -4:        GLOBALS.time_scale = LLDescriptor(100); GLOBALS.time_dimension = 'u'; break;
        case -5:        GLOBALS.time_scale = LLDescriptor(10);
        case -6:                                        GLOBALS.time_dimension = 'u'; break;

        case -10:       GLOBALS.time_scale = LLDescriptor(100); GLOBALS.time_dimension = 'p'; break;
        case -11:       GLOBALS.time_scale = LLDescriptor(10);
        case -12:                                       GLOBALS.time_dimension = 'p'; break;

        case -13:       GLOBALS.time_scale = LLDescriptor(100); GLOBALS.time_dimension = 'f'; break;
        case -14:       GLOBALS.time_scale = LLDescriptor(10);
        case -15:                                       GLOBALS.time_dimension = 'f'; break;

        case -7:        GLOBALS.time_scale = LLDescriptor(100); GLOBALS.time_dimension = 'n'; break;
        case -8:        GLOBALS.time_scale = LLDescriptor(10);
        case -9:
        default:                                        GLOBALS.time_dimension = 'n'; break;
        }
}

/*
 * $Id$
 * $Log$
 * Revision 1.1.1.1  2007/05/30 04:27:21  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:11  gtkwave
 * initial release
 *
 */

