/* 
 * Copyright (c) Tony Bybell 1999-2006.
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

#include "wave_locale.h"

#if !defined _MSC_VER && !defined __MINGW32__
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#endif

#if !defined _MSC_VER && !defined __MINGW32__ && !defined OS_X && defined WAVE_USE_GTK2
#define WAVE_USE_XID
#else
#undef WAVE_USE_XID
#endif


#ifdef HAVE_GETOPT_LONG
#include <getopt.h>
#else
#include "gnu-getopt.h"
#ifndef _MSC_VER
#include <unistd.h>
#else
#define strcasecmp _stricmp
#endif
#endif

#include "symbol.h"
#include "lx2.h"
#include "ae2.h"
#include "vzt.h"
#include "ghw.h"
#include "main.h"
#include "menu.h"
#include "vcd.h"
#include "lxt.h"
#include "lxt2_read.h"
#include "vzt_read.h"
#include "pixmaps.h"
#include "currenttime.h"
#include "fgetdynamic.h"
#include "rc.h"
#include "translate.h"
#include "ptranslate.h"

char *whoami=NULL;
struct logfile_chain *logfile=NULL;

char *stems_name = NULL;
int stems_type = WAVE_ANNO_NONE;
char *aet_name = NULL;
struct gtkwave_annotate_ipc_t *anno_ctx = NULL;

struct gtkwave_dual_ipc_t *dual_ctx = NULL;
int dual_id = 0;
static int dual_attach_id = 0;
int dual_race_lock = 0;

GtkWidget *mainwindow = NULL;
GtkWidget *signalwindow = NULL;
GtkWidget *wavewindow = NULL;
GtkWidget* toppanedwindow = NULL;    
GtkWidget* sstpane = NULL;
GtkWidget* expanderwindow = NULL;

char disable_window_manager=0;	/* for scripting...some windowmanagers would expect user to nail down window */
char paned_pack_semantics=1;	/* 1 for paned_pack, 0 for paned_add */
char zoom_was_explicitly_set=0; /* set on '*' encountered in .sav file read  */
int initial_window_x=800, initial_window_y=400; /* initial window sizes : RHEL5 seems to require 800 for this */
int initial_window_width = -1, initial_window_height = -1;

static int xy_ignore = 0; /* keeps window position from changing after initial load */

int optimize_vcd = 0; /* convert VCD to LXT2? */

int num_cpus=1;
int initial_window_xpos=-1, initial_window_ypos=-1; /* initial window position (-1 == WM choice) */

int initial_window_set_valid = 0;
int initial_window_xpos_set=-1, initial_window_ypos_set=-1; /* initial window position as set */

int initial_window_get_valid = 0;
int initial_window_xpos_get=-1, initial_window_ypos_get=-1; /* initial window position as set */

int xpos_delta = 0, ypos_delta = 0;

char use_scrollbar_only=0;
char force_toolbars=0;		/* 1 sez use toolbar rendering */

int hide_sst = 0;
int sst_expanded = 1;

#if !defined _MSC_VER && !defined __MINGW32__
static void kill_browser(void)
{
if(anno_ctx)
	{
	if(anno_ctx->browser_process)
		{
		kill(anno_ctx->browser_process, SIGKILL);
		}
	}
}
#endif


/* for XID plug handling... */
#ifdef WAVE_USE_XID
GdkNativeWindow socket_xid = 0;
#else
unsigned int socket_xid = 0;
#endif
int disable_menus = 0;	/* older versions of GTK can't handle menus in a GtkPlug properly */

static int plug_destroy (GtkWidget *widget, gpointer data)
{
exit(0);

return(FALSE);
}       


static void print_help(char *nam)
{
#if !defined _MSC_VER && !defined __MINGW32__ && !defined __FreeBSD__ && !defined __CYGWIN__
#define WAVE_GETOPT_CPUS "  -c, --cpu=NUMCPUS          specify number of CPUs for parallelizable ops\n"
#else
#define WAVE_GETOPT_CPUS
#endif

#if !defined _MSC_VER && !defined __MINGW32__
#define STEMS_GETOPT	 "  -t, --stems=FILE           specify stems file for source code annotation\n"
#define VCD_GETOPT       "  -o, --optimize             optimize VCD to LXT2\n"
#define DUAL_GETOPT      "  -D, --dualid=WHICH         specify multisession identifier\n"
#define INTR_GETOPT      "  -I, --interactive          interactive VCD mode (filename is shared mem ID)\n"
#else
#define STEMS_GETOPT
#define VCD_GETOPT
#define DUAL_GETOPT
#define INTR_GETOPT
#endif


#ifdef WAVE_USE_XID
#define XID_GETOPT 	 "  -X, --xid=XID              specify XID of window for GtkPlug to connect to\n"
#else
#define XID_GETOPT
#endif

printf(
"Usage: %s [OPTION]... [DUMPFILE] [SAVEFILE] [RCFILE]\n\n"
"  -n, --nocli=DIRPATH        use file requester for dumpfile name\n"
"  -f, --dump=FILE            specify dumpfile name\n"
VCD_GETOPT
"  -a, --save=FILE            specify savefile name\n"
"  -A, --autosavename         assume savefile is suffix modified dumpfile name\n"
"  -r, --rcfile=FILE          specify override .rcfile name\n"
"  -d, --defaultskip          if missing .rcfile, do not use useful defaults\n"
DUAL_GETOPT
"  -i, --indirect=FILE        specify indirect facs file name\n"
"  -l, --logfile=FILE         specify simulation logfile name for time values\n"
"  -s, --start=TIME           specify start time for LXT2/VZT block skip\n"
"  -e, --end=TIME             specify end time for LXT2/VZT block skip\n"  
STEMS_GETOPT
WAVE_GETOPT_CPUS
"  -N, --nowm                 disable window manager for most windows\n"
"  -M, --nomenus              do not render menubar (for making applets)\n"
"  -S, --script=FILE          specify GUI command script file for execution\n"
XID_GETOPT
INTR_GETOPT
"  -L, --legacy               use legacy VCD mode rather than the VCD recoder\n" 
"  -v, --vcd                  use stdin as a VCD dumpfile\n"
"  -V, --version              display version banner then exit\n"
"  -h, --help                 display this help then exit\n"
"  -x  --exit                 exit after loading trace (for loader benchmarks)\n\n"

"VCD files and save files may be compressed with zip or gzip.\n"
"GHW files may be compressed with gzip or bzip2.\n"
"Other formats must remain uncompressed due to their non-linear access.\n"
"Note that DUMPFILE is optional if the --dump or --nocli options are specified.\n"
"SAVEFILE and RCFILE are always optional.\n\n"

"Report bugs to <bybell@nc.rr.com>.\n",nam);

exit(0);
}


