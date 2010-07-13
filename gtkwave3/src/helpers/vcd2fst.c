/*
 * Copyright (c) 2009-2010 Tony Bybell.
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

size_t getline_replace(char **wbuf, char **buf, size_t *len, FILE *f)
{
char *fgets_rc;

if(!*wbuf)
	{
	*wbuf = malloc(32768);
	*len = 32767;
	}

(*wbuf)[0] = 0;
fgets_rc = fgets(*wbuf, 32767, f);
*buf = *wbuf;
while(*(buf)[0]==' ') { (*buf)++; } /* verilator leading spaces fix */

if((!(*buf)[0])||(!fgets_rc))
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

int pack_type = 0;  /* set to 1 for fastlz */
int repack_all = 0; /* 0 is normal, 1 does the repack (via fstapi) at end */

int fst_main(char *vname, char *fstname)
{
FILE *f;
char *buf = NULL, *wbuf = NULL;
size_t glen;
struct fstContext *ctx;
int line = 0;
size_t ss;
fstHandle returnedhandle;
JRB node;
uint64_t prev_tim = 0;
char bin_fixbuff[32769];

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

ctx = fstWriterCreate(fstname, 1);

if(!ctx)
	{
	printf("could not open '%s', exiting.\n", fstname);
	exit(255);
	}

vcd_ids = make_jrb();
fstWriterSetPackType(ctx, pack_type);
fstWriterSetRepackOnClose(ctx, repack_all);

while(!feof(f))
	{
	ss = getline_replace(&wbuf, &buf, &glen, f);
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

		vartype = FST_VT_VCD_WIRE;
		switch(st[0])
			{
			case 'w':
				if(!strcmp(st, "wire"))
					{
					}
				else
				if(!strcmp(st, "wand"))
					{
					vartype = FST_VT_VCD_WAND;
					}
				else
				if(!strcmp(st, "wor"))
					{
					vartype = FST_VT_VCD_WOR;
					}
				break;

			case 'r':
				if(!strcmp(st, "reg"))
					{
					vartype = FST_VT_VCD_REG;
					}
				else
				if(!strcmp(st, "real"))
					{
					vartype = FST_VT_VCD_REAL;
					}
				else
				if(!strcmp(st, "real_parameter"))
					{
					vartype = FST_VT_VCD_REAL_PARAMETER;
					}
				else
				if(!strcmp(st, "realtime"))
					{
					vartype = FST_VT_VCD_REALTIME;
					}
				break;
				
			case 'p':
				if(!strcmp(st, "parameter"))
					{
					vartype = FST_VT_VCD_PARAMETER;
					}
				else
				if(!strcmp(st, "port"))
					{
					vartype = FST_VT_VCD_PORT;
					}
				break;

			case 'i':
				vartype = FST_VT_VCD_INTEGER;
				break;

			case 'e':
				vartype = FST_VT_VCD_EVENT;
				break;

			case 's':
				if(!strcmp(st, "supply1"))		
					{
					vartype = FST_VT_VCD_SUPPLY1;
					}
				else
				if(!strcmp(st, "supply0"))		
					{
					vartype = FST_VT_VCD_SUPPLY0;
					}
				break;

			case 't':
				if(!strcmp(st, "time"))
					{
					vartype = FST_VT_VCD_TIME;
					}
				else
				if(!strcmp(st, "tri"))
					{
					vartype = FST_VT_VCD_TRI;
					}
				else
				if(!strcmp(st, "triand"))
					{
					vartype = FST_VT_VCD_TRIAND;
					}
				else
				if(!strcmp(st, "trior"))
					{
					vartype = FST_VT_VCD_TRIOR;
					}
				else
				if(!strcmp(st, "trireg"))
					{
					vartype = FST_VT_VCD_TRIREG;
					}
				else
				if(!strcmp(st, "tri0"))
					{
					vartype = FST_VT_VCD_TRI0;
					}
				else
				if(!strcmp(st, "tri1"))
					{
					vartype = FST_VT_VCD_TRI1;
					}
				break;

			default:
				break;			
			}

		st = strtok(NULL, " \t");
		len = atoi(st);
		if(vartype == FST_VT_VCD_PORT)
			{
			len = (len * 3) + 2;
			}

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

		if(!strcmp(st, "module"))
			{
			scopetype = FST_ST_VCD_MODULE;
			}
		else
		if(!strcmp(st, "task"))
			{
			scopetype = FST_ST_VCD_TASK;
			}
		else
		if(!strcmp(st, "function"))
			{
			scopetype = FST_ST_VCD_FUNCTION;
			}
		else
		if(!strcmp(st, "begin"))
			{
			scopetype = FST_ST_VCD_BEGIN;
			}
		else
		if(!strcmp(st, "fork"))
			{
			scopetype = FST_ST_VCD_FORK;
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
		char *num = NULL;
		int exp = -9;
		int tv = 1;

		if((pnt = strstr(buf, "$end")))
			{
			*pnt = 0;
			num = strchr(buf, '1');
			}

		if(!num)
			{
			ss = getline_replace(&wbuf, &buf, &glen, f);
			if(ss == -1)
				{
				break;
				}
			line++;
			num = buf;
			}

		pnt = num;
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

		tv = atoi(num);
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
		char *pnt, *rsp;
		int found = 0;

		if((pnt = strstr(buf, "$end")))
			{
                        *pnt = 0;
			pnt = buf + 5;
			while(*pnt && (*pnt==' ')) { *(pnt++); }
			while((rsp = strrchr(pnt, ' ')))
				{
				if(*(rsp+1) == 0)
					{
					*rsp = 0;
					}
					else
					{
					break;
					}					
				}
			if(strlen(pnt)) { found = 1; }
			}
		else
			{
			pnt = buf + 5;
			while(*pnt && (*pnt==' ')) { *(pnt++); }
			while((rsp = strrchr(pnt, ' ')))
				{
				if(*(rsp+1) == 0)
					{
					*rsp = 0;
					}
					else
					{
					break;
					}					
				}
			if(strlen(pnt) > 3) { found = 1; }
			}
		
		if(!found)		
			{
			ss = getline_replace(&wbuf, &buf, &glen, f);
			if(ss == -1)
				{
				break;
				}
			line++;
			pnt = buf;
			}

		while(*pnt == '\t') pnt++;
		fstWriterSetDate(ctx, pnt);
		}
	else
	if(!strncmp(buf, "$version", 8))
		{
		char *pnt, *crpnt, *rsp;

                if((pnt = strstr(buf, "$end")))
                        {
                        *pnt = 0;
			pnt = buf+8;
			while(*pnt && (*pnt==' ')) { *(pnt++); }
			while((rsp = strrchr(pnt, ' ')))
				{
				if(*(rsp+1) == 0)
					{
					*rsp = 0;
					}
					else
					{
					break;
					}					
				}
                        }           
			else
			{
			ss = getline_replace(&wbuf, &buf, &glen, f);
			if(ss == -1)
				{
				break;
				}
			line++;
			pnt = buf;
			}

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

	ss = getline_replace(&wbuf, &buf, &len, f);
	if(ss == -1)
		{
		break;
		}
	nl = strchr(buf, '\n');
	if(nl) *nl = 0;
	nl = strchr(buf, '\r');
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

		case 'p':
			{
			char *src = buf+1;
			char *pnt = bin_fixbuff;
			int pchar = 0;

			for(;;)
				{
				if((*src == '\n') || (*src == '\r')) break;
				if(isspace(*src))
					{
					if(pchar != ' ') { *(pnt++) = pchar = ' '; }
					src++;
					continue;
					}
				*(pnt++) = pchar = *(src++);
				}
			*pnt = 0;
			
			sp = strchr(bin_fixbuff, ' ');
			sp = strchr(sp+1, ' ');
			sp = strchr(sp+1, ' ');
			*sp = 0;
			hash = vcdid_hash(sp+1);
			node = jrb_find_int(vcd_ids, hash);
			if(node)
				{
				fstWriterEmitValueChange(ctx, node->val.i, bin_fixbuff);
				}
				else
				{
				}
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
			if((tim >= prev_tim)||(!prev_tim))
				{
				prev_tim = tim;
				fstWriterEmitTimeChange(ctx, tim);
				}
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

if(wbuf)
	{
	free(wbuf);
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
"  -F, --fastpack             use fastlz algorithm for speed\n"
"  -c, --compress             compress entire file on close\n"
"  -h, --help                 display this help then exit\n\n"

"Note that VCDFILE and FSTFILE are optional provided the\n"
"--vcdname and --fstname options are specified.\n\n"
"Report bugs to <bybell@nc.rr.com>.\n",nam);
#else
printf(
"Usage: %s [OPTION]... [VCDFILE] [FSTFILE]\n\n"
"  -v FILE                    specify VCD input filename\n"
"  -f FILE                    specify FST output filename\n"
"  -F                         use fastlz algorithm for speed\n"
"  -c                         compress entire file on close\n"
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
		{"fastpack", 0, 0, 'F'},
		{"compress", 0, 0, 'c'},
                {"help", 0, 0, 'h'},
                {0, 0, 0, 0}  
                };
                
        c = getopt_long (argc, argv, "v:f:Fch", long_options, &option_index);
#else
        c = getopt      (argc, argv, "v:f:Fch");
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

		case 'F':
			pack_type = 1;
			break;

		case 'c':
			repack_all = 1;
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
 * Revision 1.10  2010/02/22 21:13:37  gtkwave
 * added "realtime" VCD variable
 *
 * Revision 1.9  2010/02/08 17:31:19  gtkwave
 * backtracking time fix
 *
 * Revision 1.8  2009/08/06 20:03:31  gtkwave
 * warnings fixes
 *
 * Revision 1.7  2009/07/26 20:18:08  gtkwave
 * signify error on null FST context rather than run with it
 *
 * Revision 1.6  2009/07/06 21:41:36  gtkwave
 * evcd support issues
 *
 * Revision 1.5  2009/07/03 19:39:06  gtkwave
 * compatibility fix for mingw
 *
 * Revision 1.4  2009/07/01 07:19:10  gtkwave
 * fixed bug in parsing out module type
 *
 * Revision 1.3  2009/06/16 03:00:36  gtkwave
 * added monolithic post-close compress option to writer
 *
 * Revision 1.2  2009/06/14 22:40:59  gtkwave
 * fix for timescale vs initial scope
 *
 * Revision 1.1  2009/06/14 22:08:29  gtkwave
 * added vcd2fst and appropriate fst manpages
 *
 */
