## -*- makefile -*-
##

# FIXME -- how to figure out automatically when we need these?
# 
AET2LIB = 
AIXFLAGS = -bmaxdata:0xd0000000/dsa -D_WAVE_BE32
BIGFILES = -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
#OBSOLETE_CFLAGS = -DGTK_ENABLE_BROKEN


bin_PROGRAMS= rtlbrowse

rtlbrowse_SOURCES = definehash.c fgetdynamic.c fgetdynamic.h \
	logfile.c splay.c tree_widget.c \
	stem_recurse.c jrb.c jrb.h $(srcdir)/../../src/helpers/vzt_read.c \
	$(srcdir)/../../src/helpers/lxt2_read.c splay.h \
	vlex.l vlex.h wavelink.h

LDADD = $(LIBZ_LDADD) $(LIBBZ2_LDADD) $(GTK_LIBS) $(AET2_LDADD)
AM_CFLAGS= -I$(top_srcdir) -I$(top_srcdir)/src  -I$(top_srcdir)/src/helpers \
	$(GTK_CFLAGS) $(LIBZ_CFLAGS) $(LIBBZ_CFLAGS) $(AET2_CFLAGS)

definehash.c: vpp_keyword.gperf
	$(GPERF) -C -I -N is_builtin_define $(srcdir)/vpp_keyword.gperf >definehash.c

BUILT_SOURCES= definehash.c

EXTRA_DIST= vpp_keyword.gperf

