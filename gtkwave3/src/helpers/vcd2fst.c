/*
 * Copyright (c) 2009 Tony Bybell.
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

#if HAVE_GETOPT_H
#include <getopt.h>
#endif

#include "fst/fstapi.h"
#include "../../contrib/rtlbrowse/jrb.h"
#include "wave_locale.h"

size_t getline_replace(char **buf, size_t *len, FILE *f)
{
if(!*buf)
	{
	*buf = malloc(32768);
	*len = 32767;
	}

(*buf)[0] = 0;
fgets(*buf, 32767, f);
if(!(*buf)[0])
	{
	return(-1);
	}
	else
	{
	return(1);
	}
}

JRB vcd_ids = NULL;

static unsigned int vcdid_hash(char *s)
{
unsigned int val=0;
int i;
int len = strlen(s);

s+=(len-1);

for(i=0;i<len;i++)
        {
        val *= 94;                              /* was 94 but XL uses '!' as right hand side chars which act as leading zeros */
        val += (((unsigned char)*s) - 32);

        s--;
        }

return(val);
}  


int fst_main(char *vname, char *fstname)
{
FILE *f;
char *buf = NULL;
size_t glen;
struct fstContext *ctx;
int line = 0;
size_t ss;
fstHandle returnedhandle;
JRB node;
char bin_fixbuff[32769];

ctx = fstWriterCreate(fstname, 1);
if(!strcmp("-", vname))
	{
	f = stdin;
	}
	else
	{
	f = fopen(vname, "rb");
	}

if(!f)
	{
	printf("could not open '%s', exiting.\n", vname);
	exit(255);
	}

vcd_ids = make_jrb();
fstWriterSetPackType(ctx, 0);

while(!feof(f))
	{
	ss = getline_replace(&buf, &glen, f);
	if(ss == -1)
		{
		break;
		}
	line++;

	if(!strncmp(buf, "$var", 4))
		{
		char *st = strtok(buf+5, " \t");
		enum fstVarType vartype;
		int len;
		char *nam;
		unsigned int hash;

		if(!strcmp(st, "wire"))
			{
			vartype = FST_VT_VCD_WIRE;
			}
		else
		if(!strcmp(st, "reg"))
			{
			vartype = FST_VT_VCD_REG;
			}
		else
		if(!strcmp(st, "integer"))
			{
			vartype = FST_VT_VCD_INTEGER;
			}
		else
		if(!strcmp(st, "supply1"))
			{
			vartype = FST_VT_VCD_SUPPLY1;
			}
		else
		if(!strcmp(st, "time"))
			{
			vartype = FST_VT_VCD_TIME;
			}
		else
		if(!strcmp(st, "real"))
			{
			vartype = FST_VT_VCD_REAL;
			}
		else
			{
			vartype = FST_VT_VCD_WIRE;
			}

		st = strtok(NULL, " \t");
		len = atoi(st);

		st = strtok(NULL, " \t"); /* vcdid */
		hash = vcdid_hash(st);

		nam = strtok(NULL, " \t"); /* name */
		st = strtok(NULL, " \t"); /* $end */

		if(strncmp(st, "$end", 4))
			{
			*(st-1) = ' ';
			}

		node = jrb_find_int(vcd_ids, hash);
		if(!node)
			{
			Jval val;
			returnedhandle = fstWriterCreateVar(ctx, vartype, FST_VD_IMPLICIT, len, nam, 0);
			val.i = returnedhandle;
			jrb_insert_int(vcd_ids, hash, val)->val2.i = len;
			}
			else
			{
			fstWriterCreateVar(ctx, vartype, FST_VD_IMPLICIT, node->val2.i, nam, node->val.i);
			}
		}
	else
	if(!strncmp(buf, "$scope", 6))
		{
		char *st = strtok(buf+6, " \t");
		enum fstScopeType scopetype;

		if(!strcmp(buf, "module"))
			{
			scopetype = FST_ST_VCD_MODULE;
			}
		else
		if(!strcmp(buf, "task"))
			{
			scopetype = FST_ST_VCD_TASK;
			}
		else
		if(!strcmp(buf, "function"))
			{
			scopetype = FST_ST_VCD_FUNCTION;
			}
		else
		if(!strcmp(buf, "begin"))
			{
			scopetype = FST_ST_VCD_BEGIN;
			}
		else
			{
			scopetype = FST_ST_VCD_MODULE;
			}

		st = strtok(NULL, " \t");

		fstWriterSetScope(ctx, scopetype, st, NULL);
		}
	else
	if(!strncmp(buf, "$upscope", 8))
		{
		fstWriterSetUpscope(ctx);
		}
	else
	if(!strncmp(buf, "$endd", 5))
		{
		break;
		}
	else
	if(!strncmp(buf, "$timescale", 10))
		{
		char *pnt;
		int exp = -9;
		int tv = 1;
		ss = getline_replace(&buf, &glen, f);
		if(ss == -1)
			{
			break;
			}
		line++;
		pnt = buf;
		while(*pnt)
			{
			int mat = 0;
			switch(*pnt)
				{
				case 'm': exp = -3; mat = 1; break;
				case 'u': exp = -6; mat = 1; break;
				case 'n': exp = -9; mat = 1; break;
				case 'p': exp = -12; mat = 1; break;
				case 'f': exp = -15; mat = 1; break;
				case 's': exp = -0; mat = 1; break;
				default: break;
				}
			if(mat) break;
			pnt++;
			}

		tv = atoi(buf);
		if(tv == 10)
			{
			exp++;
			}
		else
		if(tv == 100)
			{
			exp+=2;
			}

		fstWriterSetTimescale(ctx, exp);
		}
	else
	if(!strncmp(buf, "$date", 5))
		{
		char *pnt;
		ss = getline_replace(&buf, &glen, f);
		if(ss == -1)
			{
			break;
			}
		line++;
		pnt = buf;
		while(*pnt == '\t') pnt++;
		fstWriterSetDate(ctx, pnt);
		}
	else
	if(!strncmp(buf, "$version", 8))
		{
		char *pnt, *crpnt;
		ss = getline_replace(&buf, &glen, f);
		if(ss == -1)
			{
			break;
			}
		line++;
		pnt = buf;
		while(*pnt == '\t') pnt++;
		crpnt = strchr(pnt, '\n');
		if(crpnt) *crpnt = 0;
		crpnt = strchr(pnt, '\r');
		if(crpnt) *crpnt = 0;
		fstWriterSetVersion(ctx, pnt);
		}
	}

while(!feof(f))
	{
	unsigned int hash;
	uint64_t tim;
	size_t len;
	char *nl, *sp;
	double doub;

	ss = getline_replace(&buf, &len, f);
	if(ss == -1)
		{
		break;
		}
	nl = strchr(buf, '\n');
	if(nl) *nl = 0;

	switch(buf[0])
		{
		case '0':
		case '1':
		case 'x':
		case 'z':
			hash = vcdid_hash(buf+1);
			node = jrb_find_int(vcd_ids, hash);
			if(node)
				{
				fstWriterEmitValueChange(ctx, node->val.i, buf);
				}
				else
				{
				}
			break;

		case 'b':
			sp = strchr(buf, ' ');
			*sp = 0;
			hash = vcdid_hash(sp+1);
			node = jrb_find_int(vcd_ids, hash);
			if(node)
				{
				int bin_len = strlen(buf+1);
				int node_len = node->val2.i;
				if(bin_len == node_len)
					{
					fstWriterEmitValueChange(ctx, node->val.i, buf+1);
					}
					else
					{
					int delta = node_len - bin_len;
					memset(bin_fixbuff, buf[1] != '1' ? buf[1] : '0', delta);
					memcpy(bin_fixbuff + delta, buf+1, bin_len);
					fstWriterEmitValueChange(ctx, node->val.i, bin_fixbuff);
					}
				}
				else
				{
				}
			break;

		case 'r':
			sp = strchr(buf, ' ');
			hash = vcdid_hash(sp+1);
			node = jrb_find_int(vcd_ids, hash);
			if(node)
				{
		                sscanf(buf+1,"%lg",&doub); 
				fstWriterEmitValueChange(ctx, node->val.i, &doub);
				}
				else
				{
				}
			break;

		case '#':
			sscanf(buf+1, "%"SCNu64, &tim);
			fstWriterEmitTimeChange(ctx, tim);
			break;

		default:
			if(!strncmp(buf, "$dumpon", 7))
				{
				fstWriterEmitDumpActive(ctx, 1);
				}
			else
			if(!strncmp(buf, "$dumpoff", 8))
				{
				fstWriterEmitDumpActive(ctx, 0);
				}
			else
			if(!strncmp(buf, "$dumpvars", 9))
				{
				/* nothing */
				}
			else
				{
				/* printf("FST '%s'\n", buf); */
				}
			break;
		}
	}

fstWriterClose(ctx);

if(buf)
	{
	free(buf);
	}
if(f != stdin) fclose(f);

exit(0);
}


