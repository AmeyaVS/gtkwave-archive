/*
 * A n t l r  T r a n s l a t i o n  H e a d e r
 *
 * Terence Parr, Will Cohen, and Hank Dietz: 1989-2001
 * Purdue University Electrical Engineering
 * With AHPCRC, University of Minnesota
 * ANTLR Version 1.33MR33
 *
 *   ../pccts/antlr/antlr -ga -k 2 -gl ./verilog.g
 *
 */

#define ANTLR_VERSION	13333
#include "pcctscfg.h"
#include "pccts_stdio.h"
#line 1 "./verilog.g"


#include "attrib.h"
#include "vpp_common.h"

#if defined __MINGW32__ || defined _MSC_VER
#define realpath(N,R) _fullpath((R),(N),_MAX_PATH)
#endif

int zzcr_attr(Attrib *attr, int token, char *text);
void vpp_update_yyfilename(const char *str);
void vpp_update_yylineno(const char *str);

  
#define LL_K 2
#define zzSET_SIZE 28
#include "antlr.h"
#include "keyword_tokens.h"
#include "tokens.h"
#include "dlgdef.h"
#include "mode.h"

/* MR23 In order to remove calls to PURIFY use the antlr -nopurify option */

#ifndef PCCTS_PURIFY
#define PCCTS_PURIFY(r,s) memset((char *) &(r),'\0',(s));
#endif

ANTLR_INFO
#line 18 "./verilog.g"

#include "../../config.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>

#include "wave_locale.h"

#include "inter.h"
#include "tokens.h"
#include "shred.h"
#include "symbol.h"
#include "jrb.h"
#include "vpp_common.h"

#ifndef PATH_MAX
#define PATH_MAX (4096)
#endif

int verilog_keyword_code(const char *s, unsigned int len); /* returns token value from gperf */
void args_expand(int argc, char **argv, int *new_argc, char ***new_argv);

int zzerrors=0;
int zzcomment_start=0, zzcomment_depth=0, zzcomment_entry=0;
char *zzfilename=NULL;	

JRB modname_tree=NULL;
int module_is_duplicate=0;

char *mod_current_name = NULL;
char *mod_current_filename = NULL;
int   mod_start_line = 0;

char *udp_current_name = NULL;
char *udp_current_filename = NULL;
int   udp_start_line = 0;

char *comp_type_name = NULL;

int emit_stems = 0, emit_vars = 0;

static void add_string_to_tree(JRB tree, char *s, int exists)
{
  Jval val;
  JRB node;
  
if(!(node=jrb_find_str(tree, s)))
  {
    val.i = (exists!=0);
    jrb_insert_str(tree, s, val);
  }
  else
  {
    node->val.i |= (exists!=0);
  }
}

char *prevlineroot=NULL;
char *prevprevlineroot=NULL;
char *lineroot=NULL, *linepos=NULL;
int linemaxlen=0;
int zzrewind=0;

/*
* add token to the analyzed verilog buffer..
*/
void addtoken(int is_newline)
{
  if(!is_newline) 
  {
    zzrewind = strlen(zztext);
    
	if((linepos-lineroot)+zzrewind >= linemaxlen)
    {
      char *t=realloc(lineroot, linemaxlen = 2*(linemaxlen+zzrewind)+1);
      prevlineroot=realloc(prevlineroot, linemaxlen);
      prevprevlineroot=realloc(prevprevlineroot, linemaxlen);
      linepos = (linepos-lineroot) + t;
      lineroot = t;
    }
    
	strcpy(linepos,zztext);
    linepos+=zzrewind;
  }
  else
  { 
    char *t=prevprevlineroot;		/* scroll the buffer ptrs to avoid copying */
    prevprevlineroot = prevlineroot;
    prevlineroot = lineroot;
    linepos = lineroot = t;
    *linepos=0; 
    zzline++; 
  }
}

void report_error_context(int tok, char *badtok)
{ 
  char *ch, v;
  int col=0;
  
ch=lineroot;
  
do
  {
    v=*(ch++);
    switch(v)
    {
      case '\t': col=(col+8)&(~0x07); break;
      case '\n': col=2;		break;
      default:   col++;		break;
    }
  } while(v);
  
col-=zzrewind; if(col<2) col=2;
  
if(tok!=zzEOF_TOKEN)
  {
    warn("   Local context near token '%s' in '%s' on line %d.%d:\n",badtok,zzfilename, zzline,col-1);
  }
  else
  {
    warn("   Local context preceding EOF in '%s' on line %d.%d:\n",zzfilename, zzline,col-1);
  }
  if(zzline>2)
  {
    warn("%6d: %s\n", zzline-2, prevprevlineroot);
  }
  if(zzline>1)
  {
    warn("%6d: %s\n", zzline-1, prevlineroot);
  }
  
if(col>2)
  {
    warn("%6d: %s\n\t%*s^\n", zzline, lineroot, col-2, "");
  }
  else
  {
    warn("%6d: %s\n\t^\n", zzline, lineroot);
  }
}


int do_not_translate = 0, do_not_translate_mask = 0;

void warn_about_translation_off(void)
{
  if(do_not_translate)
  {
    warn("** Warning: source code translation off for { %s%s%s%s%s} at EOF in '%s'.\n", 
    (do_not_translate&STMODE_XLATEOFF_IFDEF)     ? "ifdef "     : "",
    (do_not_translate&STMODE_XLATEOFF_SYNOPSYS)  ? "synopsys "  : "",
    (do_not_translate&STMODE_XLATEOFF_SYNTHESIS) ? "synthesis " : "",
    (do_not_translate&STMODE_XLATEOFF_VERILINT)  ? "verilint "  : "",
    (do_not_translate&STMODE_XLATEOFF_VERTEX)    ? "vertex "    : "",
    zzfilename);
    
	do_not_translate = 0;
  }
  
if(ifdef_stack_top)
  {
    struct ifdef_stack_t *is = ifdef_stack_top;
    
		while(ifdef_stack_top)
    {
      is=ifdef_stack_top;
      
			warn("** Warning: pending `if%sdef %s at EOF, start is file '%s' line %d.\n",
      is->polarity ? "" : "n", 
      is->deftext,
      is->zzfilename, is->zzline);
      
		        free(is->zzfilename);
      free(is->deftext);
      ifdef_stack_top = is->next;
      free(is);
    }
    
		include_stack_depth = 0;
  }
  
if(zzcomment_depth)
  {
    mesg("** Error: Unterminated comment starts at line %d in '%s'.\n", zzcomment_start, zzfilename);
    zzcomment_depth = 0;
  }
}


void parsecomment(void)
{
  char *tok=strdup(zztext);
  char *tok_sav=tok;
  strcpy(tok, zztext);
  
tok = strtok(tok,"/ \t");
  if (tok != 0) {
    if (!strcmp("vpp_file", tok))
    {
      tok = strtok(NULL, " \t");
      if(tok) vpp_update_yyfilename(tok);
    }
    else
    if (!strcmp("vpp_line", tok))
    {
      tok = strtok(NULL, "");
      if(tok) vpp_update_yylineno(tok);
    }
    else
    if(!(do_not_translate&STMODE_XLATEOFF_IFDEF))	/* make sure preprocessed block is active */
    {
      if ((!strcmp("synopsys", tok))&&(do_not_translate_mask & STMODE_XLATEOFF_SYNOPSYS))
      {
        tok = strtok(NULL, " \t");
        if(tok) 
        {
          if(!strcmp("translate_on", tok))
          {
            do_not_translate &= ~(STMODE_XLATEOFF_SYNOPSYS);
          }
          else
          if(!strcmp("translate_off", tok))
          {
            do_not_translate |= (do_not_translate_mask & STMODE_XLATEOFF_SYNOPSYS);
          }
          else
          {
            warn("** Warning: unsupported synopsys pragma '%s' on line %d in file '%s', skipping.\n",
            tok, zzline, zzfilename);
          }
        }
      }
      else
      if ((!strcmp("synthesis", tok))&&(do_not_translate_mask & STMODE_XLATEOFF_SYNTHESIS))
      {
        tok = strtok(NULL, " \t");
        if(tok) 
        {
          if(!strcmp("translate_on", tok))
          {
            do_not_translate &= ~(STMODE_XLATEOFF_SYNTHESIS);
          }
          else
          if(!strcmp("translate_off", tok))
          {
            do_not_translate |= (do_not_translate_mask & STMODE_XLATEOFF_SYNTHESIS);
          }
          else
          {
            warn("** Warning: unsupported synthesis pragma '%s' on line %d in file '%s', skipping.\n",
            tok, zzline, zzfilename);
          }
        }
      }
      else
      if ((!strcmp("verilint", tok))&&(do_not_translate_mask & STMODE_XLATEOFF_VERILINT))
      {
        tok = strtok(NULL, " \t");
        if(tok) 
        {
          if(!strcmp("translate", tok))
          {
            tok = strtok(NULL, " \t");
            if(tok)
            {
              if(!strcmp("on", tok))
              {
                do_not_translate &= ~(STMODE_XLATEOFF_VERILINT);
              }
              else
              if(!strcmp("off", tok))
              {
                do_not_translate |= (do_not_translate_mask & STMODE_XLATEOFF_VERILINT);
              }
              else
              {
                warn("** Warning: unsupported translate option '%s' on line %d in file '%s', skipping.\n",
                tok, zzline, zzfilename);
              }
            }
          }
        }
      }
      else
      if ((!strcmp("vertex", tok))&&(do_not_translate_mask & STMODE_XLATEOFF_VERTEX))
      {
        tok = strtok(NULL, " \t");
        if(tok) 
        {
          if(!strcmp("translate", tok))
          {
            tok = strtok(NULL, " \t");
            if(tok)
            {
              if(!strcmp("on", tok))
              {
                do_not_translate &= ~(STMODE_XLATEOFF_VERTEX);
              }
              else
              if(!strcmp("off", tok))
              {
                do_not_translate |= (do_not_translate_mask & STMODE_XLATEOFF_VERTEX);
              }
              else
              {
                warn("** Warning: unsupported translate option '%s' on line %d in file '%s', skipping.\n",
                tok, zzline, zzfilename);
              }
            }
          }
        }
      }
    }
  }
  
free(tok_sav);
}


void
zzsyn(char *text, int tok, char *egroup, SetWordType *eset, int etok, int k, char *bad_text)
{
  if(tok!=zzEOF_TOKEN)
  {
    mesg("** Syntax error at \"%s\"", bad_text);
  }
  else
  {
    mesg("** Syntax error at EOF");
  }
  if ( !etok && !eset ) {warn("\n"); return;}
  if ( k==1 ) warn(" missing");
  else
  {
    warn("; \"%s\" not", bad_text);
    if ( zzset_deg(eset)>1 ) warn(" in");
  }
  if ( zzset_deg(eset)>0 ) zzedecode(eset);
  else warn(" %s", zztokens[etok]);
  if ( strlen(egroup) > 0 ) warn(" in %s", egroup);
  warn("\n");
  report_error_context(tok, bad_text);
}


int zzcr_attr(Attrib *attr, int token, char *text)
{
  int len;
  
switch(token)
  {
    case V_FUNCTION_NAME:
    case V_IDENTIFIER:
    case V_IDENTIFIER2:
    case V_IDENDOT:
    attr->symbol=sym_add(text);
    break;
    
	case V_DBASE:
    case V_BBASE:
    case V_OBASE:
    case V_HBASE:
    attr->text=strdup(text);
    break;
    
	case V_STRING:
    len = strlen(text);
    text[len-1]=0;
    strcpy(attr->text = malloc(len-2+1), text+1);
    break;
    
	case V_DECIMAL_NUMBER:
    attr->ival=atoi_with_underscores(text);
    break;
    case V_FLOAT1:
    case V_FLOAT2:
    sscanf(text, "%lf", &attr->rval);	
    break;
    
	default: 
    attr->ival=0;
  }
  return(0);
}


int main(int argc, char **argv)
{
  int v_argc;
  char **v_argv;
  int i;
  struct vpp_filename *head=NULL, *curr=NULL, *t, *e;
  struct vpp_filename *lib_head=NULL, *lib_curr=NULL;
  struct vpp_filename *libext_head=NULL, *libext_curr=NULL;
  JRB node;
  int maxarg; /* scan build : was  int maxarg = v_argc - 1; */
  
WAVE_LOCALE_FIX
  
args_expand(argc, argv, &v_argc, &v_argv);
  maxarg = v_argc - 1;
  
modname_tree=make_jrb();
  define_tree=make_jrb();
  
lineroot=linepos=(char *)calloc(1, linemaxlen=16);
  prevlineroot=(char *)calloc(1, linemaxlen);
  prevprevlineroot=(char *)calloc(1, linemaxlen);
  
if(v_argc==1)
  {
    warn(VERNAME"\n"
    "No files to process (use -h for help), exiting.\n\n");
    exit(0);
  }
  
warn(VERNAME"\n\n");
  
for(i=1;i<v_argc;i++)
  {
  int arglen = strlen(v_argv[i]);
  
	if((!strcmp(v_argv[i], "-y"))||(!strcmp(v_argv[i], "-yi")))
  {
  if(i==maxarg)
  {
  warn("** Missing filename after %s option!\n", v_argv[i]);
}
else
{
t=calloc(1, sizeof(struct vpp_filename));
t->name = strdup(v_argv[++i]);

			if(v_argv[i-1][2])		/* isn't nullchar so it's 'i' */
{
dirscan(t->name, t);	/* it's a case-insensitive directory, so scan it in */
}

			if(!lib_head)
{
lib_head=lib_curr=t;
}
else
{
lib_curr->next = t;
lib_curr = t;
}
}
}
else
if(!strcmp(v_argv[i], "-emitmono"))
{
if(i==maxarg)
{
warn("** Missing filename after -emitmono option!\n");
}
else
{
i++;

			if(mgetchar_fout)
{
warn("** Ignoring extra -emitmono specification for '%s'.\n", v_argv[i]);
}
else
{
mgetchar_fout = fopen(v_argv[i], "wb");
if(!mgetchar_fout)
{
mesg("** Could not open -emitmono file '%s', exiting.\n", v_argv[i]);
perror("Why");
exit(255);
}
}
}
}
else
if(!strcmp(v_argv[i], "-pragma"))
{
if(i==maxarg)
{
warn("** Missing typename after -pragma option!\n");
}
else
{
i++;
if(!strcmp(v_argv[i], "synopsys"))
{
do_not_translate_mask |= STMODE_XLATEOFF_SYNOPSYS; 
}
else
if(!strcmp(v_argv[i], "synthesis"))
{
do_not_translate_mask |= STMODE_XLATEOFF_SYNTHESIS; 
}
else
if(!strcmp(v_argv[i], "verilint"))
{
do_not_translate_mask |= STMODE_XLATEOFF_VERILINT; 
}
else
if(!strcmp(v_argv[i], "vertex"))
{
do_not_translate_mask |= STMODE_XLATEOFF_VERTEX; 
}
else
{
warn("** Unknown -pragma type '%s', ignoring.\n", v_argv[i]);
}
}
}
else
if((!strcmp(v_argv[i], "-h"))||(!strcmp(v_argv[i], "-help")))
{
warn(	"Usage:\n"
"------\n"
"%s [input filename(s)] [options]\n\n"
"-h[elp]         prints this screen\n"
"-emitmono fname emit monolithic (parser view of) file to fname\n"
"-emitstems      emit source code stems to stdout\n"
"-emitvars       emit source code variables to stdout\n"
"-Dx=y           equivalent to `define X Y in source\n"
"+define+x=y     equivalent to `define X Y in source\n"
"+incdir+dirname add dirname to include search path\n"
"+libext+ext     add ext to filename when searching for files\n"
"-pragma name    add name (synopsys, synthesis, verilint, vertex) to accepted pragmas\n"
"-y dirname      add directory to source input path\n"
"-yi dirname     add directory to source input path (case insensitive search)\n"
"-f filename     insert args from filename (does not work recursively)\n"
"\n", v_argv[0]
);
exit(0);
}
else
if(!strcmp(v_argv[i], "-f"))
{
warn("** Cannot nest -f option inside an args file, exiting.\n");
exit(255);
}
else
if(!strcmp(v_argv[i], "-emitstems"))
{
emit_stems = 1;
}
else
if(!strcmp(v_argv[i], "-emitvars"))
{
emit_vars = 1;
}
else
if((arglen>=8)&&(!strncmp(v_argv[i],"+incdir+",8)))
{
if(arglen==8)
{
warn("** Missing path for +incdir+ in command line argument %d, ignoring.\n", i);
}   
else
{
char *lname=(char *)malloc(arglen-8+1);
char *tok;

                        strcpy(lname, v_argv[i]+8);
tok=strtok(lname,"+");                 

			while(tok)
{
int toklen=strlen(tok);
if(!toklen)
{
/* strtok seems to never hit this */
warn("** Missing path for +incdir+ (consecutive ++) in command line argument %d, ignoring.\n", i);
}
else
{
if(!incchain)
{
struct vpp_filename *l;
l=(struct vpp_filename *)calloc(1,sizeof(struct vpp_filename));
strcpy(l->name=malloc(toklen+1), tok);

		                                incchain=l;
}
else
{
struct vpp_filename *l=incchain;
struct vpp_filename *l2;

		                                while(l->next) l=l->next;

		                                l2=(struct vpp_filename *)calloc(1,sizeof(struct vpp_filename));
strcpy(l2->name=malloc(toklen+1), tok);
l->next=l2;
}
}

				tok=strtok(NULL, "+");
}

			free(lname);                        
}
}
else
if((arglen>=8)&&(!strncmp(v_argv[i],"+define+",8)))
{
if(arglen==8)
{
warn("** Missing value for +define+ in command line argument %d, ignoring.\n", i);
}   
else
{
char *lname=(char *)malloc(arglen-8+1);
char *tok;

                        strcpy(lname, v_argv[i]+8);
tok=strtok(lname,"+");                 

			while(tok)
{
int toklen=strlen(tok);
if(!toklen)
{
/* strtok seems to never hit this */
warn("** Missing value for +define+ (consecutive ++) in command line argument %d, ignoring.\n", i);
}
else
{
char *dname=(char *)malloc(toklen+8+1);
char *pnt;
sprintf(dname, "`define %s", tok);
pnt = dname+8;
while(*pnt)
{
if(*pnt=='=')
{
*pnt=' ';
break;
}
pnt++;
}
store_define(dname);
free(dname);
}

				tok=strtok(NULL, "+");
}

			free(lname);                        
}
}
else
if((arglen>=8)&&(!strncmp(v_argv[i],"+libext+",8)))
{
if(arglen==8)
{
warn("** Missing extension for +libext+ in command line argument %d, ignoring.\n", i);
}   
else
{
char *lname=(char *)malloc(arglen-8+1);
char *tok;
strcpy(lname, v_argv[i]+8);

			tok=strtok(lname,"+");                 
while(tok)
{
int toklen=strlen(tok);
if(!toklen)
{
/* strtok seems to never hit this */
warn("** Missing path for +libext+ (consecutive ++) in command line argument %d, ignoring.\n", i);
}
else
{
if(!libext_curr)
{
struct vpp_filename *l;
l=(struct vpp_filename *)calloc(1,sizeof(struct vpp_filename));
strcpy(l->name=malloc(toklen+1), tok);
libext_head=libext_curr=l;
}
else
{
struct vpp_filename *l;

		                                l=(struct vpp_filename *)calloc(1,sizeof(struct vpp_filename));
strcpy(l->name=malloc(toklen+1), tok);
libext_curr->next=l;
libext_curr=l;
}
}

				tok=strtok(NULL, "+");
}
free(lname);                        
}
}
else
if((arglen>=2)&&(v_argv[i][0] == '+'))
{
warn("** Skipping plusarg '%s' in command line argument %d.\n", v_argv[i], i);
}
else
if((arglen>=2)&&(!strncmp(v_argv[i],"-D",2)))
{
if(arglen==2)
{
warn("** Missing define for -D in command line argument %d, ignoring.\n", i);
}
else
{
char *dname=(char *)malloc(arglen-2+8+1);
char *pnt;
sprintf(dname, "`define %s", v_argv[i]+2);
pnt = dname+8;
while(*pnt)
{
if(*pnt=='=')
{
*pnt=' ';
break;
}
pnt++;
}
store_define(dname);
free(dname);
}
}
else	/* filename only.. */
{
t=calloc(1, sizeof(struct vpp_filename));
t->name = strdup(v_argv[i]);

		if(!head)
{
head=curr=t;
vlog_filenames = head;
}
else
{
curr->next = t;
curr = t;
}
}
}

shred_alloc();

prevprevlineroot[0]=prevlineroot[0]=lineroot[0]=0; linepos=lineroot; do_not_translate = 0;
ANTLRf(v_source_text(), mgetchar_fout ? mgetchar_outhandle : mgetchar); 
warn_about_translation_off();
zzerrors+=zzLexErrCount;
if(zzerrors)
{
warn("\n** %d error%s detected, exiting.\n\n",zzerrors,(zzerrors>1)?"s were":" was");
exit(255);
}

do	{
i=0;
jrb_traverse(node, modname_tree)
{
if(node->val.i==0)
{
FILE *hand;
int len = strlen(node->key.s);
int resolve = 0;
char *buff;
t=lib_head;

			while(t)
{
e=libext_head;
do
{
char *ext=e?e->name:"";
buff = (char *)malloc(strlen(t->name)+1+len+strlen(ext)+1);
sprintf(buff, "%s/%s%s", t->name, node->key.s, ext);
hand = fopen(buff, "r");
if(hand)
{
warn("Processing file '%s' ...\n", buff);
vpp_main(hand, buff);
prevprevlineroot[0]=prevlineroot[0]=lineroot[0]=0; linepos=lineroot; do_not_translate = 0;
ANTLRf(v_source_text(), mgetchar_fout ? mgetchar_outhandle : mgetchar);	/* preproc.c will do fclose() */
warn_about_translation_off();
free(buff);
zzerrors+=zzLexErrCount;
if(zzerrors)
{
warn("\n** %d error%s detected, exiting.\n\n",zzerrors,(zzerrors>1)?"s were":" was");
exit(255);
}
i=1;
resolve=1;
goto is_resolved;
}
free(buff);
} while((e)&&(e=e->next));

				t=t->next;
}

			t=lib_head;

			while(t)
{
if(t->numchildren)
{
e=libext_head;
do
{
char **realname;
char *ext=e?e->name:"";
buff = (char *)malloc(len+strlen(ext)+1);
sprintf(buff, "%s%s", node->key.s, ext);
realname = bsearch(buff, t->children, t->numchildren, sizeof(char **), compar_cstarstar_bsearch);
free(buff);
if(realname)
{
buff = (char *)malloc(strlen(t->name)+1+strlen(*realname)+1);
sprintf(buff, "%s/%s", t->name, *realname);

							hand = fopen(buff, "r");
if(hand)
{
warn("Processing file '%s' ...\n", buff);
vpp_main(hand, buff);
prevprevlineroot[0]=prevlineroot[0]=lineroot[0]=0; linepos=lineroot; do_not_translate = 0;
ANTLRf(v_source_text(), mgetchar_fout ? mgetchar_outhandle : mgetchar);	/* preproc.c will do fclose() */
warn_about_translation_off();
free(buff);
zzerrors+=zzLexErrCount;
if(zzerrors)
{
warn("\n** %d error%s detected, exiting.\n\n",zzerrors,(zzerrors>1)?"s were":" was");
exit(255);
}
i=1;
resolve=1;
goto is_resolved;
}	
free(buff);
}
} while((e)&&(e=e->next));
}

				t=t->next;
}

is_resolved:		if(!resolve)
{
warn("** Error: could not find module '%s'\n", node->key.s);
}

			node->val.i=1;
}
}
} while(i==1);

zzerrors+=zzLexErrCount;
if(zzerrors)
{
warn("\n** %d error%s detected, exiting.\n\n",zzerrors,(zzerrors>1)?"s were":" was");
exit(255);
}

shred_free();
return(0);
}



  

