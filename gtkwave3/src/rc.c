#include"globals.h"/* 
 * Copyright (c) Tony Bybell 1999-2007.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */


/* AIX may need this for alloca to work */ 
#if defined _AIX
  #pragma alloca
#endif

#include <config.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <fcntl.h>   
#include <errno.h>
#include <sys/types.h>
#include "analyzer.h"
#include "currenttime.h"
#include "symbol.h"  
#include "vcd.h"
#include "wavealloca.h"
#include "fgetdynamic.h"
#include "debug.h"
#include "main.h"
#include "menu.h"
#include "color.h"
#include "vlist.h"
#include "rc.h"

#ifndef _MSC_VER
#ifndef __MINGW32__
	#include <unistd.h>
	#include <pwd.h>
#else
	static char *rcname="gtkwave.ini";      /* name of environment file--WIN32 */
#endif
#else
	static char *rcname="gtkwave.ini";      /* name of environment file--WIN32 */
	#define strcasecmp _stricmp
#endif


/*
 * functions that set the individual rc variables..
 */
int f_accel(char *str)
{
DEBUG(printf("f_accel(\"%s\")\n",str));

if(strlen(str)) { set_wave_menu_accelerator(str); }

return(0);
}

int f_alt_hier_delimeter(char *str)
{
DEBUG(printf("f_alt_hier_delimeter(\"%s\")\n",str));

if(strlen(str)) { GLOBALS.alt_hier_delimeter=str[0]; }
return(0);
}

int f_append_vcd_hier(char *str)
{
DEBUG(printf("f_append_vcd_hier(\"%s\")\n",str));
append_vcd_slisthier(str);
return(0);
}

int f_atomic_vectors(char *str)
{
DEBUG(printf("f_atomic_vectors(\"%s\")\n",str));
GLOBALS.atomic_vectors=atoi_64(str)?1:0;
return(0);
}

int f_autoname_bundles(char *str)
{
DEBUG(printf("f_autoname_bundles(\"%s\")\n",str));
GLOBALS.autoname_bundles=atoi_64(str)?1:0;
return(0);
}

int f_autocoalesce(char *str)
{
DEBUG(printf("f_autocoalesce(\"%s\")\n",str));
GLOBALS.autocoalesce=atoi_64(str)?1:0;
return(0);
}

int f_autocoalesce_reversal(char *str)
{
DEBUG(printf("f_autocoalesce_reversal(\"%s\")\n",str));
GLOBALS.autocoalesce_reversal=atoi_64(str)?1:0;
return(0);
}

int f_constant_marker_update(char *str)
{
DEBUG(printf("f_constant_marker_update(\"%s\")\n",str));
GLOBALS.constant_marker_update=atoi_64(str)?1:0;
return(0);
}

int f_convert_to_reals(char *str)
{
DEBUG(printf("f_convert_to_reals(\"%s\")\n",str));
GLOBALS.convert_to_reals=atoi_64(str)?1:0;
return(0);
}

int f_cursor_snap(char *str)
{
int val;
DEBUG(printf("f_cursor_snap(\"%s\")\n",str));
val=atoi_64(str);
GLOBALS.cursor_snap=(val<=0)?0:val;
return(0);
}

int f_disable_mouseover(char *str)
{
DEBUG(printf("f_disable_mouseover(\"%s\")\n",str));
GLOBALS.disable_mouseover=atoi_64(str)?1:0;
return(0);
}

int f_disable_tooltips(char *str)
{
DEBUG(printf("f_disable_tooltips(\"%s\")\n",str));
GLOBALS.disable_tooltips=atoi_64(str)?1:0;
return(0);
}

int f_do_initial_zoom_fit(char *str)
{
DEBUG(printf("f_do_initial_zoom_fit(\"%s\")\n",str));
GLOBALS.do_initial_zoom_fit=atoi_64(str)?1:0;
return(0);
}

int f_dynamic_resizing(char *str)
{
DEBUG(printf("f_dynamic_resizing(\"%s\")\n",str));
GLOBALS.do_resize_signals=atoi_64(str)?1:0;
return(0);
}

