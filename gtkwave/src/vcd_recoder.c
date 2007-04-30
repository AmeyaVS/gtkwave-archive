/* 
 * Copyright (c) Tony Bybell 1999-2007.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */


/*
 * vcd.c 			23jan99ajb
 * evcd parts 			29jun99ajb
 * profiler optimizations 	15jul99ajb
 * more profiler optimizations	25jan00ajb
 * finsim parameter fix		26jan00ajb
 * vector rechaining code	03apr00ajb
 * multiple var section code	06apr00ajb
 * fix for duplicate nets	19dec00ajb
 * support for alt hier seps	23dec00ajb
 * fix for rcs identifiers	16jan01ajb
 * coredump fix for bad VCD	04apr02ajb
 * min/maxid speedup            27feb03ajb
 * bugfix on min/maxid speedup  06jul03ajb
 * escaped hier modification    20feb06ajb
 * added real_parameter vartype 04aug06ajb
 * recoder using vlists         17aug06ajb
 * code cleanup                 04sep06ajb
 * added in/out port vartype    31jan07ajb
 * use gperf for port vartypes  19feb07ajb
 * MTI SV implicit-var fix      05apr07ajb
 * MTI SV len=0 is real var     05apr07ajb
 */
#include <config.h>
#include "vcd.h"
#include "vlist.h"
#include "lx2.h"

static void vlist_emit_uv32(struct vlist_t **vl, unsigned int v)
{
unsigned int nxt;
char *pnt;
         
while((nxt = v>>7))
        {
	pnt = vlist_alloc(vl, 1);
        *pnt = (v&0x7f);
        v = nxt;
        }

pnt = vlist_alloc(vl, 1);
*pnt = (v&0x7f) | 0x80;
} 


static void vlist_emit_string(struct vlist_t **vl, char *s)
{
char *pnt;

while(*s)
	{
	pnt = vlist_alloc(vl, 1);
	*pnt = *s;
	s++;
	}
pnt = vlist_alloc(vl, 1);
*pnt = 0;
}

static void vlist_emit_mvl9_string(struct vlist_t **vl, char *s)
{
char *pnt;
unsigned int recoded_bit;
unsigned char which = 0;
unsigned char accum = 0;

while(*s)
	{
	switch(*s)
	        {
	        case '0':		recoded_bit = AN_0; break;
	        case '1':		recoded_bit = AN_1; break;
	        case 'x': case 'X':	recoded_bit = AN_X; break;
	        case 'z': case 'Z':	recoded_bit = AN_Z; break;
	        case 'h': case 'H':	recoded_bit = AN_H; break;
	        case 'u': case 'U':	recoded_bit = AN_U; break;
	        case 'w': case 'W':     recoded_bit = AN_W; break;
	        case 'l': case 'L':	recoded_bit = AN_L; break;
		default:		recoded_bit = AN_DASH; break;
		}

	if(!which)
		{
		accum = (recoded_bit << 4);
		which = 1;
		}
		else
		{
		accum |= recoded_bit;
		pnt = vlist_alloc(vl, 1);
		*pnt = accum;
		which = accum = 0;
		}	
	s++;
	}

recoded_bit = AN_MSK; /* XXX : this is assumed it is going to remain a 4 bit max quantity! */
if(!which)
	{
        accum = (recoded_bit << 4);
        }
        else
        {
        accum |= recoded_bit;
        }

pnt = vlist_alloc(vl, 1);
*pnt = accum;
}


#undef VCD_BSEARCH_IS_PERFECT		/* bsearch is imperfect under linux, but OK under AIX */

static struct vlist_t *time_vlist = NULL; /* for growable time array */
static unsigned int time_vlist_count = 0;

static FILE *vcd_handle=NULL;
static char vcd_is_compressed=0;

static void add_histent(TimeType time, struct Node *n, char ch, int regadd, char *vector);
static void vcd_build_symbols(void);
static void vcd_cleanup(void);
static void evcd_strcpy(char *dst, char *src);

static off_t vcdbyteno=0;
static int error_count=0;	/* should always be zero */

static int header_over=0;
static int dumping_off=0;
static TimeType start_time=-1;
static TimeType end_time=-1;
static TimeType current_time=-1;

static int num_glitches=0;	/* these never print... just keep around for possible later use */
static int num_glitch_regions=0;

static struct vcdsymbol *pv=NULL, *rootv=NULL;
static char *vcdbuf=NULL, *vst=NULL, *vend=NULL;

/******************************************************************/

enum Tokens   { T_VAR, T_END, T_SCOPE, T_UPSCOPE,
		T_COMMENT, T_DATE, T_DUMPALL, T_DUMPOFF, T_DUMPON,
		T_DUMPVARS, T_ENDDEFINITIONS, 
		T_DUMPPORTS, T_DUMPPORTSOFF, T_DUMPPORTSON, T_DUMPPORTSALL,
		T_TIMESCALE, T_VERSION, T_VCDCLOSE,
		T_EOF, T_STRING, T_UNKNOWN_KEY };

static char *tokens[]={ "var", "end", "scope", "upscope",
		 "comment", "date", "dumpall", "dumpoff", "dumpon",
		 "dumpvars", "enddefinitions",
		 "dumpports", "dumpportsoff", "dumpportson", "dumpportsall",
		 "timescale", "version", "vcdclose",
		 "", "", "" };

#define NUM_TOKENS 18

static int T_MAX_STR=1024;	/* was originally a const..now it reallocs */
static char *yytext=NULL;
static int yylen=0, yylen_cache=0;

#define T_GET tok=get_token();if((tok==T_END)||(tok==T_EOF))break;

/******************************************************************/

static struct vcdsymbol *vcdsymroot=NULL, *vcdsymcurr=NULL;
static struct vcdsymbol **sorted=NULL;
static struct vcdsymbol **indexed=NULL;

static int numsyms=0;

/******************************************************************/

static unsigned int vcd_minid = ~0;
static unsigned int vcd_maxid = 0;

static unsigned int vcdid_hash(char *s, int len)
{  
unsigned int val=0;
int i;

s+=(len-1);
                 
for(i=0;i<len;i++)
        {
        val *= 95;				/* was 94 but XL uses '!' as right hand side chars which act as leading zeros */
        val += (((unsigned char)*s) - 32);	/* was 33 but XL ... */
        s--;
        }

return(val);
}

/******************************************************************/

/*
 * bsearch compare
 */
static int vcdsymbsearchcompare(const void *s1, const void *s2)
{
char *v1;
struct vcdsymbol *v2;

v1=(char *)s1;
v2=*((struct vcdsymbol **)s2);

return(strcmp(v1, v2->id));
}


/*
 * actual bsearch
 */
static struct vcdsymbol *bsearch_vcd(char *key, int len)
{
struct vcdsymbol **v;
struct vcdsymbol *t;

if(indexed)
        {
        unsigned int hsh = vcdid_hash(key, len);
        if((hsh>=vcd_minid)&&(hsh<=vcd_maxid))
                {
                return(indexed[hsh-vcd_minid]);
                }

	return(NULL);
        }

if(sorted)
	{
	v=(struct vcdsymbol **)bsearch(key, sorted, numsyms, 
		sizeof(struct vcdsymbol *), vcdsymbsearchcompare);

	if(v)
		{
		#ifndef VCD_BSEARCH_IS_PERFECT
			for(;;)
				{
				t=*v;
		
				if((v==sorted)||(strcmp((*(--v))->id, key)))
					{
					return(t);
					}
				}
		#else
			return(*v);
		#endif
		}
		else
		{
		return(NULL);
		}
	}
	else
	{
	static int err = 0;
	if(!err)
		{
		fprintf(stderr, "Near byte %d, VCD search table NULL..is this a VCD file?\n", (int)(vcdbyteno+(vst-vcdbuf)));
		err=1;
		}
	return(NULL);
	}
}


/*
 * sort on vcdsymbol pointers
 */
static int vcdsymcompare(const void *s1, const void *s2)
{
struct vcdsymbol *v1, *v2;

v1=*((struct vcdsymbol **)s1);
v2=*((struct vcdsymbol **)s2);

return(strcmp(v1->id, v2->id));
}


/*
 * create sorted (by id) table
 */
