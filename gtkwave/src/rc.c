/* 
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
	static char *rcname=".gtkwaverc";	/* name of environment file--POSIX */
#else
	static char *rcname="gtkwave.ini";      /* name of environment file--WIN32 */
#endif
#else
	static char *rcname="gtkwave.ini";      /* name of environment file--WIN32 */
	#define strcasecmp _stricmp
#endif

int rc_line_no;
int possibly_use_rc_defaults = 1;		/* if not -d option, use white screen, etc on no .gtkwaverc */

/*
 * functions that set the individual rc variables..
 */
static int f_accel(char *str)
{
DEBUG(printf("f_accel(\"%s\")\n",str));

if(strlen(str)) { set_wave_menu_accelerator(str); }

return(0);
}

static int f_alt_hier_delimeter(char *str)
{
DEBUG(printf("f_alt_hier_delimeter(\"%s\")\n",str));

if(strlen(str)) { alt_hier_delimeter=str[0]; }
return(0);
}

static int f_append_vcd_hier(char *str)
{
DEBUG(printf("f_append_vcd_hier(\"%s\")\n",str));
append_vcd_slisthier(str);
return(0);
}

static int f_atomic_vectors(char *str)
{
DEBUG(printf("f_atomic_vectors(\"%s\")\n",str));
atomic_vectors=atoi_64(str)?1:0;
return(0);
}

static int f_autoname_bundles(char *str)
{
DEBUG(printf("f_autoname_bundles(\"%s\")\n",str));
autoname_bundles=atoi_64(str)?1:0;
return(0);
}

static int f_autocoalesce(char *str)
{
DEBUG(printf("f_autocoalesce(\"%s\")\n",str));
autocoalesce=atoi_64(str)?1:0;
return(0);
}

static int f_autocoalesce_reversal(char *str)
{
DEBUG(printf("f_autocoalesce_reversal(\"%s\")\n",str));
autocoalesce_reversal=atoi_64(str)?1:0;
return(0);
}

static int f_constant_marker_update(char *str)
{
DEBUG(printf("f_constant_marker_update(\"%s\")\n",str));
constant_marker_update=atoi_64(str)?1:0;
return(0);
}

static int f_convert_to_reals(char *str)
{
DEBUG(printf("f_convert_to_reals(\"%s\")\n",str));
convert_to_reals=atoi_64(str)?1:0;
return(0);
}

static int f_cursor_snap(char *str)
{
int val;
DEBUG(printf("f_cursor_snap(\"%s\")\n",str));
val=atoi_64(str);
cursor_snap=(val<=0)?0:val;
return(0);
}

static int f_disable_mouseover(char *str)
{
DEBUG(printf("f_disable_mouseover(\"%s\")\n",str));
disable_mouseover=atoi_64(str)?1:0;
return(0);
}

static int f_disable_tooltips(char *str)
{
DEBUG(printf("f_disable_tooltips(\"%s\")\n",str));
disable_tooltips=atoi_64(str)?1:0;
return(0);
}

static int f_do_initial_zoom_fit(char *str)
{
DEBUG(printf("f_do_initial_zoom_fit(\"%s\")\n",str));
do_initial_zoom_fit=atoi_64(str)?1:0;
return(0);
}

static int f_dynamic_resizing(char *str)
{
DEBUG(printf("f_dynamic_resizing(\"%s\")\n",str));
do_resize_signals=atoi_64(str)?1:0;
return(0);
}

static int f_enable_fast_exit(char *str)
{
DEBUG(printf("f_enable_fast_exit(\"%s\")\n",str));
enable_fast_exit=atoi_64(str)?1:0;
return(0);
}

static int f_enable_ghost_marker(char *str)
{
DEBUG(printf("f_enable_ghost_marker(\"%s\")\n",str));
enable_ghost_marker=atoi_64(str)?1:0;
return(0);
}

