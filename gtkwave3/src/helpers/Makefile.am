## -*- makefile -*-
##

BIGFILES = -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
AIXFLAGS = -bmaxdata:0xd0000000/dsa
LIBLZMA_CFLAGS = -I../liblzma
LIBLZMA_LDADD  = -L../liblzma -llzma

AM_CFLAGS=	-I$(srcdir)/.. -I$(srcdir)/../.. $(LIBZ_CFLAGS) $(LIBBZ2_CFLAGS) $(LIBLZMA_CFLAGS)

bin_PROGRAMS= fst2vcd ghwdump lxt2miner lxt2vcd mvl2lxt mvl2vcd \
	shmidcat tex2vcd tla2vcd vcd2lxt vcd2lxt2 vcd2vzt \
	vzt2vcd vztminer

fst2vcd_SOURCES= fst2vcd.c ./fst/fastlz.c ./fst/fastlz.h ./fst/fstapi.c ./fst/fstapi.h
fst2vcd_LDADD= $(LIBZ_LDADD)

mvl2lxt_SOURCES= mvl2lxt.c lxt_write.c lxt_write.h
mvl2lxt_LDADD= $(LIBZ_LDADD) $(LIBBZ2_LDADD)

vcd2lxt_SOURCES= vcd2lxt.c lxt_write.c lxt_write.h v2l_analyzer.h v2l_debug.c v2l_debug.h
vcd2lxt_LDADD= $(LIBZ_LDADD) $(LIBBZ2_LDADD)

lxt2vcd_SOURCES= lxt2_read.c lxt2_read.h lxt2vcd.c scopenav.c
lxt2vcd_LDADD= $(LIBZ_LDADD)

vcd2lxt2_SOURCES= vcd2lxt2.c lxt2_write.c lxt2_write.h v2l_analyzer_lxt2.h v2l_debug_lxt2.c v2l_debug_lxt2.h
vcd2lxt2_LDADD= $(LIBZ_LDADD)

vzt2vcd_SOURCES= vzt_read.c vzt_read.h vzt2vcd.c scopenav.c
vzt2vcd_LDADD= $(LIBZ_LDADD) $(LIBBZ2_LDADD) $(LIBLZMA_LDADD)

vcd2vzt_SOURCES= vcd2vzt.c vzt_write.c vzt_write.h v2l_analyzer_lxt2.h v2l_debug_lxt2.c v2l_debug_lxt2.h
vcd2vzt_LDADD= $(LIBZ_LDADD) $(LIBBZ2_LDADD) $(LIBLZMA_LDADD)

vztminer_SOURCES= vztminer.c vzt_read.c vzt_read.h
vztminer_LDADD= $(LIBZ_LDADD) $(LIBBZ2_LDADD) $(LIBLZMA_LDADD)

lxt2miner_SOURCES= lxt2miner.c lxt2_read.c lxt2_read.h
lxt2miner_LDADD= $(LIBZ_LDADD)

tla2vcd_SOURCES= tla2vcd.c $(srcdir)/../gnu_regex.c $(srcdir)/../gnu_regex.h
tla2vcd_LDADD= $(LIBZ_LDADD) $(GTK_LIBS)

ghwdump_SOURCES= ghwdump.c $(srcdir)/../ghwlib.c