static void create_sorted_table(void)
{
struct vcdsymbol *v;
struct vcdsymbol **pnt;
unsigned int vcd_distance;

if(sorted) 
	{
	free_2(sorted);	/* this means we saw a 2nd enddefinition chunk! */
	sorted=NULL;
	}

if(indexed)
	{
	free_2(indexed);
	indexed=NULL;
	}

if(numsyms)
	{
        vcd_distance = vcd_maxid - vcd_minid + 1;

        if(vcd_distance <= VCD_INDEXSIZ)
                {
                indexed = (struct vcdsymbol **)calloc_2(vcd_distance, sizeof(struct vcdsymbol *));
         
		/* printf("%d symbols span ID range of %d, using indexing...\n", numsyms, vcd_distance); */

                v=vcdsymroot;
                while(v)
                        {
                        if(!indexed[v->nid - vcd_minid]) indexed[v->nid - vcd_minid] = v;
                        v=v->next;
                        }
                }
                else
		{	
		pnt=sorted=(struct vcdsymbol **)calloc_2(numsyms, sizeof(struct vcdsymbol *));
		v=vcdsymroot;
		while(v)
			{
			*(pnt++)=v;
			v=v->next;
			}
	
		qsort(sorted, numsyms, sizeof(struct vcdsymbol *), vcdsymcompare);
		}
	}
}

/******************************************************************/

static void vlist_emit_finalize(void)
{
struct vcdsymbol *v, *vprime;

v=vcdsymroot;
while(v)
	{
	nptr n = v->narray[0];

	if(n->mv.mvlfac_vlist) 
		{
		vlist_freeze(&n->mv.mvlfac_vlist);
		}
		else
		{
		n->mv.mvlfac_vlist = vlist_create(sizeof(char), 0);

		if((vprime=bsearch_vcd(v->id, strlen(v->id)))==v) /* hash mish means dup net */
			{
			switch(v->vartype)
				{
				case V_REAL:
		                          	vlist_emit_uv32(&n->mv.mvlfac_vlist, 'R');
		                                vlist_emit_uv32(&n->mv.mvlfac_vlist, (unsigned int)v->vartype);
		                                vlist_emit_uv32(&n->mv.mvlfac_vlist, (unsigned int)v->size);
						vlist_emit_uv32(&n->mv.mvlfac_vlist, 0);
						vlist_emit_string(&n->mv.mvlfac_vlist, "NaN");
						break;

				case V_STRING:
						vlist_emit_uv32(&n->mv.mvlfac_vlist, 'S');
		                                vlist_emit_uv32(&n->mv.mvlfac_vlist, (unsigned int)v->vartype);
		                                vlist_emit_uv32(&n->mv.mvlfac_vlist, (unsigned int)v->size);
						vlist_emit_uv32(&n->mv.mvlfac_vlist, 0);
						vlist_emit_string(&n->mv.mvlfac_vlist, "UNDEF");
						break;
	
				default:
					if(v->size==1)
						{
	                                        vlist_emit_uv32(&n->mv.mvlfac_vlist, (unsigned int)'0');
	                                        vlist_emit_uv32(&n->mv.mvlfac_vlist, (unsigned int)v->vartype);
						vlist_emit_uv32(&n->mv.mvlfac_vlist, RCV_X);
						}
						else
						{
		                                vlist_emit_uv32(&n->mv.mvlfac_vlist, 'B');
		                                vlist_emit_uv32(&n->mv.mvlfac_vlist, (unsigned int)v->vartype);
		                                vlist_emit_uv32(&n->mv.mvlfac_vlist, (unsigned int)v->size);
						vlist_emit_uv32(&n->mv.mvlfac_vlist, 0);
						vlist_emit_mvl9_string(&n->mv.mvlfac_vlist, "x");
						}
					break;
				}
			}
		}
	v=v->next;
	}
}

/******************************************************************/

/*
 * single char get inlined/optimized
 */
static void getch_alloc(void)
{
vend=vst=vcdbuf=(char *)calloc_2(1,VCD_BSIZ);
}

static void getch_free(void)
{
free_2(vcdbuf);
vcdbuf=vst=vend=NULL;
}


static off_t vcd_fsiz = 0;

static int getch_fetch(void)
{
size_t rd;

errno = 0;
if(feof(vcd_handle)) return(-1);

vcdbyteno+=(vend-vcdbuf);
rd=fread(vcdbuf, sizeof(char), VCD_BSIZ, vcd_handle);
vend=(vst=vcdbuf)+rd;

if((!rd)||(errno)) return(-1);

if(vcd_fsiz)
	{
	splash_sync(vcdbyteno, vcd_fsiz); /* gnome 2.18 seems to set errno so splash moved here... */
	}

return((int)(*(vst++)));
}

#define getch() ((vst!=vend)?((int)(*(vst++))):(getch_fetch()))


static char *varsplit=NULL, *vsplitcurr=NULL;
static int getch_patched(void)
{
char ch;

ch=*vsplitcurr;
if(!ch)
	{
	return(-1);
	}
	else
	{
	vsplitcurr++;
	return((int)ch);
	}
}

/*
 * simple tokenizer
 */
static int get_token(void)
{
int ch;
int i, len=0;
int is_string=0;
char *yyshadow;

for(;;)
	{
	ch=getch();
	if(ch<0) return(T_EOF);
	if(ch<=' ') continue;	/* val<=' ' is a quick whitespace check      */
	break;			/* (take advantage of fact that vcd is text) */
	}
if(ch=='$') 
	{
	yytext[len++]=ch;
	for(;;)
		{
		ch=getch();
		if(ch<0) return(T_EOF);
		if(ch<=' ') continue;
		break;
		}
	}
	else
	{
	is_string=1;
	}

for(yytext[len++]=ch;;yytext[len++]=ch)
	{
	if(len==T_MAX_STR)
		{
		yytext=(char *)realloc_2(yytext, (T_MAX_STR=T_MAX_STR*2)+1);
		}
	ch=getch();
	if(ch<=' ') break;
	}
yytext[len]=0;	/* terminator */

if(is_string) 
	{
	yylen=len;
	return(T_STRING);
	}

yyshadow=yytext;
do
{
yyshadow++;
for(i=0;i<NUM_TOKENS;i++)
	{
	if(!strcmp(yyshadow,tokens[i]))
		{
		return(i);
		}
	}

} while(*yyshadow=='$'); /* fix for RCS ids in version strings */

return(T_UNKNOWN_KEY);
}


static int var_prevch=0;
static int get_vartoken_patched(int match_kw)
{
int ch;
int len=0;

if(!var_prevch)
	{
	for(;;)
		{
		ch=getch_patched();
		if(ch<0) { free_2(varsplit); varsplit=NULL; return(V_END); }
		if((ch==' ')||(ch=='\t')||(ch=='\n')||(ch=='\r')) continue;
		break;
		}
	}
	else
	{
	ch=var_prevch;
	var_prevch=0;
	}
	
if(ch=='[') return(V_LB);
if(ch==':') return(V_COLON);
if(ch==']') return(V_RB);

for(yytext[len++]=ch;;yytext[len++]=ch)
	{
	if(len==T_MAX_STR)
		{
		yytext=(char *)realloc_2(yytext, (T_MAX_STR=T_MAX_STR*2)+1);
		}
	ch=getch_patched();
	if(ch<0) { free_2(varsplit); varsplit=NULL; break; }
	if((ch==':')||(ch==']'))
		{
		var_prevch=ch;
		break;
		}
	}
yytext[len]=0;	/* terminator */

if(match_kw)
	{
	int vr = vcd_keyword_code(yytext, len);
	if(vr != V_STRING)
		{
		if(ch<0) { free_2(varsplit); varsplit=NULL; }
		return(vr);
		}
	}

yylen=len;
if(ch<0) { free_2(varsplit); varsplit=NULL; }
return(V_STRING);
}

static int get_vartoken(int match_kw)
{
int ch;
int len=0;

if(varsplit)
	{
	int rc=get_vartoken_patched(match_kw);
	if(rc!=V_END) return(rc);
	var_prevch=0;
	}

if(!var_prevch)
	{
	for(;;)
		{
		ch=getch();
		if(ch<0) return(V_END);
		if((ch==' ')||(ch=='\t')||(ch=='\n')||(ch=='\r')) continue;
		break;
		}
	}
	else
	{
	ch=var_prevch;
	var_prevch=0;
	}

if(ch=='[') return(V_LB);
if(ch==':') return(V_COLON);
if(ch==']') return(V_RB);

if(ch=='#')	/* for MTI System Verilog '$var reg 64 >w #implicit-var###VarElem:ram_di[0.0] [63:0] $end' style declarations */
	{	/* debussy simply escapes until the space */
	yytext[len++]= '\\';
	}

for(yytext[len++]=ch;;yytext[len++]=ch)
	{
	if(len==T_MAX_STR)
		{
		yytext=(char *)realloc_2(yytext, (T_MAX_STR=T_MAX_STR*2)+1);
		}
	ch=getch();
	if((ch==' ')||(ch=='\t')||(ch=='\n')||(ch=='\r')||(ch<0)) break;
	if((ch=='[')&&(yytext[0]!='\\'))
		{
		varsplit=yytext+len;		/* keep looping so we get the *last* one */
		}
	else
	if(((ch==':')||(ch==']'))&&(!varsplit)&&(yytext[0]!='\\'))
		{
		var_prevch=ch;
		break;
		}
	}
yytext[len]=0;	/* absolute terminator */
if((varsplit)&&(yytext[len-1]==']'))
	{
	char *vst;
	vst=malloc_2(strlen(varsplit)+1);
	strcpy(vst, varsplit);

	*varsplit=0x00;		/* zero out var name at the left bracket */
	len=varsplit-yytext;

	varsplit=vsplitcurr=vst;
	var_prevch=0;
	}
	else
	{
	varsplit=NULL;
	}

if(match_kw)
	{
        int vr = vcd_keyword_code(yytext, len);
        if(vr != V_STRING)
		{
		return(vr);
		}
	}

yylen=len;
return(V_STRING);
}