int f_enable_fast_exit(char *str)
{
DEBUG(printf("f_enable_fast_exit(\"%s\")\n",str));
GLOBALS.enable_fast_exit=atoi_64(str)?1:0;
return(0);
}

int f_enable_ghost_marker(char *str)
{
DEBUG(printf("f_enable_ghost_marker(\"%s\")\n",str));
GLOBALS.enable_ghost_marker=atoi_64(str)?1:0;
return(0);
}

int f_enable_horiz_grid(char *str)
{
DEBUG(printf("f_enable_horiz_grid(\"%s\")\n",str));
GLOBALS.enable_horiz_grid=atoi_64(str)?1:0;
return(0);
}

int f_enable_vcd_autosave(char *str)
{
DEBUG(printf("f_enable_vcd_autosave(\"%s\")\n",str));
GLOBALS.make_vcd_save_file=atoi_64(str)?1:0;
return(0);
}

int f_enable_vert_grid(char *str)
{
DEBUG(printf("f_enable_vert_grid(\"%s\")\n",str));
GLOBALS.enable_vert_grid=atoi_64(str)?1:0;
return(0);
}

int f_fontname_logfile(char *str)
{
DEBUG(printf("f_fontname_logfile(\"%s\")\n",str));
if(GLOBALS.fontname_logfile) free_2(GLOBALS.fontname_logfile);
GLOBALS.fontname_logfile=(char *)malloc_2(strlen(str)+1);
strcpy(GLOBALS.fontname_logfile,str);
return(0);
}

int f_fontname_signals(char *str)
{
DEBUG(printf("f_fontname_signals(\"%s\")\n",str));
if(GLOBALS.fontname_signals) free_2(GLOBALS.fontname_signals);
GLOBALS.fontname_signals=(char *)malloc_2(strlen(str)+1);
strcpy(GLOBALS.fontname_signals,str);
return(0);
}

int f_fontname_waves(char *str)
{
DEBUG(printf("f_fontname_signals(\"%s\")\n",str));
if(GLOBALS.fontname_waves) free_2(GLOBALS.fontname_waves);
GLOBALS.fontname_waves=(char *)malloc_2(strlen(str)+1);
strcpy(GLOBALS.fontname_waves,str);
return(0);
}

int f_force_toolbars(char *str)
{
DEBUG(printf("f_force_toolbars(\"%s\")\n",str));
GLOBALS.force_toolbars=atoi_64(str)?1:0;
return(0);
}

int f_hide_sst(char *str)
{
DEBUG(printf("f_hide_sst(\"%s\")\n",str));
GLOBALS.hide_sst=atoi_64(str)?1:0;
return(0);
}

int f_sst_expanded(char *str)
{
DEBUG(printf("f_sst_expanded(\"%s\")\n",str));
GLOBALS.sst_expanded=atoi_64(str)?1:0;
return(0);
}

int f_hier_delimeter(char *str)
{
DEBUG(printf("f_hier_delimeter(\"%s\")\n",str));

if(strlen(str)) { GLOBALS.hier_delimeter=str[0]; GLOBALS.hier_was_explicitly_set=1; }
return(0);
}

int f_hier_grouping(char *str)
{
DEBUG(printf("f_hier_grouping(\"%s\")\n",str));
GLOBALS.hier_grouping=atoi_64(str)?1:0;
return(0);
}

int f_hier_max_level(char *str)
{
DEBUG(printf("f_hier_max_level(\"%s\")\n",str));
GLOBALS.hier_max_level=atoi_64(str);
return(0);
}

int f_hpane_pack(char *str)
{
DEBUG(printf("f_hpane_pack(\"%s\")\n",str));
GLOBALS.paned_pack_semantics=atoi_64(str)?1:0;
return(0);
}

int f_ignore_savefile_pos(char *str)
{
DEBUG(printf("f_ignore_savefile_pos(\"%s\")\n",str));
GLOBALS.ignore_savefile_pos=atoi_64(str)?1:0;
return(0);
}