/*
 * file selection for -n/--nocli flag 
 */
static char *ftext_main = NULL;

static void wave_get_filename_cleanup(GtkWidget *widget, gpointer data) { gtk_main_quit(); /* do nothing but exit gtk loop */ }

static char *wave_get_filename(char *dfile)
{
if(dfile) 
	{
	int len = strlen(dfile);
	ftext_main = malloc_2(strlen(dfile)+2);
	strcpy(ftext_main, dfile);
#if !defined _MSC_VER && !defined __MINGW32__
	if((len)&&(dfile[len-1]!='/'))
		{
		strcpy(ftext_main + len, "/");
		}
#else
	if((len)&&(dfile[len-1]!='\\'))
		{
		strcpy(ftext_main + len, "\\");
		}
#endif
	}
fileselbox_old("GTKWave: Select a dumpfile...",&ftext_main,GTK_SIGNAL_FUNC(wave_get_filename_cleanup), GTK_SIGNAL_FUNC(wave_get_filename_cleanup), NULL);
gtk_main();

return(ftext_main);
}


int main(int argc, char *argv[])
{
int i;
int c;
char is_vcd=0;
char is_interactive=0;
char is_smartsave = 0;
char is_legacy = 0;
char fast_exit=0;
char opt_errors_encountered=0;

char *fname=NULL;
char *wname=NULL;
char *override_rc=NULL;
char *winname=NULL;
char *scriptfile=NULL;
static char *winprefix="GTKWave - ";
static char *winstd="GTKWave (stdio) ";
static char *vcd_autosave_name="vcd_autosave.sav";
FILE *wave;
char *indirect_fname=NULL;

GtkWidget *main_vbox, *top_table, *whole_table;
GtkWidget *menubar;
GtkWidget *text1;
GtkWidget *zoombuttons;
GtkWidget *pagebuttons;
GtkWidget *fetchbuttons;
GtkWidget *discardbuttons;
GtkWidget *shiftbuttons;
GtkWidget *entry;
GtkWidget *timebox;
GtkWidget *panedwindow;
GtkWidget *dummy1, *dummy2;
GtkWidget *toolhandle=NULL;

int splash_disable_rc_override = 0;

char *skip_start=NULL, *skip_end=NULL;

WAVE_LOCALE_FIX

whoami=malloc_2(strlen(argv[0])+1);	/* cache name in case we fork later */
strcpy(whoami, argv[0]);

if(!gtk_init_check(&argc, &argv))
	{
	printf("Could not initialize GTK!  Is DISPLAY env var/xhost set?\n\n");
	print_help(argv[0]);
	}

init_filetrans_data(); /* for file translation splay trees */
init_proctrans_data(); /* for proc translation structs */
atexit(remove_all_proc_filters);

while (1)
        {
        int option_index = 0;

        static struct option long_options[] =
                {
                {"dump", 1, 0, 'f'},
                {"optimize", 0, 0, 'o'},
                {"nocli", 1, 0, 'n'},
                {"save", 1, 0, 'a'},
                {"autosavename", 0, 0, 'A'},
		{"rcfile", 1, 0, 'r'},
		{"defaultskip", 0, 0, 'd'},
		{"indirect", 1, 0, 'i'},
		{"logfile", 1, 0, 'l'},
                {"start", 1, 0, 's'},
                {"end", 1, 0, 'e'},
                {"cpus", 1, 0, 'c'},
		{"stems", 1, 0, 't'},
		{"nowm", 0, 0, 'N'},
		{"script", 1, 0, 'S'},
                {"vcd", 0, 0, 'v'},
                {"version", 0, 0, 'V'},
                {"help", 0, 0, 'h'},
                {"exit", 0, 0, 'x'},
                {"xid", 1, 0, 'X'},
		{"nomenus", 0, 0, 'M'},
		{"dualid", 1, 0, 'D'},
		{"interactive", 0, 0, 'I'},
		{"legacy", 0, 0, 'L'},
                {0, 0, 0, 0}
                };

        c = getopt_long (argc, argv, "f:on:a:Ar:di:l:s:e:c:t:NS:vVhxX:MD:IL", long_options, &option_index);

        if (c == -1) break;     /* no more args */

        switch (c)
                {
		case 'V':
			printf(
			WAVE_VERSION_INFO"\n\n"
			"This is free software; see the source for copying conditions.  There is NO\n"
			"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
			);
			exit(0);

		case 'I':
#if !defined _MSC_VER && !defined __MINGW32__
			is_interactive = 1;
#endif
			break;

		case 'L':
			is_legacy = 1;
			break;

		case 'D':
#if !defined _MSC_VER && !defined __MINGW32__
			{
			char *s = optarg;
			char *plus = strchr(s, '+');
			if((plus)&&(*(plus+1)))
				{
				sscanf(plus+1, "%x", &dual_attach_id);
				if(plus != s)
					{
					char p = *(plus-1);

					if(p=='0')
						{
						dual_id = 0;
						break;
						}
					else
					if(p=='1')
						{
						dual_id = 1;
						break;
						}
					}
				}

			fprintf(stderr, "Malformed dual session ID.  Must be of form m+nnnnnnnn where m is 0 or 1,\n"
					"and n is a hexadecimal shared memory ID for use with shmat()\n");
			exit(255);
			}
#else
			{
			fprintf(stderr, "Dual operation not implemented for Win32, exiting.\n");
			exit(255);
			}
#endif
			break;

		case 'A':
			is_smartsave = 1;
			break;

                case 'v':
			is_vcd = 1;
			if(fname) free_2(fname);
			fname = malloc_2(4+1);
			strcpy(fname, "-vcd");
                        break;

		case 'o':
			optimize_vcd = 1;
			break;

		case 'n':
			wave_get_filename(optarg);
			if(filesel_ok)
				{
				if(fname) free_2(fname);
				fname = ftext_main;
				ftext_main = NULL;
				}
			break;

                case 'h':
			print_help(argv[0]);
                        break;

#ifdef WAVE_USE_XID
                case 'X': 
                        sscanf(optarg, "%x", &socket_xid);
			splash_disable_rc_override = 1;
                        break;
#endif

		case 'M':
			disable_menus = 1;
			break;

                case 'x':
			fast_exit = 1;
			splash_disable_rc_override = 1;
                        break;

		case 'd':
			possibly_use_rc_defaults = 0;
			break;

                case 'f':
			is_vcd = 0;
			if(fname) free_2(fname);
			fname = malloc_2(strlen(optarg)+1);
			strcpy(fname, optarg);
			break;

                case 'a':
			if(wname) free_2(wname);
			wname = malloc_2(strlen(optarg)+1);
			strcpy(wname, optarg);
			break;

                case 'r':
			if(override_rc) free_2(override_rc);
			override_rc = malloc_2(strlen(optarg)+1);
			strcpy(override_rc, optarg);
			break;

                case 'i':
			if(indirect_fname) free_2(indirect_fname);
			indirect_fname = malloc_2(strlen(optarg)+1);
			strcpy(indirect_fname, optarg);
			break;

                case 's':
			if(skip_start) free_2(skip_start);
			skip_start = malloc_2(strlen(optarg)+1);
			strcpy(skip_start, optarg);
			break;			

                case 'e':
			if(skip_end) free_2(skip_end);
			skip_end = malloc_2(strlen(optarg)+1);
			strcpy(skip_end, optarg);
                        break;

		case 't':
#if !defined _MSC_VER && !defined __MINGW32__
			if(stems_name) free_2(stems_name);
			stems_name = malloc_2(strlen(optarg)+1);
			strcpy(stems_name, optarg);
#endif
			break;

                case 'c':
#if !defined _MSC_VER && !defined __MINGW32__ && !defined __FreeBSD__ && !defined __CYGWIN__
			num_cpus = atoi(optarg);
			if(num_cpus<1) num_cpus = 1;
			if(num_cpus>8) num_cpus = 8;
#endif
                        break;

		case 'N':
			disable_window_manager = 1;
			break;

		case 'S':
			if(scriptfile) free_2(scriptfile);
			scriptfile = malloc_2(strlen(optarg)+1);
			strcpy(scriptfile, optarg);
			splash_disable_rc_override = 1;
			break;

		case 'l':
			{
			struct logfile_chain *l = calloc_2(1, sizeof(struct logfile_chain));
			struct logfile_chain *ltraverse;
			l->name = malloc_2(strlen(optarg)+1);
			strcpy(l->name, optarg);

			if(logfile)
				{
				ltraverse = logfile;
				while(ltraverse->next) ltraverse = ltraverse->next;
				ltraverse->next = l;
				}
				else
				{
				logfile = l;
				}
			}
			break;

                case '?':
			opt_errors_encountered=1;
                        break;

                default:
                        /* unreachable */
                        break;
                }
        }

if(opt_errors_encountered)
	{
	print_help(argv[0]);
	}

if (optind < argc)
        {
        while (optind < argc)
		{
		if(!fname)
			{
			is_vcd = 0;
			fname = malloc_2(strlen(argv[optind])+1);
			strcpy(fname, argv[optind++]);
			}
		else if(!wname)
			{
			wname = malloc_2(strlen(argv[optind])+1);
			strcpy(wname, argv[optind++]);
			}
		else if(!override_rc)
			{
			override_rc = malloc_2(strlen(argv[optind])+1);
			strcpy(override_rc, argv[optind++]);
			break; /* skip any extra args */
			}
		}
        }

if(!fname)
	{
	print_help(argv[0]);
	}

read_rc_file(override_rc);
splash_disable |= splash_disable_rc_override;


fprintf(stderr, "\n%s\n\n",WAVE_VERSION_INFO);

if((!wname)&&(make_vcd_save_file))
	{
	vcd_save_handle=fopen(vcd_autosave_name,"wb");
	errno=0;	/* just in case */
	is_smartsave = (vcd_save_handle != NULL); /* use smartsave if for some reason can't open auto savefile */
	}

sym=(struct symbol **)calloc_2(SYMPRIME,sizeof(struct symbol *));

/* load either the vcd or aet file depending on suffix then mode setting */
if(is_vcd)
	{
	winname=malloc_2(strlen(winstd)+4+1);
	strcpy(winname,winstd);
	}
	else
	{
	if(!is_interactive)
		{
		winname=malloc_2(strlen(fname)+strlen(winprefix)+1);
		strcpy(winname,winprefix);
		}
		else
		{
		char *iact = "GTKWave - Interactive Shared Memory ID ";
		winname=malloc_2(strlen(fname)+strlen(iact)+1);
		strcpy(winname,iact);
		}
	}

strcat(winname,fname);

loader_check_head:

if((strlen(fname)>3)&&((!strcasecmp(fname+strlen(fname)-4,".lxt"))||(!strcasecmp(fname+strlen(fname)-4,".lx2"))))
	{
	FILE *f = fopen(fname, "rb");
	int typ = 0;

	if(f)
		{
		char buf[2];
		unsigned int matchword;

		if(fread(buf, 2, 1, f))
			{
			matchword = (((unsigned int)buf[0])<<8) | ((unsigned int)buf[1]);
			if(matchword == LT_HDRID) typ = 1;
			}

		fclose(f);
		}

	if(typ)
		{
		lxt_main(fname);
		}
		else
		{
#if !defined _MSC_VER && !defined __MINGW32__
		stems_type = WAVE_ANNO_LXT2;
		aet_name = malloc_2(strlen(fname)+1);
		strcpy(aet_name, fname);
#endif
		lx2_main(fname, skip_start, skip_end);
		}	
	}
else
if((strlen(fname)>3)&&(!strcasecmp(fname+strlen(fname)-4,".vzt")))
	{
#if !defined _MSC_VER && !defined __MINGW32__
	stems_type = WAVE_ANNO_VZT;
	aet_name = malloc_2(strlen(fname)+1);
	strcpy(aet_name, fname);
#endif

	vzt_main(fname, skip_start, skip_end);
	}
else if ((strlen(fname)>3)&&((!strcasecmp(fname+strlen(fname)-4,".aet"))||(!strcasecmp(fname+strlen(fname)-4,".ae2"))))
	{
#if !defined _MSC_VER && !defined __MINGW32__
	stems_type = WAVE_ANNO_AE2;
	aet_name = malloc_2(strlen(fname)+1);
	strcpy(aet_name, fname);
#endif

	ae2_main(fname, skip_start, skip_end, indirect_fname);
	}
else if (
	((strlen(fname)>3)&&(!strcasecmp(fname+strlen(fname)-4,".ghw"))) ||
	((strlen(fname)>6)&&(!strcasecmp(fname+strlen(fname)-7,".ghw.gz"))) ||
	((strlen(fname)>7)&&(!strcasecmp(fname+strlen(fname)-8,".ghw.bz2")))
	)
	{
	ghw_main(fname);
	}
else if (strlen(fname)>4)	/* case for .aet? type filenames */
	{
	char sufbuf[5];
	memcpy(sufbuf, fname+strlen(fname)-5, 4);
	sufbuf[4] = 0;
	if(!strcasecmp(sufbuf, ".aet"))	/* strncasecmp() in windows? */
		{
#if !defined _MSC_VER && !defined __MINGW32__
		stems_type = WAVE_ANNO_AE2;
		aet_name = malloc_2(strlen(fname)+1);
		strcpy(aet_name, fname);
#endif

		ae2_main(fname, skip_start, skip_end, indirect_fname);
		}
		else
		{
		goto load_vcd;
		}
	}
else	/* nothing else left so default to "something" */
	{
load_vcd:
#if !defined _MSC_VER && !defined __MINGW32__
	if(optimize_vcd)
		{
		optimize_vcd = 0;

		if(!strcmp("-vcd", fname))
			{
			pid_t pid;
			char *buf = malloc_2(strlen("vcd") + 4 + 1);
			sprintf(buf, "%s.lx2", "vcd");
			pid = fork();
        
                        if(((int)pid) < 0)
                                {
                                /* can't do anything about this */
                                }
                                else
				{
				if(pid)
					{
					int stat;
					int rc = waitpid(pid, &stat, 0);

					if(rc > 0)
						{
						free_2(fname);
						fname = buf;
						is_vcd = 0;
						goto loader_check_head;						
						}					
					}
					else
					{
				        execlp("vcd2lxt2", "vcd2lxt2", "--", "-", buf, NULL);
					exit(255);
					}
				}
			}
			else
			{
			pid_t pid;
			char *buf = malloc_2(strlen(fname) + 4 + 1);
			sprintf(buf, "%s.lx2", fname);
			pid = fork();
        
                        if(((int)pid) < 0)
                                {
                                /* can't do anything about this */
                                }
                                else
				{
				if(pid)
					{
					int stat;
					int rc = waitpid(pid, &stat, 0);

					if(rc > 0)
						{
						free_2(fname);
						fname = buf;
						is_vcd = 0;
						goto loader_check_head;						
						}					
					}
					else
					{
				        execlp("vcd2lxt2", "vcd2lxt2", fname, buf, NULL);
					exit(255);
					}
				}
			}
		}
#endif

#if !defined _MSC_VER && !defined __MINGW32__
	if(is_interactive)
		{
		vcd_partial_main(fname);
		}
		else
#endif
		{
		if(is_legacy)
			{
			vcd_main(fname);
			}
			else
			{
			vcd_recoder_main(fname);
			}
		}
	}

if(indirect_fname)
	{
	free_2(indirect_fname);
	indirect_fname=NULL;
	}
if(skip_start)
	{
	free_2(skip_start); skip_start=NULL;
	}
if(skip_end)
	{
	free_2(skip_end); skip_end=NULL;
	}

for(i=0;i<26;i++) named_markers[i]=-1;	/* reset all named markers */

tims.last=max_time;
tims.end=tims.last;		/* until the configure_event of wavearea */
tims.first=tims.start=tims.laststart=min_time;
tims.zoom=tims.prevzoom=0;	/* 1 pixel/ns default */
tims.marker=tims.lmbcache=-1;	/* uninitialized at first */
tims.baseline=-1;		/* middle button toggle marker */

if((wname)||(vcd_save_handle)||(is_smartsave))
	{
	int wave_is_compressed;
        char *str = NULL;

	if(vcd_save_handle)
		{
		wname=vcd_autosave_name;
		do_initial_zoom_fit=1;
		}
	else
	if((!wname) /* && (is_smartsave) */)
		{
		char *pnt = wave_alloca(strlen(fname) + 1);
		char *pnt2;
		strcpy(pnt, fname);

	        if((strlen(pnt)>2)&&(!strcasecmp(pnt+strlen(pnt)-3,".gz")))
			{
			pnt[strlen(pnt)-3] = 0x00;
			}
		else if ((strlen(pnt)>3)&&(!strcasecmp(pnt+strlen(pnt)-4,".zip")))
			{
			pnt[strlen(pnt)-4] = 0x00;
			}

		pnt2 = pnt + strlen(pnt);
		if(pnt != pnt2)
			{
			do
				{
				if(*pnt2 == '.')
					{
					*pnt2 = 0x00;
					break;
					}
				} while(pnt2-- != pnt);
			}	

		wname = malloc_2(strlen(pnt) + 5);
		strcpy(wname, pnt);
		strcat(wname, ".sav");
		}

	if(((strlen(wname)>2)&&(!strcasecmp(wname+strlen(wname)-3,".gz")))||
	   ((strlen(wname)>3)&&(!strcasecmp(wname+strlen(wname)-4,".zip"))))
	        {
        	int dlen;
        	dlen=strlen(WAVE_DECOMPRESSOR);
	        str=wave_alloca(strlen(wname)+dlen+1);
	        strcpy(str,WAVE_DECOMPRESSOR);
	        strcpy(str+dlen,wname);
	        wave=popen(str,"r");
	        wave_is_compressed=~0;
	        }
	        else
	        {
	        wave=fopen(wname,"rb");
	        wave_is_compressed=0;

		filesel_writesave = malloc_2(strlen(wname)+1); /* don't handle compressed files */
		strcpy(filesel_writesave, wname);
	        }

	if(!wave)
	        {
	        fprintf(stderr, "** WARNING: Error opening .sav file '%s', skipping.\n",wname);
	        }	
	        else
	        {
	        char *iline;
		char any_shadow = 0;

		if(is_lx2)
			{
		        while((iline=fgetmalloc(wave)))
		                {
		                parsewavline_lx2(iline, 0);
				free_2(iline);
		                }

			switch(is_lx2)
				{
				case LXT2_IS_LXT2: lx2_import_masked(); break;
				case LXT2_IS_AET2: ae2_import_masked(); break;
				case LXT2_IS_VZT:  vzt_import_masked(); break;
				case LXT2_IS_VLIST: vcd_import_masked(); break;
				}

			if(wave_is_compressed)
			        {
				pclose(wave);
			        wave=popen(str,"r");
			        }
			        else
			        {
				fclose(wave);
			        wave=fopen(wname,"rb");
			        }

			if(!wave)
			        {
			        fprintf(stderr, "** WARNING: Error opening .sav file '%s', skipping.\n",wname);
				goto savefile_bail;
			        }	
			}

		default_flags=TR_RJUSTIFY;
		shift_timebase_default_for_add=LLDescriptor(0);
	        while((iline=fgetmalloc(wave)))
	                {
	                parsewavline(iline, 0);
			any_shadow |= shadow_active;
			free_2(iline);
	                }
		default_flags=TR_RJUSTIFY;
		shift_timebase_default_for_add=LLDescriptor(0);

		if(wave_is_compressed) pclose(wave); else fclose(wave);

                if(any_shadow)
                        {
                        if(shadow_straces)
                                {
                                shadow_active = 1;

                                swap_strace_contexts();
                                strace_maketimetrace(1);
                                swap_strace_contexts();

				shadow_active = 0;
                                }
                        }
	        }
	}

savefile_bail:
current_translate_file = 0;

if(fast_exit)
	{
	printf("Exiting early because of --exit request.\n");
	exit(0);
	}

if ((!zoom_was_explicitly_set)&&
	((tims.last-tims.first)<=400)) do_initial_zoom_fit=1;  /* force zoom on small traces */

calczoom(tims.zoom);

#ifdef WAVE_USE_XID
if(!socket_xid)
#endif
        {
	mainwindow = gtk_window_new(disable_window_manager ? GTK_WINDOW_POPUP : GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(mainwindow), winname);
	gtk_widget_set_usize(GTK_WIDGET(mainwindow), initial_window_x, initial_window_y);

	if((initial_window_width>0)&&(initial_window_height>0))
		{
		gtk_window_set_default_size(GTK_WINDOW (mainwindow), initial_window_width, initial_window_height);
		}

	gtk_signal_connect(GTK_OBJECT(mainwindow), "delete_event", 	/* formerly was "destroy" */
				GTK_SIGNAL_FUNC(file_quit_cmd_callback), 
			       	"WM destroy");

	gtk_widget_show(mainwindow);
	}
#ifdef WAVE_USE_XID
	else
	{
        mainwindow = gtk_plug_new(socket_xid);
        gtk_widget_show(mainwindow);
                                
        gtk_signal_connect(GTK_OBJECT(mainwindow), "destroy",   /* formerly was "destroy" */
                                GTK_SIGNAL_FUNC(plug_destroy),
                                "Plug destroy");
	}
#endif

make_pixmaps(mainwindow);

main_vbox = gtk_vbox_new(FALSE, 5);
gtk_container_border_width(GTK_CONTAINER(main_vbox), 1);
gtk_container_add(GTK_CONTAINER(mainwindow), main_vbox);
gtk_widget_show(main_vbox);

if(!disable_menus)
	{
	get_main_menu(mainwindow, &menubar);
	gtk_widget_show(menubar);

	if(force_toolbars)
		{
		toolhandle=gtk_handle_box_new();
		gtk_widget_show(toolhandle);
		gtk_container_add(GTK_CONTAINER(toolhandle), menubar);
		gtk_box_pack_start(GTK_BOX(main_vbox), toolhandle, FALSE, TRUE, 0);
		}
		else
		{
		gtk_box_pack_start(GTK_BOX(main_vbox), menubar, FALSE, TRUE, 0);
		}
	}

top_table = gtk_table_new (1, 284, FALSE);

if(force_toolbars)
	{
	toolhandle=gtk_handle_box_new();
	gtk_widget_show(toolhandle);
	gtk_container_add(GTK_CONTAINER(toolhandle), top_table);
	}

whole_table = gtk_table_new (256, 16, FALSE);

text1 = create_text ();
gtk_table_attach (GTK_TABLE (top_table), text1, 0, 141, 0, 1,
                      	GTK_FILL,
                      	GTK_FILL | GTK_SHRINK, 0, 0);
gtk_widget_set_usize(GTK_WIDGET(text1), 200, -1);
gtk_widget_show (text1);

dummy1=gtk_label_new("");
gtk_table_attach (GTK_TABLE (top_table), dummy1, 141, 171, 0, 1,
                      	GTK_FILL,
                      	GTK_SHRINK, 0, 0);
gtk_widget_show (dummy1);

zoombuttons = create_zoom_buttons ();
gtk_table_attach (GTK_TABLE (top_table), zoombuttons, 171, 173, 0, 1,
                      	GTK_FILL,
                      	GTK_SHRINK, 0, 0);
gtk_widget_show (zoombuttons);

if(!use_scrollbar_only)
	{
	pagebuttons = create_page_buttons ();
	gtk_table_attach (GTK_TABLE (top_table), pagebuttons, 173, 174, 0, 1,
	                      	GTK_FILL,
	                      	GTK_SHRINK, 0, 0);
	gtk_widget_show (pagebuttons);
	fetchbuttons = create_fetch_buttons ();
	gtk_table_attach (GTK_TABLE (top_table), fetchbuttons, 174, 175, 0, 1,
	                      	GTK_FILL,
	                      	GTK_SHRINK, 0, 0);
	gtk_widget_show (fetchbuttons);
	discardbuttons = create_discard_buttons ();
	gtk_table_attach (GTK_TABLE (top_table), discardbuttons, 175, 176, 0, 1,
	                      	GTK_FILL,
	                      	GTK_SHRINK, 0, 0);
	gtk_widget_show (discardbuttons);
	
	shiftbuttons = create_shift_buttons ();
	gtk_table_attach (GTK_TABLE (top_table), shiftbuttons, 176, 177, 0, 1,
	                      	GTK_FILL,
	                      	GTK_SHRINK, 0, 0);
	gtk_widget_show (shiftbuttons);
	}

dummy2=gtk_label_new("");
gtk_table_attach (GTK_TABLE (top_table), dummy2, 177, 215, 0, 1,
                      	GTK_FILL,
                      	GTK_SHRINK, 0, 0);
gtk_widget_show (dummy2);

entry = create_entry_box();
gtk_table_attach (GTK_TABLE (top_table), entry, 215, 216, 0, 1,
                      	GTK_SHRINK,
                      	GTK_SHRINK, 0, 0);
gtk_widget_show(entry);

timebox = create_time_box();
gtk_table_attach (GTK_TABLE (top_table), timebox, 216, 284, 0, 1,
                      	GTK_FILL | GTK_EXPAND,
                      	GTK_FILL | GTK_EXPAND | GTK_SHRINK, 20, 0);
gtk_widget_show (timebox);

wavewindow = create_wavewindow();
load_all_fonts(); /* must be done before create_signalwindow() */
gtk_widget_show(wavewindow);
signalwindow = create_signalwindow();

if(do_resize_signals) 
                {
                int os;
                os=max_signal_name_pixel_width;
                os=(os<48)?48:os;
                gtk_widget_set_usize(GTK_WIDGET(signalwindow),
                                os+30, -1);
                }

gtk_widget_show(signalwindow);

#if GTK_CHECK_VERSION(2,4,0)
if(!hide_sst)
	{
	toppanedwindow = gtk_hpaned_new();
	sstpane = treeboxframe("SST", GTK_SIGNAL_FUNC(mkmenu_treesearch_cleanup));
 
	expanderwindow = gtk_expander_new_with_mnemonic("_SST");
	gtk_expander_set_expanded(GTK_EXPANDER(expanderwindow), (sst_expanded==TRUE));
	gtk_container_add(GTK_CONTAINER(expanderwindow), sstpane);
	gtk_widget_show(expanderwindow);
	}
#endif

panedwindow=gtk_hpaned_new();

#ifdef HAVE_PANED_PACK
if(paned_pack_semantics)
	{
	gtk_paned_pack1(GTK_PANED(panedwindow), signalwindow, 0, 0); 
	gtk_paned_pack2(GTK_PANED(panedwindow), wavewindow, ~0, 0);
	}
	else
#endif
	{
	gtk_paned_add1(GTK_PANED(panedwindow), signalwindow);
	gtk_paned_add2(GTK_PANED(panedwindow), wavewindow);
	}

gtk_widget_show(panedwindow);

#if GTK_CHECK_VERSION(2,4,0)
if(!hide_sst)
	{
	gtk_paned_pack1(GTK_PANED(toppanedwindow), expanderwindow, 0, 0);
	gtk_paned_pack2(GTK_PANED(toppanedwindow), panedwindow, ~0, 0);
	gtk_widget_show(toppanedwindow);
	}
#endif

gtk_widget_show(top_table);

gtk_table_attach (GTK_TABLE (whole_table), force_toolbars?toolhandle:top_table, 0, 16, 0, 1,
                      	GTK_FILL | GTK_EXPAND,
                      	GTK_FILL | GTK_EXPAND | GTK_SHRINK, 0, 0);

gtk_table_attach (GTK_TABLE (whole_table), toppanedwindow ? toppanedwindow : panedwindow, 0, 16, 1, 256,
                      	GTK_FILL | GTK_EXPAND,
                      	GTK_FILL | GTK_EXPAND | GTK_SHRINK, 0, 0);
gtk_widget_show(whole_table);
gtk_container_add (GTK_CONTAINER (main_vbox), whole_table);

update_markertime(time_trunc(tims.marker));

set_window_xypos(initial_window_xpos, initial_window_ypos);
xy_ignore = 1;

if(logfile) 
	{
	struct logfile_chain *lprev;
	char buf[50];
	int which = 1;
	while(logfile)
		{
		sprintf(buf, "Logfile viewer [%d]", which++);
		logbox(buf, 480, logfile->name);
		lprev = logfile;
		logfile = logfile->next;
		free_2(lprev->name);
		free_2(lprev);
		}
	}

activate_stems_reader(stems_name);

while (gtk_events_pending()) gtk_main_iteration();

if(1)	/* here in order to calculate window manager delta if present... window is completely rendered by here */
	{
	int dummy_x, dummy_y;
	get_window_xypos(&dummy_x, &dummy_y);
	}

init_busy();

if(scriptfile)
	{
	execute_script(scriptfile);
	free_2(scriptfile); scriptfile=NULL;
	}

#if !defined _MSC_VER && !defined __MINGW32__
if(dual_attach_id)
	{
	fprintf(stderr, "GTKWAVE | Attaching %08X as dual head session %d\n", dual_attach_id, dual_id);

	dual_ctx = shmat(dual_attach_id, NULL, 0);
	if(dual_ctx)
		{
		if(memcmp(dual_ctx[dual_id].matchword, DUAL_MATCHWORD, 4))
			{
			fprintf(stderr, "Not a valid shared memory ID for dual head operation, exiting.\n");
			exit(255);
			}

		dual_ctx[dual_id].viewer_is_initialized = 1;
		for(;;)
			{
		        GtkAdjustment *hadj;
		        TimeType pageinc, gt;
			struct timeval tv;

			if(dual_ctx[1-dual_id].use_new_times)
				{
				dual_race_lock = 1;

			        gt = dual_ctx[dual_id].left_margin_time = dual_ctx[1-dual_id].left_margin_time;

				dual_ctx[dual_id].marker = dual_ctx[1-dual_id].marker;
				dual_ctx[dual_id].baseline = dual_ctx[1-dual_id].baseline;
				dual_ctx[dual_id].zoom = dual_ctx[1-dual_id].zoom;
				dual_ctx[1-dual_id].use_new_times = 0;
				dual_ctx[dual_id].use_new_times = 0;

				if(dual_ctx[dual_id].baseline != tims.baseline)
					{
					if((tims.marker != -1) && (dual_ctx[dual_id].marker == -1))
						{
				        	Trptr t;
  
        					for(t=traces.first;t;t=t->t_next)
                					{
                					if(t->asciivalue) { free_2(t->asciivalue); t->asciivalue=NULL; }
                					}

	        				for(t=traces.buffer;t;t=t->t_next)
	                				{
	                				if(t->asciivalue) { free_2(t->asciivalue); t->asciivalue=NULL; }
	                				}
						}

					tims.marker = dual_ctx[dual_id].marker;
					tims.baseline = dual_ctx[dual_id].baseline;
					update_basetime(tims.baseline);
					update_markertime(tims.marker);
					signalwindow_width_dirty = 1;
					button_press_release_common();
					}
				else
				if(dual_ctx[dual_id].marker != tims.marker)
					{
					if((tims.marker != -1) && (dual_ctx[dual_id].marker == -1))
						{
				        	Trptr t;
  
        					for(t=traces.first;t;t=t->t_next)
                					{
                					if(t->asciivalue) { free_2(t->asciivalue); t->asciivalue=NULL; }
                					}

	        				for(t=traces.buffer;t;t=t->t_next)
	                				{
	                				if(t->asciivalue) { free_2(t->asciivalue); t->asciivalue=NULL; }
	                				}
						}

					tims.marker = dual_ctx[dual_id].marker;
					update_markertime(tims.marker);
					signalwindow_width_dirty = 1;
					button_press_release_common();
					}

				tims.prevzoom=tims.zoom;
				tims.zoom=dual_ctx[dual_id].zoom;

			        if(gt<tims.first) gt=tims.first;
			        else if(gt>tims.last) gt=tims.last;

			        hadj=GTK_ADJUSTMENT(wave_hslider);
			        hadj->value=gt;
        
			        pageinc=(TimeType)(((gdouble)wavewidth)*nspx);
			        if(gt<(tims.last-pageinc+1))
			                tims.timecache=gt;
			                else
			                {
			                tims.timecache=tims.last-pageinc+1;
			                if(tims.timecache<tims.first) tims.timecache=tims.first;
			                }

			        time_update();
				}

			if(is_interactive)
				{
				kick_partial_vcd();
				}
				else
				{
				while (gtk_events_pending()) gtk_main_iteration();
				}

			dual_race_lock = 0;

			tv.tv_sec = 0;
       			tv.tv_usec = 1000000 / 25;
			select(0, NULL, NULL, NULL, &tv);
			}
		}
		else
		{
		fprintf(stderr, "Could not attach to %08X, exiting.\n", dual_attach_id);
		exit(255);
		}
	}
else
if(is_interactive)
	{
	for(;;) kick_partial_vcd();
	}
	else
#endif
	{
	gtk_main();
	}

return(0);
}

