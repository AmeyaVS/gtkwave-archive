/* C code produced by gperf version 2.7.2 */
/* Command-line: gperf -o -i 1 -C -k '1,$' -L C -H keyword_hash -N check_identifier -tT vcd_keywords.gperf  */

/* AIX may need this for alloca to work */
#if defined _AIX
  #pragma alloca
#endif

#include <config.h>
#include <string.h>
#include "vcd.h"

struct vcd_keyword { const char *name; int token; };


#define TOTAL_KEYWORDS 23
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 14
#define MIN_HASH_VALUE 6
#define MAX_HASH_VALUE 41
/* maximum key range = 36, duplicates = 0 */

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
  static const unsigned char asso_values[] =
    {
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42,  6, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 21, 26,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
       1, 16, 42, 11, 42,  6, 42, 42,  1, 42,
       1, 16, 31, 42,  1,  1,  1, 42, 42, 19,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42
    };
  return len + asso_values[(unsigned char)str[len - 1]] + asso_values[(unsigned char)str[0]];
}

#ifdef __GNUC__
__inline
#endif
const struct vcd_keyword *
check_identifier (str, len)
     register const char *str;
     register unsigned int len;
{
  static const struct vcd_keyword wordlist[] =
    {
      {""}, {""}, {""}, {""}, {""}, {""},
      {"real", V_REAL},
      {"trior", V_TRIOR},
      {"triand", V_TRIAND},
      {"in", V_IN},
      {"tri", V_TRI},
      {"$end", V_END},
      {"inout", V_INOUT},
      {""},
      {"integer", V_INTEGER},
      {"reg", V_REG},
      {"real_parameter", V_REAL_PARAMETER},
      {""},
      {"trireg", V_TRIREG},
      {""},
      {"out", V_OUT},
      {"time", V_TIME},
      {"event", V_EVENT},
      {"wor", V_WOR},
      {"wand", V_WAND},
      {""},
      {"tri0", V_TRI0},
      {""}, {""},
      {"supply0", V_SUPPLY0},
      {""},
      {"tri1", V_TRI1},
      {""}, {""},
      {"supply1", V_SUPPLY1},
      {""},
      {"port", V_PORT},
      {""}, {""},
      {"wire", V_WIRE},
      {""},
      {"parameter", V_PARAMETER}
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

int vcd_keyword_code(const char *s, unsigned int len)
{
const struct vcd_keyword *rc = check_identifier(s, len);
return(rc ? rc->token : V_STRING);
}