int f_ignore_savefile_size(char *str)
{
DEBUG(printf("f_ignore_savefile_size(\"%s\")\n",str));
GLOBALS.ignore_savefile_size=atoi_64(str)?1:0;
return(0);
}

int f_initial_window_x(char *str)
{
int val;
DEBUG(printf("f_initial_window_x(\"%s\")\n",str));
val=atoi_64(str);
GLOBALS.initial_window_x=(val<=0)?-1:val;
return(0);
}

int f_initial_window_xpos(char *str)
{
int val;
DEBUG(printf("f_initial_window_xpos(\"%s\")\n",str));
val=atoi_64(str);
GLOBALS.initial_window_xpos=(val<=0)?-1:val;
return(0);
}

int f_initial_window_y(char *str)
{
int val;
DEBUG(printf("f_initial_window_y(\"%s\")\n",str));
val=atoi_64(str);
GLOBALS.initial_window_y=(val<=0)?-1:val;
return(0);
}

int f_initial_window_ypos(char *str)
{
int val;
DEBUG(printf("f_initial_window_ypos(\"%s\")\n",str));
val=atoi_64(str);
GLOBALS.initial_window_ypos=(val<=0)?-1:val;
return(0);
}

int f_left_justify_sigs(char *str)
{
DEBUG(printf("f_left_justify_sigs(\"%s\")\n",str));
GLOBALS.left_justify_sigs=atoi_64(str)?1:0;
return(0);
}

int f_lxt_clock_compress_to_z(char *str)
{
DEBUG(printf("f_lxt_clock_compress_to_z(\"%s\")\n",str));
GLOBALS.lxt_clock_compress_to_z=atoi_64(str)?1:0;
return(0);
}

int f_page_divisor(char *str)
{
DEBUG(printf("f_page_divisor(\"%s\")\n",str));
sscanf(str,"%lg",&GLOBALS.page_divisor);

if(GLOBALS.page_divisor<0.01)
	{
	GLOBALS.page_divisor=0.01;
	}
else
if(GLOBALS.page_divisor>100.0)
	{
	GLOBALS.page_divisor=100.0;
	}

if(GLOBALS.page_divisor>1.0) GLOBALS.page_divisor=1.0/GLOBALS.page_divisor;

return(0);
}

int f_ps_maxveclen(char *str)
{
DEBUG(printf("f_ps_maxveclen(\"%s\")\n",str));
GLOBALS.ps_maxveclen=atoi_64(str);
if(GLOBALS.ps_maxveclen<4)
	{
	GLOBALS.ps_maxveclen=4;
	}
else
if(GLOBALS.ps_maxveclen>66)
	{
	GLOBALS.ps_maxveclen=66;
	}

return(0);
}

int f_show_base_symbols(char *str)
{
DEBUG(printf("f_show_base_symbols(\"%s\")\n",str));
GLOBALS.show_base=atoi_64(str)?1:0;
return(0);
}

int f_show_grid(char *str)
{
DEBUG(printf("f_show_grid(\"%s\")\n",str));
GLOBALS.display_grid=atoi_64(str)?1:0;
return(0);
}

int f_splash_disable(char *str)
{
DEBUG(printf("f_splash_disable(\"%s\")\n",str));
GLOBALS.splash_disable=atoi_64(str)?1:0;
return(0);
}

int f_use_big_fonts(char *str)
{
DEBUG(printf("f_use_big_fonts(\"%s\")\n",str));
GLOBALS.use_big_fonts=atoi_64(str)?1:0;
return(0);
}

int f_use_full_precision(char *str)
{
DEBUG(printf("f_use_full_precision(\"%s\")\n",str));
GLOBALS.use_full_precision=atoi_64(str)?1:0;
return(0);
}

int f_use_frequency_display(char *str)
{
DEBUG(printf("f_use_frequency_display(\"%s\")\n",str));
GLOBALS.use_frequency_delta=atoi_64(str)?1:0;
return(0);
}

