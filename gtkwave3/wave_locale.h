
#ifndef __WAVE_LOCALE_H__
#define __WAVE_LOCALE_H__

#include "config.h"

#if HAVE_SETENV && HAVE_UNSETENV
#define WAVE_LOCALE_FIX \
{ \
char *e = getenv("LANG"); \
if(e) \
        { \
        if(strcmp(e, "C")) \
                { \
                setenv("LC_NUMERIC", "C", 1); \
                setenv("LC_COLLATE", "C", 1); \
                setenv("LC_CTYPE", "C", 1); \
                } \
        } \
e = getenv("LC_ALL"); \
if(e) \
        { \
        if(strcmp(e, "C")) \
                { \
                unsetenv("LC_ALL"); \
                } \
        } \
}
#else
#define WAVE_LOCALE_FIX \
{ \
putenv(strdup("LANG=C")); \
putenv(strdup("LC_NUMERIC=C")); \
putenv(strdup("LC_COLLATE=C")); \
putenv(strdup("LC_CTYPE=C")); \
putenv(strdup("LC_ALL=C")); \
}
#endif

#endif /* __WAVE_LOCALE_H__*/

