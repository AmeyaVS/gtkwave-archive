/* C code produced by gperf version 2.7.2 */
/* Command-line: gperf -C -I -N is_builtin_define vpp_keyword.gperf  */
#include <string.h>

#define TOTAL_KEYWORDS 24
#define MIN_WORD_LENGTH 4
#define MAX_WORD_LENGTH 22
#define MIN_HASH_VALUE 4
#define MAX_HASH_VALUE 65
/* maximum key range = 62, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash (str, len)
     register const char *str;
     register unsigned int len;
{
  static const unsigned char asso_values[] =
    {
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66,  5,  0,
       0,  0,  0, 66, 15, 30, 66, 66,  0, 66,
      10, 66, 15, 66,  0, 25, 10, 23, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66
    };
  return len + asso_values[(unsigned char)str[len - 1]] + asso_values[(unsigned char)str[0]];
}

#ifdef __GNUC__
__inline
#endif
const char *
is_builtin_define (str, len)
     register const char *str;
     register unsigned int len;
{
  static const char * const wordlist[] =
    {
      "", "", "", "",
      "else",
      "endif",
      "define",
      "",
      "resetall",
      "",
      "celldefine",
      "", "",
      "endcelldefine",
      "",
      "default_nettype",
      "", "", "",
      "timescale",
      "endprotect",
      "",
      "delay_mode_distributed",
      "", "",
      "delay_mode_unit",
      "", "",
      "undef",
      "nounconnected_drive",
      "delay_mode_path",
      "",
      "protect",
      "",
      "uselib",
      "ifdef",
      "ifndef",
      "include",
      "", "",
      "unconnected_drive",
      "",
      "enable_portfaults",
      "disable_portfaults",
      "", "", "", "", "", "", "", "",
      "nosuppress_faults",
      "", "", "", "", "", "", "", "", "",
      "", "", "",
      "suppress_faults"
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key];

          if (*str == *s && !strcmp (str + 1, s + 1))
            return s;
        }
    }
  return 0;
}

/*
 * $Id$
 * $Log$
 * Revision 1.1  2007/04/21 21:08:51  gtkwave
 * changed from vertex to vermin
 *
 * Revision 1.2  2007/04/20 02:08:10  gtkwave
 * initial release
 *
 */