int f_use_maxtime_display(char *str)
{
DEBUG(printf("f_use_maxtime_display(\"%s\")\n",str));
GLOBALS.use_maxtime_display=atoi_64(str)?1:0;
return(0);
}

int f_use_nonprop_fonts(char *str)
{
DEBUG(printf("f_use_nonprop_fonts(\"%s\")\n",str));
GLOBALS.use_nonprop_fonts=atoi_64(str)?1:0;
return(0);
}

int f_use_roundcaps(char *str)
{
DEBUG(printf("f_use_roundcaps(\"%s\")\n",str));
GLOBALS.use_roundcaps=atoi_64(str)?1:0;
return(0);
}

int f_use_scrollbar_only(char *str)
{
DEBUG(printf("f_use_scrollbar_only(\"%s\")\n",str));
GLOBALS.use_scrollbar_only=atoi_64(str)?1:0;
return(0);
}

int f_vcd_explicit_zero_subscripts(char *str)
{
DEBUG(printf("f_vcd_explicit_zero_subscripts(\"%s\")\n",str));
GLOBALS.vcd_explicit_zero_subscripts=atoi_64(str)?0:-1;	/* 0==yes, -1==no */
return(0);
}

int f_vcd_preserve_glitches(char *str)
{
DEBUG(printf("f_vcd_preserve_glitches(\"%s\")\n",str));
GLOBALS.vcd_preserve_glitches=atoi_64(str)?0:-1;	/* 0==yes, -1==no */
return(0);
}

int f_vcd_warning_filesize(char *str)
{
DEBUG(printf("f_vcd_preserve_glitches(\"%s\")\n",str));
GLOBALS.vcd_warning_filesize=atoi_64(str);
return(0);
}

int f_vector_padding(char *str)
{
DEBUG(printf("f_vector_padding(\"%s\")\n",str));
GLOBALS.vector_padding=atoi_64(str);
if(GLOBALS.vector_padding<4) GLOBALS.vector_padding=4;
else if(GLOBALS.vector_padding>16) GLOBALS.vector_padding=16;
return(0);
}

int f_vlist_compression(char *str)
{
DEBUG(printf("f_vlist_compression(\"%s\")\n",str));
GLOBALS.vlist_compression_depth=atoi_64(str);
if(GLOBALS.vlist_compression_depth<0) GLOBALS.vlist_compression_depth = -1;
if(GLOBALS.vlist_compression_depth>9) GLOBALS.vlist_compression_depth = 9;
return(0);
}

int f_wave_scrolling(char *str)
{
DEBUG(printf("f_wave_scrolling(\"%s\")\n",str));
GLOBALS.wave_scrolling=atoi_64(str)?1:0;
return(0);
}

int f_zoom_base(char *str)
{
float f;
DEBUG(printf("f_zoom_base(\"%s\")\n",str));
sscanf(str,"%f",&f);
if(f<1.5) f=1.5; else if(f>10.0) f=10.0;
GLOBALS.zoombase=(gdouble)f;
return(0);
}

int f_zoom_center(char *str)
{
DEBUG(printf("f_zoom_center(\"%s\")\n",str));
GLOBALS.do_zoom_center=atoi_64(str)?1:0;
return(0);
}

int f_zoom_pow10_snap(char *str)
{
DEBUG(printf("f_zoom_pow10_snap(\"%s\")\n",str));
GLOBALS.zoom_pow10_snap=atoi_64(str)?1:0;
return(0);
}


int rc_compare(const void *v1, const void *v2)
{
return(strcasecmp((char *)v1, ((struct rc_entry *)v2)->name));
}


/* make the color functions */
#define color_make(Z) int f_color_##Z (char *str) \
{ \
int rgb; \
if((rgb=get_rgb_from_name(str))!=~0) \
	{ \
	color_##Z=rgb; \
	} \
return(0); \
}



