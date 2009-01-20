## -*- makefile -*-
##
## To compile this, add AC_PROG_LIBTOOL to configure.ac
## also add contrib/vpi/Makefile to AC_CONFIG_FILES
## also copy ltmain.sh into the same firectory as configure.ac.
##
## This is not built by default.  In the future this will be a normal part of
## the build process.
##
## Note that this explicitly uses the libz and libbz2 libraries found in this
## distro as this might be necessary if your simulator is not already linked
## against those system libraries.
##
## 20jan09ajb
##

lib_LTLIBRARIES=	libsysvzt.la

libsysvzt_la_SOURCES= ../../src/libz/adler32.c ../../src/libz/compress.c ../../src/libz/crc32.c ../../src/libz/crc32.h \
	../../src/libz/deflate.c ../../src/libz/deflate.h ../../src/libz/gzio.c ../../src/libz/infback.c ../../src/libz/inffast.c \
	../../src/libz/inffast.h ../../src/libz/inffixed.h ../../src/libz/inflate.c ../../src/libz/inflate.h \
	../../src/libz/inftrees.c ../../src/libz/inftrees.h ../../src/libz/trees.c ../../src/libz/trees.h ../../src/libz/uncompr.c \
	../../src/libz/zconf.h ../../src/libz/zlib.h ../../src/libz/zutil.c ../../src/libz/zutil.h \
	../../src/libbz2/blocksort.c ../../src/libbz2/huffman.c ../../src/libbz2/crctable.c ../../src/libbz2/randtable.c \
	../../src/libbz2/bzcompress.c ../../src/libbz2/bzdecompress.c ../../src/libbz2/bzlib.c ../../src/libbz2/bzlib_private.h \
	../../src/helpers/lxt_write.h ../../src/helpers/lxt_write.c \
	../../src/helpers/lxt2_read.h ../../src/helpers/lxt2_read.c \
	../../src/helpers/lxt2_write.h ../../src/helpers/lxt2_write.c \
	../../src/helpers/vzt_read.h ../../src/helpers/vzt_read.c \
	../../src/helpers/vzt_write.h ../../src/helpers/vzt_write.c \
	sys_vzt.c sys_lxt2.c sys_lxt.c ../../src/wavealloca.h

libsysvzt_la_LDFLAGS = -version-info 1:0:0
	
AM_CFLAGS= -I../../src/libz/ -I../../src/libbz2/ -I../../src/ -I../../src/helpers/ -I.
