#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(gtkwave, 3.x, bybell@nc.rr.com)
AC_CONFIG_SRCDIR([src/vcd.c])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADER([config.h])
AM_MAINTAINER_MODE

# Checks for build options

AC_MSG_CHECKING([if gtk1 or gtk2 should be used])

GTK1=no
AC_ARG_ENABLE([gtk1],
[  --enable-gtk1           Causes the GTK+ frontend to be built with gtk1 
                          instead of gtk2.],
[
if test "X$enable_gtk1" = "Xno" ; then
	GTK1=no
else
	GTK1=yes
fi
],
[
GTK1=no
])

if test "X$GTK1" = "Xyes" ; then
	AC_MSG_RESULT([gtk1])
else
	AC_MSG_RESULT([gtk2])
	CPPFLAGS="$CPPFLAGS -DWAVE_USE_GTK2"
fi


AC_MSG_CHECKING([if ae2 support should be used])

AE2_CFLAGS=
AET2_LDADD=
AE2=no
AC_ARG_ENABLE([ae2],
[  --enable-ae2            Compiles in support for AE2 files via simarama.],
[
if test "X$enable_ae2" = "Xno" ; then
	AE2=no
else
	AE2=yes
fi
],
[
AE2=no
])

if test "X$AE2" = "Xyes" ; then
	AET2_CFLAGS="-DAET2_IS_PRESENT"
	AET2_LDADD="/afs/awd/projects/simarama/releases/latest/libae2rw.so"
	AC_MSG_RESULT([yes])
else
	AC_MSG_RESULT([no])
fi

AC_SUBST(AET2_CFLAGS)
AC_SUBST(AET2_LDADD)


# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_LEX
AC_PROG_RANLIB

#
# gperf only needed if the user updates the gperf data
# files which only developers will be doing...
#
# AC_PATH_PROG(GPERF, [gperf], [notfound])
# if test "$GPERF" = "notfound" ; then
#	AC_MSG_ERROR([Couldn't find a usable gperf program.
# Please install gperf which is available from
# ftp://ftp.gnu.org/pub/gnu/gperf/
# ])
# fi

# Checks for libraries.
AC_CHECK_LIB([m], [sqrt])
AC_CHECK_LIB([pthread], [main])

# included or system libz and libbz2.  By default, we search for the
# system libz and libbz2.  If not found, build the included ones.  
# The --enable-local-libz and --enable-local_libbz2 arguments can be
# used to force the use of the included libs.  --disable-local-libz
# and --disable-local-libbz2 can be used to force the use of system
# libs
AC_ARG_ENABLE([local-libz],
[  --enable-local-libz     Use the bundled libz instead of any libz which may
                          already be installed on your system.  Default is to
                          first look for an installed libz and fall back to
                          using the bundled one.],
[
if test "X$enable_local_libz" = "Xno" ; then
	force_system_libz=yes
else
	force_bundled_libz=yes
fi
], , )

need_libz=no
LIBZ_LDADD=
LIBZ_CFLAGS=
if test "X$force_bundled_libz" != "Xyes" ; then
	AC_CHECK_LIB([z], [gzread], [LIBZ_LDADD=-lz] , [need_libz=yes])
	if test "$need_libz" = "yes" -a "X$force_system_libz" = "Xyes" ; then
		AC_MSG_ERROR([
You have disabled building the bundled libz but no system libz could
be found.  Either allow building the bundled libz (by not using
--disable-local-libz) or make sure your system has a libz installed])
	fi
else
	need_libz=yes
fi

need_libbz2=no
LIBBZ2_LDADD=
LIBBZ2_CFLAGS=
if test "X$force_bundled_libbz2" != "Xyes" ; then
	AC_CHECK_LIB([bz2], [BZ2_bzread], [LIBBZ2_LDADD=-lbz2], [need_libbz2=yes])
	if test "$need_libbz2" = "yes" -a "X$force_system_libbz2" = "Xyes" ; then
		AC_MSG_ERROR([
You have disabled building the bundled libbz2 but no system libbz2 could
be found.  Either allow building the bundled libbz2 (by not using
--disable-local-libbz2) or make sure your system has a libbz2 installed])
	fi
else
	need_libbz2=yes
fi

LIBZ_DIR=
if test "$need_libz" = "yes" ; then
	LIBZ_CFLAGS='-I$(top_srcdir)/src/libz'
	LIBZ_LDADD='$(top_builddir)/src/libz/libz.a'
	LIBZ_DIR=libz