void
#ifdef __USE_PROTOS
v_source_text(void)
#else
v_source_text()
#endif
{
#line 1084 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1084 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (setwd1[LA(1)]&0x1) ) {
#line 1084 "./verilog.g"
      v_description();
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
#line 1084 "./verilog.g"
  zzmatch(V_EOF); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x2);
  }
}

void
#ifdef __USE_PROTOS
v_description(void)
#else
v_description()
#endif
{
#line 1087 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (setwd1[LA(1)]&0x4) ) {
#line 1087 "./verilog.g"
    v_module();
  }
  else {
    if ( (LA(1)==V_PRIMITIVE) ) {
#line 1088 "./verilog.g"
      v_udp();
    }
    else {zzFAIL(1,zzerr1,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x8);
  }
}

void
#ifdef __USE_PROTOS
v_module(void)
#else
v_module()
#endif
{
#line 1091 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1091 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==V_MODULE) ) {
#line 1091 "./verilog.g"
      zzmatch(V_MODULE); zzCONSUME;
    }
    else {
      if ( (LA(1)==V_MACROMODULE)
 ) {
#line 1091 "./verilog.g"
        zzmatch(V_MACROMODULE); zzCONSUME;
      }
      else {zzFAIL(1,zzerr2,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
#line 1092 "./verilog.g"
  {
    struct i_symbol_scope *sb = (struct i_symbol_scope *)calloc(1, sizeof(struct i_symbol_scope)); 
    sb->symtable = make_jrb(); 
    sb->parent = sym_base;
    sym_base = sb;
    
				module_is_duplicate=0; 
    if(mod_current_filename)
    {
      free(mod_current_filename);
    }
    mod_current_filename = strdup(zzfilename);
    
				if(mod_current_name)
    {
      free(mod_current_name);
      mod_current_name = NULL;
    }
    
				mod_start_line = zzline;
  }
#line 1114 "./verilog.g"
  v_identifier_nodot();
#line 1115 "./verilog.g"
  {
    JRB node = jrb_find_str(modname_tree, zzaArg(zztasp1,2 ).symbol->name);
    if((!node)||((node)&&(!node->val.i)))
    {
      add_string_to_tree(modname_tree, zzaArg(zztasp1,2 ).symbol->name, TRUE);
      mod_current_name = strdup(zzaArg(zztasp1,2 ).symbol->name);
      
					}
    else
    {
      warn("** Warning: skipping duplicate module '%s' at in file '%s' line %d\n", zzaArg(zztasp1,2 ).symbol->name, zzfilename, zzline);
      module_is_duplicate = 1;
    }
  }
#line 1131 "./verilog.g"
  v_module_body();
#line 1133 "./verilog.g"
  zzmatch(V_ENDMODULE);
#line 1134 "./verilog.g"
  {
    if(sym_base)
    {
      if(module_is_duplicate)
      {
        JRB symtree = sym_base->symtable;
        struct i_symbol_scope *sb = sym_base->parent;
        JRB node;
        
						jrb_traverse(node, symtree)
        {
          free(((struct i_symbol *)node->val.v)->name);
          free(node->val.v);	/* free up strings for any stray symbols (should be only modname) */
        }
        jrb_free_tree(symtree);						
        free(sym_base);
        sym_base = sb;
      }
      else
      {
        JRB symtree = sym_base->symtable;
        JRB node;
        
						sym_base = sym_base->parent;
        
						if(emit_stems)
        {
          char real_path[PATH_MAX];
          
							printf("++ module %s file %s lines %d - %d\n",
          mod_current_name, realpath(mod_current_filename, real_path), mod_start_line, zzline);
        }
        
						if(emit_vars)
        {
          jrb_traverse(node, symtree)						
          {
            printf("++ var %s module %s\n",
            ((struct i_symbol *)node->val.v)->name,
            mod_current_name);
          }
        }
        
						if(mod_current_filename)
        {
          free(mod_current_filename);
          mod_current_filename = NULL;
        }
        
						if(mod_current_name)
        {
          free(mod_current_name);
          mod_current_name = NULL;
        }
      }
    }
  }
 zzCONSUME;

  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x10);
  }
}

void
#ifdef __USE_PROTOS
v_module_body(void)
#else
v_module_body()
#endif
{
#line 1194 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1194 "./verilog.g"
  v_list_of_ports();
#line 1194 "./verilog.g"
  zzmatch(V_SEMI); zzCONSUME;
#line 1195 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (setwd1[LA(1)]&0x20) ) {
#line 1195 "./verilog.g"
      v_module_item();
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x40);
  }
}

void
#ifdef __USE_PROTOS
v_list_of_ports(void)
#else
v_list_of_ports()
#endif
{
#line 1198 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_LP) ) {
#line 1198 "./verilog.g"
    zzmatch(V_LP); zzCONSUME;
#line 1198 "./verilog.g"
    v_port();
#line 1198 "./verilog.g"
    {
      zzBLOCK(zztasp2);
      zzMake0;
      {
      while ( (LA(1)==V_COMMA) ) {
#line 1198 "./verilog.g"
        zzmatch(V_COMMA); zzCONSUME;
#line 1198 "./verilog.g"
        v_port();
        zzLOOP(zztasp2);
      }
      zzEXIT(zztasp2);
      }
    }
#line 1198 "./verilog.g"
    zzmatch(V_RP); zzCONSUME;
  }
  else {
    if ( (LA(1)==V_SEMI) ) {
    }
    else {zzFAIL(1,zzerr3,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x80);
  }
}

void
#ifdef __USE_PROTOS
v_port(void)
#else
v_port()
#endif
{
#line 1202 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (setwd2[LA(1)]&0x1)
 ) {
#line 1202 "./verilog.g"
    v_port_expression();
  }
  else {
    if ( (LA(1)==V_DOT) ) {
#line 1203 "./verilog.g"
      zzmatch(V_DOT); zzCONSUME;
#line 1203 "./verilog.g"
      v_identifier_nodot();
#line 1203 "./verilog.g"
      zzmatch(V_LP); zzCONSUME;
#line 1203 "./verilog.g"
      v_port_expression();
#line 1203 "./verilog.g"
      zzmatch(V_RP); zzCONSUME;
    }
    else {zzFAIL(1,zzerr4,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x2);
  }
}