static int get_strtoken(void)
{
int ch;
int len=0;

if(!var_prevch)
      {
      for(;;)
              {
              ch=getch();
              if(ch<0) return(V_END);
              if((ch==' ')||(ch=='\t')||(ch=='\n')||(ch=='\r')) continue;
              break;
              }
      }
      else
      {
      ch=var_prevch;
      var_prevch=0;
      }
      
for(yytext[len++]=ch;;yytext[len++]=ch)
      {
	if(len==T_MAX_STR)
		{
		yytext=(char *)realloc_2(yytext, (T_MAX_STR=T_MAX_STR*2)+1);
		}
      ch=getch();
      if((ch==' ')||(ch=='\t')||(ch=='\n')||(ch=='\r')||(ch<0)) break;
      }
yytext[len]=0;        /* terminator */

yylen=len;
return(V_STRING);
}

static void sync_end(char *hdr)
{
int tok;

if(hdr) DEBUG(fprintf(stderr,"%s",hdr));
for(;;)
	{
	tok=get_token();
	if((tok==T_END)||(tok==T_EOF)) break;
	if(hdr)DEBUG(fprintf(stderr," %s",yytext));
	}
if(hdr) DEBUG(fprintf(stderr,"\n"));
}

static void parse_valuechange(void)
{
struct vcdsymbol *v;
char *vector;
int vlen;
unsigned char typ;

switch((typ = yytext[0]))
	{
	/* encode bits as (time delta<<4) + (enum AnalyzerBits value) */
        case '0':
        case '1':
        case 'x': case 'X':
        case 'z': case 'Z':
        case 'h': case 'H':
        case 'u': case 'U':
        case 'w': case 'W':
        case 'l': case 'L':
        case '-':
                if(yylen>1)
                        {
                        v=bsearch_vcd(yytext+1, yylen-1);
                        if(!v)
                                {
                                fprintf(stderr,"Near byte %d, Unknown VCD identifier: '%s'\n",(int)(vcdbyteno+(vst-vcdbuf)),yytext+1);
                                }
                                else
                                {
				nptr n = v->narray[0];
				unsigned int time_delta;
				unsigned int rcv;

				if(!n->mv.mvlfac_vlist) /* overloaded for vlist, numhist = last position used */
					{
					n->mv.mvlfac_vlist = vlist_create(sizeof(char), 0);
					vlist_emit_uv32(&n->mv.mvlfac_vlist, (unsigned int)'0'); /* represents single bit routine for decompression */
					vlist_emit_uv32(&n->mv.mvlfac_vlist, (unsigned int)v->vartype);
					}

				time_delta = time_vlist_count - (unsigned int)n->numhist;
				n->numhist = time_vlist_count;				

				switch(yytext[0])
				        {
				        case '0':		
				        case '1':		rcv = ((yytext[0]&1)<<1) | (time_delta<<2);
								break; /* pack more delta bits in for 0/1 vchs */

				        case 'x': case 'X':	rcv = RCV_X | (time_delta<<4); break;
				        case 'z': case 'Z':	rcv = RCV_Z | (time_delta<<4); break;
				        case 'h': case 'H':	rcv = RCV_H | (time_delta<<4); break;
				        case 'u': case 'U':	rcv = RCV_U | (time_delta<<4); break;
				        case 'w': case 'W':     rcv = RCV_W | (time_delta<<4); break;
				        case 'l': case 'L':	rcv = RCV_L | (time_delta<<4); break;
					default:		rcv = RCV_D | (time_delta<<4); break;
					}

				vlist_emit_uv32(&n->mv.mvlfac_vlist, rcv);
                                }
                        }
                        else
                        {
                        fprintf(stderr,"Near byte %d, Malformed VCD identifier\n", (int)(vcdbyteno+(vst-vcdbuf)));
                        }
                break;

	/* encode everything else literally as a time delta + a string */
        case 'b':
        case 'B':
        case 'r':
        case 'R':
#ifndef STRICT_VCD_ONLY         
        case 's':
        case 'S':
#endif                          
                vector=wave_alloca(yylen_cache=yylen);
                strcpy(vector,yytext+1); 
                vlen=yylen-1;
                                         
                get_strtoken();
process_binary:
                v=bsearch_vcd(yytext, yylen);
                if(!v)
			{
                        fprintf(stderr,"Near byte %d, Unknown VCD identifier: '%s'\n",(int)(vcdbyteno+(vst-vcdbuf)),yytext+1);
                        }
                        else
                        {
			nptr n = v->narray[0];
			unsigned int time_delta;

			if(!n->mv.mvlfac_vlist) /* overloaded for vlist, numhist = last position used */
				{
				unsigned char typ2 = toupper(typ);
				n->mv.mvlfac_vlist = vlist_create(sizeof(char), 0);

				if(v->vartype!=V_REAL) 
					{
					/* ok, typical case */
					}
					else
					{
					if(typ2=='B')
						{
						typ2 = 'S';	/* should never be necessary...this is defensive */
						}
					}

				vlist_emit_uv32(&n->mv.mvlfac_vlist, (unsigned int)toupper(typ2)); /* B/R/P/S for decompress */
				vlist_emit_uv32(&n->mv.mvlfac_vlist, (unsigned int)v->vartype);
				vlist_emit_uv32(&n->mv.mvlfac_vlist, (unsigned int)v->size);
				}

			time_delta = time_vlist_count - (unsigned int)n->numhist;
			n->numhist = time_vlist_count;				

			vlist_emit_uv32(&n->mv.mvlfac_vlist, time_delta);

			if((typ=='b')||(typ=='B'))
				{
				if(v->vartype!=V_REAL)
					{
					vlist_emit_mvl9_string(&n->mv.mvlfac_vlist, vector);
					}
					else
					{
					vlist_emit_string(&n->mv.mvlfac_vlist, vector);
					}
				}
				else
				{
				if(v->vartype == V_REAL)
					{
					vlist_emit_string(&n->mv.mvlfac_vlist, vector);
					}
					else
					{
					char *bits = wave_alloca(v->size + 1);
					int i, j, k=0;

					memset(bits, 0x0, v->size + 1);

					for(i=0;i<vlen;i++)
						{
						for(j=0;j<8;j++)
							{
							bits[k++] = ((vector[i] >> (7-j)) & 1) | '0';
							if(k >= v->size) goto bit_term;
							}
						}					

					bit_term:
					vlist_emit_mvl9_string(&n->mv.mvlfac_vlist, bits);
					}
				}
                        }
		break;

	case 'p':
	case 'P':
		/* extract port dump value.. */
		vector=wave_alloca(yylen_cache=yylen); 
		evcd_strcpy(vector,yytext+1);	/* convert to regular vcd */
		vlen=yylen-1;

		get_strtoken();	/* throw away 0_strength_component */
		get_strtoken(); /* throw away 0_strength_component */
		get_strtoken(); /* this is the id                  */

		typ = 'b';			/* convert to regular vcd */
		goto process_binary; /* store string literally */

	default:
		break;
	}
}


static void evcd_strcpy(char *dst, char *src)
{
static char *evcd="DUNZduLHXTlh01?FAaBbCcf";
static char  *vcd="01xz0101xz0101xzxxxxxxx";

char ch;
int i;

while((ch=*src))
	{
	for(i=0;i<23;i++)
		{
		if(evcd[i]==ch)
			{
			*dst=vcd[i];
			break;
			}
		}	
	if(i==23) *dst='x';

	src++;
	dst++;
	}

*dst=0;	/* null terminate destination */
}


