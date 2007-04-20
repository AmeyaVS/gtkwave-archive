/*
 * Copyright (c) 2003-7 Tony Bybell.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <config.h>
#include "lxt2_read.h"

#if HAVE_GETOPT_H
#include <getopt.h>
#endif

static char *match = NULL;
static int matchlen = 0;
static int names_only = 0;
static char *killed_list = NULL;

extern void free_hier(void);
extern char *output_hier(char *name);

char vcd_blackout;

void vcd_callback(struct lxt2_rd_trace **lt, lxtint64_t *pnt_time, lxtint32_t *pnt_facidx, char **pnt_value)
{
struct lxt2_rd_geometry *g = lxt2_rd_get_fac_geometry(*lt, *pnt_facidx);

/* fprintf(stderr, "%lld %d %s\n", *pnt_time, *pnt_facidx, *pnt_value); */

if(!(*pnt_value)[0])
	{
	if(!vcd_blackout)
		{
		vcd_blackout = 1;
		/* printf("$dumpoff\n"); */
		}

	return;
	}
	else
	{
	if(vcd_blackout)
		{
		vcd_blackout = 0;
		/* printf("$dumpon\n"); */
		}
	}

if(g->len >= matchlen)
	{
	if(!killed_list[*pnt_facidx])
		{
		if(strstr(*pnt_value, match))
			{
			if(g->len > 1)
				{
				if(!names_only)
					{
					printf("#%lld %s[%d:%d] %s\n", *pnt_time, lxt2_rd_get_facname(*lt, *pnt_facidx), g->msb, g->lsb, *pnt_value);
					}
					else
					{
					printf("%s[%d:%d]\n", lxt2_rd_get_facname(*lt, *pnt_facidx), g->msb, g->lsb);
					}
				}
				else
				{
				if(g->msb < 0)
					{
					if(!names_only)
						{
						printf("#%lld %s %s\n", *pnt_time, lxt2_rd_get_facname(*lt, *pnt_facidx), *pnt_value);
						}
						else
						{
						printf("%s\n", lxt2_rd_get_facname(*lt, *pnt_facidx));
						}
					}
					else
					{
					if(!names_only)
						{
						printf("#%lld %s[%d] %s\n", *pnt_time, lxt2_rd_get_facname(*lt, *pnt_facidx), g->msb, *pnt_value);
						}
						else
						{
						printf("%s[%d]\n", lxt2_rd_get_facname(*lt, *pnt_facidx), g->msb);
						}
					}
				}
			lxt2_rd_clr_fac_process_mask(*lt, *pnt_facidx);
			killed_list[*pnt_facidx] = 1;
			}
		}
	}
}


int process_lxt2(char *fname)
{
struct lxt2_rd_trace *lt;

lt=lxt2_rd_init(fname);
if(lt)
	{
	int numfacs;
	
	numfacs = lxt2_rd_get_num_facs(lt);
	killed_list = calloc(numfacs, sizeof(char));
	lxt2_rd_set_fac_process_mask_all(lt);
	lxt2_rd_set_max_block_mem_usage(lt, 0);	/* no need to cache blocks */

	lxt2_rd_iter_blocks(lt, vcd_callback, NULL);

	lxt2_rd_close(lt);
	free(killed_list);
	}
	else
	{
	fprintf(stderr, "lxt2_rd_init failed\n");
	return(255);
	}

return(0);
}

/*******************************************************************************/

void print_help(char *nam)
{
#ifdef __linux__ 
printf(
"Usage: %s [OPTION]... [VZTFILE]\n\n"
"  -d, --dumpfile=FILE        specify LXT2 input dumpfile\n"
"  -m, --match                bitwise match value\n"
"  -x, --hex                  hex match value\n"
"  -n, --namesonly            emit facsnames only (gtkwave savefile)\n"
"  -h, --help                 display this help then exit\n\n"
"First occurrence of facnames with times and matching values are emitted to\nstdout.  Using -n generates a gtkwave save file.\n\n"
"Report bugs to <bybell@nc.rr.com>.\n",nam);
#else
printf(
"Usage: %s [OPTION]... [VZTFILE]\n\n"
"  -d                         specify LXT2 input dumpfile\n"
"  -m                         bitwise match value\n"
"  -x                         hex match value\n"
"  -n                         emit facsnames only\n"
"  -h                         display this help then exit (gtkwave savefile)\n\n"
"First occurrence of facnames with times and matching values are emitted to\nstdout.  Using -n generates a gtkwave save file.\n\n"
"Report bugs to <bybell@nc.rr.com>.\n",nam);
#endif

exit(0);
}


int main(int argc, char **argv)
{
char opt_errors_encountered=0;
char *lxname=NULL;
int c;
int rc;
int i, j, k;

WAVE_LOCALE_FIX

while (1)
        {
        int option_index = 0;
                        
#ifdef __linux__
        static struct option long_options[] =
                {
		{"dumpfile", 1, 0, 'd'},
		{"match", 1, 0, 'm'},
		{"hex", 1, 0, 'x'},
		{"namesonly", 0, 0, 'n'},
                {"help", 0, 0, 'h'},
                {0, 0, 0, 0}  
                };
                
        c = getopt_long (argc, argv, "d:m:x:nh", long_options, &option_index);
#else
        c = getopt      (argc, argv, "d:m:x:nh");
#endif
                        
        if (c == -1) break;     /* no more args */
                        
        switch (c)
                {
		case 'n':
			names_only = 1;
			break;

		case 'd':
			if(lxname) free(lxname);
                        lxname = malloc(strlen(optarg)+1);
                        strcpy(lxname, optarg);
			break;

		case 'm': if(match) free(match);
			match = malloc((matchlen = strlen(optarg))+1);
			strcpy(match, optarg);
			break;

		case 'x': if(match) free(match);
			match = malloc((matchlen = 4*strlen(optarg))+1);
			for(i=0,k=0;i<matchlen;i+=4,k++)
				{
				int ch = tolower(optarg[k]);

				if(ch=='z')
					{
					for(j=0;j<4;j++)
						{
						match[i+j] = 'z';
						}
					}
				else
				if((ch>='0')&&(ch<='9'))
					{
					ch -= '0';
					for(j=0;j<4;j++)
						{
						match[i+j] = ((ch>>(3-j))&1) + '0';
						}
					}
				else
				if((ch>='a')&&(ch<='f'))
					{
					ch = ch - 'a' + 10;
					for(j=0;j<4;j++)
						{
						match[i+j] = ((ch>>(3-j))&1) + '0';
						}
					}
				else /* "x" */
					{
					for(j=0;j<4;j++)
						{
						match[i+j] = 'x';
						}
					}
				}
			match[matchlen] = 0;
			break;

                case 'h':
			print_help(argv[0]);
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
                if(!lxname)
                        {
                        lxname = malloc(strlen(argv[optind])+1);
                        strcpy(lxname, argv[optind++]);
                        }
                }
        }
                        
if(!lxname)
        {
        print_help(argv[0]);
        }

rc=process_lxt2(lxname);
free(lxname);

return(rc);
}

/*
 * $Id$
 * $Log$
 */