void
#ifdef __USE_PROTOS
v_port_expression(void)
#else
v_port_expression()
#endif
{
#line 1206 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (setwd2[LA(1)]&0x4) ) {
#line 1206 "./verilog.g"
    v_port_reference();
  }
  else {
    if ( (LA(1)==V_LBRACE) ) {
#line 1207 "./verilog.g"
      zzmatch(V_LBRACE); zzCONSUME;
#line 1207 "./verilog.g"
      v_port_reference();
#line 1207 "./verilog.g"
      {
        zzBLOCK(zztasp2);
        zzMake0;
        {
        while ( (LA(1)==V_COMMA) ) {
#line 1207 "./verilog.g"
          zzmatch(V_COMMA); zzCONSUME;
#line 1207 "./verilog.g"
          v_port_reference();
          zzLOOP(zztasp2);
        }
        zzEXIT(zztasp2);
        }
      }
#line 1207 "./verilog.g"
      zzmatch(V_RBRACE); zzCONSUME;
    }
    else {
      if ( (setwd2[LA(1)]&0x8)
 ) {
      }
      else {zzFAIL(1,zzerr5,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x10);
  }
}

void
#ifdef __USE_PROTOS
v_port_reference(void)
#else
v_port_reference()
#endif
{
#line 1211 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1211 "./verilog.g"
  v_identifier_nodot();
#line 1213 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==V_LBRACK) ) {
#line 1212 "./verilog.g"
      {
        zzBLOCK(zztasp3);
        zzMake0;
        {
#line 1212 "./verilog.g"
        zzmatch(V_LBRACK); zzCONSUME;
#line 1212 "./verilog.g"
        v_expression();
#line 1212 "./verilog.g"
        {
          zzBLOCK(zztasp4);
          zzMake0;
          {
          if ( (LA(1)==V_COLON) ) {
#line 1212 "./verilog.g"
            zzmatch(V_COLON); zzCONSUME;
#line 1212 "./verilog.g"
            v_expression();
          }
          else {
            if ( (LA(1)==V_RBRACK) ) {
            }
            else {zzFAIL(1,zzerr6,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
          }
          zzEXIT(zztasp4);
          }
        }
#line 1213 "./verilog.g"
        zzmatch(V_RBRACK); zzCONSUME;
        zzEXIT(zztasp3);
        }
      }
    }
    else {
      if ( (setwd2[LA(1)]&0x20) ) {
      }
      else {zzFAIL(1,zzerr7,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x40);
  }
}

void
#ifdef __USE_PROTOS
v_module_item(void)
#else
v_module_item()
#endif
{
#line 1216 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_PARAMETER)
 ) {
#line 1216 "./verilog.g"
    v_parameter_declaration();
  }
  else {
    if ( (LA(1)==V_INPUT) ) {
#line 1217 "./verilog.g"
      v_input_declaration();
    }
    else {
      if ( (LA(1)==V_OUTPUT) ) {
#line 1218 "./verilog.g"
        v_output_declaration();
      }
      else {
        if ( (LA(1)==V_INOUT) ) {
#line 1219 "./verilog.g"
          v_inout_declaration();
        }
        else {
          if ( (LA(1)==V_REG) ) {
#line 1220 "./verilog.g"
            v_reg_declaration();
          }
          else {
            if ( (LA(1)==V_TIME)
 ) {
#line 1221 "./verilog.g"
              v_time_declaration();
            }
            else {
              if ( (LA(1)==V_INTEGER) ) {
#line 1222 "./verilog.g"
                v_integer_declaration();
              }
              else {
                if ( (LA(1)==V_REAL) ) {
#line 1223 "./verilog.g"
                  v_real_declaration();
                }
                else {
                  if ( (LA(1)==V_EVENT) ) {
#line 1224 "./verilog.g"
                    v_event_declaration();
                  }
                  else {
                    if ( (setwd2[LA(1)]&0x80) ) {
#line 1225 "./verilog.g"
                      v_gate_declaration();
                    }
                    else {
                      if ( (LA(1)==V_PRIMITIVE)
 ) {
#line 1226 "./verilog.g"
                        v_udp();
                      }
                      else {
                        if ( (setwd3[LA(1)]&0x1) ) {
#line 1227 "./verilog.g"
                          v_module_instantiation();
                        }
                        else {
                          if ( (LA(1)==V_DEFPARAM) ) {
#line 1228 "./verilog.g"
                            v_parameter_override();
                          }
                          else {
                            if ( (setwd3[LA(1)]&0x2) ) {
#line 1229 "./verilog.g"
                              v_continuous_assign();
                            }
                            else {
                              if ( (LA(1)==V_INITIAL) ) {
#line 1230 "./verilog.g"
                                v_initial_statement();
                              }
                              else {
                                if ( (LA(1)==V_ALWAYS)
 ) {
#line 1231 "./verilog.g"
                                  v_always_statement();
                                }
                                else {
                                  if ( (LA(1)==V_TASK) ) {
#line 1232 "./verilog.g"
                                    v_task();
                                  }
                                  else {
                                    if ( (LA(1)==V_FUNCTION) ) {
#line 1233 "./verilog.g"
                                      v_function();
                                    }
                                    else {
                                      if ( (LA(1)==V_SPECIFY) ) {
#line 1234 "./verilog.g"
                                        v_specify_block();
                                      }
                                      else {zzFAIL(1,zzerr8,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x4);
  }
}

void
#ifdef __USE_PROTOS
v_udp(void)
#else
v_udp()
#endif
{
#line 1237 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1237 "./verilog.g"
  zzmatch(V_PRIMITIVE);
#line 1238 "./verilog.g"
  { 
    struct i_symbol_scope *sb = (struct i_symbol_scope *)calloc(1, sizeof(struct i_symbol_scope)); 
    sb->symtable = make_jrb(); 
    sb->parent = sym_base;
    sym_base = sb;
    
			module_is_duplicate = 0;
    
			if(udp_current_filename)
    {
      free(udp_current_filename);
    }
    udp_current_filename = strdup(zzfilename);
    
			if(udp_current_name)
    {
      free(udp_current_name);
      udp_current_name = NULL;
    }
    
			udp_start_line = zzline;
  }
 zzCONSUME;

#line 1261 "./verilog.g"
  v_name_of_udp();
#line 1262 "./verilog.g"
  
  {
    JRB node = jrb_find_str(modname_tree, zzaArg(zztasp1,2 ).symbol->name);
    if((!node)||((node)&&(!node->val.i)))
    {
      add_string_to_tree(modname_tree, zzaArg(zztasp1,2 ).symbol->name, TRUE);
      udp_current_name = strdup(zzaArg(zztasp1,2 ).symbol->name);
    }
    else
    {
      warn("** Warning: skipping duplicate UDP '%s' at in file '%s' line %d\n", zzaArg(zztasp1,2 ).symbol->name, zzfilename, zzline);
      module_is_duplicate = 1;
    }
  }
#line 1278 "./verilog.g"
  zzmatch(V_LP); zzCONSUME;
#line 1278 "./verilog.g"
  v_name_of_variable();
#line 1279 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (LA(1)==V_COMMA) ) {
#line 1279 "./verilog.g"
      zzmatch(V_COMMA); zzCONSUME;
#line 1279 "./verilog.g"
      v_name_of_variable();
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
#line 1279 "./verilog.g"
  zzmatch(V_RP); zzCONSUME;
#line 1279 "./verilog.g"
  zzmatch(V_SEMI); zzCONSUME;
#line 1280 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    int zzcnt=1;
    zzMake0;
    {
    do {
#line 1280 "./verilog.g"
      v_udp_declaration();
      zzLOOP(zztasp2);
    } while ( (setwd3[LA(1)]&0x8)
 );
    zzEXIT(zztasp2);
    }
  }
#line 1281 "./verilog.g"
  v_udp_initial_statement();
#line 1282 "./verilog.g"
  v_table_definition();
#line 1283 "./verilog.g"
  zzmatch(V_ENDPRIMITIVE);
#line 1284 "./verilog.g"
  
  {
    if(sym_base)
    {
      if(module_is_duplicate)
      {
        JRB symtree = sym_base->symtable;
        struct i_symbol_scope *sb = sym_base->parent;
        JRB node;
        
						jrb_traverse(node, symtree)
        {
          free(((struct i_symbol *)node->val.v)->name);
          free(node->val.v);	/* free up strings for any stray symbols (should be only modname) */
        }
        jrb_free_tree(symtree);						
        free(sym_base);
        sym_base = sb;
      }
      else
      {
        sym_base = sym_base->parent;
        
						if(emit_stems)
        {
          printf("++ udp %s file %s lines %d - %d\n",
          udp_current_name, udp_current_filename, udp_start_line, zzline);
        }
        
						if(udp_current_filename)
        {
          free(udp_current_filename);
          udp_current_filename = NULL;
        }
        
						if(udp_current_name)
        {
          free(udp_current_name);
          udp_current_name = NULL;
        }
      }
    }
  }
 zzCONSUME;

  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x10);
  }
}

void
#ifdef __USE_PROTOS
v_udp_declaration(void)
#else
v_udp_declaration()
#endif
{
#line 1330 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_OUTPUT) ) {
#line 1330 "./verilog.g"
    v_output_declaration();
  }
  else {
    if ( (LA(1)==V_REG) ) {
#line 1331 "./verilog.g"
      v_reg_declaration();
    }
    else {
      if ( (LA(1)==V_INPUT) ) {
#line 1332 "./verilog.g"
        v_input_declaration();
      }
      else {zzFAIL(1,zzerr9,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x20);
  }
}

void
#ifdef __USE_PROTOS
v_udp_initial_statement(void)
#else
v_udp_initial_statement()
#endif
{
#line 1335 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_INITIAL) ) {
#line 1335 "./verilog.g"
    zzmatch(V_INITIAL); zzCONSUME;
#line 1335 "./verilog.g"
    v_output_terminal_name();
#line 1335 "./verilog.g"
    zzmatch(V_EQ); zzCONSUME;
#line 1335 "./verilog.g"
    v_init_val();
#line 1335 "./verilog.g"
    zzmatch(V_SEMI); zzCONSUME;
  }
  else {
    if ( (LA(1)==V_TABLE)
 ) {
    }
    else {zzFAIL(1,zzerr10,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x40);
  }
}

void
#ifdef __USE_PROTOS
v_init_val(void)
#else
v_init_val()
#endif
{
#line 1340 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_DECIMAL_NUMBER) ) {
#line 1340 "./verilog.g"
    zzmatch(V_DECIMAL_NUMBER); zzCONSUME;
  }
  else {
    if ( (LA(1)==V_BBASE) ) {
#line 1341 "./verilog.g"
      zzmatch(V_BBASE);
#line 1341 "./verilog.g"
      zzaRet.num = i_number_basemake(NV_BBASE, zzaArg(zztasp1,1 ).text);
 zzCONSUME;

    }
    else {zzFAIL(1,zzerr11,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x80);
  }
}

void
#ifdef __USE_PROTOS
v_output_terminal_name(void)
#else
v_output_terminal_name()
#endif
{
#line 1344 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1344 "./verilog.g"
  v_name_of_variable();
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd4, 0x1);
  }
}

void
#ifdef __USE_PROTOS
v_table_definition(void)
#else
v_table_definition()
#endif
{
#line 1347 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1347 "./verilog.g"
  zzmatch(V_TABLE); zzCONSUME;
#line 1347 "./verilog.g"
  v_table_entries();
#line 1347 "./verilog.g"
  zzmatch(V_ENDTABLE); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd4, 0x2);
  }
}

void
#ifdef __USE_PROTOS
v_table_entries(void)
#else
v_table_entries()
#endif
{
#line 1350 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1350 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    int zzcnt=1;
    zzMake0;
    {
    do {
#line 1350 "./verilog.g"
      v_com_seq_entry();
      zzLOOP(zztasp2);
    } while ( (setwd4[LA(1)]&0x4) );
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd4, 0x8);
  }
}

void
#ifdef __USE_PROTOS
v_com_seq_entry(void)
#else
v_com_seq_entry()
#endif
{
#line 1353 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1353 "./verilog.g"
  v_input_list();
#line 1353 "./verilog.g"
  zzmatch(V_COLON); zzCONSUME;
#line 1353 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (setwd4[LA(1)]&0x10) && (LA(2)==V_COLON) ) {
#line 1353 "./verilog.g"
      v_state();
#line 1353 "./verilog.g"
      zzmatch(V_COLON); zzCONSUME;
#line 1353 "./verilog.g"
      v_next_state();
    }
    else {
      if ( (LA(1)==V_OUTPUT_SYMBOL) && 
(LA(2)==V_SEMI) ) {
#line 1354 "./verilog.g"
        zzmatch(V_OUTPUT_SYMBOL); zzCONSUME;
      }
      else {zzFAIL(2,zzerr12,zzerr13,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
#line 1354 "./verilog.g"
  zzmatch(V_SEMI); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd4, 0x20);
  }
}

void
#ifdef __USE_PROTOS
v_level_symbol(void)
#else
v_level_symbol()
#endif
{
#line 1357 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_OUTPUT_SYMBOL) ) {
#line 1357 "./verilog.g"
    zzmatch(V_OUTPUT_SYMBOL); zzCONSUME;
  }
  else {
    if ( (LA(1)==V_LEVEL_SYMBOL_EXTRA) ) {
#line 1358 "./verilog.g"
      zzmatch(V_LEVEL_SYMBOL_EXTRA); zzCONSUME;
    }
    else {zzFAIL(1,zzerr14,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd4, 0x40);
  }
}

void
#ifdef __USE_PROTOS
v_edge(void)
#else
v_edge()
#endif
{
#line 1361 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_LP) ) {
#line 1361 "./verilog.g"
    zzmatch(V_LP); zzCONSUME;
#line 1361 "./verilog.g"
    v_fake_edge();
#line 1361 "./verilog.g"
    zzmatch(V_RP); zzCONSUME;
  }
  else {
    if ( (LA(1)==V_EDGE_SYMBOL) ) {
#line 1362 "./verilog.g"
      zzmatch(V_EDGE_SYMBOL); zzCONSUME;
    }
    else {zzFAIL(1,zzerr15,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd4, 0x80);
  }
}

void
#ifdef __USE_PROTOS
v_fake_edge(void)
#else
v_fake_edge()
#endif
{
#line 1366 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (setwd5[LA(1)]&0x1)
 ) {
#line 1366 "./verilog.g"
    v_level_symbol();
#line 1366 "./verilog.g"
    v_level_symbol();
  }
  else {
    if ( (LA(1)==V_IDENTIFIER) ) {
#line 1367 "./verilog.g"
      zzmatch(V_IDENTIFIER); zzCONSUME;
    }
    else {zzFAIL(1,zzerr16,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd5, 0x2);
  }
}

void
#ifdef __USE_PROTOS
v_level_symbol2(void)
#else
v_level_symbol2()
#endif
{
#line 1370 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (setwd5[LA(1)]&0x4) ) {
#line 1370 "./verilog.g"
    v_level_symbol();
  }
  else {
    if ( (setwd5[LA(1)]&0x8) ) {
#line 1371 "./verilog.g"
      v_edge();
    }
    else {zzFAIL(1,zzerr17,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd5, 0x10);
  }
}

void
#ifdef __USE_PROTOS
v_input_list(void)
#else
v_input_list()
#endif
{
#line 1374 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1374 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (setwd5[LA(1)]&0x20) ) {
#line 1374 "./verilog.g"
      v_level_symbol2();
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd5, 0x40);
  }
}

void
#ifdef __USE_PROTOS
v_state(void)
#else
v_state()
#endif
{
#line 1378 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1378 "./verilog.g"
  v_level_symbol();
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd5, 0x80);
  }
}

void
#ifdef __USE_PROTOS
v_next_state(void)
#else
v_next_state()
#endif
{
#line 1381 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_OUTPUT_SYMBOL)
 ) {
#line 1381 "./verilog.g"
    zzmatch(V_OUTPUT_SYMBOL); zzCONSUME;
  }
  else {
    if ( (LA(1)==V_HYPHEN) ) {
#line 1382 "./verilog.g"
      zzmatch(V_HYPHEN); zzCONSUME;
    }
    else {zzFAIL(1,zzerr18,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd6, 0x1);
  }
}

void
#ifdef __USE_PROTOS
v_task(void)
#else
v_task()
#endif
{
#line 1385 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1385 "./verilog.g"
  zzmatch(V_TASK);
#line 1386 "./verilog.g"
  { 
    struct i_symbol_scope *sb = (struct i_symbol_scope *)calloc(1, sizeof(struct i_symbol_scope)); 
    sb->symtable = make_jrb(); 
    sb->parent = sym_base;
    sym_base = sb;
  }
 zzCONSUME;

#line 1393 "./verilog.g"
  v_identifier_nodot();
#line 1393 "./verilog.g"
  zzmatch(V_SEMI); zzCONSUME;
#line 1394 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (setwd6[LA(1)]&0x2) ) {
#line 1394 "./verilog.g"
      v_tf_declaration();
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
#line 1395 "./verilog.g"
  v_statement_or_null();
#line 1395 "./verilog.g"
  zzmatch(V_ENDTASK);
#line 1396 "./verilog.g"
  if(sym_base) sym_base = sym_base->parent;
 zzCONSUME;

  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd6, 0x4);
  }
}

void
#ifdef __USE_PROTOS
v_function(void)
#else
v_function()
#endif
{
#line 1399 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1399 "./verilog.g"
  zzmatch(V_FUNCTION);
#line 1400 "./verilog.g"
  { 
    struct i_symbol_scope *sb = (struct i_symbol_scope *)calloc(1, sizeof(struct i_symbol_scope)); 
    sb->symtable = make_jrb(); 
    sb->parent = sym_base;
    sym_base = sb;
  }
 zzCONSUME;

#line 1407 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (setwd6[LA(1)]&0x8) ) {
#line 1407 "./verilog.g"
      v_range_or_type();
    }
    else {
      if ( (setwd6[LA(1)]&0x10) ) {
      }
      else {zzFAIL(1,zzerr19,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
#line 1407 "./verilog.g"
  v_identifier_nodot();
#line 1407 "./verilog.g"
  zzmatch(V_SEMI); zzCONSUME;
#line 1408 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    int zzcnt=1;
    zzMake0;
    {
    do {
#line 1408 "./verilog.g"
      v_tf_declaration();
      zzLOOP(zztasp2);
    } while ( (setwd6[LA(1)]&0x20)
 );
    zzEXIT(zztasp2);
    }
  }
#line 1409 "./verilog.g"
  v_statement();
#line 1409 "./verilog.g"
  zzmatch(V_ENDFUNCTION);
#line 1410 "./verilog.g"
  if(sym_base) sym_base = sym_base->parent;
 zzCONSUME;

  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd6, 0x40);
  }
}

void
#ifdef __USE_PROTOS
v_range_or_type(void)
#else
v_range_or_type()
#endif
{
#line 1413 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_LBRACK) ) {
#line 1413 "./verilog.g"
    v_range();
  }
  else {
    if ( (LA(1)==V_INTEGER) ) {
#line 1414 "./verilog.g"
      zzmatch(V_INTEGER); zzCONSUME;
    }
    else {
      if ( (LA(1)==V_REAL) ) {
#line 1415 "./verilog.g"
        zzmatch(V_REAL); zzCONSUME;
      }
      else {zzFAIL(1,zzerr20,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd6, 0x80);
  }
}

void
#ifdef __USE_PROTOS
v_tf_declaration(void)
#else
v_tf_declaration()
#endif
{
#line 1418 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_PARAMETER) ) {
#line 1418 "./verilog.g"
    v_parameter_declaration();
  }
  else {
    if ( (LA(1)==V_INPUT)
 ) {
#line 1419 "./verilog.g"
      v_input_declaration();
    }
    else {
      if ( (LA(1)==V_OUTPUT) ) {
#line 1420 "./verilog.g"
        v_output_declaration();
      }
      else {
        if ( (LA(1)==V_INOUT) ) {
#line 1421 "./verilog.g"
          v_inout_declaration();
        }
        else {
          if ( (LA(1)==V_REG) ) {
#line 1422 "./verilog.g"
            v_reg_declaration();
          }
          else {
            if ( (LA(1)==V_TIME) ) {
#line 1423 "./verilog.g"
              v_time_declaration();
            }
            else {
              if ( (LA(1)==V_INTEGER)
 ) {
#line 1424 "./verilog.g"
                v_integer_declaration();
              }
              else {
                if ( (LA(1)==V_REAL) ) {
#line 1425 "./verilog.g"
                  v_real_declaration();
                }
                else {zzFAIL(1,zzerr21,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
              }
            }
          }
        }
      }
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd7, 0x1);
  }
}

void
#ifdef __USE_PROTOS
v_parameter_declaration(void)
#else
v_parameter_declaration()
#endif
{
#line 1432 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1432 "./verilog.g"
  zzmatch(V_PARAMETER); zzCONSUME;
#line 1432 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==V_LBRACK) ) {
#line 1432 "./verilog.g"
      v_range();
    }
    else {
      if ( (setwd7[LA(1)]&0x2) ) {
      }
      else {zzFAIL(1,zzerr22,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
#line 1432 "./verilog.g"
  v_list_of_param_assignments();
#line 1432 "./verilog.g"
  zzmatch(V_SEMI); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd7, 0x4);
  }
}

void
#ifdef __USE_PROTOS
v_param_assignment(void)
#else
v_param_assignment()
#endif
{
#line 1435 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1435 "./verilog.g"
  v_identifier();
#line 1435 "./verilog.g"
  zzmatch(V_EQ); zzCONSUME;
#line 1435 "./verilog.g"
  v_constant_expression();
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd7, 0x8);
  }
}

void
#ifdef __USE_PROTOS
v_list_of_param_assignments(void)
#else
v_list_of_param_assignments()
#endif
{
#line 1438 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1438 "./verilog.g"
  v_param_assignment();
#line 1439 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (LA(1)==V_COMMA) ) {
#line 1439 "./verilog.g"
      zzmatch(V_COMMA); zzCONSUME;
#line 1439 "./verilog.g"
      v_param_assignment();
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd7, 0x10);
  }
}

void
#ifdef __USE_PROTOS
v_input_declaration(void)
#else
v_input_declaration()
#endif
{
#line 1442 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1442 "./verilog.g"
  zzmatch(V_INPUT); zzCONSUME;
#line 1442 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (setwd7[LA(1)]&0x20)
 ) {
#line 1442 "./verilog.g"
      v_list_of_variables();
#line 1442 "./verilog.g"
      zzmatch(V_SEMI); zzCONSUME;
    }
    else {
      if ( (LA(1)==V_LBRACK) ) {
#line 1443 "./verilog.g"
        v_range();
#line 1443 "./verilog.g"
        v_list_of_variables();
#line 1443 "./verilog.g"
        zzmatch(V_SEMI); zzCONSUME;
      }
      else {zzFAIL(1,zzerr23,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd7, 0x40);
  }
}

void
#ifdef __USE_PROTOS
v_output_declaration(void)
#else
v_output_declaration()
#endif
{
#line 1446 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1446 "./verilog.g"
  zzmatch(V_OUTPUT); zzCONSUME;
#line 1446 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (setwd7[LA(1)]&0x80) ) {
#line 1446 "./verilog.g"
      v_list_of_variables();
#line 1446 "./verilog.g"
      zzmatch(V_SEMI); zzCONSUME;
    }
    else {
      if ( (LA(1)==V_LBRACK) ) {
#line 1447 "./verilog.g"
        v_range();
#line 1447 "./verilog.g"
        v_list_of_variables();
#line 1447 "./verilog.g"
        zzmatch(V_SEMI); zzCONSUME;
      }
      else {zzFAIL(1,zzerr24,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd8, 0x1);
  }
}

void
#ifdef __USE_PROTOS
v_inout_declaration(void)
#else
v_inout_declaration()
#endif
{
#line 1450 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1450 "./verilog.g"
  zzmatch(V_INOUT); zzCONSUME;
#line 1450 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (setwd8[LA(1)]&0x2) ) {
#line 1450 "./verilog.g"
      v_list_of_variables();
#line 1450 "./verilog.g"
      zzmatch(V_SEMI); zzCONSUME;
    }
    else {
      if ( (LA(1)==V_LBRACK)
 ) {
#line 1451 "./verilog.g"
        v_range();
#line 1451 "./verilog.g"
        v_list_of_variables();
#line 1451 "./verilog.g"
        zzmatch(V_SEMI); zzCONSUME;
      }
      else {zzFAIL(1,zzerr25,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd8, 0x4);
  }
}

void
#ifdef __USE_PROTOS
v_net_chg(void)
#else
v_net_chg()
#endif
{
#line 1454 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_LP) ) {
#line 1454 "./verilog.g"
    v_charge_strength();
  }
  else {
    if ( (setwd8[LA(1)]&0x8) ) {
    }
    else {zzFAIL(1,zzerr26,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd8, 0x10);
  }
}

void
#ifdef __USE_PROTOS
v_nettype(void)
#else
v_nettype()
#endif
{
#line 1458 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_WIRE) ) {
#line 1458 "./verilog.g"
    zzmatch(V_WIRE); zzCONSUME;
  }
  else {
    if ( (LA(1)==V_TRI) ) {
#line 1459 "./verilog.g"
      zzmatch(V_TRI); zzCONSUME;
    }
    else {
      if ( (LA(1)==V_TRI1)
 ) {
#line 1460 "./verilog.g"
        zzmatch(V_TRI1); zzCONSUME;
      }
      else {
        if ( (LA(1)==V_SUPPLY0) ) {
#line 1461 "./verilog.g"
          zzmatch(V_SUPPLY0); zzCONSUME;
        }
        else {
          if ( (LA(1)==V_WAND) ) {
#line 1462 "./verilog.g"
            zzmatch(V_WAND); zzCONSUME;
          }
          else {
            if ( (LA(1)==V_TRIAND) ) {
#line 1463 "./verilog.g"
              zzmatch(V_TRIAND); zzCONSUME;
            }
            else {
              if ( (LA(1)==V_TRI0) ) {
#line 1464 "./verilog.g"
                zzmatch(V_TRI0); zzCONSUME;
              }
              else {
                if ( (LA(1)==V_SUPPLY1)
 ) {
#line 1465 "./verilog.g"
                  zzmatch(V_SUPPLY1); zzCONSUME;
                }
                else {
                  if ( (LA(1)==V_WOR) ) {
#line 1466 "./verilog.g"
                    zzmatch(V_WOR); zzCONSUME;
                  }
                  else {
                    if ( (LA(1)==V_TRIOR) ) {
#line 1467 "./verilog.g"
                      zzmatch(V_TRIOR); zzCONSUME;
                    }
                    else {
                      if ( (LA(1)==V_TRIREG) ) {
#line 1468 "./verilog.g"
                        zzmatch(V_TRIREG); zzCONSUME;
                      }
                      else {zzFAIL(1,zzerr27,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd8, 0x20);
  }
}

void
#ifdef __USE_PROTOS
v_expandrange(void)
#else
v_expandrange()
#endif
{
#line 1471 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_LBRACK) ) {
#line 1471 "./verilog.g"
    v_range();
  }
  else {
    if ( (LA(1)==V_SCALARED)
 ) {
#line 1472 "./verilog.g"
      zzmatch(V_SCALARED); zzCONSUME;
#line 1472 "./verilog.g"
      v_range();
    }
    else {
      if ( (LA(1)==V_VECTORED) ) {
#line 1473 "./verilog.g"
        zzmatch(V_VECTORED); zzCONSUME;
#line 1473 "./verilog.g"
        v_range();
      }
      else {zzFAIL(1,zzerr28,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd8, 0x40);
  }
}

void
#ifdef __USE_PROTOS
v_reg_declaration(void)
#else
v_reg_declaration()
#endif
{
#line 1476 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1476 "./verilog.g"
  zzmatch(V_REG); zzCONSUME;
#line 1476 "./verilog.g"
  v_reg_range();
#line 1476 "./verilog.g"
  v_list_of_register_variables();
#line 1476 "./verilog.g"
  zzmatch(V_SEMI); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd8, 0x80);
  }
}

void
#ifdef __USE_PROTOS
v_reg_range(void)
#else
v_reg_range()
#endif
{
#line 1479 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_LBRACK) ) {
#line 1479 "./verilog.g"
    v_range();
  }
  else {
    if ( (setwd9[LA(1)]&0x1) ) {
    }
    else {zzFAIL(1,zzerr29,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd9, 0x2);
  }
}

void
#ifdef __USE_PROTOS
v_time_declaration(void)
#else
v_time_declaration()
#endif
{
#line 1483 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1483 "./verilog.g"
  zzmatch(V_TIME); zzCONSUME;
#line 1483 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==V_LBRACK) ) {
#line 1483 "./verilog.g"
      v_range();
    }
    else {
      if ( (setwd9[LA(1)]&0x4)
 ) {
      }
      else {zzFAIL(1,zzerr30,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
#line 1483 "./verilog.g"
  v_list_of_register_variables();
#line 1483 "./verilog.g"
  zzmatch(V_SEMI); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd9, 0x8);
  }
}

void
#ifdef __USE_PROTOS
v_integer_declaration(void)
#else
v_integer_declaration()
#endif
{
#line 1486 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1486 "./verilog.g"
  zzmatch(V_INTEGER); zzCONSUME;
#line 1486 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==V_LBRACK) ) {
#line 1486 "./verilog.g"
      v_range();
    }
    else {
      if ( (setwd9[LA(1)]&0x10) ) {
      }
      else {zzFAIL(1,zzerr31,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
#line 1486 "./verilog.g"
  v_list_of_register_variables();
#line 1486 "./verilog.g"
  zzmatch(V_SEMI); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd9, 0x20);
  }
}

void
#ifdef __USE_PROTOS
v_real_declaration(void)
#else
v_real_declaration()
#endif
{
#line 1489 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1489 "./verilog.g"
  zzmatch(V_REAL); zzCONSUME;
#line 1489 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==V_LBRACK) ) {
#line 1489 "./verilog.g"
      v_range();
    }
    else {
      if ( (setwd9[LA(1)]&0x40) ) {
      }
      else {zzFAIL(1,zzerr32,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
#line 1489 "./verilog.g"
  v_list_of_variables();
#line 1489 "./verilog.g"
  zzmatch(V_SEMI); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd9, 0x80);
  }
}

void
#ifdef __USE_PROTOS
v_event_declaration(void)
#else
v_event_declaration()
#endif
{
#line 1492 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1492 "./verilog.g"
  zzmatch(V_EVENT); zzCONSUME;
#line 1492 "./verilog.g"
  v_name_of_event();
#line 1492 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (LA(1)==V_COMMA)
 ) {
#line 1492 "./verilog.g"
      zzmatch(V_COMMA); zzCONSUME;
#line 1492 "./verilog.g"
      v_name_of_event();
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
#line 1492 "./verilog.g"
  zzmatch(V_SEMI); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd10, 0x1);
  }
}

void
#ifdef __USE_PROTOS
v_continuous_assign(void)
#else
v_continuous_assign()
#endif
{
#line 1495 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_ASSIGN) ) {
#line 1495 "./verilog.g"
    zzmatch(V_ASSIGN); zzCONSUME;
#line 1495 "./verilog.g"
    v_cont_drv();
#line 1495 "./verilog.g"
    v_cont_dly();
#line 1495 "./verilog.g"
    v_list_of_assignments();
#line 1495 "./verilog.g"
    zzmatch(V_SEMI); zzCONSUME;
  }
  else {
    if ( (setwd10[LA(1)]&0x2) ) {
#line 1496 "./verilog.g"
      v_nettype();
#line 1497 "./verilog.g"
      v_net_chg();
#line 1498 "./verilog.g"
      v_cont_exr();
#line 1498 "./verilog.g"
      v_cont_dly();
#line 1499 "./verilog.g"
      {
        zzBLOCK(zztasp2);
        zzMake0;
        {
        if ( (setwd10[LA(1)]&0x4) && (setwd10[LA(2)]&0x8) && !(
 LA(1)==V_IDENTIFIER && LA(2)==V_COMMA
|| LA(1)==V_IDENTIFIER2 && LA(2)==V_COMMA
|| LA(1)==V_FUNCTION_NAME && LA(2)==V_COMMA
|| LA(1)==V_IDENDOT && LA(2)==V_COMMA
) ) {
#line 1499 "./verilog.g"
          v_list_of_assignments();
        }
        else {
          if ( (setwd10[LA(1)]&0x10) && (setwd10[LA(2)]&0x20) ) {
#line 1499 "./verilog.g"
            v_list_of_variables();
          }
          else {zzFAIL(2,zzerr33,zzerr34,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
        }
        zzEXIT(zztasp2);
        }
      }
#line 1499 "./verilog.g"
      zzmatch(V_SEMI); zzCONSUME;
    }
    else {zzFAIL(1,zzerr35,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd10, 0x40);
  }
}

void
#ifdef __USE_PROTOS
v_cont_drv(void)
#else
v_cont_drv()
#endif
{
#line 1502 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_LP)
 ) {
#line 1502 "./verilog.g"
    v_drive_strength();
  }
  else {
    if ( (setwd10[LA(1)]&0x80) ) {
    }
    else {zzFAIL(1,zzerr36,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd11, 0x1);
  }
}

void
#ifdef __USE_PROTOS
v_cont_exr(void)
#else
v_cont_exr()
#endif
{
#line 1506 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (setwd11[LA(1)]&0x2) ) {
#line 1506 "./verilog.g"
    v_expandrange();
  }
  else {
    if ( (setwd11[LA(1)]&0x4) ) {
    }
    else {zzFAIL(1,zzerr37,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd11, 0x8);
  }
}

void
#ifdef __USE_PROTOS
v_cont_dly(void)
#else
v_cont_dly()
#endif
{
#line 1510 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_POUND) ) {
#line 1510 "./verilog.g"
    v_delay();
  }
  else {
    if ( (setwd11[LA(1)]&0x10)
 ) {
    }
    else {zzFAIL(1,zzerr38,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd11, 0x20);
  }
}

void
#ifdef __USE_PROTOS
v_parameter_override(void)
#else
v_parameter_override()
#endif
{
#line 1515 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1515 "./verilog.g"
  zzmatch(V_DEFPARAM); zzCONSUME;
#line 1515 "./verilog.g"
  v_list_of_param_assignments();
#line 1515 "./verilog.g"
  zzmatch(V_SEMI); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd11, 0x40);
  }
}

void
#ifdef __USE_PROTOS
v_list_of_variables(void)
#else
v_list_of_variables()
#endif
{
#line 1518 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1518 "./verilog.g"
  v_name_of_variable();
#line 1519 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (LA(1)==V_COMMA) ) {
#line 1519 "./verilog.g"
      zzmatch(V_COMMA); zzCONSUME;
#line 1519 "./verilog.g"
      v_name_of_variable();
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd11, 0x80);
  }
}

void
#ifdef __USE_PROTOS
v_name_of_variable(void)
#else
v_name_of_variable()
#endif
{
#line 1522 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1522 "./verilog.g"
  v_identifier();
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd12, 0x1);
  }
}

void
#ifdef __USE_PROTOS
v_list_of_register_variables(void)
#else
v_list_of_register_variables()
#endif
{
#line 1525 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1525 "./verilog.g"
  v_register_variable();
#line 1526 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (LA(1)==V_COMMA) ) {
#line 1526 "./verilog.g"
      zzmatch(V_COMMA); zzCONSUME;
#line 1526 "./verilog.g"
      v_register_variable();
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd12, 0x2);
  }
}

void
#ifdef __USE_PROTOS
v_register_variable(void)
#else
v_register_variable()
#endif
{
#line 1529 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (setwd12[LA(1)]&0x4) && (setwd12[LA(2)]&0x8) ) {
#line 1529 "./verilog.g"
    v_name_of_register();
  }
  else {
    if ( (setwd12[LA(1)]&0x10) && (LA(2)==V_LBRACK) ) {
#line 1530 "./verilog.g"
      v_name_of_memory();
#line 1530 "./verilog.g"
      zzmatch(V_LBRACK); zzCONSUME;
#line 1530 "./verilog.g"
      v_expression();
#line 1530 "./verilog.g"
      zzmatch(V_COLON); zzCONSUME;
#line 1531 "./verilog.g"
      v_expression();
#line 1531 "./verilog.g"
      zzmatch(V_RBRACK); zzCONSUME;
    }
    else {zzFAIL(2,zzerr39,zzerr40,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd12, 0x20);
  }
}

void
#ifdef __USE_PROTOS
v_name_of_register(void)
#else
v_name_of_register()
#endif
{
#line 1534 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1534 "./verilog.g"
  v_identifier();
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd12, 0x40);
  }
}

void
#ifdef __USE_PROTOS
v_name_of_memory(void)
#else
v_name_of_memory()
#endif
{
#line 1537 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1537 "./verilog.g"
  v_identifier();
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd12, 0x80);
  }
}