int f_color_back (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_back=rgb; } return(0); }
int f_color_baseline (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_baseline=rgb; } return(0); }
int f_color_grid (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_grid=rgb; } return(0); }
int f_color_high (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_high=rgb; } return(0); }
int f_color_low (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_low=rgb; } return(0); }
int f_color_1 (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_1=rgb; } return(0); }
int f_color_0 (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_0=rgb; } return(0); }
int f_color_mark (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_mark=rgb; } return(0); }
int f_color_mid (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_mid=rgb; } return(0); }
int f_color_time (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_time=rgb; } return(0); }
int f_color_timeb (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_timeb=rgb; } return(0); }
int f_color_trans (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_trans=rgb; } return(0); }
int f_color_umark (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_umark=rgb; } return(0); }
int f_color_value (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_value=rgb; } return(0); }
int f_color_vbox (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_vbox=rgb; } return(0); }
int f_color_vtrans (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_vtrans=rgb; } return(0); }
int f_color_x (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_x=rgb; } return(0); }
int f_color_xfill (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_xfill=rgb; } return(0); }
int f_color_u (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_u=rgb; } return(0); }
int f_color_ufill (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_ufill=rgb; } return(0); }
int f_color_w (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_w=rgb; } return(0); }
int f_color_wfill (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_wfill=rgb; } return(0); }
int f_color_dash (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_dash=rgb; } return(0); }
int f_color_dashfill (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_dashfill=rgb; } return(0); }
int f_color_white (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_white=rgb; } return(0); }
int f_color_black (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_black=rgb; } return(0); }
int f_color_ltgray (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_ltgray=rgb; } return(0); }
int f_color_normal (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_normal=rgb; } return(0); }
int f_color_mdgray (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_mdgray=rgb; } return(0); }
int f_color_dkgray (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_dkgray=rgb; } return(0); }
int f_color_dkblue (char *str) { int rgb; if((rgb=get_rgb_from_name(str))!=~0) { GLOBALS.color_dkblue=rgb; } return(0); }


/*color_make(back)
color_make(baseline)
color_make(grid)
color_make(high)
color_make(low)
color_make(1)
color_make(0)
color_make(mark)
color_make(mid)
color_make(time)
color_make(timeb)
color_make(trans)
color_make(umark)
color_make(value)
color_make(vbox)
color_make(vtrans)
color_make(x)
color_make(xfill)
color_make(u)
color_make(ufill)
color_make(w)
color_make(wfill)
color_make(dash)
color_make(dashfill)
color_make(white)
color_make(black)
color_make(ltgray)
color_make(normal)
color_make(mdgray)
color_make(dkgray)
color_make(dkblue)*/


/*
 * rc variables...these MUST be in alphabetical order for the bsearch!
 */ 


static void vanilla_rc(void)
{
f_splash_disable 	("off");
f_zoom_pow10_snap	("on");
f_hier_max_level	("1");
f_cursor_snap		("8");
f_use_frequency_display ("off");
f_use_maxtime_display	("off");
f_use_roundcaps		("on");
f_use_nonprop_fonts	("on");
f_constant_marker_update("on");
f_show_base_symbols	("off");
f_color_back   		("000000");
f_color_baseline 	("ffffff");
f_color_grid		("202070");
f_color_high		("79f6f2");
f_color_low		("5dbebb");
f_color_1		("00ff00");
f_color_0		("008000");
f_color_trans		("00c000");
f_color_mid		("c0c000");  
f_color_value		("ffffff");
f_color_vbox		("00ff00");
f_color_vtrans		("00c000");
f_color_x		("00ff00");
f_color_xfill		("004000");
f_color_u		("ff0000");
f_color_ufill		("400000");
f_color_w		("79f6f2");
f_color_wfill		("3f817f");
f_color_dash		("edf508");
f_color_dashfill	("7d8104");
f_color_umark		("ff8080");
f_color_mark		("ffff80");
f_color_time		("ffffff");
f_color_timeb		("000000");
f_color_white		("ffffff");
f_color_black		("000000");
f_color_ltgray		("f5f5f5");
f_color_normal		("e6e6e6");
f_color_mdgray		("eeeeee");
f_color_dkgray		("cccccc");
f_color_dkblue		("4464ac");
}