void
get_window_size (int *x, int *y)
{
#ifdef WAVE_USE_GTK2
  gtk_window_get_size (GTK_WINDOW (mainwindow), x, y);
#else
  *x = initial_window_x;
  *y = initial_window_y;
#endif
}

void
set_window_size (int x, int y)
{
  if (mainwindow == NULL)
    {
      initial_window_width = x;
      initial_window_height = y;
    }
  else
    {
      if(!socket_xid)
	{
      	gtk_window_set_default_size(GTK_WINDOW (mainwindow), x, y);
	}
    }
}


void 
get_window_xypos(int *root_x, int *root_y)
{
if(!mainwindow) return;

#ifdef WAVE_USE_GTK2
gtk_window_get_position(GTK_WINDOW(mainwindow), root_x, root_y);

if(!initial_window_get_valid)
	{
	if((mainwindow->window))
		{
		initial_window_get_valid = 1;
		initial_window_xpos_get = *root_x;
		initial_window_ypos_get = *root_y;

		xpos_delta = initial_window_xpos_set - initial_window_xpos_get;
		ypos_delta = initial_window_ypos_set - initial_window_ypos_get;
		}
	}
#else
*root_x = *root_y = -1;
#endif
}

void 
set_window_xypos(int root_x, int root_y)
{
if(xy_ignore) return;

#if !defined __MINGW32__ && !defined _MSC_VER
initial_window_xpos = root_x;
initial_window_ypos = root_y;

if(!mainwindow) return;
if((initial_window_xpos>=0)||(initial_window_ypos>=0))
	{
	if (initial_window_xpos<0) { initial_window_xpos = 0; }
	if (initial_window_ypos<0) { initial_window_ypos = 0; }
#ifdef WAVE_USE_GTK2
	gtk_window_move(GTK_WINDOW(mainwindow), initial_window_xpos, initial_window_ypos);
#else
	gtk_window_reposition(GTK_WINDOW(mainwindow), initial_window_xpos, initial_window_ypos);
#endif

	if(!initial_window_set_valid)
		{
		initial_window_set_valid = 1;
		initial_window_xpos_set = initial_window_xpos;
		initial_window_ypos_set = initial_window_ypos;
		}
	}
#endif
}