static void vcd_parse(void)
{
int tok;

for(;;)
	{
	switch(tok=get_token())
		{
		case T_COMMENT:
			sync_end("COMMENT:");
			break;
		case T_DATE:
			sync_end("DATE:");
			break;
		case T_VERSION:
			sync_end("VERSION:");
			break;
		case T_TIMESCALE:
			{
			int vtok;
			int i;
			char prefix=' ';

			vtok=get_token();
			if((vtok==T_END)||(vtok==T_EOF)) break;
			time_scale=atoi_64(yytext);
			if(!time_scale) time_scale=1;
			for(i=0;i<yylen;i++)
				{
				if((yytext[i]<'0')||(yytext[i]>'9'))
					{
					prefix=yytext[i];
					break;
					}
				}
			if(prefix==' ')
				{
				vtok=get_token();
				if((vtok==T_END)||(vtok==T_EOF)) break;
				prefix=yytext[0];		
				}
			switch(prefix)
				{
				case ' ':
				case 'm':
				case 'u':
				case 'n':
				case 'p':
				case 'f':
					time_dimension=prefix;
					break;
				case 's':
					time_dimension=' ';
					break;
				default:	/* unknown */
					time_dimension='n';
					break;
				}

			DEBUG(fprintf(stderr,"TIMESCALE: "TTFormat" %cs\n",time_scale, time_dimension));
			sync_end(NULL);
			}
			break;
		case T_SCOPE:
			T_GET;
			T_GET;
			if(tok==T_STRING)
				{
				struct slist *s;
				s=(struct slist *)calloc_2(1,sizeof(struct slist));
				s->len=yylen;
				s->str=(char *)malloc_2(yylen+1);
				strcpy(s->str, yytext);

				if(slistcurr)
					{
					slistcurr->next=s;
					slistcurr=s;
					}
					else
					{
					slistcurr=slistroot=s;
					}

				build_slisthier();
				DEBUG(fprintf(stderr, "SCOPE: %s\n",slisthier));
				}
			sync_end(NULL);
			break;
		case T_UPSCOPE:
			if(slistroot)
				{
				struct slist *s;

				s=slistroot;
				if(!s->next)
					{
					free_2(s->str);
					free_2(s);
					slistroot=slistcurr=NULL;
					}
				else
				for(;;)
					{
					if(!s->next->next)
						{
						free_2(s->next->str);
						free_2(s->next);
						s->next=NULL;
						slistcurr=s;
						break;
						}
					s=s->next;
					}
				build_slisthier();
				DEBUG(fprintf(stderr, "SCOPE: %s\n",slisthier));
				}
			sync_end(NULL);
			break;
		case T_VAR:
			if((header_over)&&(0))
			{
			fprintf(stderr,"$VAR encountered after $ENDDEFINITIONS near byte %d.  VCD is malformed, exiting.\n",
				(int)(vcdbyteno+(vst-vcdbuf)));
			exit(0);
			}
			else
			{
			int vtok;
			struct vcdsymbol *v=NULL;

			var_prevch=0;
                        if(varsplit)
                                {
                                free_2(varsplit);
                                varsplit=NULL;
                                }
			vtok=get_vartoken(1);
			if(vtok>V_PORT) goto bail;

			v=(struct vcdsymbol *)calloc_2(1,sizeof(struct vcdsymbol));
			v->vartype=vtok;
			v->msi=v->lsi=vcd_explicit_zero_subscripts; /* indicate [un]subscripted status */

			if(vtok==V_PORT)
				{
				vtok=get_vartoken(0);
				if(vtok==V_STRING)
					{
					v->size=atoi_64(yytext);
					if(!v->size) v->size=1;
					}
					else 
					if(vtok==V_LB)
					{
					vtok=get_vartoken(1);
					if(vtok==V_END) goto err;
					if(vtok!=V_STRING) goto err;
					v->msi=atoi_64(yytext);
					vtok=get_vartoken(0);
					if(vtok==V_RB)
						{
						v->lsi=v->msi;
						v->size=1;
						}
						else
						{
						if(vtok!=V_COLON) goto err;
						vtok=get_vartoken(0);
						if(vtok!=V_STRING) goto err;
						v->lsi=atoi_64(yytext);
						vtok=get_vartoken(0);
						if(vtok!=V_RB) goto err;

						if(v->msi>v->lsi)
							{
							v->size=v->msi-v->lsi+1;
							}
							else
							{
							v->size=v->lsi-v->msi+1;
							}
						}
					}
					else goto err;

				vtok=get_strtoken();
				if(vtok==V_END) goto err;
				v->id=(char *)malloc_2(yylen+1);
				strcpy(v->id, yytext);
                                v->nid=vcdid_hash(yytext,yylen);

                                if(v->nid < vcd_minid) vcd_minid = v->nid;
                                if(v->nid > vcd_maxid) vcd_maxid = v->nid;

				vtok=get_vartoken(0);
				if(vtok!=V_STRING) goto err;
				if(slisthier_len)
					{
					v->name=(char *)malloc_2(slisthier_len+1+yylen+1);
					strcpy(v->name,slisthier);
					strcpy(v->name+slisthier_len,vcd_hier_delimeter);
					if(alt_hier_delimeter)
						{
						strcpy_vcdalt(v->name+slisthier_len+1,yytext,alt_hier_delimeter);
						}
						else
						{
						if((strcpy_delimfix(v->name+slisthier_len+1,yytext)) && (yytext[0] != '\\'))
							{
							char *sd=(char *)malloc_2(slisthier_len+1+yylen+2);
							strcpy(sd,slisthier);
							strcpy(sd+slisthier_len,vcd_hier_delimeter);
							sd[slisthier_len+1] = '\\';
							strcpy(sd+slisthier_len+2,v->name+slisthier_len+1);
							free_2(v->name);
							v->name = sd;
							}
						}
					}
					else
					{
					v->name=(char *)malloc_2(yylen+1);
					if(alt_hier_delimeter)
						{
						strcpy_vcdalt(v->name,yytext,alt_hier_delimeter);
						}
						else
						{
						if((strcpy_delimfix(v->name,yytext)) && (yytext[0] != '\\'))
							{
							char *sd=(char *)malloc_2(yylen+2);
							sd[0] = '\\';
							strcpy(sd+1,v->name);
							free_2(v->name);
							v->name = sd;
							}
						}
					}

                                if(pv)
                                        { 
                                        if(!strcmp(pv->name,v->name))
                                                {
                                                pv->chain=v;
                                                v->root=rootv;
                                                if(pv==rootv) pv->root=rootv;
                                                }
                                                else
                                                {
                                                rootv=v;
                                                }
                                        }
					else
					{
					rootv=v;
					}
                                pv=v;
				}
				else	/* regular vcd var, not an evcd port var */
				{
				vtok=get_vartoken(1);
				if(vtok==V_END) goto err;
				v->size=atoi_64(yytext);
				vtok=get_strtoken();
				if(vtok==V_END) goto err;
				v->id=(char *)malloc_2(yylen+1);
				strcpy(v->id, yytext);
                                v->nid=vcdid_hash(yytext,yylen);
                                
                                if(v->nid < vcd_minid) vcd_minid = v->nid;
                                if(v->nid > vcd_maxid) vcd_maxid = v->nid;

				vtok=get_vartoken(0);
				if(vtok!=V_STRING) goto err;

				if(slisthier_len)
					{
					v->name=(char *)malloc_2(slisthier_len+1+yylen+1);
					strcpy(v->name,slisthier);
					strcpy(v->name+slisthier_len,vcd_hier_delimeter);
					if(alt_hier_delimeter)
						{
						strcpy_vcdalt(v->name+slisthier_len+1,yytext,alt_hier_delimeter);
						}
						else
						{
						if((strcpy_delimfix(v->name+slisthier_len+1,yytext)) && (yytext[0] != '\\'))
							{
                                                        char *sd=(char *)malloc_2(slisthier_len+1+yylen+2);
                                                        strcpy(sd,slisthier);
                                                        strcpy(sd+slisthier_len,vcd_hier_delimeter);
                                                        sd[slisthier_len+1] = '\\';
                                                        strcpy(sd+slisthier_len+2,v->name+slisthier_len+1);
                                                        free_2(v->name);
                                                        v->name = sd;
							}
						}
					}
					else
					{
					v->name=(char *)malloc_2(yylen+1);
					if(alt_hier_delimeter)
						{
						strcpy_vcdalt(v->name,yytext,alt_hier_delimeter);
						}
						else
						{
                                                if((strcpy_delimfix(v->name,yytext)) && (yytext[0] != '\\'))
                                                        {
                                                        char *sd=(char *)malloc_2(yylen+2);
                                                        sd[0] = '\\';
                                                        strcpy(sd+1,v->name);
                                                        free_2(v->name);
                                                        v->name = sd;
                                                        }
						}
					}

                                if(pv)
                                        { 
                                        if(!strcmp(pv->name,v->name))
                                                {
                                                pv->chain=v;
                                                v->root=rootv;
                                                if(pv==rootv) pv->root=rootv;
                                                }
                                                else
                                                {
                                                rootv=v;
                                                }
                                        }
					else
					{
					rootv=v;
					}
                                pv=v;
				
				vtok=get_vartoken(1);
				if(vtok==V_END) goto dumpv;
				if(vtok!=V_LB) goto err;
				vtok=get_vartoken(0);
				if(vtok!=V_STRING) goto err;
				v->msi=atoi_64(yytext);
				vtok=get_vartoken(0);
				if(vtok==V_RB)
					{
					v->lsi=v->msi;
					goto dumpv;
					}
				if(vtok!=V_COLON) goto err;
				vtok=get_vartoken(0);
				if(vtok!=V_STRING) goto err;
				v->lsi=atoi_64(yytext);
				vtok=get_vartoken(0);
				if(vtok!=V_RB) goto err;
				}

			dumpv:
			if(v->size == 0) { v->vartype = V_REAL; } /* MTI fix */

			if(v->vartype==V_REAL)
				{
				v->vartype=V_REAL;
				v->size=1;		/* override any data we parsed in */
				v->msi=v->lsi=0;
				}
			else
			if((v->size>1)&&(v->msi<=0)&&(v->lsi<=0))
				{
				if(v->vartype==V_EVENT) 
					{
					v->size=1;
					}
					else
					{
					/* any criteria for the direction here? */
					v->msi=v->size-1;	
					v->lsi=0;
					}
				}
			else
			if((v->msi>v->lsi)&&((v->msi-v->lsi+1)!=v->size))
				{
				if((v->vartype!=V_EVENT)&&(v->vartype!=V_PARAMETER)) goto err;
				v->size=v->msi-v->lsi+1;
				}
			else
			if((v->lsi>=v->msi)&&((v->lsi-v->msi+1)!=v->size)) 
				{
				if((v->vartype!=V_EVENT)&&(v->vartype!=V_PARAMETER)) goto err;
				v->size=v->msi-v->lsi+1;
				}

			/* initial conditions */
			v->narray=(struct Node **)calloc_2(1,sizeof(struct Node *));
			v->narray[0]=(struct Node *)calloc_2(1,sizeof(struct Node));
			v->narray[0]->head.time=-1;
			v->narray[0]->head.v.h_val=AN_X;

			if(!vcdsymroot)
				{
				vcdsymroot=vcdsymcurr=v;
				}
				else
				{
				vcdsymcurr->next=v;
				vcdsymcurr=v;
				}
			numsyms++;

			if(vcd_save_handle)
				{
				if(v->msi==v->lsi)
					{
					if(v->vartype==V_REAL)
						{
						fprintf(vcd_save_handle,"%s\n",v->name);
						}
						else
						{
						if(v->msi>=0)
							{
							if(!vcd_explicit_zero_subscripts)
								fprintf(vcd_save_handle,"%s%c%d\n",v->name,hier_delimeter,v->msi);
								else
								fprintf(vcd_save_handle,"%s[%d]\n",v->name,v->msi);
							}
							else
							{
							fprintf(vcd_save_handle,"%s\n",v->name);
							}
						}
					}
					else
					{
					fprintf(vcd_save_handle,"%s[%d:%d]\n",v->name,v->msi,v->lsi);
					}
				}

			DEBUG(fprintf(stderr,"VAR %s %d %s %s[%d:%d]\n",
				vartypes[v->vartype], v->size, v->id, v->name, 
					v->msi, v->lsi));
			goto bail;
			err:
			if(v)
				{
				error_count++;
				if(v->name) 
					{
					fprintf(stderr, "Near byte %d, $VAR parse error encountered with '%s'\n", (int)(vcdbyteno+(vst-vcdbuf)), v->name);
					free_2(v->name);
					}
					else
					{
					fprintf(stderr, "Near byte %d, $VAR parse error encountered\n", (int)(vcdbyteno+(vst-vcdbuf)));
					}
				if(v->id) free_2(v->id);
				free_2(v);
				pv = NULL;
				}

			bail:
			if(vtok!=V_END) sync_end(NULL);
			break;
			}
		case T_ENDDEFINITIONS:
			header_over=1;	/* do symbol table management here */
			create_sorted_table();
			if((!sorted)&&(!indexed))
				{
				fprintf(stderr, "No symbols in VCD file..nothing to do!\n");
				exit(1);
				}
			if(error_count)
				{
				fprintf(stderr, "\n%d VCD parse errors encountered, exiting.\n", error_count);
				exit(1);
				}
			break;
		case T_STRING:
			if(!header_over)
				{
				header_over=1;	/* do symbol table management here */
				create_sorted_table();
				if((!sorted)&&(!indexed)) break;
				}
				{
				/* catchall for events when header over */
				if(yytext[0]=='#')
					{
					TimeType time;
					TimeType *tt;

					time=atoi_64(yytext+1);
					
					if(start_time<0)
						{
						start_time=time;
						}

					current_time=time;
					if(end_time<time) end_time=time;	/* in case of malformed vcd files */
					DEBUG(fprintf(stderr,"#"TTFormat"\n",time));

					tt = vlist_alloc(&time_vlist, 0);
					*tt = time;
					time_vlist_count++;
					}
					else
					{
					if(time_vlist_count)
						{
						/* OK, otherwise fix for System C which doesn't emit time zero... */
						}
						else
						{
						TimeType time = LLDescriptor(0);
						TimeType *tt;

						start_time=current_time=end_time=time;

						tt = vlist_alloc(&time_vlist, 0);
						*tt = time;
						time_vlist_count=1;
						}
					parse_valuechange();
					}
				}
			break;
		case T_DUMPALL:	/* dump commands modify vals anyway so */
		case T_DUMPPORTSALL:
			break;	/* just loop through..                 */
		case T_DUMPOFF:
		case T_DUMPPORTSOFF:
			dumping_off=1;
			if((!blackout_regions)||((blackout_regions)&&(blackout_regions->bstart<=blackout_regions->bend)))
				{
				struct blackout_region_t *bt = calloc_2(1, sizeof(struct blackout_region_t));

				bt->bstart = current_time;
				bt->next = blackout_regions;
				blackout_regions = bt;
				}
			break;
		case T_DUMPON:
		case T_DUMPPORTSON:
			dumping_off=0;
			if((blackout_regions)&&(blackout_regions->bstart>blackout_regions->bend))
				{
				blackout_regions->bend = current_time;
				}
			break;
		case T_DUMPVARS:
		case T_DUMPPORTS:
			if(current_time<0)
				{ start_time=current_time=end_time=0; }
			break;
		case T_VCDCLOSE:
			break;	/* next token will be '#' time related followed by $end */
		case T_END:	/* either closure for dump commands or */
			break;	/* it's spurious                       */
		case T_UNKNOWN_KEY:
			sync_end(NULL);	/* skip over unknown keywords */
			break;
		case T_EOF:
			if((blackout_regions)&&(blackout_regions->bstart>blackout_regions->bend))
				{
				blackout_regions->bend = current_time;
				}
			return;
		default:
			DEBUG(fprintf(stderr,"UNKNOWN TOKEN\n"));
		}
	}
}