void
#ifdef __USE_PROTOS
v_name_of_event(void)
#else
v_name_of_event()
#endif
{
#line 1540 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1540 "./verilog.g"
  v_identifier();
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd13, 0x1);
  }
}

void
#ifdef __USE_PROTOS
v_charge_strength(void)
#else
v_charge_strength()
#endif
{
#line 1543 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1543 "./verilog.g"
  zzmatch(V_LP); zzCONSUME;
#line 1543 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==V_SMALL)
 ) {
#line 1543 "./verilog.g"
      zzmatch(V_SMALL); zzCONSUME;
    }
    else {
      if ( (LA(1)==V_MEDIUM) ) {
#line 1544 "./verilog.g"
        zzmatch(V_MEDIUM); zzCONSUME;
      }
      else {
        if ( (LA(1)==V_LARGE) ) {
#line 1545 "./verilog.g"
          zzmatch(V_LARGE); zzCONSUME;
        }
        else {
          if ( (setwd13[LA(1)]&0x2) ) {
#line 1546 "./verilog.g"
            v_strength0();
#line 1546 "./verilog.g"
            zzmatch(V_COMMA); zzCONSUME;
#line 1546 "./verilog.g"
            v_strength1();
          }
          else {
            if ( (setwd13[LA(1)]&0x4) ) {
#line 1547 "./verilog.g"
              v_strength1();
#line 1547 "./verilog.g"
              zzmatch(V_COMMA); zzCONSUME;
#line 1547 "./verilog.g"
              v_strength0();
            }
            else {zzFAIL(1,zzerr41,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
          }
        }
      }
    }
    zzEXIT(zztasp2);
    }
  }
#line 1547 "./verilog.g"
  zzmatch(V_RP); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd13, 0x8);
  }
}

void
#ifdef __USE_PROTOS
v_drive_strength(void)
#else
v_drive_strength()
#endif
{
#line 1550 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1550 "./verilog.g"
  zzmatch(V_LP); zzCONSUME;
#line 1550 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (setwd13[LA(1)]&0x10)
 ) {
#line 1550 "./verilog.g"
      v_strength0();
#line 1550 "./verilog.g"
      zzmatch(V_COMMA); zzCONSUME;
#line 1550 "./verilog.g"
      v_strength1();
    }
    else {
      if ( (setwd13[LA(1)]&0x20) ) {
#line 1551 "./verilog.g"
        v_strength1();
#line 1551 "./verilog.g"
        zzmatch(V_COMMA); zzCONSUME;
#line 1551 "./verilog.g"
        v_strength0();
      }
      else {zzFAIL(1,zzerr42,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
#line 1551 "./verilog.g"
  zzmatch(V_RP); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd13, 0x40);
  }
}

void
#ifdef __USE_PROTOS
v_strength0(void)
#else
v_strength0()
#endif
{
#line 1554 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_SUPPLY0) ) {
#line 1554 "./verilog.g"
    zzmatch(V_SUPPLY0); zzCONSUME;
  }
  else {
    if ( (LA(1)==V_STRONG0) ) {
#line 1555 "./verilog.g"
      zzmatch(V_STRONG0); zzCONSUME;
    }
    else {
      if ( (LA(1)==V_PULL0) ) {
#line 1556 "./verilog.g"
        zzmatch(V_PULL0); zzCONSUME;
      }
      else {
        if ( (LA(1)==V_WEAK0)
 ) {
#line 1557 "./verilog.g"
          zzmatch(V_WEAK0); zzCONSUME;
        }
        else {
          if ( (LA(1)==V_HIGHZ0) ) {
#line 1558 "./verilog.g"
            zzmatch(V_HIGHZ0); zzCONSUME;
          }
          else {zzFAIL(1,zzerr43,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
        }
      }
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd13, 0x80);
  }
}

void
#ifdef __USE_PROTOS
v_strength1(void)
#else
v_strength1()
#endif
{
#line 1561 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_SUPPLY1) ) {
#line 1561 "./verilog.g"
    zzmatch(V_SUPPLY1); zzCONSUME;
  }
  else {
    if ( (LA(1)==V_STRONG1) ) {
#line 1562 "./verilog.g"
      zzmatch(V_STRONG1); zzCONSUME;
    }
    else {
      if ( (LA(1)==V_PULL1) ) {
#line 1563 "./verilog.g"
        zzmatch(V_PULL1); zzCONSUME;
      }
      else {
        if ( (LA(1)==V_WEAK1)
 ) {
#line 1564 "./verilog.g"
          zzmatch(V_WEAK1); zzCONSUME;
        }
        else {
          if ( (LA(1)==V_HIGHZ1) ) {
#line 1565 "./verilog.g"
            zzmatch(V_HIGHZ1); zzCONSUME;
          }
          else {zzFAIL(1,zzerr44,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
        }
      }
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd14, 0x1);
  }
}

void
#ifdef __USE_PROTOS
v_range(void)
#else
v_range()
#endif
{
#line 1568 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1568 "./verilog.g"
  zzmatch(V_LBRACK); zzCONSUME;
#line 1568 "./verilog.g"
  v_expression();
#line 1569 "./verilog.g"
  zzmatch(V_COLON); zzCONSUME;
#line 1569 "./verilog.g"
  v_expression();
#line 1569 "./verilog.g"
  zzmatch(V_RBRACK); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd14, 0x2);
  }
}

void
#ifdef __USE_PROTOS
v_list_of_assignments(void)
#else
v_list_of_assignments()
#endif
{
#line 1572 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1572 "./verilog.g"
  v_assignment();
#line 1572 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (LA(1)==V_COMMA) ) {
#line 1572 "./verilog.g"
      zzmatch(V_COMMA); zzCONSUME;
#line 1572 "./verilog.g"
      v_assignment();
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd14, 0x4);
  }
}

void
#ifdef __USE_PROTOS
v_gate_declaration(void)
#else
v_gate_declaration()
#endif
{
#line 1580 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1580 "./verilog.g"
  v_gatetype();
#line 1580 "./verilog.g"
  v_gate_drv();
#line 1580 "./verilog.g"
  v_gate_dly();
#line 1580 "./verilog.g"
  v_gate_instance();
#line 1581 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (LA(1)==V_COMMA) ) {
#line 1581 "./verilog.g"
      zzmatch(V_COMMA); zzCONSUME;
#line 1581 "./verilog.g"
      v_gate_instance();
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
#line 1581 "./verilog.g"
  zzmatch(V_SEMI); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd14, 0x8);
  }
}

