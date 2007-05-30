/* C code produced by gperf version 2.7.2 */
/* Command-line: gperf -o -i 1 -C -k '1-3,$' -L C -H keyword_hash -N check_identifier -tT verilog_keyword.gperf  */

#include <string.h>
#include "keyword_tokens.h"

struct verilog_keyword { const char *name; int token; };


#define TOTAL_KEYWORDS 100
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 12
#define MIN_HASH_VALUE 7
#define MAX_HASH_VALUE 284
/* maximum key range = 278, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
keyword_hash (str, len)
     register const char *str;
     register unsigned int len;
{
  static const unsigned short asso_values[] =
    {
      285, 285, 285, 285, 285, 285, 285, 285, 285, 285,
      285, 285, 285, 285, 285, 285, 285, 285, 285, 285,
      285, 285, 285, 285, 285, 285, 285, 285, 285, 285,
      285, 285, 285, 285, 285, 285, 285, 285, 285, 285,
      285, 285, 285, 285, 285, 285, 285, 285,  71,  56,
      285, 285, 285, 285, 285, 285, 285, 285, 285, 285,
      285, 285, 285, 285, 285, 285, 285, 285, 285, 285,
      285, 285, 285, 285, 285, 285, 285, 285, 285, 285,
      285, 285, 285, 285, 285, 285, 285, 285, 285, 285,
      285, 285, 285, 285, 285, 285, 285,  46,  11,  71,
        1,   1,  66,  96,  26,  51,   6,  21, 115, 101,
        1,  21,  81, 285,   1,   1,   6,   1,  16,  41,
        6,  11,  11, 285, 285, 285, 285, 285, 285, 285,
      285, 285, 285, 285, 285, 285, 285, 285, 285, 285,
      285, 285, 285, 285, 285, 285, 285, 285, 285, 285,
      285, 285, 285, 285, 285, 285, 285, 285, 285, 285,
      285, 285, 285, 285, 285, 285, 285, 285, 285, 285,
      285, 285, 285, 285, 285, 285, 285, 285, 285, 285,
      285, 285, 285, 285, 285, 285, 285, 285, 285, 285,
      285, 285, 285, 285, 285, 285, 285, 285, 285, 285,
      285, 285, 285, 285, 285, 285, 285, 285, 285, 285,
      285, 285, 285, 285, 285, 285, 285, 285, 285, 285,
      285, 285, 285, 285, 285, 285, 285, 285, 285, 285,
      285, 285, 285, 285, 285, 285, 285, 285, 285, 285,
      285, 285, 285, 285, 285, 285, 285, 285, 285, 285,
      285, 285, 285, 285, 285, 285
    };
  register int hval = len;

  switch (hval)
    {
      default:
      case 3:
        hval += asso_values[(unsigned char)str[2]];
      case 2:
        hval += asso_values[(unsigned char)str[1]];
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}

#ifdef __GNUC__
__inline
#endif
const struct verilog_keyword *
check_identifier (str, len)
     register const char *str;
     register unsigned int len;
{
  static const struct verilog_keyword wordlist[] =
    {
      {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {"end", V_END},
      {""}, {""}, {""},
      {"endcase", V_ENDCASE},
      {"endtable", V_ENDTABLE},
      {"endmodule", V_ENDMODULE},
      {"rtran", V_RTRAN},
      {"endfunction", V_ENDFUNCTION},
      {"endprimitive", V_ENDPRIMITIVE},
      {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {"endspecify", V_ENDSPECIFY},
      {"or", V_ORLIT},
      {""},
      {"nor", V_NORLIT},
      {""},
      {"event", V_EVENT},
      {""},
      {"endtask", V_ENDTASK},
      {"xor", V_XORLIT},
      {"xnor", V_XNORLIT},
      {""}, {""}, {""},
      {"not", V_NOTLIT},
      {""}, {""},
      {"output", V_OUTPUT},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""},
      {"and", V_ANDLIT},
      {"nand", V_NANDLIT},
      {""},
      {"assign", V_ASSIGN},
      {""},
      {"deassign", V_DEASSIGN},
      {"tran", V_TRAN},
      {""}, {""},
      {"disable", V_DISABLE},
      {""}, {""},
      {"trior", V_TRIOR},
      {"triand", V_TRIAND},
      {"integer", V_INTEGER},
      {"wor", V_WOR},
      {""},
      {"table", V_TABLE},
      {""},
      {"strong1", V_STRONG1},
      {"rtranif1", V_RTRANIF1},
      {""}, {""}, {""}, {""},
      {"function", V_FUNCTION},
      {"task", V_TASK},
      {""}, {""},
      {"default", V_DEFAULT},
      {""},
      {"join", V_JOIN},
      {"inout", V_INOUT},
      {""},
      {"strong0", V_STRONG0},
      {"rtranif0", V_RTRANIF0},
      {""}, {""},
      {"notif1", V_NOTIF1},
      {""},
      {"for", V_FOR},
      {"wand", V_WAND},
      {"force", V_FORCE},
      {"repeat", V_REPEAT},
      {"forever", V_FOREVER},
      {"vectored", V_VECTORED},
      {"wire", V_WIRE},
      {""}, {""},
      {"specify", V_SPECIFY},
      {""},
      {"edge", V_EDGE},
      {""},
      {"notif0", V_NOTIF0},
      {"negedge", V_NEGEDGE},
      {""}, {""},
      {"rnmos", V_RNMOS},
      {""},
      {"posedge", V_POSEDGE},
      {"tri", V_TRI},
      {"fork", V_FORK},
      {"begin", V_BEGIN},
      {""},
      {"tranif1", V_TRANIF1},
      {""},
      {"tri1", V_TRI1},
      {""}, {""}, {""},
      {"else", V_ELSE},
      {"case", V_CASE},
      {"while", V_WHILE},
      {"release", V_RELEASE},
      {""},
      {"scalared", V_SCALARED},
      {"nmos", V_NMOS},
      {"casex", V_CASEX},
      {"module", V_MODULE},
      {"tranif0", V_TRANIF0},
      {""},
      {"tri0", V_TRI0},
      {"casez", V_CASEZ},
      {""}, {""}, {""},
      {"parameter", V_PARAMETER},
      {""},
      {"bufif1", V_BUFIF1},
      {""}, {""},
      {"primitive", V_PRIMITIVE},
      {"input", V_INPUT},
      {""},
      {"supply1", V_SUPPLY1},
      {"buf", V_BUF},
      {"wait", V_WAIT},
      {"weak1", V_WEAK1},
      {""}, {""}, {""}, {""}, {""},
      {"bufif0", V_BUFIF0},
      {""}, {""}, {""}, {""},
      {"trireg", V_TRIREG},
      {"supply0", V_SUPPLY0},
      {""},
      {"time", V_TIME},
      {"weak0", V_WEAK0},
      {""}, {""},
      {"real", V_REAL},
      {"large", V_LARGE},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {"defparam", V_DEFPARAM},
      {""},
      {"rcmos", V_RCMOS},
      {""}, {""}, {""}, {""}, {""},
      {"if", V_IF},
      {""}, {""}, {""},
      {"rpmos", V_RPMOS},
      {""}, {""}, {""},
      {"specparam", V_SPECPARAM},
      {""}, {""}, {""},
      {"reg", V_REG},
      {"cmos", V_CMOS},
      {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {"pulldown", V_PULLDOWN},
      {""},
      {"pmos", V_PMOS},
      {"always", V_ALWAYS},
      {"medium", V_MEDIUM},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""},
      {"initial", V_INITIAL},
      {""}, {""}, {""}, {""},
      {"macromodule", V_MACROMODULE},
      {""}, {""}, {""}, {""},
      {"highz1", V_HIGHZ1},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""},
      {"highz0", V_HIGHZ0},
      {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {"pull1", V_PULL1},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {"small", V_SMALL},
      {""}, {""}, {""}, {""},
      {"pull0", V_PULL0},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""},
      {"pullup", V_PULLUP}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = keyword_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}

int verilog_keyword_code(const char *s, unsigned int len)
{
const struct verilog_keyword *rc = check_identifier(s, len);
return(rc ? rc->token : V_IDENTIFIER);
}

/*
 * $Id$
 * $Log$
 * Revision 1.1  2007/04/21 21:08:51  gtkwave
 * changed from vertex to vermin
 *
 * Revision 1.2  2007/04/20 02:08:11  gtkwave
 * initial release
 *
 */