void print_help(char *nam)
{
#ifdef __linux__ 
printf(
"Usage: %s [OPTION]... [VCDFILE] [FSTFILE]\n\n"
"  -v, --vcdname=FILE         specify VCD input filename\n"
"  -f, --fstname=FILE         specify FST output filename\n"
"  -h, --help                 display this help then exit\n\n"

"Note that VCDFILE and FSTFILE are optional provided the\n"
"--vcdname and --fstname options are specified.\n\n"
"Report bugs to <bybell@nc.rr.com>.\n",nam);
#else
printf(
"Usage: %s [OPTION]... [VCDFILE] [FSTFILE]\n\n"
"  -v FILE                    specify VCD input filename\n"
"  -f FILE                    specify FST output filename\n"
"  -h                         display this help then exit\n\n"

"Note that VCDFILE and FSTFILE are optional provided the\n"
"--vcdname and --fstname options are specified.\n\n"
"Report bugs to <bybell@nc.rr.com>.\n",nam);
#endif

exit(0);
}


int main(int argc, char **argv)
{
char opt_errors_encountered=0;
char *vname=NULL, *lxname=NULL;
int c;

WAVE_LOCALE_FIX

while (1)
        {
#ifdef __linux__
        int option_index = 0;
                        
        static struct option long_options[] =
                {
		{"vcdname", 1, 0, 'v'},
		{"fstname", 1, 0, 'f'},
                {"help", 0, 0, 'h'},
                {0, 0, 0, 0}  
                };
                
        c = getopt_long (argc, argv, "v:f:h", long_options, &option_index);
#else
        c = getopt      (argc, argv, "v:f:h");
#endif
                        
        if (c == -1) break;     /* no more args */
                        
        switch (c)
                {
		case 'v':
			if(vname) free(vname);
                        vname = malloc(strlen(optarg)+1);
                        strcpy(vname, optarg);
			break;

		case 'f':
			if(lxname) free(lxname);
                        lxname = malloc(strlen(optarg)+1);
                        strcpy(lxname, optarg);
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
                if(!vname)
                        {
                        vname = malloc(strlen(argv[optind])+1);
                        strcpy(vname, argv[optind++]);
                        } 
                else if(!lxname)
                        {
                        lxname = malloc(strlen(argv[optind])+1);
                        strcpy(lxname, argv[optind++]);
                        }
                }
        }
                        
if((!vname)||(!lxname))
        {
        print_help(argv[0]);
        }

fst_main(vname, lxname);

free(vname); free(lxname);

return(0);
}

/*
 * $Id$
 * $Log$
 */
