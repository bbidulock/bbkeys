#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([bbkeys],[0.9.2],[https://github.com/bbidulock/bbkeys/issues])
AC_REVISION([0.9.2])

# set better defaults
AC_PREFIX_DEFAULT([/usr])
if test "$prefix" = /usr -o "$prefix" = NONE; then
	test "$sysconfdir" = '${prefix}/etc' && sysconfdir=/etc
	test "$sharedstatedir" = '${prefix}/com' && sharedstatedir=/var
	test "$localstatedir" = '${prefix}/var' && localstatedir=/var
fi

AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([scripts])
AC_USE_SYSTEM_EXTENSIONS
AC_CANONICAL_TARGET

DATE='2019-09-09'
AC_SUBST([DATE])

MDOCDATE='September 9, 2019'
AC_SUBST([MDOCDATE])

BRANCH='master'
AC_SUBST([BRANCH])

# Initialize Automake
AM_INIT_AUTOMAKE([1.16 dist-lzip no-dist-gzip std-options -Wall color-tests silent-rules tar-pax])
AM_MAINTAINER_MODE
AM_SILENT_RULES([yes])

AC_CHECK_PROGS([regex_cmd],[sed])
if test x$regex_cmd = "x"; then
 AC_MSG_ERROR([error. sed is required to build the default bbtoolsrc file.])
fi

AC_ARG_ENABLE([debug],
	AC_HELP_STRING([--enable-debug],
		[Enable debuggin support @<:@default=disabled@:>@]))
if test "x$enable_debug" = xyes ; then
	AC_DEFINE_UNQUOTED([DEBUG],[],[Define to enable debugging])
fi
AC_MSG_CHECKING([for debug support])
AC_MSG_RESULT([${enable_debug:-no}])

# Checks for programs.
AC_PROG_SED
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AM_PROG_AR
AC_PROG_MKDIR_P
AC_ARG_VAR([LD], [Linker loader command])

AC_ARG_VAR([MARKDOWN],[markdown command. @<:@default=markdown@:>@])
AC_CHECK_PROGS([MARKDOWN],[markdown],[${am_missing_run}markdown],[$PATH])

AC_ARG_VAR([ASCIIDOC],[asciidoc command. @<:@default=asciidoctor@:>@])
AC_CHECK_PROGS([ASCIIDOC],[asciidoc asciidoctor],[${am_missing_run}asciidoc],[$PATH])

AC_ARG_VAR([ASCIIDOC_BACKEND],[asciidoc backend @<:@default=NONE@:>@])
if test -z "$ASCIIDOC_BACKEND"; then
	case "$ASCIIDOC" in
		asciidoc) ASCIIDOC_BACKEND="--backend=html4" ;;
		asciidoctor) ASCIIDOC_BACKEND="--backend=html5" ;;
		*) AC_MSG_WARN([ASCIIDOC $ASCIIDOC is not supported.]) ;;
	esac
fi

AC_ARG_VAR([ASCIIMAN],[asciidoc to manpage converter. @<:@default=a2x@:>@])
AC_CHECK_PROGS([ASCIIMAN],[a2x asciidoctor],[${am_missing_run}a2x],[$PATH])

AC_ARG_VAR([ASCIIMAN_FORMAT],[asciidoc manpage option @<:@default=NONE@:>@])
if test -z "$ASCIIMAN_FORMAT"; then
	case "$ASCIIMAN" in
		a2x) ASCIIMAN_FORMAT="--format=manpage" ;;
		asciidoctor) ASCIIMAN_FORMAT="--backend=manpage" ;;
		*) AC_MSG_WARN([ASCIIMAN $ASCIIMAN is not supported.]) ;;
	esac
fi

AC_ARG_VAR([LINKS],[links command. @<:@default=lynx@:>@])
AC_CHECK_PROGS([LINKS],[lynx links],[${am_missing_run}lynx],[$PATH])
if test "$LINKS" = "links"; then
	LINKS="links -codepage utf-8"
fi

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

# Checks for libraries.

# Checks for header files.
AS_BOX([Header Files])
AC_HEADER_STDC
AC_PATH_X
AC_PATH_XTRA
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS(ctype.h strings.h fcntl.h libgen.h locale.h process.h signal.h stdio.h time.h errno.h unistd.h sys/signal.h sys/param.h sys/select.h sys/stat.h sys/time.h sys/types.h sys/wait.h stdlib.h string.h malloc.h)

# Checks for typedefs, structures, and compiler characteristics.
AS_BOX([Typedefs, Structures, Compiler])
AC_C_CONST
AC_C_INLINE
AC_HEADER_TIME
AC_STRUCT_TM

# Checks for library functions.
AS_BOX([Library Functions])
dnl AC_FUNC_FORK
dnl AC_FUNC_MALLOC
AC_TYPE_SIGNAL
dnl AC_FUNC_STAT
AC_CHECK_FUNCS(setlocale sigaction strftime gettimeofday memset strcasecmp strchr strcspn strdup strncasecmp select strstr)

PKG_PROG_PKG_CONFIG

AS_BOX([X11 Extension Libraries])

PKG_CHECK_MODULES([X11], [x11])
PKG_CHECK_MODULES([XEXT], [xext])
PKG_CHECK_MODULES([XFT], [xft])
dnl pkgconfig checks for blackbox library
PKG_CHECK_MODULES([LIBBT], [libbt])

dnl Output files
AC_OUTPUT([
	version.h
	Makefile
	src/Makefile
	doc/Makefile
	data/Makefile
	rpm/Makefile
	rpm/bbkeys.spec
])

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