fi
AC_SUBST(LIBZ_CFLAGS)
AC_SUBST(LIBZ_LDADD)
AC_SUBST(LIBZ_DIR)

LIBBZ2_DIR=
if test "$need_libbz2" = "yes" ; then
	LIBBZ2_CFLAGS='-I$(top_srcdir)/src/libbz2'
	LIBBZ2_LDADD='$(top_builddir)/src/libbz2/libbz2.a'
	LIBBZ2_DIR=libbz2
fi
AC_SUBST(LIBBZ2_CFLAGS)
AC_SUBST(LIBBZ2_LDADD)
AC_SUBST(LIBBZ2_DIR)

# ------------- GTK -------------------
if test "X$GTK1" = "Xyes" ; then
	AC_PATH_PROG(GTK_CONFIG, [gtk-config], [notfound])
	if test "$GTK_CONFIG" = "notfound"; then
		AC_MSG_ERROR([You must have gtk installed on your system])
	else
		AC_MSG_CHECKING([gtk1 version])
		GTK_VER=`$GTK_CONFIG --version`
		AC_MSG_RESULT([$GTK_VER])

		case $GTK_VER in
			1.2.* )
				AC_MSG_CHECKING([for gtk1 CFLAGS])
				GTK_CFLAGS=`$GTK_CONFIG --cflags`
				AC_MSG_RESULT([$GTK_CFLAGS])
				AC_MSG_CHECKING([for gtk1 libs])
				GTK_LIBS=`$GTK_CONFIG --libs`
				AC_MSG_RESULT([$GTK_LIBS])
				;;

			* )
				AC_MSG_ERROR([when building with gtk1, you need version 1.2.*])
				;;
		esac
	fi
else
	PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.2.0)
	GTK_VER=`$PKG_CONFIG gtk+-2.0 --modversion`
fi 

# Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h inttypes.h libintl.h limits.h malloc.h stddef.h stdint.h stdlib.h string.h strings.h sys/time.h unistd.h wchar.h wctype.h rpc/xdr.h getopt.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_FUNC_FSEEKO
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_SETVBUF_REVERSED
AC_FUNC_STAT
AC_FUNC_STRTOD
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([atexit btowc bzero dup2 memmove memset munmap pow putenv re_comp realpath regcomp select setenv strcasecmp strchr strdup strerror strncasecmp strrchr strstr getopt_long setenv unsetenv])
AC_CHECK_LIB(nsl,xdrmem_create)
AC_CHECK_LIB(rpc,xdrmem_create)

AC_CONFIG_FILES([Makefile
		doc/Makefile
		contrib/Makefile
		contrib/pccts/IBM_VISUAL_AGE_PROJECTS/Makefile
		contrib/pccts/Makefile
		contrib/pccts/antlr/Makefile
		contrib/pccts/dlg/Makefile
		contrib/pccts/h/Makefile
		contrib/pccts/sorcerer/Makefile
		contrib/pccts/sorcerer/h/Makefile
		contrib/pccts/sorcerer/lib/Makefile
		contrib/pccts/sorcerer/test/Makefile
		contrib/pccts/sorcerer/test/test7/Makefile
		contrib/pccts/sorcerer/testcpp/Makefile
		contrib/pccts/support/DECmms/Makefile
		contrib/pccts/support/Makefile
		contrib/pccts/support/genmk/Makefile
		contrib/pccts/support/rexpr/Makefile
		contrib/pccts/support/set/Makefile
		contrib/pccts/support/sym/Makefile
		contrib/pccts/testcpp/Makefile
		contrib/rtlbrowse/Makefile
		contrib/vermin/Makefile
		examples/Makefile
		man/Makefile
		src/Makefile
		src/helpers/Makefile
		src/libbz2/Makefile
		src/libz/Makefile
])


AC_MSG_NOTICE([         
--------------------------------------------
  Configuration summary:

  gtk1          : $GTK1
  gtk version   : $GTK_VER
  GTK_CFLAGS    : $GTK_CFLAGS
  GTK_LIBS      : $GTK_LIBS

  LIBZ_CFLAGS   : $LIBZ_CFLAGS
  LIBZ_LDADD    : $LIBZ_LDADD

  LIBBZ2_CFLAGS : $LIBBZ2_CFLAGS
  LIBBZ2_LDADD  : $LIBBZ2_LDADD

  AET2_CFLAGS   : $AET2_CFLAGS
  AET2_LDADD    : $AET2_LDADD

  CPPFLAGS      : $CPPFLAGS
  CFLAGS        : $CFLAGS
  LIBS          : $LIBS
--------------------------------------------
])

AC_OUTPUT