static int f_enable_horiz_grid(char *str)
{
DEBUG(printf("f_enable_horiz_grid(\"%s\")\n",str));
enable_horiz_grid=atoi_64(str)?1:0;
return(0);
}

static int f_enable_vcd_autosave(char *str)
{
DEBUG(printf("f_enable_vcd_autosave(\"%s\")\n",str));
make_vcd_save_file=atoi_64(str)?1:0;
return(0);
}

static int f_enable_vert_grid(char *str)
{
DEBUG(printf("f_enable_vert_grid(\"%s\")\n",str));
enable_vert_grid=atoi_64(str)?1:0;
return(0);
}

static int f_fontname_logfile(char *str)
{
DEBUG(printf("f_fontname_logfile(\"%s\")\n",str));
if(fontname_logfile) free_2(fontname_logfile);
fontname_logfile=(char *)malloc_2(strlen(str)+1);
strcpy(fontname_logfile,str);
return(0);
}

static int f_fontname_signals(char *str)
{
DEBUG(printf("f_fontname_signals(\"%s\")\n",str));
if(fontname_signals) free_2(fontname_signals);
fontname_signals=(char *)malloc_2(strlen(str)+1);
strcpy(fontname_signals,str);
return(0);
}

static int f_fontname_waves(char *str)
{
DEBUG(printf("f_fontname_signals(\"%s\")\n",str));
if(fontname_waves) free_2(fontname_waves);
fontname_waves=(char *)malloc_2(strlen(str)+1);
strcpy(fontname_waves,str);
return(0);
}

static int f_force_toolbars(char *str)
{
DEBUG(printf("f_force_toolbars(\"%s\")\n",str));
force_toolbars=atoi_64(str)?1:0;
return(0);
}

static int f_hide_sst(char *str)
{
DEBUG(printf("f_hide_sst(\"%s\")\n",str));
hide_sst=atoi_64(str)?1:0;
return(0);
}

static int f_sst_expanded(char *str)
{
DEBUG(printf("f_sst_expanded(\"%s\")\n",str));
sst_expanded=atoi_64(str)?1:0;
return(0);
}

static int f_hier_delimeter(char *str)
{
DEBUG(printf("f_hier_delimeter(\"%s\")\n",str));

if(strlen(str)) { hier_delimeter=str[0]; hier_was_explicitly_set=1; }
return(0);
}

static int f_hier_grouping(char *str)
{
DEBUG(printf("f_hier_grouping(\"%s\")\n",str));
hier_grouping=atoi_64(str)?1:0;
return(0);
}

static int f_hier_max_level(char *str)
{
DEBUG(printf("f_hier_max_level(\"%s\")\n",str));
hier_max_level=atoi_64(str);
return(0);
}

static int f_hpane_pack(char *str)
{
DEBUG(printf("f_hpane_pack(\"%s\")\n",str));
paned_pack_semantics=atoi_64(str)?1:0;
return(0);
}

static int f_initial_window_x(char *str)
{
int val;
DEBUG(printf("f_initial_window_x(\"%s\")\n",str));
val=atoi_64(str);
initial_window_x=(val<=0)?-1:val;
return(0);
}

static int f_initial_window_xpos(char *str)
{
int val;
DEBUG(printf("f_initial_window_xpos(\"%s\")\n",str));
val=atoi_64(str);
initial_window_xpos=(val<=0)?-1:val;
return(0);
}

static int f_initial_window_y(char *str)
{
int val;
DEBUG(printf("f_initial_window_y(\"%s\")\n",str));
val=atoi_64(str);
initial_window_y=(val<=0)?-1:val;
return(0);
}

static int f_initial_window_ypos(char *str)
{
int val;
DEBUG(printf("f_initial_window_ypos(\"%s\")\n",str));
val=atoi_64(str);
initial_window_ypos=(val<=0)?-1:val;
return(0);
}

static int f_left_justify_sigs(char *str)
{
DEBUG(printf("f_left_justify_sigs(\"%s\")\n",str));
left_justify_sigs=atoi_64(str)?1:0;
return(0);
}