/*
 * bring up stems browser
 */
#if !defined _MSC_VER && !defined __MINGW32__
int stems_are_active(void)
{
if(anno_ctx && anno_ctx->browser_process)
	{
	int stat =0;
	pid_t pid = waitpid(anno_ctx->browser_process, &stat, WNOHANG);
	if(!pid)
		{
		status_text("Stems reader already active.\n");
		return(1);
		}
		else
		{
		shmdt(anno_ctx);
		anno_ctx = NULL;
		}
	}

return(0);
}
#endif

void activate_stems_reader(char *stems_name)
{
#if !defined _MSC_VER && !defined __MINGW32__

#ifdef __CYGWIN__
static int cyg_called = 0;
#endif

if(!stems_name) return;

#ifdef __CYGWIN__
if(stems_type != WAVE_ANNO_NONE)
	{
	if(!cyg_called)
		{
		fprintf(stderr, "GTKWAVE | If the viewer crashes with a Bad system call error,\n");
		fprintf(stderr, "GTKWAVE | make sure that Cygserver is enabled.\n");
		cyg_called = 1;
		}
	}
#endif

if(stems_type != WAVE_ANNO_NONE)
	{
	int shmid = shmget(0, sizeof(struct gtkwave_annotate_ipc_t), IPC_CREAT | 0600 );
	if(shmid >=0)
		{
		struct shmid_ds ds;

		anno_ctx = shmat(shmid, NULL, 0);
		if(anno_ctx)
			{
			pid_t pid;

			memset(anno_ctx, 0, sizeof(struct gtkwave_annotate_ipc_t));

			memcpy(anno_ctx->matchword, WAVE_MATCHWORD, 4);
			anno_ctx->aet_type = stems_type;			
			strcpy(anno_ctx->aet_name, aet_name);
			strcpy(anno_ctx->stems_name, stems_name);

			anno_ctx->gtkwave_process = getpid();			
			update_markertime(tims.marker);

#ifdef __linux__
			shmctl(shmid, IPC_RMID, &ds); /* mark for destroy */
#endif

		        pid=fork();

		        if(((int)pid) < 0) 
				{ 
				/* can't do anything about this */
				}
				else
				{
			        if(pid) /* parent==original server_pid */
			                {
					anno_ctx->browser_process = pid;
					atexit(kill_browser);
#ifndef __linux__
					sleep(2);
					shmctl(shmid, IPC_RMID, &ds); /* mark for destroy */
#endif
			                }
					else
					{
					char buf[64];
					sprintf(buf, "%08x", shmid);
				        execlp("rtlbrowse", "rtlbrowse", buf, NULL);
				        exit(0);        /* control never gets here if successful */
					}
				}
			}
			else
			{
			shmctl(shmid, IPC_RMID, &ds); /* actually destroy */
			stems_type = WAVE_ANNO_NONE;
			}
		}
	}
	else
	{
	fprintf(stderr, "GTKWAVE | Unsupported dumpfile type for rtlbrowse.\n");
	}
#endif
}

/*
 * $Id$
 * $Log$
 * Revision 1.1.1.1  2007/05/30 04:27:56  gtkwave
 * Imported sources
 *
 * Revision 1.3  2007/05/28 00:55:06  gtkwave
 * added support for arrays as a first class dumpfile datatype
 *
 * Revision 1.2  2007/04/20 02:08:13  gtkwave
 * initial release
 *
 */