/*******************************************************************************/

void add_histent(TimeType time, struct Node *n, char ch, int regadd, char *vector)
{
struct HistEnt *he;
char heval;

if(!vector)
{
if(!n->curr)
	{
	he=histent_calloc();
        he->time=-1;
        he->v.h_val=AN_X;

	n->curr=he;
	n->head.next=he;

	add_histent(time,n,ch,regadd, vector);
	}
	else
	{
	if(regadd) { time*=(time_scale); }

	if(ch=='0')              heval=AN_0; else
	if(ch=='1')              heval=AN_1; else
        if((ch=='x')||(ch=='X')) heval=AN_X; else
        if((ch=='z')||(ch=='Z')) heval=AN_Z; else
        if((ch=='h')||(ch=='H')) heval=AN_H; else
        if((ch=='u')||(ch=='U')) heval=AN_U; else
        if((ch=='w')||(ch=='W')) heval=AN_W; else
        if((ch=='l')||(ch=='L')) heval=AN_L; else
        /* if(ch=='-') */        heval=AN_DASH;		/* default */
	
	if((n->curr->v.h_val!=heval)||(time==start_time)||(vcd_preserve_glitches)) /* same region == go skip */ 
        	{
		if(n->curr->time==time)
			{
			DEBUG(printf("Warning: Glitch at time ["TTFormat"] Signal [%p], Value [%c->%c].\n",
				time, n, AN_STR[n->curr->v.h_val], ch));
			n->curr->v.h_val=heval;		/* we have a glitch! */

			num_glitches++;
			if(!(n->curr->flags&HIST_GLITCH))
				{
				n->curr->flags|=HIST_GLITCH;	/* set the glitch flag */
				num_glitch_regions++;
				}
			}
			else
			{
                	he=histent_calloc();
                	he->time=time;
                	he->v.h_val=heval;

                	n->curr->next=he;
			n->curr=he;
                	regions+=regadd;
			}
                }
       }
}
else
{
switch(ch)
	{
	case 's': /* string */
	{
	if(!n->curr)
		{
		he=histent_calloc();
		he->flags=(HIST_STRING|HIST_REAL);
	        he->time=-1;
	        he->v.h_vector=NULL;
	
		n->curr=he;
		n->head.next=he;
	
		add_histent(time,n,ch,regadd, vector);
		}
		else
		{
		if(regadd) { time*=(time_scale); }
	
			if(n->curr->time==time)
				{
				DEBUG(printf("Warning: String Glitch at time ["TTFormat"] Signal [%p].\n",
					time, n));
				if(n->curr->v.h_vector) free_2(n->curr->v.h_vector);
				n->curr->v.h_vector=vector;		/* we have a glitch! */
	
				num_glitches++;
				if(!(n->curr->flags&HIST_GLITCH))
					{
					n->curr->flags|=HIST_GLITCH;	/* set the glitch flag */
					num_glitch_regions++;
					}
				}
				else
				{
	                	he=histent_calloc();
				he->flags=(HIST_STRING|HIST_REAL);
	                	he->time=time;
	                	he->v.h_vector=vector;
	
	                	n->curr->next=he;
				n->curr=he;
	                	regions+=regadd;
				}
	       }
	break;
	}
	case 'g': /* real number */
	{
	if(!n->curr)
		{
		he=histent_calloc();
		he->flags=HIST_REAL;
	        he->time=-1;
	        he->v.h_vector=NULL;
	
		n->curr=he;
		n->head.next=he;
	
		add_histent(time,n,ch,regadd, vector);
		}
		else
		{
		if(regadd) { time*=(time_scale); }
	
		if(
		  (n->curr->v.h_vector&&vector&&(*(double *)n->curr->v.h_vector!=*(double *)vector))
			||(time==start_time)
			||(!n->curr->v.h_vector)
			||(vcd_preserve_glitches)
			) /* same region == go skip */ 
	        	{
			if(n->curr->time==time)
				{
				DEBUG(printf("Warning: Real number Glitch at time ["TTFormat"] Signal [%p].\n",
					time, n));
				if(n->curr->v.h_vector) free_2(n->curr->v.h_vector);
				n->curr->v.h_vector=vector;		/* we have a glitch! */
	
				num_glitches++;
				if(!(n->curr->flags&HIST_GLITCH))
					{
					n->curr->flags|=HIST_GLITCH;	/* set the glitch flag */
					num_glitch_regions++;
					}
				}
				else
				{
	                	he=histent_calloc();
				he->flags=HIST_REAL;
	                	he->time=time;
	                	he->v.h_vector=vector;
	
	                	n->curr->next=he;
				n->curr=he;
	                	regions+=regadd;
				}
	                }
			else
			{
			free_2(vector);
			}
	       }
	break;
	}
	default:
	{
	if(!n->curr)
		{
		he=histent_calloc();
	        he->time=-1;
	        he->v.h_vector=NULL;
	
		n->curr=he;
		n->head.next=he;
	
		add_histent(time,n,ch,regadd, vector);
		}
		else
		{
		if(regadd) { time*=(time_scale); }
	
		if(
		  (n->curr->v.h_vector&&vector&&(strcmp(n->curr->v.h_vector,vector)))
			||(time==start_time)
			||(!n->curr->v.h_vector)
			||(vcd_preserve_glitches)
			) /* same region == go skip */ 
	        	{
			if(n->curr->time==time)
				{
				DEBUG(printf("Warning: Glitch at time ["TTFormat"] Signal [%p], Value [%c->%c].\n",
					time, n, AN_STR[n->curr->v.h_val], ch));
				if(n->curr->v.h_vector) free_2(n->curr->v.h_vector);
				n->curr->v.h_vector=vector;		/* we have a glitch! */
	
				num_glitches++;
				if(!(n->curr->flags&HIST_GLITCH))
					{
					n->curr->flags|=HIST_GLITCH;	/* set the glitch flag */
					num_glitch_regions++;
					}
				}
				else
				{
	                	he=histent_calloc();
	                	he->time=time;
	                	he->v.h_vector=vector;
	
	                	n->curr->next=he;
				n->curr=he;
	                	regions+=regadd;
				}
	                }
			else
			{
			free_2(vector);
			}
	       }
	break;
	}
	}
}

}