static int f_lxt_clock_compress_to_z(char *str)
{
DEBUG(printf("f_lxt_clock_compress_to_z(\"%s\")\n",str));
lxt_clock_compress_to_z=atoi_64(str)?1:0;
return(0);
}

static int f_page_divisor(char *str)
{
DEBUG(printf("f_page_divisor(\"%s\")\n",str));
sscanf(str,"%lg",&page_divisor);

if(page_divisor<0.01)
	{
	page_divisor=0.01;
	}
else
if(page_divisor>100.0)
	{
	page_divisor=100.0;
	}

if(page_divisor>1.0) page_divisor=1.0/page_divisor;

return(0);
}

static int f_ps_maxveclen(char *str)
{
DEBUG(printf("f_ps_maxveclen(\"%s\")\n",str));
ps_maxveclen=atoi_64(str);
if(ps_maxveclen<4)
	{
	ps_maxveclen=4;
	}
else
if(ps_maxveclen>66)
	{
	ps_maxveclen=66;
	}

return(0);
}

static int f_show_base_symbols(char *str)
{
DEBUG(printf("f_show_base_symbols(\"%s\")\n",str));
show_base=atoi_64(str)?1:0;
return(0);
}

static int f_show_grid(char *str)
{
DEBUG(printf("f_show_grid(\"%s\")\n",str));
display_grid=atoi_64(str)?1:0;
return(0);
}

static int f_splash_disable(char *str)
{
DEBUG(printf("f_splash_disable(\"%s\")\n",str));
splash_disable=atoi_64(str)?1:0;
return(0);
}

static int f_use_big_fonts(char *str)
{
DEBUG(printf("f_use_big_fonts(\"%s\")\n",str));
use_big_fonts=atoi_64(str)?1:0;
return(0);
}

static int f_use_full_precision(char *str)
{
DEBUG(printf("f_use_full_precision(\"%s\")\n",str));
use_full_precision=atoi_64(str)?1:0;
return(0);
}

static int f_use_frequency_display(char *str)
{
DEBUG(printf("f_use_frequency_display(\"%s\")\n",str));
use_frequency_delta=atoi_64(str)?1:0;
return(0);
}

static int f_use_maxtime_display(char *str)
{
DEBUG(printf("f_use_maxtime_display(\"%s\")\n",str));
use_maxtime_display=atoi_64(str)?1:0;
return(0);
}

static int f_use_nonprop_fonts(char *str)
{
DEBUG(printf("f_use_nonprop_fonts(\"%s\")\n",str));
use_nonprop_fonts=atoi_64(str)?1:0;
return(0);
}

static int f_use_roundcaps(char *str)
{
DEBUG(printf("f_use_roundcaps(\"%s\")\n",str));
use_roundcaps=atoi_64(str)?1:0;
return(0);
}

static int f_use_scrollbar_only(char *str)
{
DEBUG(printf("f_use_scrollbar_only(\"%s\")\n",str));
use_scrollbar_only=atoi_64(str)?1:0;
return(0);
}

static int f_vcd_explicit_zero_subscripts(char *str)
{
DEBUG(printf("f_vcd_explicit_zero_subscripts(\"%s\")\n",str));
vcd_explicit_zero_subscripts=atoi_64(str)?0:-1;	/* 0==yes, -1==no */
return(0);
}

static int f_vcd_preserve_glitches(char *str)
{
DEBUG(printf("f_vcd_preserve_glitches(\"%s\")\n",str));
vcd_preserve_glitches=atoi_64(str)?0:-1;	/* 0==yes, -1==no */
return(0);
}

static int f_vcd_warning_filesize(char *str)
{
DEBUG(printf("f_vcd_preserve_glitches(\"%s\")\n",str));
vcd_warning_filesize=atoi_64(str);
return(0);
}

