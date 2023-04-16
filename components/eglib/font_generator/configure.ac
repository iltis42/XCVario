#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(
	[eglib-font_generator],
	m4_esyscmd([build-aux/git-version-gen .tarball-version]),
	[https://github.com/fornellas/eglib/issues],
	[eglib],
	[https://fornellas.github.io/eglib/])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AM_SILENT_RULES([yes])
AC_CONFIG_SRCDIR([font_generator.c])
AC_CONFIG_MACRO_DIR([/usr/share/aclocal])

##
## Misc
##

# Common warning flags
CFLAGS_WARNINGS="-Wextra -Wshadow -Wno-unused-variable -Wimplicit-function-declaration"
CFLAGS_WARNINGS="$CFLAGS_WARNINGS -Wredundant-decls -Wstrict-prototypes -Wmissing-prototypes"
CFLAGS_WARNINGS="$CFLAGS_WARNINGS -Wall -Wundef"
CFLAGS_WARNINGS="$CFLAGS_WARNINGS -Werror -Wfatal-errors"
AC_SUBST([CFLAGS_WARNINGS], [$CFLAGS_WARNINGS])

##
## Checks for programs.
##

: ${CFLAGS="-g -O3"}
AC_PROG_CC
PKG_PROG_PKG_CONFIG

##
## Checks for libraries.
##

PKG_CHECK_MODULES([FREETYPE2], [freetype2 >= 2.10.1])

##
## Checks for header files.
##

AC_CHECK_HEADERS([stdint.h], [], AC_MSG_FAILURE([stdint.h not found!]))
AC_CHECK_HEADERS([stdlib.h], [], AC_MSG_FAILURE([stdlib.h not found!]))

##
## Checks for typedefs, structures, and compiler characteristics.
##

AC_CHECK_HEADER_STDBOOL
if test "x$ac_cv_header_stdbool_h" != xyes ; then
	AC_MSG_FAILURE([stdbool.h not found!])
fi
AC_TYPE_INT16_T
AC_TYPE_UINT16_T
AC_TYPE_UINT8_T

##
## Checks for library functions.
##

# N/A

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