void read_rc_file(char *override_rc)
{
FILE *handle;
int i;
int num_rcitems = sizeof(GLOBALS.rcitems_rc_c_1)/sizeof(struct rc_entry);

for(i=0;i<(num_rcitems-1);i++)
	{
	if(strcmp(GLOBALS.rcitems_rc_c_1[i].name, GLOBALS.rcitems_rc_c_1[i+1].name) > 0)
		{
		fprintf(stderr, "GLOBALS.rcitems_rc_c_1 misordering: '%s' vs '%s'\n", GLOBALS.rcitems_rc_c_1[i].name, GLOBALS.rcitems_rc_c_1[i+1].name);
		exit(255);
		}
	}

/* move defaults first and only go whitescreen if instructed to do so */
if(GLOBALS.possibly_use_rc_defaults) vanilla_rc();

if((override_rc)&&((handle=fopen(override_rc,"rb"))))
	{
	/* good, we have a handle */
	}
else
#if !defined __MINGW32__ && !defined _MSC_VER
if(!(handle=fopen(GLOBALS.rcname_rc_c_1,"rb")))
	{
	char *home;
	char *rcpath;
	
	home=getpwuid(geteuid())->pw_dir;
	rcpath=(char *)alloca(strlen(home)+1+strlen(GLOBALS.rcname_rc_c_1)+1);
	strcpy(rcpath,home);
	strcat(rcpath,"/");
	strcat(rcpath,GLOBALS.rcname_rc_c_1);

	if(!(handle=fopen(rcpath,"rb")))
		{
		errno=0;
		return; /* no .rc file */
		} 
	}
#else
if(!(handle=fopen(rcname,"rb")))		/* no concept of ~ in win32 */
	{
	errno=0;
	if(possibly_use_rc_defaults) vanilla_rc();
	return; /* no .rc file */
	} 
#endif

GLOBALS.rc_line_no=0;
while(!feof(handle))
	{
	char *str;

	GLOBALS.rc_line_no++;
	if((str=fgetmalloc(handle)))
		{
		int i, len;
		len=strlen(str);
		if(len)
			{
			for(i=0;i<len;i++)
				{
				int pos;
				if((str[i]==' ')||(str[i]=='\t')) continue;	/* skip leading ws */
				if(str[i]=='#') break; 				/* is a comment */
				for(pos=i;i<len;i++)
					{
					if((str[i]==' ')||(str[i]=='\t'))
						{
						str[i]=0; /* null term envname */
	
						for(i=i+1;i<len;i++)
							{
							struct rc_entry *r;
	
							if((str[i]==' ')||(str[i]=='\t')) continue;
							if((r=bsearch((void *)(str+pos), (void *)GLOBALS.rcitems_rc_c_1, 
								sizeof(GLOBALS.rcitems_rc_c_1)/sizeof(struct rc_entry), 
								sizeof(struct rc_entry), rc_compare)))
								{
								int j;

								for(j=len-1;j>=i;j--)
									{
									if((str[j]==' ')||(str[j]=='\t')) /* nuke trailing spaces */
										{
										str[j]=0;
										continue;
										}
										else
										{
										break;
										}
									}
								r->func(str+i); /* call resolution function */
								}
							break;
							}
						break;	/* added so multiple word values work properly*/
						}
					}
				break;
				}

			}
		free_2(str);
		}
	}

fclose(handle);
errno=0;
return;
}

/*
 * $Id$
 * $Log$
 * Revision 1.1.1.1.2.3  2007/07/31 03:18:01  kermin
 * Merge Complete - I hope
 *
 * Revision 1.1.1.1.2.2  2007/07/28 19:50:40  kermin
 * Merged in the main line
 *
 * Revision 1.2  2007/07/23 23:13:08  gtkwave
 * adds for color tags in filtered trace data
 *
 * Revision 1.1.1.1  2007/05/30 04:27:37  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:17  gtkwave
 * initial release
 *
 */