/*******************************************************************************/

static void vcd_build_symbols(void)
{
int j;
int max_slen=-1;
struct sym_chain *sym_chain=NULL, *sym_curr=NULL;
int duphier=0;
char hashdirty;
struct vcdsymbol *v, *vprime;
char *str = NULL;

v=vcdsymroot;
while(v)
	{
	int msi;
	int delta;

		{
		int slen;
		int substnode;

		msi=v->msi;
		delta=((v->lsi-v->msi)<0)?-1:1;
		substnode=0;

		slen=strlen(v->name);
		str=(slen>max_slen)?(wave_alloca((max_slen=slen)+32)):(str); /* more than enough */
		strcpy(str,v->name);

		if(v->msi>=0)
			{
			strcpy(str+slen,vcd_hier_delimeter);
			slen++;
			}

		if((vprime=bsearch_vcd(v->id, strlen(v->id)))!=v) /* hash mish means dup net */
			{
			if(v->size!=vprime->size)
				{
				fprintf(stderr,"ERROR: Duplicate IDs with differing width: %s %s\n", v->name, vprime->name);
				}
				else
				{
				substnode=1;
				}
			}

		if((v->size==1)&&(v->vartype!=V_REAL))
			{
			struct symbol *s = NULL;
	
			for(j=0;j<v->size;j++)
				{
				if(v->msi>=0) 
					{
					if(!vcd_explicit_zero_subscripts)
						sprintf(str+slen,"%d",msi);
						else
						sprintf(str+slen-1,"[%d]",msi);
					}

				hashdirty=0;
				if(symfind(str))
					{
					char *dupfix=(char *)malloc_2(max_slen+32);
					hashdirty=1;
					DEBUG(fprintf(stderr,"Warning: %s is a duplicate net name.\n",str));

					do sprintf(dupfix, "$DUP%d%s%s", duphier++, vcd_hier_delimeter, str);
						while(symfind(dupfix));

					strcpy(str, dupfix);
					free_2(dupfix);
					duphier=0; /* reset for next duplicate resolution */
					}
					/* fallthrough */
					{
					s=symadd(str,hashdirty?hash(str):hashcache);
	
					s->n=v->narray[j];
					if(substnode)
						{
						struct Node *n, *n2;
	
						n=s->n;
						n2=vprime->narray[j];
						/* nname stays same */
						/* n->head=n2->head; */
						/* n->curr=n2->curr; */
						n->curr=(hptr)n2;
						/* harray calculated later */
						n->numhist=n2->numhist;
						}
	
					s->n->nname=s->name;
					s->h=s->n->curr;
					if(!firstnode)
						{
						firstnode=curnode=s;
						}
						else
						{
						curnode->nextinaet=s;
						curnode=s;
						}
	
					numfacs++;
					DEBUG(fprintf(stderr,"Added: %s\n",str));
					}
				msi+=delta;
				}

			if((j==1)&&(v->root))
				{
				s->vec_root=(struct symbol *)v->root;		/* these will get patched over */
				s->vec_chain=(struct symbol *)v->chain;		/* these will get patched over */
				v->sym_chain=s;

				if(!sym_chain)
					{
					sym_curr=(struct sym_chain *)calloc_2(1,sizeof(struct sym_chain));
					sym_chain=sym_curr;
					}
					else
					{
					sym_curr->next=(struct sym_chain *)calloc_2(1,sizeof(struct sym_chain));
					sym_curr=sym_curr->next;
					}
				sym_curr->val=s;
				}
			}
			else	/* atomic vector */
			{
			if(v->vartype!=V_REAL)
				{
				sprintf(str+slen-1,"[%d:%d]",v->msi,v->lsi);
				}
				else
				{
				*(str+slen-1)=0;
				}


			hashdirty=0;
			if(symfind(str))
				{
				char *dupfix=(char *)malloc_2(max_slen+32);
				hashdirty=1;
				DEBUG(fprintf(stderr,"Warning: %s is a duplicate net name.\n",str));

				do sprintf(dupfix, "$DUP%d%s%s", duphier++, vcd_hier_delimeter, str);
					while(symfind(dupfix));

				strcpy(str, dupfix);
				free_2(dupfix);
				duphier=0; /* reset for next duplicate resolution */
				}
				/* fallthrough */
				{
				struct symbol *s;

				s=symadd(str,hashdirty?hash(str):hashcache);	/* cut down on double lookups.. */

				s->n=v->narray[0];
				if(substnode)
					{
					struct Node *n, *n2;

					n=s->n;
					n2=vprime->narray[0];
					/* nname stays same */
					/* n->head=n2->head; */
					/* n->curr=n2->curr; */
					n->curr=(hptr)n2;
					/* harray calculated later */
					n->numhist=n2->numhist;
					n->ext=n2->ext;
					}
					else
					{
					struct ExtNode *en;
					en=(struct ExtNode *)malloc_2(sizeof(struct ExtNode));
					en->msi=v->msi;
					en->lsi=v->lsi;

					s->n->ext=en;
					}

				s->n->nname=s->name;
				s->h=s->n->curr;
				if(!firstnode)
					{
					firstnode=curnode=s;
					}
					else
					{
					curnode->nextinaet=s;
					curnode=s;
					}

				numfacs++;
				DEBUG(fprintf(stderr,"Added: %s\n",str));
				}
			}
		}

	v=v->next;
	}

if(sym_chain)
	{
	sym_curr=sym_chain;	
	while(sym_curr)
		{
		sym_curr->val->vec_root= ((struct vcdsymbol *)sym_curr->val->vec_root)->sym_chain;

		if ((struct vcdsymbol *)sym_curr->val->vec_chain)
			sym_curr->val->vec_chain=((struct vcdsymbol *)sym_curr->val->vec_chain)->sym_chain;

		DEBUG(printf("Link: ('%s') '%s' -> '%s'\n",sym_curr->val->vec_root->name, sym_curr->val->name, sym_curr->val->vec_chain?sym_curr->val->vec_chain->name:"(END)"));

		sym_chain=sym_curr;
		sym_curr=sym_curr->next;
		free_2(sym_chain);
		}
	}
}