void
#ifdef __USE_PROTOS
v_gatetype(void)
#else
v_gatetype()
#endif
{
#line 1583 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_ANDLIT) ) {
#line 1584 "./verilog.g"
    zzmatch(V_ANDLIT); zzCONSUME;
  }
  else {
    if ( (LA(1)==V_NANDLIT)
 ) {
#line 1584 "./verilog.g"
      zzmatch(V_NANDLIT); zzCONSUME;
    }
    else {
      if ( (LA(1)==V_ORLIT) ) {
#line 1584 "./verilog.g"
        zzmatch(V_ORLIT); zzCONSUME;
      }
      else {
        if ( (LA(1)==V_NORLIT) ) {
#line 1584 "./verilog.g"
          zzmatch(V_NORLIT); zzCONSUME;
        }
        else {
          if ( (LA(1)==V_XORLIT) ) {
#line 1584 "./verilog.g"
            zzmatch(V_XORLIT); zzCONSUME;
          }
          else {
            if ( (LA(1)==V_XNORLIT) ) {
#line 1584 "./verilog.g"
              zzmatch(V_XNORLIT); zzCONSUME;
            }
            else {
              if ( (LA(1)==V_BUF)
 ) {
#line 1585 "./verilog.g"
                zzmatch(V_BUF); zzCONSUME;
              }
              else {
                if ( (LA(1)==V_BUFIF0) ) {
#line 1585 "./verilog.g"
                  zzmatch(V_BUFIF0); zzCONSUME;
                }
                else {
                  if ( (LA(1)==V_BUFIF1) ) {
#line 1585 "./verilog.g"
                    zzmatch(V_BUFIF1); zzCONSUME;
                  }
                  else {
                    if ( (LA(1)==V_NOTLIT) ) {
#line 1585 "./verilog.g"
                      zzmatch(V_NOTLIT); zzCONSUME;
                    }
                    else {
                      if ( (LA(1)==V_NOTIF0) ) {
#line 1585 "./verilog.g"
                        zzmatch(V_NOTIF0); zzCONSUME;
                      }
                      else {
                        if ( (LA(1)==V_NOTIF1)
 ) {
#line 1585 "./verilog.g"
                          zzmatch(V_NOTIF1); zzCONSUME;
                        }
                        else {
                          if ( (LA(1)==V_PULLDOWN) ) {
#line 1585 "./verilog.g"
                            zzmatch(V_PULLDOWN); zzCONSUME;
                          }
                          else {
                            if ( (LA(1)==V_PULLUP) ) {
#line 1585 "./verilog.g"
                              zzmatch(V_PULLUP); zzCONSUME;
                            }
                            else {
                              if ( (LA(1)==V_NMOS) ) {
#line 1586 "./verilog.g"
                                zzmatch(V_NMOS); zzCONSUME;
                              }
                              else {
                                if ( (LA(1)==V_RNMOS) ) {
#line 1586 "./verilog.g"
                                  zzmatch(V_RNMOS); zzCONSUME;
                                }
                                else {
                                  if ( (LA(1)==V_PMOS)
 ) {
#line 1586 "./verilog.g"
                                    zzmatch(V_PMOS); zzCONSUME;
                                  }
                                  else {
                                    if ( (LA(1)==V_RPMOS) ) {
#line 1586 "./verilog.g"
                                      zzmatch(V_RPMOS); zzCONSUME;
                                    }
                                    else {
                                      if ( (LA(1)==V_CMOS) ) {
#line 1586 "./verilog.g"
                                        zzmatch(V_CMOS); zzCONSUME;
                                      }
                                      else {
                                        if ( (LA(1)==V_RCMOS) ) {
#line 1586 "./verilog.g"
                                          zzmatch(V_RCMOS); zzCONSUME;
                                        }
                                        else {
                                          if ( (LA(1)==V_TRAN) ) {
#line 1586 "./verilog.g"
                                            zzmatch(V_TRAN); zzCONSUME;
                                          }
                                          else {
                                            if ( (LA(1)==V_RTRAN)
 ) {
#line 1586 "./verilog.g"
                                              zzmatch(V_RTRAN); zzCONSUME;
                                            }
                                            else {
                                              if ( (LA(1)==V_TRANIF0) ) {
#line 1587 "./verilog.g"
                                                zzmatch(V_TRANIF0); zzCONSUME;
                                              }
                                              else {
                                                if ( (LA(1)==V_RTRANIF0) ) {
#line 1587 "./verilog.g"
                                                  zzmatch(V_RTRANIF0); zzCONSUME;
                                                }
                                                else {
                                                  if ( (LA(1)==V_TRANIF1) ) {
#line 1587 "./verilog.g"
                                                    zzmatch(V_TRANIF1); zzCONSUME;
                                                  }
                                                  else {
                                                    if ( (LA(1)==V_RTRANIF1) ) {
#line 1587 "./verilog.g"
                                                      zzmatch(V_RTRANIF1); zzCONSUME;
                                                    }
                                                    else {zzFAIL(1,zzerr45,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
                                                  }
                                                }
                                              }
                                            }
                                          }
                                        }
                                      }
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd14, 0x10);
  }
}

void
#ifdef __USE_PROTOS
v_gate_drv(void)
#else
v_gate_drv()
#endif
{
#line 1590 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_LP) && 
(setwd14[LA(2)]&0x20) ) {
#line 1590 "./verilog.g"
    v_drive_strength();
  }
  else {
    if ( (setwd14[LA(1)]&0x40) && (setwd14[LA(2)]&0x80) ) {
    }
    else {zzFAIL(2,zzerr46,zzerr47,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd15, 0x1);
  }
}

void
#ifdef __USE_PROTOS
v_gate_dly(void)
#else
v_gate_dly()
#endif
{
#line 1594 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_POUND) ) {
#line 1594 "./verilog.g"
    v_delay();
  }
  else {
    if ( (setwd15[LA(1)]&0x2) ) {
    }
    else {zzFAIL(1,zzerr48,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd15, 0x4);
  }
}

void
#ifdef __USE_PROTOS
v_gate_range(void)
#else
v_gate_range()
#endif
{
#line 1598 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_LBRACK) ) {
#line 1598 "./verilog.g"
    v_range();
  }
  else {
    if ( (LA(1)==V_LP)
 ) {
    }
    else {zzFAIL(1,zzerr49,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd15, 0x8);
  }
}

void
#ifdef __USE_PROTOS
v_gate_instance(void)
#else
v_gate_instance()
#endif
{
#line 1603 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1603 "./verilog.g"
  v_name_of_gate_instance();
#line 1603 "./verilog.g"
  zzmatch(V_LP); zzCONSUME;
#line 1603 "./verilog.g"
  v_terminal();
#line 1604 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (LA(1)==V_COMMA) ) {
#line 1604 "./verilog.g"
      zzmatch(V_COMMA); zzCONSUME;
#line 1604 "./verilog.g"
      v_terminal();
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
#line 1604 "./verilog.g"
  zzmatch(V_RP); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd15, 0x10);
  }
}

void
#ifdef __USE_PROTOS
v_name_of_gate_instance(void)
#else
v_name_of_gate_instance()
#endif
{
#line 1607 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (setwd15[LA(1)]&0x20) ) {
#line 1607 "./verilog.g"
    v_identifier_nodot();
#line 1607 "./verilog.g"
    v_gate_range();
  }
  else {
    if ( (LA(1)==V_LP) ) {
    }
    else {zzFAIL(1,zzerr50,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd15, 0x40);
  }
}

void
#ifdef __USE_PROTOS
v_terminal(void)
#else
v_terminal()
#endif
{
#line 1611 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (setwd15[LA(1)]&0x80) ) {
#line 1611 "./verilog.g"
    v_expression();
  }
  else {
    if ( (setwd16[LA(1)]&0x1)
 ) {
    }
    else {zzFAIL(1,zzerr51,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd16, 0x2);
  }
}

void
#ifdef __USE_PROTOS
v_udp_instantiation(void)
#else
v_udp_instantiation()
#endif
{
#line 1615 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1615 "./verilog.g"
  v_name_of_udp();
#line 1615 "./verilog.g"
  v_gate_drv();
#line 1615 "./verilog.g"
  v_gate_dly();
#line 1615 "./verilog.g"
  v_udp_instance();
#line 1616 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (LA(1)==V_COMMA) ) {
#line 1616 "./verilog.g"
      zzmatch(V_COMMA); zzCONSUME;
#line 1616 "./verilog.g"
      v_udp_instance();
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
#line 1616 "./verilog.g"
  zzmatch(V_SEMI); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd16, 0x4);
  }
}

void
#ifdef __USE_PROTOS
v_name_of_udp(void)
#else
v_name_of_udp()
#endif
{
#line 1619 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1619 "./verilog.g"
  v_identifier_nodot();
#line 1620 "./verilog.g"
  zzaRet.symbol = zzaArg(zztasp1,1 ).symbol;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd16, 0x8);
  }
}

void
#ifdef __USE_PROTOS
v_udp_instance(void)
#else
v_udp_instance()
#endif
{
#line 1623 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1623 "./verilog.g"
  v_name_of_udp_instance();
#line 1623 "./verilog.g"
  zzmatch(V_LP); zzCONSUME;
#line 1623 "./verilog.g"
  v_terminal();
#line 1624 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (LA(1)==V_COMMA) ) {
#line 1624 "./verilog.g"
      zzmatch(V_COMMA); zzCONSUME;
#line 1624 "./verilog.g"
      v_terminal();
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
#line 1624 "./verilog.g"
  zzmatch(V_RP); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd16, 0x10);
  }
}

void
#ifdef __USE_PROTOS
v_name_of_udp_instance(void)
#else
v_name_of_udp_instance()
#endif
{
#line 1627 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (setwd16[LA(1)]&0x20) ) {
#line 1627 "./verilog.g"
    v_identifier_nodot();
#line 1627 "./verilog.g"
    v_gate_range();
  }
  else {
    if ( (LA(1)==V_LP) ) {
    }
    else {zzFAIL(1,zzerr52,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd16, 0x40);
  }
}

void
#ifdef __USE_PROTOS
v_module_instantiation(void)
#else
v_module_instantiation()
#endif
{
#line 1635 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1635 "./verilog.g"
  v_name_of_module();
#line 1636 "./verilog.g"
  
  if(!module_is_duplicate)
  {
    add_string_to_tree(modname_tree, zzaArg(zztasp1,1 ).symbol->name, FALSE);
    if(comp_type_name)
    {
      free(comp_type_name);
    }
    comp_type_name = strdup(zzaArg(zztasp1,1 ).symbol->name);
  } /* to keep transitive closure from looking for nonexistant modules if duplicate modules differ! */
#line 1647 "./verilog.g"
  v_parameter_value_assignment();
#line 1648 "./verilog.g"
  v_module_instance();
#line 1649 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (LA(1)==V_COMMA)
 ) {
#line 1649 "./verilog.g"
      zzmatch(V_COMMA); zzCONSUME;
#line 1649 "./verilog.g"
      v_module_instance();
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
#line 1649 "./verilog.g"
  zzmatch(V_SEMI);
#line 1650 "./verilog.g"
  
  if(comp_type_name)
  {
    free(comp_type_name);
    comp_type_name = NULL;
  }
 zzCONSUME;

  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd16, 0x80);
  }
}

void
#ifdef __USE_PROTOS
v_name_of_module(void)
#else
v_name_of_module()
#endif
{
#line 1659 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1659 "./verilog.g"
  v_identifier_nodot();
#line 1659 "./verilog.g"
  zzaRet.symbol = zzaArg(zztasp1,1 ).symbol;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd17, 0x1);
  }
}

void
#ifdef __USE_PROTOS
v_parameter_value_assignment(void)
#else
v_parameter_value_assignment()
#endif
{
#line 1662 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_POUND) && (LA(2)==V_LP) ) {
#line 1663 "./verilog.g"
    zzmatch(V_POUND); zzCONSUME;
#line 1663 "./verilog.g"
    zzmatch(V_LP); zzCONSUME;
#line 1663 "./verilog.g"
    v_mexplist();
#line 1663 "./verilog.g"
    zzmatch(V_RP); zzCONSUME;
  }
  else {
    if ( (LA(1)==V_POUND) && (setwd17[LA(2)]&0x2) ) {
#line 1664 "./verilog.g"
      zzmatch(V_POUND); zzCONSUME;
#line 1664 "./verilog.g"
      v_number();
    }
    else {
      if ( (setwd17[LA(1)]&0x4) ) {
      }
      else {zzFAIL(2,zzerr53,zzerr54,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd17, 0x8);
  }
}

void
#ifdef __USE_PROTOS
v_module_instance(void)
#else
v_module_instance()
#endif
{
#line 1668 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1668 "./verilog.g"
  v_name_of_instance_opt();
#line 1668 "./verilog.g"
  zzmatch(V_LP); zzCONSUME;
#line 1668 "./verilog.g"
  v_list_of_module_connections();
#line 1668 "./verilog.g"
  zzmatch(V_RP); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd17, 0x10);
  }
}

void
#ifdef __USE_PROTOS
v_name_of_instance_opt(void)
#else
v_name_of_instance_opt()
#endif
{
#line 1672 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (setwd17[LA(1)]&0x20) ) {
#line 1672 "./verilog.g"
    v_name_of_instance();
  }
  else {
    if ( (LA(1)==V_LP)
 ) {
    }
    else {zzFAIL(1,zzerr55,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd17, 0x40);
  }
}

void
#ifdef __USE_PROTOS
v_name_of_instance(void)
#else
v_name_of_instance()
#endif
{
#line 1676 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1676 "./verilog.g"
  v_identifier_nodot();
#line 1676 "./verilog.g"
  v_mod_range();
#line 1677 "./verilog.g"
  
  if(!module_is_duplicate)
  {
    if(emit_stems)
    {
      printf("++ comp %s type %s parent %s\n", zzaArg(zztasp1,1 ).symbol->name, comp_type_name, mod_current_name);
    }	
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd17, 0x80);
  }
}

void
#ifdef __USE_PROTOS
v_mod_range(void)
#else
v_mod_range()
#endif
{
#line 1688 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_LBRACK) ) {
#line 1688 "./verilog.g"
    v_range();
  }
  else {
    if ( (LA(1)==V_LP) ) {
    }
    else {zzFAIL(1,zzerr56,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd18, 0x1);
  }
}

void
#ifdef __USE_PROTOS
v_list_of_module_connections(void)
#else
v_list_of_module_connections()
#endif
{
#line 1692 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (setwd18[LA(1)]&0x2) ) {
#line 1692 "./verilog.g"
    v_module_port_connection();
#line 1692 "./verilog.g"
    {
      zzBLOCK(zztasp2);
      zzMake0;
      {
      while ( (LA(1)==V_COMMA) ) {
#line 1692 "./verilog.g"
        zzmatch(V_COMMA); zzCONSUME;
#line 1692 "./verilog.g"
        v_module_port_connection();
        zzLOOP(zztasp2);
      }
      zzEXIT(zztasp2);
      }
    }
  }
  else {
    if ( (LA(1)==V_DOT)
 ) {
#line 1693 "./verilog.g"
      v_named_port_connection();
#line 1693 "./verilog.g"
      {
        zzBLOCK(zztasp2);
        zzMake0;
        {
        while ( (LA(1)==V_COMMA) ) {
#line 1693 "./verilog.g"
          zzmatch(V_COMMA); zzCONSUME;
#line 1693 "./verilog.g"
          v_named_port_connection();
          zzLOOP(zztasp2);
        }
        zzEXIT(zztasp2);
        }
      }
    }
    else {zzFAIL(1,zzerr57,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd18, 0x4);
  }
}

void
#ifdef __USE_PROTOS
v_module_port_connection(void)
#else
v_module_port_connection()
#endif
{
#line 1696 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (setwd18[LA(1)]&0x8) ) {
#line 1696 "./verilog.g"
    v_expression();
  }
  else {
    if ( (setwd18[LA(1)]&0x10) ) {
    }
    else {zzFAIL(1,zzerr58,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd18, 0x20);
  }
}

void
#ifdef __USE_PROTOS
v_named_port_connection(void)
#else
v_named_port_connection()
#endif
{
#line 1700 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1700 "./verilog.g"
  zzmatch(V_DOT); zzCONSUME;
#line 1700 "./verilog.g"
  v_identifier_nodot();
#line 1700 "./verilog.g"
  zzmatch(V_LP); zzCONSUME;
#line 1700 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (setwd18[LA(1)]&0x40) ) {
#line 1700 "./verilog.g"
      v_expression();
    }
    else {
      if ( (LA(1)==V_RP)
 ) {
      }
      else {zzFAIL(1,zzerr59,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
#line 1700 "./verilog.g"
  zzmatch(V_RP); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd18, 0x80);
  }
}

void
#ifdef __USE_PROTOS
v_initial_statement(void)
#else
v_initial_statement()
#endif
{
#line 1708 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1708 "./verilog.g"
  zzmatch(V_INITIAL); zzCONSUME;
#line 1708 "./verilog.g"
  v_statement();
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd19, 0x1);
  }
}

void
#ifdef __USE_PROTOS
v_always_statement(void)
#else
v_always_statement()
#endif
{
#line 1711 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1711 "./verilog.g"
  zzmatch(V_ALWAYS); zzCONSUME;
#line 1711 "./verilog.g"
  v_statement();
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd19, 0x2);
  }
}

void
#ifdef __USE_PROTOS
v_statement_or_null(void)
#else
v_statement_or_null()
#endif
{
#line 1714 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (setwd19[LA(1)]&0x4) ) {
#line 1714 "./verilog.g"
    v_statement();
  }
  else {
    if ( (LA(1)==V_SEMI) ) {
#line 1715 "./verilog.g"
      zzmatch(V_SEMI); zzCONSUME;
    }
    else {zzFAIL(1,zzerr60,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd19, 0x8);
  }
}

