## -*- makefile -*-
##

# FIXME -- how to figure out automatically when we need these?
# 
AET2LIB = 
AIXFLAGS = -bmaxdata:0xd0000000/dsa -D_WAVE_BE32
BIGFILES = -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
LIBFST_CFLAGS  = -I../../src/helpers/fst
LIBFST_LDADD   = -L../../src/helpers/fst -lfst
LIBLZMA_CFLAGS = -I../../src/liblzma
LIBLZMA_LDADD  = -L../../src/liblzma -llzma


bin_PROGRAMS= rtlbrowse

rtlbrowse_SOURCES = definehash.c fgetdynamic.c fgetdynamic.h \
	logfile.c splay.c tree_widget.c \
	stem_recurse.c jrb.c jrb.h $(srcdir)/../../src/helpers/vzt_read.c \
	$(srcdir)/../../src/helpers/lxt2_read.c splay.h \
	vlex.l vlex.h wavelink.h

LDADD= $(GTK_LIBS) $(LIBZ_LDADD) $(LIBBZ2_LDADD) $(LIBLZMA_LDADD) $(LIBFST_LDADD) $(AET2_LDADD)
AM_CFLAGS= -I$(top_srcdir) -I$(top_srcdir)/src  -I$(top_srcdir)/src/helpers \
	$(GTK_CFLAGS) $(LIBZ_CFLAGS) $(LIBBZ2_CFLAGS) $(LIBLZMA_CFLAGS) $(LIBFST_CFLAGS) $(AET2_CFLAGS)

definehash.c: vpp_keyword.gperf
	$(GPERF) -C -I -N is_builtin_define $(srcdir)/vpp_keyword.gperf >definehash.c

BUILT_SOURCES= definehash.c

EXTRA_DIST= vpp_keyword.gperf