/*******************************************************************************/

static void vcd_cleanup(void)
{
struct slist *s, *s2;
struct vcdsymbol *v, *vt;

if(indexed)
	{
	free_2(indexed); indexed=NULL; 
	}

if(sorted)
	{
	free_2(sorted); sorted=NULL; 
	}

v=vcdsymroot;
while(v)
	{
	if(v->name) free_2(v->name);
	if(v->id) free_2(v->id);
	if(v->ev) free_2(v->ev);
	if(v->narray) free_2(v->narray);
	vt=v;
	v=v->next;
	free_2(vt);
	}
vcdsymroot=vcdsymcurr=NULL;

if(slisthier) { free_2(slisthier); slisthier=NULL; }
s=slistroot;
while(s)
	{
	s2=s->next;
	if(s->str)free_2(s->str);
	free_2(s);
	s=s2;
	}

slistroot=slistcurr=NULL; slisthier_len=0;

if(vcd_is_compressed)
	{
	pclose(vcd_handle);
	}
	else
	{
	fclose(vcd_handle);
	}

if(yytext)
	{
	free_2(yytext);
	yytext=NULL;
	}
}

/*******************************************************************************/

TimeType vcd_recoder_main(char *fname)
{
int flen;

pv=rootv=NULL;
vcd_hier_delimeter[0]=hier_delimeter;

errno=0;	/* reset in case it's set for some reason */

yytext=(char *)malloc_2(T_MAX_STR+1);

if(!hier_was_explicitly_set) /* set default hierarchy split char */
	{
	hier_delimeter='.';
	}

flen=strlen(fname);
if (((flen>2)&&(!strcmp(fname+flen-3,".gz")))||
   ((flen>3)&&(!strcmp(fname+flen-4,".zip"))))
	{
	char *str;
	int dlen;
	dlen=strlen(WAVE_DECOMPRESSOR);
	str=wave_alloca(strlen(fname)+dlen+1);
	strcpy(str,WAVE_DECOMPRESSOR);
	strcpy(str+dlen,fname);
	vcd_handle=popen(str,"r");
	vcd_is_compressed=~0;
	}
	else
	{
	if(strcmp("-vcd",fname))
		{
		vcd_handle=fopen(fname,"rb");

		if(vcd_handle)
			{
			fseeko(vcd_handle, 0, SEEK_END);	/* do status bar for vcd load */
			vcd_fsiz = ftello(vcd_handle);
			fseeko(vcd_handle, 0, SEEK_SET);
			}

		if(vcd_warning_filesize < 0) vcd_warning_filesize = VCD_SIZE_WARN;

		if(vcd_warning_filesize)
		if(vcd_fsiz > (vcd_warning_filesize * (1024 * 1024)))
			{
			fprintf(stderr, "Warning! File size is %d MB.  This might fail in recoding.\n"
					"Consider converting it to lxt, lxt2, or vzt database formats instead.  (See\n"
					"the vcd2lxt(1), vcd2lxt2(1), and vzt2vzt(1) manpages for more information.)\n"
					"To disable this warning, set rc variable vcd_warning_filesize to zero.\n"
					"Alternatively, use the -o, --optimize command line option to convert to LXT2.\n\n",
						(int)(vcd_fsiz/(1024*1024)));
			}
		}
		else
		{
		splash_disable = 1;
		vcd_handle=stdin;
		}
	vcd_is_compressed=0;
	}

if(!vcd_handle)
	{
	fprintf(stderr, "Error opening %s .vcd file '%s'.\n",
		vcd_is_compressed?"compressed":"", fname);
	exit(1);
	}

/* SPLASH */				splash_create(); 

getch_alloc();		/* alloc membuff for vcd getch buffer */

build_slisthier();

time_vlist = vlist_create(sizeof(TimeType), 0);

vcd_parse();
if(varsplit)
	{
        free_2(varsplit);
        varsplit=NULL;
        }

vlist_freeze(&time_vlist);
vlist_emit_finalize();

if((!sorted)&&(!indexed))
	{
	fprintf(stderr, "No symbols in VCD file..is it malformed?  Exiting!\n");
	exit(1);
	}

if(vcd_save_handle) fclose(vcd_save_handle);

fprintf(stderr, "["TTFormat"] start time.\n["TTFormat"] end time.\n", start_time*time_scale, end_time*time_scale);

if(vcd_fsiz)
        {
        splash_sync(vcd_fsiz, vcd_fsiz);
	vcd_fsiz = 0;
        }
else
if(vcd_is_compressed)
	{
        splash_sync(1,1);
	vcd_fsiz = 0;
	}

vcd_build_symbols();
vcd_sortfacs();
vcd_cleanup();

getch_free();		/* free membuff for vcd getch buffer */

min_time=start_time*time_scale;
max_time=end_time*time_scale;

if((min_time==max_time)||(max_time==0))
        {
        fprintf(stderr, "VCD times range is equal to zero.  Exiting.\n");
        exit(1);
        }

/* is_vcd=~0; */
is_lx2 = LXT2_IS_VLIST;

return(max_time);
}

/*******************************************************************************/

void vcd_import_masked(void)
{
/* nothing */
}

void vcd_set_fac_process_mask(nptr np)
{
if(np && np->mv.mvlfac_vlist)
	{
	import_vcd_trace(np);
	}
}