static int f_vector_padding(char *str)
{
DEBUG(printf("f_vector_padding(\"%s\")\n",str));
vector_padding=atoi_64(str);
if(vector_padding<4) vector_padding=4;
else if(vector_padding>16) vector_padding=16;
return(0);
}

static int f_vlist_compression(char *str)
{
DEBUG(printf("f_vlist_compression(\"%s\")\n",str));
vlist_compression_depth=atoi_64(str);
if(vlist_compression_depth<0) vlist_compression_depth = -1;
if(vlist_compression_depth>9) vlist_compression_depth = 9;
return(0);
}

static int f_wave_scrolling(char *str)
{
DEBUG(printf("f_wave_scrolling(\"%s\")\n",str));
wave_scrolling=atoi_64(str)?1:0;
return(0);
}

static int f_zoom_base(char *str)
{
float f;
DEBUG(printf("f_zoom_base(\"%s\")\n",str));
sscanf(str,"%f",&f);
if(f<1.5) f=1.5; else if(f>10.0) f=10.0;
zoombase=(gdouble)f;
return(0);
}

static int f_zoom_center(char *str)
{
DEBUG(printf("f_zoom_center(\"%s\")\n",str));
do_zoom_center=atoi_64(str)?1:0;
return(0);
}

static int f_zoom_pow10_snap(char *str)
{
DEBUG(printf("f_zoom_pow10_snap(\"%s\")\n",str));
zoom_pow10_snap=atoi_64(str)?1:0;
return(0);
}


static int rc_compare(const void *v1, const void *v2)
{
return(strcasecmp((char *)v1, ((struct rc_entry *)v2)->name));
}


/* make the color functions */
#define color_make(Z) static int f_color_##Z (char *str) \
{ \
int rgb; \
if((rgb=get_rgb_from_name(str))!=~0) \
	{ \
	color_##Z=rgb; \
	} \
return(0); \
}

color_make(back)
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
color_make(dkblue)


/*
 * rc variables...these MUST be in alphabetical order for the bsearch!
 */ 