void
#ifdef __USE_PROTOS
v_statement(void)
#else
v_statement()
#endif
{
#line 1718 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (setwd19[LA(1)]&0x10) && (setwd19[LA(2)]&0x20) && !(
 LA(1)==V_IDENTIFIER && LA(2)==V_LP
|| LA(1)==V_IDENTIFIER2 && LA(2)==V_LP
|| LA(1)==V_FUNCTION_NAME && LA(2)==V_LP
|| LA(1)==V_IDENDOT && LA(2)==V_LP
) ) {
#line 1718 "./verilog.g"
    v_block_or_non_assignment();
#line 1718 "./verilog.g"
    zzmatch(V_SEMI); zzCONSUME;
  }
  else {
    if ( (LA(1)==V_IF) ) {
#line 1719 "./verilog.g"
      zzmatch(V_IF); zzCONSUME;
#line 1719 "./verilog.g"
      zzmatch(V_LP); zzCONSUME;
#line 1719 "./verilog.g"
      v_expression();
#line 1719 "./verilog.g"
      zzmatch(V_RP); zzCONSUME;
#line 1719 "./verilog.g"
      v_statement_or_null();
#line 1720 "./verilog.g"
      {
        zzBLOCK(zztasp2);
        zzMake0;
        {
        if ( (LA(1)==V_ELSE) && 
(setwd19[LA(2)]&0x40) ) {
#line 1720 "./verilog.g"
          zzmatch(V_ELSE); zzCONSUME;
#line 1720 "./verilog.g"
          v_statement_or_null();
        }
        else {
          if ( (setwd19[LA(1)]&0x80) && (setwd20[LA(2)]&0x1) ) {
          }
          else {zzFAIL(2,zzerr61,zzerr62,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
        }
        zzEXIT(zztasp2);
        }
      }
    }
    else {
      if ( (LA(1)==V_CASE) ) {
#line 1721 "./verilog.g"
        zzmatch(V_CASE); zzCONSUME;
#line 1721 "./verilog.g"
        zzmatch(V_LP); zzCONSUME;
#line 1721 "./verilog.g"
        v_expression();
#line 1721 "./verilog.g"
        zzmatch(V_RP); zzCONSUME;
#line 1721 "./verilog.g"
        {
          zzBLOCK(zztasp2);
          int zzcnt=1;
          zzMake0;
          {
          do {
#line 1721 "./verilog.g"
            v_case_item();
            zzLOOP(zztasp2);
          } while ( (setwd20[LA(1)]&0x2) );
          zzEXIT(zztasp2);
          }
        }
#line 1721 "./verilog.g"
        zzmatch(V_ENDCASE); zzCONSUME;
      }
      else {
        if ( (LA(1)==V_CASEX) ) {
#line 1722 "./verilog.g"
          zzmatch(V_CASEX); zzCONSUME;
#line 1722 "./verilog.g"
          zzmatch(V_LP); zzCONSUME;
#line 1722 "./verilog.g"
          v_expression();
#line 1722 "./verilog.g"
          zzmatch(V_RP); zzCONSUME;
#line 1722 "./verilog.g"
          {
            zzBLOCK(zztasp2);
            int zzcnt=1;
            zzMake0;
            {
            do {
#line 1722 "./verilog.g"
              v_case_item();
              zzLOOP(zztasp2);
            } while ( (setwd20[LA(1)]&0x4)
 );
            zzEXIT(zztasp2);
            }
          }
#line 1722 "./verilog.g"
          zzmatch(V_ENDCASE); zzCONSUME;
        }
        else {
          if ( (LA(1)==V_CASEZ) ) {
#line 1723 "./verilog.g"
            zzmatch(V_CASEZ); zzCONSUME;
#line 1723 "./verilog.g"
            zzmatch(V_LP); zzCONSUME;
#line 1723 "./verilog.g"
            v_expression();
#line 1723 "./verilog.g"
            zzmatch(V_RP); zzCONSUME;
#line 1723 "./verilog.g"
            {
              zzBLOCK(zztasp2);
              int zzcnt=1;
              zzMake0;
              {
              do {
#line 1723 "./verilog.g"
                v_case_item();
                zzLOOP(zztasp2);
              } while ( (setwd20[LA(1)]&0x8) );
              zzEXIT(zztasp2);
              }
            }
#line 1723 "./verilog.g"
            zzmatch(V_ENDCASE); zzCONSUME;
          }
          else {
            if ( (LA(1)==V_FOREVER) ) {
#line 1724 "./verilog.g"
              zzmatch(V_FOREVER); zzCONSUME;
#line 1724 "./verilog.g"
              v_statement();
            }
            else {
              if ( (LA(1)==V_REPEAT) ) {
#line 1725 "./verilog.g"
                zzmatch(V_REPEAT); zzCONSUME;
#line 1725 "./verilog.g"
                zzmatch(V_LP); zzCONSUME;
#line 1725 "./verilog.g"
                v_expression();
#line 1725 "./verilog.g"
                zzmatch(V_RP); zzCONSUME;
#line 1725 "./verilog.g"
                v_statement();
              }
              else {
                if ( (LA(1)==V_WHILE)
 ) {
#line 1726 "./verilog.g"
                  zzmatch(V_WHILE); zzCONSUME;
#line 1726 "./verilog.g"
                  zzmatch(V_LP); zzCONSUME;
#line 1726 "./verilog.g"
                  v_expression();
#line 1726 "./verilog.g"
                  zzmatch(V_RP); zzCONSUME;
#line 1726 "./verilog.g"
                  v_statement();
                }
                else {
                  if ( (LA(1)==V_FOR) ) {
#line 1727 "./verilog.g"
                    zzmatch(V_FOR); zzCONSUME;
#line 1727 "./verilog.g"
                    zzmatch(V_LP); zzCONSUME;
#line 1727 "./verilog.g"
                    v_assignment();
#line 1727 "./verilog.g"
                    zzmatch(V_SEMI); zzCONSUME;
#line 1727 "./verilog.g"
                    v_expression();
#line 1727 "./verilog.g"
                    zzmatch(V_SEMI); zzCONSUME;
#line 1728 "./verilog.g"
                    v_assignment();
#line 1728 "./verilog.g"
                    zzmatch(V_RP); zzCONSUME;
#line 1728 "./verilog.g"
                    v_statement();
                  }
                  else {
                    if ( (setwd20[LA(1)]&0x10) ) {
#line 1729 "./verilog.g"
                      v_delay_or_event_control_stmt();
#line 1729 "./verilog.g"
                      v_statement_or_null();
                    }
                    else {
                      if ( (LA(1)==V_WAIT) ) {
#line 1730 "./verilog.g"
                        zzmatch(V_WAIT); zzCONSUME;
#line 1730 "./verilog.g"
                        zzmatch(V_LP); zzCONSUME;
#line 1730 "./verilog.g"
                        v_expression();
#line 1730 "./verilog.g"
                        zzmatch(V_RP); zzCONSUME;
#line 1730 "./verilog.g"
                        v_statement_or_null();
                      }
                      else {
                        if ( (LA(1)==V_RARROW) ) {
#line 1731 "./verilog.g"
                          zzmatch(V_RARROW); zzCONSUME;
#line 1731 "./verilog.g"
                          v_name_of_event();
#line 1731 "./verilog.g"
                          zzmatch(V_SEMI); zzCONSUME;
                        }
                        else {
                          if ( (LA(1)==V_BEGIN)
 ) {
#line 1732 "./verilog.g"
                            v_seq_block();
                          }
                          else {
                            if ( (LA(1)==V_FORK) ) {
#line 1733 "./verilog.g"
                              v_par_block();
                            }
                            else {
                              if ( (setwd20[LA(1)]&0x20) && (setwd20[LA(2)]&0x40) ) {
#line 1734 "./verilog.g"
                                v_task_enable();
                              }
                              else {
                                if ( (LA(1)==V_DISABLE) ) {
#line 1735 "./verilog.g"
                                  zzmatch(V_DISABLE); zzCONSUME;
#line 1735 "./verilog.g"
                                  {
                                    zzBLOCK(zztasp2);
                                    zzMake0;
                                    {
#line 1735 "./verilog.g"
                                    v_name_of_task_or_block();
                                    zzEXIT(zztasp2);
                                    }
                                  }
#line 1735 "./verilog.g"
                                  zzmatch(V_SEMI); zzCONSUME;
                                }
                                else {
                                  if ( (LA(1)==V_ASSIGN) ) {
#line 1736 "./verilog.g"
                                    zzmatch(V_ASSIGN); zzCONSUME;
#line 1736 "./verilog.g"
                                    v_assignment();
#line 1736 "./verilog.g"
                                    zzmatch(V_SEMI); zzCONSUME;
                                  }
                                  else {
                                    if ( (LA(1)==V_DEASSIGN)
 ) {
#line 1737 "./verilog.g"
                                      zzmatch(V_DEASSIGN); zzCONSUME;
#line 1737 "./verilog.g"
                                      v_lvalue();
#line 1737 "./verilog.g"
                                      zzmatch(V_SEMI); zzCONSUME;
                                    }
                                    else {
                                      if ( (LA(1)==V_FORCE) ) {
#line 1738 "./verilog.g"
                                        zzmatch(V_FORCE); zzCONSUME;
#line 1738 "./verilog.g"
                                        v_assignment();
#line 1738 "./verilog.g"
                                        zzmatch(V_SEMI); zzCONSUME;
                                      }
                                      else {
                                        if ( (LA(1)==V_RELEASE) ) {
#line 1739 "./verilog.g"
                                          zzmatch(V_RELEASE); zzCONSUME;
#line 1739 "./verilog.g"
                                          v_lvalue();
#line 1739 "./verilog.g"
                                          zzmatch(V_SEMI); zzCONSUME;
                                        }
                                        else {zzFAIL(2,zzerr63,zzerr64,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
                                      }
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd20, 0x80);
  }
}

void
#ifdef __USE_PROTOS
v_assignment(void)
#else
v_assignment()
#endif
{
#line 1742 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1742 "./verilog.g"
  v_lvalue();
#line 1742 "./verilog.g"
  zzmatch(V_EQ); zzCONSUME;
#line 1742 "./verilog.g"
  v_expression();
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd21, 0x1);
  }
}

void
#ifdef __USE_PROTOS
v_block_or_non_assignment(void)
#else
v_block_or_non_assignment()
#endif
{
#line 1745 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1745 "./verilog.g"
  v_lvalue();
#line 1745 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==V_EQ) ) {
#line 1745 "./verilog.g"
      v_blocking_assignment();
    }
    else {
      if ( (LA(1)==V_LEQ) ) {
#line 1746 "./verilog.g"
        v_non_blocking_assignment();
      }
      else {zzFAIL(1,zzerr65,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd21, 0x2);
  }
}

void
#ifdef __USE_PROTOS
v_blocking_assignment(void)
#else
v_blocking_assignment()
#endif
{
#line 1749 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1749 "./verilog.g"
  zzmatch(V_EQ); zzCONSUME;
#line 1749 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (setwd21[LA(1)]&0x4)
 ) {
#line 1749 "./verilog.g"
      v_expression();
    }
    else {
      if ( (setwd21[LA(1)]&0x8) ) {
#line 1750 "./verilog.g"
        v_delay_or_event_control();
#line 1750 "./verilog.g"
        v_expression();
      }
      else {zzFAIL(1,zzerr66,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd21, 0x10);
  }
}

void
#ifdef __USE_PROTOS
v_non_blocking_assignment(void)
#else
v_non_blocking_assignment()
#endif
{
#line 1753 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1753 "./verilog.g"
  zzmatch(V_LEQ); zzCONSUME;
#line 1753 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (setwd21[LA(1)]&0x20) ) {
#line 1753 "./verilog.g"
      v_expression();
    }
    else {
      if ( (setwd21[LA(1)]&0x40) ) {
#line 1754 "./verilog.g"
        v_delay_or_event_control();
#line 1754 "./verilog.g"
        v_expression();
      }
      else {zzFAIL(1,zzerr67,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd21, 0x80);
  }
}

void
#ifdef __USE_PROTOS
v_delay_or_event_control(void)
#else
v_delay_or_event_control()
#endif
{
#line 1757 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_POUND) ) {
#line 1757 "./verilog.g"
    v_delay_control();
  }
  else {
    if ( (LA(1)==V_AT)
 ) {
#line 1758 "./verilog.g"
      v_event_control();
    }
    else {
      if ( (LA(1)==V_REPEAT) ) {
#line 1759 "./verilog.g"
        zzmatch(V_REPEAT); zzCONSUME;
#line 1759 "./verilog.g"
        zzmatch(V_LP); zzCONSUME;
#line 1759 "./verilog.g"
        v_expression();
#line 1759 "./verilog.g"
        zzmatch(V_RP); zzCONSUME;
#line 1759 "./verilog.g"
        v_event_control();
      }
      else {zzFAIL(1,zzerr68,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd22, 0x1);
  }
}

void
#ifdef __USE_PROTOS
v_delay_or_event_control_stmt(void)
#else
v_delay_or_event_control_stmt()
#endif
{
#line 1762 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_POUND) ) {
#line 1762 "./verilog.g"
    v_delay_control();
  }
  else {
    if ( (LA(1)==V_AT) ) {
#line 1763 "./verilog.g"
      v_event_control();
    }
    else {zzFAIL(1,zzerr69,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd22, 0x2);
  }
}

void
#ifdef __USE_PROTOS
v_case_item(void)
#else
v_case_item()
#endif
{
#line 1766 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (setwd22[LA(1)]&0x4) ) {
#line 1766 "./verilog.g"
    v_explist();
#line 1767 "./verilog.g"
    zzmatch(V_COLON); zzCONSUME;
#line 1767 "./verilog.g"
    v_statement_or_null();
  }
  else {
    if ( (LA(1)==V_DEFAULT)
 ) {
#line 1768 "./verilog.g"
      zzmatch(V_DEFAULT); zzCONSUME;
#line 1768 "./verilog.g"
      {
        zzBLOCK(zztasp2);
        zzMake0;
        {
        if ( (LA(1)==V_COLON) ) {
#line 1768 "./verilog.g"
          zzmatch(V_COLON); zzCONSUME;
#line 1768 "./verilog.g"
          v_statement_or_null();
        }
        else {
          if ( (setwd22[LA(1)]&0x8) ) {
#line 1769 "./verilog.g"
            v_statement_or_null();
          }
          else {zzFAIL(1,zzerr70,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
        }
        zzEXIT(zztasp2);
        }
      }
    }
    else {zzFAIL(1,zzerr71,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd22, 0x10);
  }
}

void
#ifdef __USE_PROTOS
v_seq_block(void)
#else
v_seq_block()
#endif
{
#line 1772 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1772 "./verilog.g"
  zzmatch(V_BEGIN);
#line 1773 "./verilog.g"
  { 
    struct i_symbol_scope *sb = (struct i_symbol_scope *)calloc(1, sizeof(struct i_symbol_scope)); 
    sb->symtable = make_jrb(); 
    sb->parent = sym_base;
    sym_base = sb;
  }
 zzCONSUME;

#line 1780 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (setwd22[LA(1)]&0x20) ) {
#line 1780 "./verilog.g"
      {
        zzBLOCK(zztasp3);
        zzMake0;
        {
        while ( (setwd22[LA(1)]&0x40) ) {
#line 1780 "./verilog.g"
          v_statement();
          zzLOOP(zztasp3);
        }
        zzEXIT(zztasp3);
        }
      }
    }
    else {
      if ( (LA(1)==V_COLON)
 ) {
#line 1781 "./verilog.g"
        zzmatch(V_COLON); zzCONSUME;
#line 1781 "./verilog.g"
        v_name_of_block();
#line 1781 "./verilog.g"
        {
          zzBLOCK(zztasp3);
          zzMake0;
          {
          while ( (setwd22[LA(1)]&0x80) ) {
#line 1781 "./verilog.g"
            v_block_declaration();
            zzLOOP(zztasp3);
          }
          zzEXIT(zztasp3);
          }
        }
#line 1782 "./verilog.g"
        {
          zzBLOCK(zztasp3);
          zzMake0;
          {
          while ( (setwd23[LA(1)]&0x1) ) {
#line 1782 "./verilog.g"
            v_statement();
            zzLOOP(zztasp3);
          }
          zzEXIT(zztasp3);
          }
        }
      }
      else {zzFAIL(1,zzerr72,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
#line 1783 "./verilog.g"
  zzmatch(V_END);
#line 1784 "./verilog.g"
  if(sym_base) sym_base = sym_base->parent;
 zzCONSUME;

  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd23, 0x2);
  }
}

void
#ifdef __USE_PROTOS
v_par_block(void)
#else
v_par_block()
#endif
{
#line 1787 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1787 "./verilog.g"
  zzmatch(V_FORK);
#line 1788 "./verilog.g"
  { 
    struct i_symbol_scope *sb = (struct i_symbol_scope *)calloc(1, sizeof(struct i_symbol_scope)); 
    sb->symtable = make_jrb(); 
    sb->parent = sym_base;
    sym_base = sb;
  }
 zzCONSUME;

#line 1795 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (setwd23[LA(1)]&0x4) ) {
#line 1795 "./verilog.g"
      {
        zzBLOCK(zztasp3);
        zzMake0;
        {
        while ( (setwd23[LA(1)]&0x8) ) {
#line 1795 "./verilog.g"
          v_statement();
          zzLOOP(zztasp3);
        }
        zzEXIT(zztasp3);
        }
      }
    }
    else {
      if ( (LA(1)==V_COLON)
 ) {
#line 1796 "./verilog.g"
        zzmatch(V_COLON); zzCONSUME;
#line 1796 "./verilog.g"
        v_name_of_block();
#line 1796 "./verilog.g"
        {
          zzBLOCK(zztasp3);
          zzMake0;
          {
          while ( (setwd23[LA(1)]&0x10) ) {
#line 1796 "./verilog.g"
            v_block_declaration();
            zzLOOP(zztasp3);
          }
          zzEXIT(zztasp3);
          }
        }
#line 1797 "./verilog.g"
        {
          zzBLOCK(zztasp3);
          zzMake0;
          {
          while ( (setwd23[LA(1)]&0x20) ) {
#line 1797 "./verilog.g"
            v_statement();
            zzLOOP(zztasp3);
          }
          zzEXIT(zztasp3);
          }
        }
      }
      else {zzFAIL(1,zzerr73,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
#line 1797 "./verilog.g"
  zzmatch(V_JOIN);
#line 1798 "./verilog.g"
  if(sym_base) sym_base = sym_base->parent;
 zzCONSUME;

  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd23, 0x40);
  }
}

void
#ifdef __USE_PROTOS
v_name_of_block(void)
#else
v_name_of_block()
#endif
{
#line 1801 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1801 "./verilog.g"
  v_identifier_nodot();
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd23, 0x80);
  }
}

void
#ifdef __USE_PROTOS
v_block_declaration(void)
#else
v_block_declaration()
#endif
{
#line 1804 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_PARAMETER) ) {
#line 1804 "./verilog.g"
    v_parameter_declaration();
  }
  else {
    if ( (LA(1)==V_REG) ) {
#line 1805 "./verilog.g"
      v_reg_declaration();
    }
    else {
      if ( (LA(1)==V_INTEGER)
 ) {
#line 1806 "./verilog.g"
        v_integer_declaration();
      }
      else {
        if ( (LA(1)==V_REAL) ) {
#line 1807 "./verilog.g"
          v_real_declaration();
        }
        else {
          if ( (LA(1)==V_TIME) ) {
#line 1808 "./verilog.g"
            v_time_declaration();
          }
          else {
            if ( (LA(1)==V_EVENT) ) {
#line 1809 "./verilog.g"
              v_event_declaration();
            }
            else {zzFAIL(1,zzerr74,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
          }
        }
      }
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd24, 0x1);
  }
}

void
#ifdef __USE_PROTOS
v_task_enable(void)
#else
v_task_enable()
#endif
{
#line 1812 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (setwd24[LA(1)]&0x2) && (LA(2)==V_SEMI) ) {
#line 1812 "./verilog.g"
    v_name_of_task();
#line 1812 "./verilog.g"
    zzmatch(V_SEMI); zzCONSUME;
  }
  else {
    if ( (setwd24[LA(1)]&0x4) && 
(LA(2)==V_LP) ) {
#line 1813 "./verilog.g"
      v_name_of_task();
#line 1813 "./verilog.g"
      zzmatch(V_LP); zzCONSUME;
#line 1813 "./verilog.g"
      v_explist();
#line 1813 "./verilog.g"
      zzmatch(V_RP); zzCONSUME;
#line 1813 "./verilog.g"
      zzmatch(V_SEMI); zzCONSUME;
    }
    else {zzFAIL(2,zzerr75,zzerr76,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd24, 0x8);
  }
}

void
#ifdef __USE_PROTOS
v_name_of_task(void)
#else
v_name_of_task()
#endif
{
#line 1816 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1816 "./verilog.g"
  v_identifier();
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd24, 0x10);
  }
}

void
#ifdef __USE_PROTOS
v_name_of_task_or_block(void)
#else
v_name_of_task_or_block()
#endif
{
#line 1820 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1820 "./verilog.g"
  v_identifier_nodot();
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd24, 0x20);
  }
}

void
#ifdef __USE_PROTOS
v_specify_block(void)
#else
v_specify_block()
#endif
{
#line 1827 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1827 "./verilog.g"
  zzmatch(V_SPECIFY); zzCONSUME;
#line 1827 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (setwd24[LA(1)]&0x40) ) {
#line 1827 "./verilog.g"
      zzsetmatch(zzerr77, zzerr78); zzCONSUME;
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
#line 1827 "./verilog.g"
  zzmatch(V_ENDSPECIFY); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd24, 0x80);
  }
}

void
#ifdef __USE_PROTOS
v_constant_expression(void)
#else
v_constant_expression()
#endif
{
#line 1830 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1830 "./verilog.g"
  v_expression();
#line 1831 "./verilog.g"
  zzaRet.prim = zzaArg(zztasp1,1 ).prim;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd25, 0x1);
  }
}

void
#ifdef __USE_PROTOS
v_lvalue(void)
#else
v_lvalue()
#endif
{
#line 1838 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (setwd25[LA(1)]&0x2) ) {
#line 1838 "./verilog.g"
    v_identifier();
#line 1840 "./verilog.g"
    {
      zzBLOCK(zztasp2);
      zzMake0;
      {
      if ( (LA(1)==V_LBRACK) ) {
#line 1840 "./verilog.g"
        zzmatch(V_LBRACK); zzCONSUME;
#line 1840 "./verilog.g"
        v_expression();
#line 1841 "./verilog.g"
        {
          zzBLOCK(zztasp3);
          zzMake0;
          {
          if ( (LA(1)==V_COLON) ) {
#line 1841 "./verilog.g"
            zzmatch(V_COLON); zzCONSUME;
#line 1841 "./verilog.g"
            v_expression();
#line 1842 "./verilog.g"
            zzaRet.prim = i_primary_symrange_make(zzaArg(zztasp1,1).symbol,zzaArg(zztasp2,2).prim,zzaArg(zztasp3,2).prim);
          }
          else {
            if ( (LA(1)==V_RBRACK)
 ) {
#line 1843 "./verilog.g"
              zzaRet.prim = i_primary_symbit_make(zzaArg(zztasp1,1).symbol,zzaArg(zztasp2,2).prim);
            }
            else {zzFAIL(1,zzerr79,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
          }
          zzEXIT(zztasp3);
          }
        }
#line 1844 "./verilog.g"
        zzmatch(V_RBRACK); zzCONSUME;
      }
      else {
        if ( (setwd25[LA(1)]&0x4) ) {
#line 1844 "./verilog.g"
          zzaRet.prim = i_primary_make(PRIM_SYMBOL,zzaArg(zztasp1,1).symbol);
        }
        else {zzFAIL(1,zzerr80,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
      }
      zzEXIT(zztasp2);
      }
    }
  }
  else {
    if ( (LA(1)==V_LBRACE) ) {
#line 1846 "./verilog.g"
      v_concatenation();
#line 1846 "./verilog.g"
      zzaRet.prim = zzaArg(zztasp1,1 ).prim;
    }
    else {zzFAIL(1,zzerr81,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd25, 0x8);
  }
}

void
#ifdef __USE_PROTOS
v_expression(void)
#else
v_expression()
#endif
{
#line 1849 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1849 "./verilog.g"
  push_exp_now();
#line 1849 "./verilog.g"
  v_expression2();
#line 1849 "./verilog.g"
  
  if(!zzerrors)
  {
    push_oper(i_oper_make(V_EOF,0));
    push_primary(NULL);
    pop_exp_now(); 
    zzaRet.prim = shred_expression(); 
  }
  else
  {
    zzaRet.prim = NULL;
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd25, 0x10);
  }
}

void
#ifdef __USE_PROTOS
v_expression2(void)
#else
v_expression2()
#endif
{
#line 1864 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1864 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (setwd25[LA(1)]&0x20) ) {
#line 1864 "./verilog.g"
      v_primary();
#line 1864 "./verilog.g"
      push_primary(zzaArg(zztasp2,1 ).prim);
    }
    else {
      if ( (setwd25[LA(1)]&0x40) ) {
#line 1865 "./verilog.g"
        v_unary_operator();
#line 1865 "./verilog.g"
        v_primary();
#line 1866 "./verilog.g"
        push_primary(NULL); push_oper(zzaArg(zztasp2,1 ).oper); push_primary(zzaArg(zztasp2,2 ).prim);
      }
      else {
        if ( (LA(1)==V_STRING)
 ) {
#line 1867 "./verilog.g"
          zzmatch(V_STRING);
#line 1867 "./verilog.g"
          push_primary(i_primary_make(PRIM_NUMBER, i_number_basemake(NV_STRING, zzaArg(zztasp2,1 ).text)));
 zzCONSUME;

        }
        else {zzFAIL(1,zzerr82,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
      }
    }
    zzEXIT(zztasp2);
    }
  }
#line 1868 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (setwd25[LA(1)]&0x80) ) {
#line 1868 "./verilog.g"
      v_binary_operator();
#line 1868 "./verilog.g"
      push_oper(zzaArg(zztasp2,1 ).oper);
#line 1868 "./verilog.g"
      v_expression2();
    }
    else {
      if ( (LA(1)==V_QUEST) ) {
#line 1869 "./verilog.g"
        zzmatch(V_QUEST); zzCONSUME;
#line 1869 "./verilog.g"
        v_expression();
#line 1869 "./verilog.g"
        zzmatch(V_COLON); zzCONSUME;
#line 1869 "./verilog.g"
        v_expression();
#line 1870 "./verilog.g"
        push_oper(i_oper_make(V_QUEST,1));
        push_primary(i_bin_expr_make(zzaArg(zztasp2,2 ).prim,i_oper_make(V_COLON, 1),zzaArg(zztasp2,4 ).prim));
      }
      else {
        if ( (setwd26[LA(1)]&0x1) ) {
        }
        else {zzFAIL(1,zzerr83,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
      }
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd26, 0x2);
  }
}

void
#ifdef __USE_PROTOS
v_mintypmax_expression(void)
#else
v_mintypmax_expression()
#endif
{
#line 1875 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1875 "./verilog.g"
  v_expression();
#line 1876 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==V_COLON) ) {
#line 1876 "./verilog.g"
      zzmatch(V_COLON); zzCONSUME;
#line 1876 "./verilog.g"
      v_expression();
#line 1876 "./verilog.g"
      zzmatch(V_COLON); zzCONSUME;
#line 1876 "./verilog.g"
      v_expression();
#line 1877 "./verilog.g"
      zzaRet.prim = i_primary_mintypmax_make(zzaArg(zztasp1,1).prim, zzaArg(zztasp2,2).prim, zzaArg(zztasp2,4).prim);
    }
    else {
      if ( (setwd26[LA(1)]&0x4)
 ) {
#line 1878 "./verilog.g"
        zzaRet.prim = zzaArg(zztasp1,1).prim;
      }
      else {zzFAIL(1,zzerr84,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd26, 0x8);
  }
}

void
#ifdef __USE_PROTOS
v_unary_operator(void)
#else
v_unary_operator()
#endif
{
#line 1881 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_PLUS) ) {
#line 1881 "./verilog.g"
    zzmatch(V_PLUS);
#line 1881 "./verilog.g"
    zzaRet.oper = i_oper_make(V_PLUS, 11);
 zzCONSUME;

  }
  else {
    if ( (LA(1)==V_MINUS) ) {
#line 1882 "./verilog.g"
      zzmatch(V_MINUS);
#line 1882 "./verilog.g"
      zzaRet.oper = i_oper_make(V_MINUS,11);
 zzCONSUME;

    }
    else {
      if ( (LA(1)==V_BANG) ) {
#line 1883 "./verilog.g"
        zzmatch(V_BANG);
#line 1883 "./verilog.g"
        zzaRet.oper = i_oper_make(V_BANG, 11);
 zzCONSUME;

      }
      else {
        if ( (LA(1)==V_TILDE) ) {
#line 1884 "./verilog.g"
          zzmatch(V_TILDE);
#line 1884 "./verilog.g"
          zzaRet.oper = i_oper_make(V_TILDE,11);
 zzCONSUME;

        }
        else {
          if ( (LA(1)==V_AND)
 ) {
#line 1885 "./verilog.g"
            zzmatch(V_AND);
#line 1885 "./verilog.g"
            zzaRet.oper = i_oper_make(V_AND,  11);
 zzCONSUME;

          }
          else {
            if ( (LA(1)==V_NAND) ) {
#line 1886 "./verilog.g"
              zzmatch(V_NAND);
#line 1886 "./verilog.g"
              zzaRet.oper = i_oper_make(V_NAND, 11);
 zzCONSUME;

            }
            else {
              if ( (LA(1)==V_OR) ) {
#line 1887 "./verilog.g"
                zzmatch(V_OR);
#line 1887 "./verilog.g"
                zzaRet.oper = i_oper_make(V_OR,   11);
 zzCONSUME;

              }
              else {
                if ( (LA(1)==V_NOR) ) {
#line 1888 "./verilog.g"
                  zzmatch(V_NOR);
#line 1888 "./verilog.g"
                  zzaRet.oper = i_oper_make(V_NOR,  11);
 zzCONSUME;

                }
                else {
                  if ( (LA(1)==V_XOR) ) {
#line 1889 "./verilog.g"
                    zzmatch(V_XOR);
#line 1889 "./verilog.g"
                    zzaRet.oper = i_oper_make(V_XOR,  11);
 zzCONSUME;

                  }
                  else {
                    if ( (LA(1)==V_XNOR)
 ) {
#line 1890 "./verilog.g"
                      zzmatch(V_XNOR);
#line 1890 "./verilog.g"
                      zzaRet.oper = i_oper_make(V_XNOR, 11);
 zzCONSUME;

                    }
                    else {
                      if ( (LA(1)==V_XNOR2) ) {
#line 1891 "./verilog.g"
                        zzmatch(V_XNOR2);
#line 1891 "./verilog.g"
                        zzaRet.oper = i_oper_make(V_XNOR, 11);
 zzCONSUME;

                      }
                      else {zzFAIL(1,zzerr85,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd26, 0x10);
  }
}

void
#ifdef __USE_PROTOS
v_binary_operator(void)
#else
v_binary_operator()
#endif
{
#line 1894 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_STAR) ) {
#line 1894 "./verilog.g"
    zzmatch(V_STAR);
#line 1894 "./verilog.g"
    zzaRet.oper = i_oper_make(V_STAR, 10);
 zzCONSUME;

  }
  else {
    if ( (LA(1)==V_SLASH) ) {
#line 1895 "./verilog.g"
      zzmatch(V_SLASH);
#line 1895 "./verilog.g"
      zzaRet.oper = i_oper_make(V_SLASH,10);
 zzCONSUME;

    }
    else {
      if ( (LA(1)==V_MOD) ) {
#line 1896 "./verilog.g"
        zzmatch(V_MOD);
#line 1896 "./verilog.g"
        zzaRet.oper = i_oper_make(V_MOD,  10);
 zzCONSUME;

      }
      else {
        if ( (LA(1)==V_PLUS)
 ) {
#line 1897 "./verilog.g"
          zzmatch(V_PLUS);
#line 1897 "./verilog.g"
          zzaRet.oper = i_oper_make(V_PLUS,  9);
 zzCONSUME;

        }
        else {
          if ( (LA(1)==V_MINUS) ) {
#line 1898 "./verilog.g"
            zzmatch(V_MINUS);
#line 1898 "./verilog.g"
            zzaRet.oper = i_oper_make(V_MINUS, 9);
 zzCONSUME;

          }
          else {
            if ( (LA(1)==V_SHL) ) {
#line 1899 "./verilog.g"
              zzmatch(V_SHL);
#line 1899 "./verilog.g"
              zzaRet.oper = i_oper_make(V_SHL,   8);
 zzCONSUME;

            }
            else {
              if ( (LA(1)==V_SHR) ) {
#line 1900 "./verilog.g"
                zzmatch(V_SHR);
#line 1900 "./verilog.g"
                zzaRet.oper = i_oper_make(V_SHR,   8);
 zzCONSUME;

              }
              else {
                if ( (LA(1)==V_LT) ) {
#line 1901 "./verilog.g"
                  zzmatch(V_LT);
#line 1901 "./verilog.g"
                  zzaRet.oper = i_oper_make(V_LT,    7);
 zzCONSUME;

                }
                else {
                  if ( (LA(1)==V_LEQ)
 ) {
#line 1902 "./verilog.g"
                    zzmatch(V_LEQ);
#line 1902 "./verilog.g"
                    zzaRet.oper = i_oper_make(V_LEQ,   7);
 zzCONSUME;

                  }
                  else {
                    if ( (LA(1)==V_GT) ) {
#line 1903 "./verilog.g"
                      zzmatch(V_GT);
#line 1903 "./verilog.g"
                      zzaRet.oper = i_oper_make(V_GT,    7);
 zzCONSUME;

                    }
                    else {
                      if ( (LA(1)==V_GEQ) ) {
#line 1904 "./verilog.g"
                        zzmatch(V_GEQ);
#line 1904 "./verilog.g"
                        zzaRet.oper = i_oper_make(V_GEQ,   7);
 zzCONSUME;

                      }
                      else {
                        if ( (LA(1)==V_EQ2) ) {
#line 1905 "./verilog.g"
                          zzmatch(V_EQ2);
#line 1905 "./verilog.g"
                          zzaRet.oper = i_oper_make(V_EQ2,   6);
 zzCONSUME;

                        }
                        else {
                          if ( (LA(1)==V_NEQ) ) {
#line 1906 "./verilog.g"
                            zzmatch(V_NEQ);
#line 1906 "./verilog.g"
                            zzaRet.oper = i_oper_make(V_NEQ,   6);
 zzCONSUME;

                          }
                          else {
                            if ( (LA(1)==V_EQ3)
 ) {
#line 1907 "./verilog.g"
                              zzmatch(V_EQ3);
#line 1907 "./verilog.g"
                              zzaRet.oper = i_oper_make(V_EQ3,   6);
 zzCONSUME;

                            }
                            else {
                              if ( (LA(1)==V_NEQ2) ) {
#line 1908 "./verilog.g"
                                zzmatch(V_NEQ2);
#line 1908 "./verilog.g"
                                zzaRet.oper = i_oper_make(V_NEQ2,  6);
 zzCONSUME;

                              }
                              else {
                                if ( (LA(1)==V_AND) ) {
#line 1909 "./verilog.g"
                                  zzmatch(V_AND);
#line 1909 "./verilog.g"
                                  zzaRet.oper = i_oper_make(V_AND,   5);
 zzCONSUME;

                                }
                                else {
                                  if ( (LA(1)==V_NAND) ) {
#line 1910 "./verilog.g"
                                    zzmatch(V_NAND);
#line 1910 "./verilog.g"
                                    zzaRet.oper = i_oper_make(V_NAND,  5);
 zzCONSUME;

                                  }
                                  else {
                                    if ( (LA(1)==V_XOR) ) {
#line 1911 "./verilog.g"
                                      zzmatch(V_XOR);
#line 1911 "./verilog.g"
                                      zzaRet.oper = i_oper_make(V_XOR,   5);
 zzCONSUME;

                                    }
                                    else {
                                      if ( (LA(1)==V_XNOR)
 ) {
#line 1912 "./verilog.g"
                                        zzmatch(V_XNOR);
#line 1912 "./verilog.g"
                                        zzaRet.oper = i_oper_make(V_XNOR,  5);
 zzCONSUME;

                                      }
                                      else {
                                        if ( (LA(1)==V_XNOR2) ) {
#line 1913 "./verilog.g"
                                          zzmatch(V_XNOR2);
#line 1913 "./verilog.g"
                                          zzaRet.oper = i_oper_make(V_XNOR,  5);
 zzCONSUME;

                                        }
                                        else {
                                          if ( (LA(1)==V_OR) ) {
#line 1914 "./verilog.g"
                                            zzmatch(V_OR);
#line 1914 "./verilog.g"
                                            zzaRet.oper = i_oper_make(V_OR,    4);
 zzCONSUME;

                                          }
                                          else {
                                            if ( (LA(1)==V_NOR) ) {
#line 1915 "./verilog.g"
                                              zzmatch(V_NOR);
#line 1915 "./verilog.g"
                                              zzaRet.oper = i_oper_make(V_NOR,   4);
 zzCONSUME;

                                            }
                                            else {
                                              if ( (LA(1)==V_AND2) ) {
#line 1916 "./verilog.g"
                                                zzmatch(V_AND2);
#line 1916 "./verilog.g"
                                                zzaRet.oper = i_oper_make(V_AND2,  3);
 zzCONSUME;

                                              }
                                              else {
                                                if ( (LA(1)==V_OR2)
 ) {
#line 1917 "./verilog.g"
                                                  zzmatch(V_OR2);
#line 1917 "./verilog.g"
                                                  zzaRet.oper = i_oper_make(V_OR2,   2);
 zzCONSUME;

                                                }
                                                else {zzFAIL(1,zzerr86,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
                                              }
                                            }
                                          }
                                        }
                                      }
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd26, 0x20);
  }
}

void
#ifdef __USE_PROTOS
v_primary(void)
#else
v_primary()
#endif
{
#line 1920 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (setwd26[LA(1)]&0x40) ) {
#line 1920 "./verilog.g"
    v_number();
#line 1920 "./verilog.g"
    zzaRet.prim = i_primary_make(PRIM_NUMBER,zzaArg(zztasp1,1 ).num);
  }
  else {
    if ( (setwd26[LA(1)]&0x80) && (setwd27[LA(2)]&0x1) ) {
#line 1921 "./verilog.g"
      v_identifier();
#line 1922 "./verilog.g"
      {
        zzBLOCK(zztasp2);
        zzMake0;
        {
        if ( (LA(1)==V_LBRACK) ) {
#line 1922 "./verilog.g"
          zzmatch(V_LBRACK); zzCONSUME;
#line 1922 "./verilog.g"
          v_expression();
#line 1923 "./verilog.g"
          {
            zzBLOCK(zztasp3);
            zzMake0;
            {
            if ( (LA(1)==V_RBRACK) ) {
#line 1923 "./verilog.g"
              zzmatch(V_RBRACK);
#line 1924 "./verilog.g"
              zzaRet.prim= i_primary_symbit_make(zzaArg(zztasp1,1).symbol,zzaArg(zztasp2,2).prim);
 zzCONSUME;

            }
            else {
              if ( (LA(1)==V_COLON)
 ) {
#line 1925 "./verilog.g"
                zzmatch(V_COLON); zzCONSUME;
#line 1925 "./verilog.g"
                v_expression();
#line 1925 "./verilog.g"
                zzmatch(V_RBRACK);
#line 1926 "./verilog.g"
                zzaRet.prim= i_primary_symrange_make(zzaArg(zztasp1,1).symbol,zzaArg(zztasp2,2).prim,zzaArg(zztasp3,2).prim);
 zzCONSUME;

              }
              else {zzFAIL(1,zzerr87,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
            }
            zzEXIT(zztasp3);
            }
          }
        }
        else {
          if ( (setwd27[LA(1)]&0x2) ) {
#line 1927 "./verilog.g"
            zzaRet.prim= i_primary_make(PRIM_SYMBOL,zzaArg(zztasp1,1).symbol);
          }
          else {zzFAIL(1,zzerr88,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
        }
        zzEXIT(zztasp2);
        }
      }
    }
    else {
      if ( (LA(1)==V_LBRACE) ) {
#line 1928 "./verilog.g"
        v_multiple_concatenation();
#line 1928 "./verilog.g"
        zzaRet.prim = zzaArg(zztasp1,1 ).prim;
      }
      else {
        if ( (setwd27[LA(1)]&0x4) && (LA(2)==V_LP) ) {
#line 1929 "./verilog.g"
          v_function_call();
#line 1929 "./verilog.g"
          zzaRet.prim = zzaArg(zztasp1,1 ).prim;
        }
        else {
          if ( (LA(1)==V_LP) ) {
#line 1930 "./verilog.g"
            zzmatch(V_LP); zzCONSUME;
#line 1930 "./verilog.g"
            v_mintypmax_expression();
#line 1930 "./verilog.g"
            zzmatch(V_RP);
#line 1930 "./verilog.g"
            zzaRet.prim = zzaArg(zztasp1,2 ).prim;
 zzCONSUME;

          }
          else {zzFAIL(2,zzerr89,zzerr90,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
        }
      }
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd27, 0x8);
  }
}

void
#ifdef __USE_PROTOS
v_number(void)
#else
v_number()
#endif
{
#line 1934 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_DECIMAL_NUMBER)
 ) {
#line 1934 "./verilog.g"
    zzmatch(V_DECIMAL_NUMBER);
#line 1934 "./verilog.g"
    zzaRet.num = i_number_make(zzaArg(zztasp1,1).ival);
 zzCONSUME;

  }
  else {
    if ( (LA(1)==V_HBASE) ) {
#line 1935 "./verilog.g"
      zzmatch(V_HBASE);
#line 1935 "./verilog.g"
      zzaRet.num = i_number_basemake(NV_HBASE, zzaArg(zztasp1,1 ).text);
 zzCONSUME;

    }
    else {
      if ( (LA(1)==V_DBASE) ) {
#line 1936 "./verilog.g"
        zzmatch(V_DBASE);
#line 1936 "./verilog.g"
        zzaRet.num = i_number_basemake(NV_DBASE, zzaArg(zztasp1,1 ).text);
 zzCONSUME;

      }
      else {
        if ( (LA(1)==V_BBASE) ) {
#line 1937 "./verilog.g"
          zzmatch(V_BBASE);
#line 1937 "./verilog.g"
          zzaRet.num = i_number_basemake(NV_BBASE, zzaArg(zztasp1,1 ).text);
 zzCONSUME;

        }
        else {
          if ( (LA(1)==V_OBASE) ) {
#line 1938 "./verilog.g"
            zzmatch(V_OBASE);
#line 1938 "./verilog.g"
            zzaRet.num = i_number_basemake(NV_OBASE, zzaArg(zztasp1,1 ).text);
 zzCONSUME;

          }
          else {
            if ( (LA(1)==V_FLOAT1)
 ) {
#line 1939 "./verilog.g"
              zzmatch(V_FLOAT1);
#line 1939 "./verilog.g"
              zzaRet.num = i_number_fmake(zzaArg(zztasp1,1 ).rval);
 zzCONSUME;

            }
            else {
              if ( (LA(1)==V_FLOAT2) ) {
#line 1940 "./verilog.g"
                zzmatch(V_FLOAT2);
#line 1940 "./verilog.g"
                zzaRet.num = i_number_fmake(zzaArg(zztasp1,1 ).rval);
 zzCONSUME;

              }
              else {zzFAIL(1,zzerr91,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
            }
          }
        }
      }
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd27, 0x10);
  }
}

void
#ifdef __USE_PROTOS
v_concatenation(void)
#else
v_concatenation()
#endif
{
#line 1943 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1944 "./verilog.g"
  zzmatch(V_LBRACE); zzCONSUME;
#line 1944 "./verilog.g"
  v_explist();
#line 1944 "./verilog.g"
  zzmatch(V_RBRACE);
#line 1945 "./verilog.g"
  zzaRet.prim = i_primary_concat_make(NULL,zzaArg(zztasp1,2 ).explist);
 zzCONSUME;

  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd27, 0x20);
  }
}

void
#ifdef __USE_PROTOS
v_multiple_concatenation(void)
#else
v_multiple_concatenation()
#endif
{
#line 1948 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1949 "./verilog.g"
  struct i_explist *lroot=NULL, *lcurrent=NULL;
#line 1950 "./verilog.g"
  zzmatch(V_LBRACE); zzCONSUME;
#line 1950 "./verilog.g"
  v_expression();
#line 1951 "./verilog.g"
  lroot=lcurrent=(struct i_explist *)calloc(1,sizeof(struct i_explist));
  lcurrent->item=zzaArg(zztasp1,2 ).prim;
#line 1960 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (setwd27[LA(1)]&0x40) ) {
#line 1960 "./verilog.g"
      {
        zzBLOCK(zztasp3);
        zzMake0;
        {
        while ( (LA(1)==V_COMMA) ) {
#line 1955 "./verilog.g"
          zzmatch(V_COMMA); zzCONSUME;
#line 1955 "./verilog.g"
          v_expression();
#line 1956 "./verilog.g"
          lcurrent->next=(struct i_explist *)calloc(1,sizeof(struct i_explist));
          lcurrent=lcurrent->next;
          lcurrent->item=zzaArg(zztasp3,2 ).prim;
          zzLOOP(zztasp3);
        }
        zzEXIT(zztasp3);
        }
      }
#line 1961 "./verilog.g"
      zzaRet.prim = i_primary_concat_make(NULL,i_explist_make(lroot));
    }
    else {
      if ( (LA(1)==V_LBRACE) ) {
#line 1963 "./verilog.g"
        zzmatch(V_LBRACE); zzCONSUME;
#line 1963 "./verilog.g"
        v_explist();
#line 1963 "./verilog.g"
        zzmatch(V_RBRACE);
#line 1964 "./verilog.g"
        zzaRet.prim = i_primary_concat_make(lroot->item,zzaArg(zztasp2,2 ).explist); 
        free(lroot);
 zzCONSUME;

      }
      else {zzFAIL(1,zzerr92,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
#line 1968 "./verilog.g"
  zzmatch(V_RBRACE); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd27, 0x80);
  }
}

void
#ifdef __USE_PROTOS
v_function_call(void)
#else
v_function_call()
#endif
{
#line 1971 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1971 "./verilog.g"
  v_name_of_function();
#line 1971 "./verilog.g"
  zzmatch(V_LP); zzCONSUME;
#line 1971 "./verilog.g"
  v_explist();
#line 1971 "./verilog.g"
  zzmatch(V_RP);
#line 1972 "./verilog.g"
  zzaRet.prim=i_primary_funcall_make(zzaArg(zztasp1,1 ).symbol,zzaArg(zztasp1,3 ).explist);
 zzCONSUME;

  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd28, 0x1);
  }
}

void
#ifdef __USE_PROTOS
v_name_of_function(void)
#else
v_name_of_function()
#endif
{
#line 1975 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1975 "./verilog.g"
  v_identifier_nodot();
#line 1975 "./verilog.g"
  zzaRet.symbol = zzaArg(zztasp1,1 ).symbol;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd28, 0x2);
  }
}

void
#ifdef __USE_PROTOS
v_explist(void)
#else
v_explist()
#endif
{
#line 1978 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1978 "./verilog.g"
  struct i_explist *lroot=NULL, *lcurrent=NULL;
#line 1979 "./verilog.g"
  v_fn_expression();
#line 1980 "./verilog.g"
  lroot=lcurrent=(struct i_explist *)calloc(1,sizeof(struct i_explist));
  lcurrent->item=zzaArg(zztasp1,1 ).prim;
#line 1988 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (LA(1)==V_COMMA)
 ) {
#line 1983 "./verilog.g"
      zzmatch(V_COMMA); zzCONSUME;
#line 1983 "./verilog.g"
      v_fn_expression();
#line 1984 "./verilog.g"
      lcurrent->next=(struct i_explist *)calloc(1,sizeof(struct i_explist));
      lcurrent=lcurrent->next;
      lcurrent->item=zzaArg(zztasp2,2 ).prim;
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
#line 1989 "./verilog.g"
  zzaRet.explist=i_explist_make(lroot);
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd28, 0x4);
  }
}

void
#ifdef __USE_PROTOS
v_fn_expression(void)
#else
v_fn_expression()
#endif
{
#line 1992 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (setwd28[LA(1)]&0x8) ) {
#line 1993 "./verilog.g"
    v_expression();
#line 1993 "./verilog.g"
    zzaRet.prim = zzaArg(zztasp1,1 ).prim;
  }
  else {
    if ( (setwd28[LA(1)]&0x10) ) {
#line 1994 "./verilog.g"
      zzaRet.prim = NULL;
    }
    else {zzFAIL(1,zzerr93,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd28, 0x20);
  }
}

void
#ifdef __USE_PROTOS
v_mexplist(void)
#else
v_mexplist()
#endif
{
#line 1998 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 1998 "./verilog.g"
  struct i_explist *lroot=NULL, *lcurrent=NULL;
#line 1999 "./verilog.g"
  v_mfn_expression();
#line 2000 "./verilog.g"
  lroot=lcurrent=(struct i_explist *)calloc(1,sizeof(struct i_explist));
  lcurrent->item=zzaArg(zztasp1,1 ).prim;
#line 2008 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (LA(1)==V_COMMA) ) {
#line 2003 "./verilog.g"
      zzmatch(V_COMMA); zzCONSUME;
#line 2003 "./verilog.g"
      v_mfn_expression();
#line 2004 "./verilog.g"
      lcurrent->next=(struct i_explist *)calloc(1,sizeof(struct i_explist));
      lcurrent=lcurrent->next;
      lcurrent->item=zzaArg(zztasp2,2 ).prim;
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
#line 2009 "./verilog.g"
  zzaRet.explist=i_explist_make(lroot);
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd28, 0x40);
  }
}

void
#ifdef __USE_PROTOS
v_mfn_expression(void)
#else
v_mfn_expression()
#endif
{
#line 2012 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_DOT) ) {
#line 2013 "./verilog.g"
    zzmatch(V_DOT); zzCONSUME;
#line 2013 "./verilog.g"
    v_identifier_nodot();
#line 2013 "./verilog.g"
    zzmatch(V_LP); zzCONSUME;
#line 2013 "./verilog.g"
    v_expression();
#line 2013 "./verilog.g"
    zzmatch(V_RP);
#line 2014 "./verilog.g"
    
    struct i_primary *ip = i_primary_make(PRIM_NAMEDPARAM,NULL);
    ip->primval.named_param.sym = zzaArg(zztasp1,2 ).symbol;
    ip->primval.named_param.exp = zzaArg(zztasp1,4 ).prim;			
    
			zzaRet.prim = ip;
 zzCONSUME;

  }
  else {
    if ( (setwd28[LA(1)]&0x80)
 ) {
#line 2021 "./verilog.g"
      v_expression();
#line 2021 "./verilog.g"
      zzaRet.prim = zzaArg(zztasp1,1 ).prim;
    }
    else {
      if ( (setwd29[LA(1)]&0x1) ) {
#line 2022 "./verilog.g"
        zzaRet.prim = NULL;
      }
      else {zzFAIL(1,zzerr94,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd29, 0x2);
  }
}

void
#ifdef __USE_PROTOS
v_identifier(void)
#else
v_identifier()
#endif
{
#line 2029 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (setwd29[LA(1)]&0x4) ) {
#line 2029 "./verilog.g"
    v_identifier_nodot();
#line 2029 "./verilog.g"
    zzaRet.symbol = zzaArg(zztasp1,1 ).symbol;
  }
  else {
    if ( (LA(1)==V_IDENDOT) ) {
#line 2030 "./verilog.g"
      zzmatch(V_IDENDOT);
#line 2030 "./verilog.g"
      zzaRet.symbol = zzaArg(zztasp1,1 ).symbol;
 zzCONSUME;

    }
    else {zzFAIL(1,zzerr95,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd29, 0x8);
  }
}

void
#ifdef __USE_PROTOS
v_identifier_nodot(void)
#else
v_identifier_nodot()
#endif
{
#line 2033 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==V_IDENTIFIER) ) {
#line 2034 "./verilog.g"
    zzmatch(V_IDENTIFIER);
#line 2034 "./verilog.g"
    zzaRet.symbol = zzaArg(zztasp1,1 ).symbol;
 zzCONSUME;

  }
  else {
    if ( (LA(1)==V_IDENTIFIER2)
 ) {
#line 2035 "./verilog.g"
      zzmatch(V_IDENTIFIER2);
#line 2035 "./verilog.g"
      zzaRet.symbol = zzaArg(zztasp1,1 ).symbol;
 zzCONSUME;

    }
    else {
      if ( (LA(1)==V_FUNCTION_NAME) ) {
#line 2036 "./verilog.g"
        zzmatch(V_FUNCTION_NAME);
#line 2036 "./verilog.g"
        zzaRet.symbol = zzaArg(zztasp1,1 ).symbol;
 zzCONSUME;

      }
      else {zzFAIL(1,zzerr96,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd29, 0x10);
  }
}

void
#ifdef __USE_PROTOS
v_delay(void)
#else
v_delay()
#endif
{
#line 2039 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 2039 "./verilog.g"
  zzmatch(V_POUND); zzCONSUME;
#line 2039 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (setwd29[LA(1)]&0x20) ) {
#line 2039 "./verilog.g"
      v_number();
    }
    else {
      if ( (setwd29[LA(1)]&0x40) ) {
#line 2040 "./verilog.g"
        v_identifier();
      }
      else {
        if ( (LA(1)==V_LP) ) {
#line 2041 "./verilog.g"
          zzmatch(V_LP); zzCONSUME;
#line 2041 "./verilog.g"
          v_mintypmax_expression();
#line 2042 "./verilog.g"
          {
            zzBLOCK(zztasp3);
            zzMake0;
            {
            while ( (LA(1)==V_COMMA)
 ) {
#line 2042 "./verilog.g"
              zzmatch(V_COMMA); zzCONSUME;
#line 2042 "./verilog.g"
              v_mintypmax_expression();
              zzLOOP(zztasp3);
            }
            zzEXIT(zztasp3);
            }
          }
#line 2042 "./verilog.g"
          zzmatch(V_RP); zzCONSUME;
        }
        else {zzFAIL(1,zzerr97,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
      }
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd29, 0x80);
  }
}

void
#ifdef __USE_PROTOS
v_delay_control(void)
#else
v_delay_control()
#endif
{
#line 2045 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 2045 "./verilog.g"
  zzmatch(V_POUND); zzCONSUME;
#line 2045 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (setwd30[LA(1)]&0x1) ) {
#line 2045 "./verilog.g"
      v_number();
    }
    else {
      if ( (setwd30[LA(1)]&0x2) ) {
#line 2046 "./verilog.g"
        v_identifier();
      }
      else {
        if ( (LA(1)==V_LP) ) {
#line 2047 "./verilog.g"
          zzmatch(V_LP); zzCONSUME;
#line 2047 "./verilog.g"
          v_mintypmax_expression();
#line 2047 "./verilog.g"
          zzmatch(V_RP); zzCONSUME;
        }
        else {zzFAIL(1,zzerr98,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
      }
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd30, 0x4);
  }
}

void
#ifdef __USE_PROTOS
v_event_control(void)
#else
v_event_control()
#endif
{
#line 2050 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 2050 "./verilog.g"
  zzmatch(V_AT); zzCONSUME;
#line 2050 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (setwd30[LA(1)]&0x8) ) {
#line 2050 "./verilog.g"
      v_identifier();
    }
    else {
      if ( (LA(1)==V_LP)
 ) {
#line 2051 "./verilog.g"
        zzmatch(V_LP); zzCONSUME;
#line 2051 "./verilog.g"
        v_event_expression();
#line 2051 "./verilog.g"
        zzmatch(V_RP); zzCONSUME;
      }
      else {zzFAIL(1,zzerr99,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd30, 0x10);
  }
}

void
#ifdef __USE_PROTOS
v_event_expression(void)
#else
v_event_expression()
#endif
{
#line 2054 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 2055 "./verilog.g"
  v_event_expression2();
#line 2055 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (LA(1)==V_ORLIT) ) {
#line 2055 "./verilog.g"
      zzmatch(V_ORLIT); zzCONSUME;
#line 2055 "./verilog.g"
      v_event_expression2();
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd30, 0x20);
  }
}

void
#ifdef __USE_PROTOS
v_event_expression2(void)
#else
v_event_expression2()
#endif
{
#line 2058 "./verilog.g"
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
#line 2058 "./verilog.g"
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (setwd30[LA(1)]&0x40) ) {
#line 2058 "./verilog.g"
      v_expression();
    }
    else {
      if ( (LA(1)==V_POSEDGE) ) {
#line 2059 "./verilog.g"
        zzmatch(V_POSEDGE); zzCONSUME;
#line 2059 "./verilog.g"
        v_expression();
      }
      else {
        if ( (LA(1)==V_NEGEDGE) ) {
#line 2060 "./verilog.g"
          zzmatch(V_NEGEDGE); zzCONSUME;
#line 2060 "./verilog.g"
          v_expression();
        }
        else {zzFAIL(1,zzerr100,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
      }
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd30, 0x80);
  }
}
