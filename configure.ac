dnl Process this file with autoconf to produce a configure script.
AC_INIT(src/main.cpp)
AM_INIT_AUTOMAKE(bbkeys, 0.9.0)
AM_MAINTAINER_MODE
AC_PREFIX_DEFAULT(/usr/local)

AC_CHECK_PROGS(regex_cmd, sed)
if test x$regex_cmd = "x"; then
 AC_MSG_ERROR([error. sed is required to build the default bbtoolsrc file.])
fi

dnl get the debug values right
AC_DEFUN(AC_SET_DEBUG,
[
  _CFLAGS="-g -Wall -DDEBUG"
  _CXXFLAGS="-g -Wall -DDEBUG"
  _LDFLAGS=""
  case $host_os in
  [*solaris*)]
    dnl On solaris, we get a ton of multiply-defined errors if we don't include
    dnl this parameter
    _LDFLAGS="$_LDFLAGS -z muldefs"
    ;;
  esac
  test "$CFLAGS" = "" && CFLAGS=$_CFLAGS
  test "$CXXFLAGS" = "" && CXXFLAGS=$_CXXFLAGS
  test "$LDFLAGS" = "" && LDFLAGS=$_LDFLAGS
])

AC_DEFUN(AC_SET_NODEBUG,
[
  _CFLAGS="-DNDEBUG"
  _CXXFLAGS="-DNDEBUG"
  _LDFLAGS=""
  case $host_os in
  [*solaris*)]
     _CFLAGS="$_CFLAGS -O"
     _CXXFLAGS="$_CXXFLAGS -O"
     dnl On solaris, we get a ton of multiply-defined errors if we don't include
     dnl this parameter
     _LDFLAGS="$_LDFLAGS -z muldefs"
     ;;
  [*)]
     _CFLAGS="$_CFLAGS -O2"
     _CXXFLAGS="$_CXXFLAGS -O2"
     ;;
  esac
  test "$CFLAGS" = "" && CFLAGS=$_CFLAGS
  test "$CXXFLAGS" = "" && CXXFLAGS=$_CXXFLAGS
  test "$LDFLAGS" = "" && LDFLAGS=$_LDFLAGS
])

AC_ARG_ENABLE(debug,[  --enable-debug	create debugging code [default=no]],
[
if test $enableval = "no";
 then AC_SET_NODEBUG
 else AC_SET_DEBUG
fi
],
AC_SET_NODEBUG)

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

dnl minimum version we can live with
PERL_VERSION=5.004
dnl check for perl for our command-line configurator
AC_PATH_PROGS(PERL, $PERL perl5 perl )
if test -z "$PERL" || test "$PERL" = ":"; then
    AC_MSG_ERROR([perl not found in \$PATH])
fi

AC_MSG_CHECKING([for minimum required perl version >= $PERL_VERSION])
_perl_version=`PERL_VERSION=$PERL_VERSION $PERL -e 'print "$]"; if ($] >= $ENV{PERL_VERSION}) { exit(0); } else { exit(1); }' 2>&5`
_perl_res=$?
AC_MSG_RESULT([$_perl_version])

if test "$_perl_res" != 0; then
    AC_MSG_ERROR([Perl $PERL_VERSION or higher is required.])
fi

AC_MSG_CHECKING([for full perl installation])
_perl_archlib=`$PERL -e 'use Config; if ( -d $Config{archlib} ) { exit(0); } else { exit(1); }' 2>&5`
_perl_res=$?
if test "$_perl_res" != 0; then
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([Cannot find Config.pm or \$Config{archlib}.  A full perl installation is required.])
else
    AC_MSG_RESULT([yes])    
fi


# Checks for header files.
AC_PATH_XTRA
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS(ctype.h strings.h fcntl.h libgen.h locale.h process.h signal.h stdio.h time.h errno.h unistd.h sys/signal.h sys/param.h sys/select.h sys/stat.h sys/time.h sys/types.h sys/wait.h stdlib.h string.h malloc.h)

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_HEADER_TIME
AC_STRUCT_TM

# Checks for library functions.
dnl AC_FUNC_FORK
dnl AC_FUNC_MALLOC
AC_TYPE_SIGNAL
dnl AC_FUNC_STAT
AC_CHECK_FUNCS(setlocale sigaction strftime gettimeofday memset strcasecmp strchr strcspn strdup strncasecmp select strstr)

dnl these have to be here for X tests...
CFLAGS="$CFLAGS $X_CFLAGS"
CXXFLAGS="$CXXFLAGS $X_CFLAGS"
LDFLAGS="$LDFLAGS $X_LIBS $X_PRE_LIBS"


dnl Checks for X libraries.
AC_CHECK_LIB(X11, XOpenDisplay, LIBS="-lX11",
             AC_MSG_ERROR(XOpenDisplay not found in -lX11))

dnl Checks for Xextension
AC_CHECK_LIB(Xext, XMissingExtension, LIBS="$LIBS -lXext",
             AC_MSG_ERROR(XMissingExtension not found in -lXext))

dnl pkgconfig checks for blackbox library
PKG_CHECK_MODULES(LIBBT, libbt)
CXXFLAGS="$CXXFLAGS $LIBBT_CFLAGS"
LIBS="$LIBS $LIBBT_LIBS"

dnl generate the config header
AM_CONFIG_HEADER(config.h) 

dnl Output files
AC_OUTPUT( \
version.h \
Makefile \
src/Makefile \
doc/Makefile \
data/Makefile \
doc/bbkeys.1 \
rpm/Makefile \
rpm/bbkeys.spec \
)

dnl Print results
AC_MSG_RESULT([])
AC_MSG_RESULT([	$PACKAGE version $VERSION configured successfully.])
AC_MSG_RESULT([])
AC_MSG_RESULT([Using '$prefix' for installation.])
AC_MSG_RESULT([Using '$CXX' for C++ compiler.])
AC_MSG_RESULT([Building with '$CXXFLAGS' for C++ compiler flags.])
AC_MSG_RESULT([Building with '$LDFLAGS' for linker flags.])
AC_MSG_RESULT([Building with '$LIBS' libraries.])
AC_MSG_RESULT([])