void import_vcd_trace(nptr np)
{
struct vlist_t *v = np->mv.mvlfac_vlist;
int len = 1;
int list_size;
unsigned char vlist_type;
unsigned int vartype = 0;
unsigned int vlist_pos = 0;
unsigned char *chp;
unsigned int time_idx = 0;
TimeType *curtime_pnt;
unsigned char arr[5];
int arr_pos;
unsigned int accum;
unsigned char ch;
double *d;

if(!v) return;
vlist_uncompress(&v);

if(!(list_size=vlist_size(v)))
	{
	len = 1;
	vlist_type = '!'; /* possible alias */
	}
	else
	{
	chp = vlist_locate(v, vlist_pos++);
	if(chp)
		{
		switch((vlist_type = (*chp & 0x7f)))
			{
			case '0':
				len = 1;
				chp = vlist_locate(v, vlist_pos++);
				vartype = (unsigned int)(*chp & 0x7f);

				break;

			case 'B':
			case 'R':
			case 'S':
				chp = vlist_locate(v, vlist_pos++);
				vartype = (unsigned int)(*chp & 0x7f);

				arr_pos = accum = 0;
	
		                do      {
		                	chp = vlist_locate(v, vlist_pos++);
		                        if(!chp) break;
		                        ch = *chp;  
					arr[arr_pos++] = ch;				
		                        } while (!(ch & 0x80));
	
				for(--arr_pos; arr_pos>=0; arr_pos--)
					{
					ch = arr[arr_pos];
					accum <<= 7;
					accum |= (unsigned int)(ch & 0x7f);
					}

				len = accum;

				break;				

			default:
				fprintf(stderr, "Unsupported vlist type '%c', exiting.", vlist_type);
				exit(255);
				break;
			}
		}		
		else
		{
		len = 1;
		vlist_type = '!'; /* possible alias */
		}
	}


if(vlist_type == '0') /* single bit */
	{
	while(vlist_pos < list_size)
		{
		unsigned int delta, bitval;
		char ascval;

		arr_pos = accum = 0;

                do      {
                	chp = vlist_locate(v, vlist_pos++);
                        if(!chp) break;
                        ch = *chp;  
			arr[arr_pos++] = ch;				
                        } while (!(ch & 0x80));

		for(--arr_pos; arr_pos>=0; arr_pos--)
			{
			ch = arr[arr_pos];
			accum <<= 7;
			accum |= (unsigned int)(ch & 0x7f);
			}

		if(!(accum&1))
			{
			delta = accum >> 2;
			bitval = (accum >> 1) & 1;
			ascval = '0' + bitval;
			}
			else
			{
			delta = accum >> 4;
			bitval = (accum >> 1) & 7;
			ascval = RCV_STR[bitval];
			}
		time_idx += delta;

		curtime_pnt = vlist_locate(time_vlist, time_idx ? time_idx-1 : 0);

		if(vartype != V_EVENT)
			{
			add_histent(*curtime_pnt,np,ascval,1, NULL);
			}
			else
			{
			if((ascval == '0') || (ascval == '1'))
				{
				TimeType ev = *curtime_pnt;
				add_histent(ev,  np,'1',1, NULL);
				add_histent(ev+1,np,'0',1, NULL);
				}
				else
				{
				add_histent(*curtime_pnt,np,ascval,1, NULL);
				}
			}
		}

	add_histent(MAX_HISTENT_TIME-1, np, 'x', 0, NULL);
	add_histent(MAX_HISTENT_TIME,   np, 'z', 0, NULL);
	}
else if(vlist_type == 'B') /* bit vector, port type was converted to bit vector already */
	{
	char *sbuf = malloc_2(len+1);
	int dst_len;
	char *vector;

	while(vlist_pos < list_size)
		{
		unsigned int delta;

		arr_pos = accum = 0;

                do      {
                	chp = vlist_locate(v, vlist_pos++);
                        if(!chp) break;
                        ch = *chp;  
			arr[arr_pos++] = ch;				
                        } while (!(ch & 0x80));

		for(--arr_pos; arr_pos>=0; arr_pos--)
			{
			ch = arr[arr_pos];
			accum <<= 7;
			accum |= (unsigned int)(ch & 0x7f);
			}

		delta = accum;
		time_idx += delta;

		curtime_pnt = vlist_locate(time_vlist,  time_idx ? time_idx-1 : 0);

		dst_len = 0;
		for(;;)
			{
			chp = vlist_locate(v, vlist_pos++);
			if(!chp) break;
			ch = *chp;
			if((ch >> 4) == AN_MSK) break;
			if(dst_len == len) { if(len != 1) memmove(sbuf, sbuf+1, dst_len - 1); dst_len--; }
			sbuf[dst_len++] = AN_STR[ch >> 4];
			if((ch & AN_MSK) == AN_MSK) break;
			if(dst_len == len) { if(len != 1) memmove(sbuf, sbuf+1, dst_len - 1); dst_len--; }
			sbuf[dst_len++] = AN_STR[ch & AN_MSK];
			}

		if(len == 1)
			{
			add_histent(*curtime_pnt, np,sbuf[0],1, NULL);
			}
			else
			{
			vector = malloc_2(len+1);
			if(dst_len < len)
				{
				unsigned char extend=(sbuf[0]=='1')?'0':sbuf[0];
				memset(vector, extend, len - dst_len);
				memcpy(vector + (len - dst_len), sbuf, dst_len);
				}
				else
				{
				memcpy(vector, sbuf, len);
				}

			vector[len] = 0;
			add_histent(*curtime_pnt, np,0,1,vector);
			}
		}

	if(len==1)
		{
		add_histent(MAX_HISTENT_TIME-1, np, 'x', 0, NULL);
		add_histent(MAX_HISTENT_TIME,   np, 'z', 0, NULL);
		}
		else
		{
		add_histent(MAX_HISTENT_TIME-1, np, 'x', 0, (char *)calloc_2(1,sizeof(char)));
		add_histent(MAX_HISTENT_TIME,   np, 'z', 0, (char *)calloc_2(1,sizeof(char)));
		}

	free_2(sbuf);
	}
else if(vlist_type == 'R') /* real */
	{
	char *sbuf = malloc_2(64);
	int dst_len;
	char *vector;

	while(vlist_pos < list_size)
		{
		unsigned int delta;

		arr_pos = accum = 0;

                do      {
                	chp = vlist_locate(v, vlist_pos++);
                        if(!chp) break;
                        ch = *chp;  
			arr[arr_pos++] = ch;				
                        } while (!(ch & 0x80));

		for(--arr_pos; arr_pos>=0; arr_pos--)
			{
			ch = arr[arr_pos];
			accum <<= 7;
			accum |= (unsigned int)(ch & 0x7f);
			}

		delta = accum;
		time_idx += delta;

		curtime_pnt = vlist_locate(time_vlist,  time_idx ? time_idx-1 : 0);

		dst_len = 0;
		do
			{
			chp = vlist_locate(v, vlist_pos++);
			if(!chp) break;
			ch = *chp;
			sbuf[dst_len++] = ch;
			} while(ch);

		vector=malloc_2(sizeof(double));
		sscanf(sbuf,"%lg",(double *)vector);
		add_histent(*curtime_pnt, np,'g',1,(char *)vector);
		}

	d=malloc_2(sizeof(double));
	*d=1.0;
	add_histent(MAX_HISTENT_TIME-1, np, 'g', 0, (char *)d);

	d=malloc_2(sizeof(double));
	*d=0.0;
	add_histent(MAX_HISTENT_TIME, np, 'g', 0, (char *)d);

	free_2(sbuf);
	}
else if(vlist_type == 'S') /* string */
	{
	char *sbuf = malloc_2(list_size); /* being conservative */
	int dst_len;
	char *vector;

	while(vlist_pos < list_size)
		{
		unsigned int delta;

		arr_pos = accum = 0;

                do      {
                	chp = vlist_locate(v, vlist_pos++);
                        if(!chp) break;
                        ch = *chp;  
			arr[arr_pos++] = ch;				
                        } while (!(ch & 0x80));

		for(--arr_pos; arr_pos>=0; arr_pos--)
			{
			ch = arr[arr_pos];
			accum <<= 7;
			accum |= (unsigned int)(ch & 0x7f);
			}

		delta = accum;
		time_idx += delta;

		curtime_pnt = vlist_locate(time_vlist,  time_idx ? time_idx-1 : 0);

		dst_len = 0;
		do
			{
			chp = vlist_locate(v, vlist_pos++);
			if(!chp) break;
			ch = *chp;
			sbuf[dst_len++] = ch;
			} while(ch);

		vector=malloc_2(dst_len + 1);
		strcpy(vector, sbuf);
		add_histent(*curtime_pnt, np,'s',1,(char *)vector);
		}

	d=malloc_2(sizeof(double));
	*d=1.0;
	add_histent(MAX_HISTENT_TIME-1, np, 'g', 0, (char *)d);

	d=malloc_2(sizeof(double));
	*d=0.0;
	add_histent(MAX_HISTENT_TIME, np, 'g', 0, (char *)d);

	free_2(sbuf);
	}
else if(vlist_type == '!') /* error in loading */
	{
	nptr n2 = (nptr)np->curr;
	
	if((n2)&&(n2 != np))	/* keep out any possible infinite recursion from corrupt pointer bugs */
		{
		import_vcd_trace(n2);

		vlist_destroy(v);
		np->mv.mvlfac_vlist = NULL;

		np->head = n2->head;
		np->curr = n2->curr;
		return;
		}

	fprintf(stderr, "Error in decompressing vlist for '%s', exiting.\n", np->nname);
	exit(255);
	}

vlist_destroy(v);
np->mv.mvlfac_vlist = NULL;
}

/*
 * $Id$
 * $Log$
 * Revision 1.4  2007/04/29 06:07:28  gtkwave
 * fixed memory leaks in vcd parser
 *
 * Revision 1.3  2007/04/29 04:13:49  gtkwave
 * changed anon union defined in struct Node to a named one as anon unions
 * are a gcc extension
 *
 * Revision 1.2  2007/04/20 02:08:17  gtkwave
 * initial release
 *
 */