static struct rc_entry rcitems[]=
{
{ "accel", f_accel },
{ "alt_hier_delimeter", f_alt_hier_delimeter },
{ "append_vcd_hier", f_append_vcd_hier },
{ "atomic_vectors", f_atomic_vectors },
{ "autocoalesce", f_autocoalesce },
{ "autocoalesce_reversal", f_autocoalesce_reversal },
{ "autoname_bundles", f_autoname_bundles },
{ "color_0", f_color_0 },
{ "color_1", f_color_1 },
{ "color_back", f_color_back },
{ "color_baseline", f_color_baseline },
{ "color_black", f_color_black },
{ "color_dash", f_color_dash },
{ "color_dashfill", f_color_dashfill },
{ "color_dkblue", f_color_dkblue },
{ "color_dkgray", f_color_dkgray },
{ "color_grid", f_color_grid },
{ "color_high", f_color_high },
{ "color_low", f_color_low },
{ "color_ltgray", f_color_ltgray },
{ "color_mark", f_color_mark },
{ "color_mdgray", f_color_mdgray },
{ "color_mid", f_color_mid },
{ "color_normal", f_color_normal },
{ "color_time", f_color_time },
{ "color_timeb", f_color_timeb },
{ "color_trans", f_color_trans },
{ "color_u", f_color_u },
{ "color_ufill", f_color_ufill },
{ "color_umark", f_color_umark },
{ "color_value", f_color_value },
{ "color_vbox", f_color_vbox },
{ "color_vtrans", f_color_vtrans },
{ "color_w", f_color_w },
{ "color_wfill", f_color_wfill },
{ "color_white", f_color_white },
{ "color_x", f_color_x },
{ "color_xfill", f_color_xfill },
{ "constant_marker_update", f_constant_marker_update },
{ "convert_to_reals", f_convert_to_reals },
{ "cursor_snap", f_cursor_snap },
{ "disable_mouseover", f_disable_mouseover },
{ "disable_tooltips", f_disable_tooltips },
{ "do_initial_zoom_fit", f_do_initial_zoom_fit },
{ "dynamic_resizing", f_dynamic_resizing },
{ "enable_fast_exit", f_enable_fast_exit },
{ "enable_ghost_marker", f_enable_ghost_marker },
{ "enable_horiz_grid", f_enable_horiz_grid }, 
{ "enable_vcd_autosave", f_enable_vcd_autosave },
{ "enable_vert_grid", f_enable_vert_grid }, 
{ "fontname_logfile", f_fontname_logfile }, 
{ "fontname_signals", f_fontname_signals }, 
{ "fontname_waves", f_fontname_waves }, 
{ "force_toolbars", f_force_toolbars }, 
{ "hide_sst", f_hide_sst },
{ "hier_delimeter", f_hier_delimeter },
{ "hier_grouping", f_hier_grouping },
{ "hier_max_level", f_hier_max_level },
{ "hpane_pack", f_hpane_pack },
{ "initial_window_x", f_initial_window_x },
{ "initial_window_xpos", f_initial_window_xpos },
{ "initial_window_y", f_initial_window_y },
{ "initial_window_ypos", f_initial_window_ypos },
{ "left_justify_sigs", f_left_justify_sigs },
{ "lxt_clock_compress_to_z", f_lxt_clock_compress_to_z },
{ "page_divisor", f_page_divisor },
{ "ps_maxveclen", f_ps_maxveclen },
{ "show_base_symbols", f_show_base_symbols },
{ "show_grid", f_show_grid },
{ "splash_disable", f_splash_disable },
{ "sst_expanded", f_sst_expanded },
{ "use_big_fonts", f_use_big_fonts },
{ "use_frequency_display", f_use_frequency_display },
{ "use_full_precision", f_use_full_precision },
{ "use_maxtime_display", f_use_maxtime_display },
{ "use_nonprop_fonts", f_use_nonprop_fonts },
{ "use_roundcaps", f_use_roundcaps },
{ "use_scrollbar_only", f_use_scrollbar_only },
{ "vcd_explicit_zero_subscripts", f_vcd_explicit_zero_subscripts },
{ "vcd_preserve_glitches", f_vcd_preserve_glitches },
{ "vcd_warning_filesize", f_vcd_warning_filesize },
{ "vector_padding", f_vector_padding },
{ "vlist_compression", f_vlist_compression },
{ "wave_scrolling", f_wave_scrolling },
{ "zoom_base", f_zoom_base },
{ "zoom_center", f_zoom_center },
{ "zoom_pow10_snap", f_zoom_pow10_snap }
};


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
int num_rcitems = sizeof(rcitems)/sizeof(struct rc_entry);

for(i=0;i<(num_rcitems-1);i++)
	{
	if(strcmp(rcitems[i].name, rcitems[i+1].name) > 0)
		{
		fprintf(stderr, "rcitems misordering: '%s' vs '%s'\n", rcitems[i].name, rcitems[i+1].name);
		exit(255);
		}
	}

/* move defaults first and only go whitescreen if instructed to do so */
if(possibly_use_rc_defaults) vanilla_rc();

if((override_rc)&&((handle=fopen(override_rc,"rb"))))
	{
	/* good, we have a handle */
	}
else
#if !defined __MINGW32__ && !defined _MSC_VER
if(!(handle=fopen(rcname,"rb")))
	{
	char *home;
	char *rcpath;
	
	home=getpwuid(geteuid())->pw_dir;
	rcpath=(char *)alloca(strlen(home)+1+strlen(rcname)+1);
	strcpy(rcpath,home);
	strcat(rcpath,"/");
	strcat(rcpath,rcname);

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

rc_line_no=0;
while(!feof(handle))
	{
	char *str;

	rc_line_no++;
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
							if((r=bsearch((void *)(str+pos), (void *)rcitems, 
								sizeof(rcitems)/sizeof(struct rc_entry), 
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
