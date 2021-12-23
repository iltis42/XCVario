#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(
	[eglib-example-libopencm3_stm32f4],
	[m4_esyscmd([build-aux/git-version-gen .tarball-version])],
	[https://github.com/fornellas/eglib/issues],
	[eglib],
	[https://fornellas.github.io/eglib/])
AC_CONFIG_SRCDIR([sh1106-four_wire_spi.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AM_SILENT_RULES([yes])
AC_REQUIRE_AUX_FILE([libopencm3.mk])

##
## Canonical
##

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_MSG_CHECKING([for valid HOST])
if test "x$host" != xarm-none-eabi ; then
	AC_MSG_ERROR([Invalid host: this must be cross-compiled with --host=arm-none-eabi.])
fi
AC_MSG_RESULT([ok])

##
## libopencm3
##

# --with-libopencm3
AC_ARG_WITH(
	[libopencm3],
	[AS_HELP_STRING([--with-libopencm3], [http://libopencm3.org/ built sources path @<:@default=no@:>@])],
	[OPENCM3_DIR="$(cd "$with_libopencm3" && pwd)"],
	AC_MSG_ERROR([Missing --with-libopencm3=\$OPENCM3_DIR]))
AC_SUBST([OPENCM3_DIR], [$OPENCM3_DIR])
# TODO Python for $(OPENCM3_DIR)/scripts/genlink.py
# CFLAGS
AC_MSG_CHECKING([required libopencm3 stm32f4 CFLAGS])
CFLAGS_OPENCM3="$(make --silent -f ${srcdir}/build-aux/libopencm3.mk OPENCM3_DIR="$OPENCM3_DIR" DEVICE=stm32f4 ARCH_FLAGS)" || exit 1
AC_SUBST([CFLAGS_OPENCM3], [$CFLAGS_OPENCM3])
AC_MSG_RESULT([ok])
# LDFLAGS
LDFLAGS_OPENCM3="-L$OPENCM3_DIR/lib"
AC_SUBST([LDFLAGS_OPENCM3], [$LDFLAGS_OPENCM3])
# LIBS
LIBS_OPENCM3="-lopencm3_stm32f4 -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group"
AC_SUBST([LIBS_OPENCM3], [$LIBS_OPENCM3])
# CPPFLAGS
AC_MSG_CHECKING([required libopencm3_stm32f4 CPPFLAGS])
CPPFLAGS_OPENCM3="$(make --silent -f ${srcdir}/build-aux/libopencm3.mk OPENCM3_DIR="$OPENCM3_DIR" DEVICE=stm32f4 CPPFLAGS)" || exit 1
AC_SUBST([CPPFLAGS_OPENCM3], [$CPPFLAGS_OPENCM3])
AC_MSG_RESULT([ok])

##
## eglib
##

# CFLAGS
CFLAGS_EGLIB="$CFLAGS_OPENCM3"
AC_SUBST([CFLAGS_EGLIB], [$CFLAGS_EGLIB])
# LDFLAGS
AC_ARG_WITH(
	[eglib-libdir],
	[AS_HELP_STRING([--with-eglib-libdir], [eglib libdir @<:@default=no@:>@])],
	[EGLIB_LIBDIR="$(cd "$with_eglib_libdir" && pwd)"],
	AC_MSG_ERROR([Missing --with-eglib-libdir=\$EGLIB_LIBDIR]))
LDFLAGS_EGLIB="-L${EGLIB_LIBDIR}"
AC_SUBST([EGLIB_LIBDIR], [$EGLIB_LIBDIR])
AC_SUBST([LDFLAGS_EGLIB], [$LDFLAGS_EGLIB])
# LIBS
LIBS_EGLIB="-leglib -Wl,--start-group -lm -Wl,--end-group"
AC_SUBST([LIBS_EGLIB], [$LIBS_EGLIB])
# CPPFLAGS
AC_ARG_WITH(
	[eglib-includedir],
	[AS_HELP_STRING([--with-eglib-includedir], [eglib includedir @<:@default=no@:>@])],
	[EGLIB_INCLUDEDIR="$(cd "$with_eglib_includedir" && pwd)"],
	AC_MSG_ERROR([Missing --with-eglib-includedir=\$EGLIB_INCLUDEDIR]))
CPPFLAGS_EGLIB="-I$EGLIB_INCLUDEDIR"
AC_SUBST([EGLIB_INCLUDEDIR], [$EGLIB_INCLUDEDIR])
AC_SUBST([CPPFLAGS_EGLIB], [$CPPFLAGS_EGLIB])

##
## Misc
##

# Avoid "undefined reference to `_exit'" during tests
LIBS_NOSYS_SPECS="--specs=nosys.specs"

# Common warning flags
CFLAGS_WARNINGS="-Wextra -Wshadow -Wno-unused-variable -Wimplicit-function-declaration"
CFLAGS_WARNINGS="$CFLAGS_WARNINGS -Wredundant-decls -Wstrict-prototypes -Wmissing-prototypes"
CFLAGS_WARNINGS="$CFLAGS_WARNINGS -Wall -Wundef"
CFLAGS_WARNINGS="$CFLAGS_WARNINGS -Werror -Wfatal-errors"
AC_SUBST([CFLAGS_WARNINGS], [$CFLAGS_WARNINGS])

##
## Checks for programs.
##

# CC
: ${CFLAGS="-g -O3"}
ORIGINAL_LIBS="$LIBS"
LIBS="$LIBS_NOSYS_SPECS $LIBS"
AC_PROG_CC
LIBS="$ORIGINAL_LIBS"

# OBJCOPY
AC_CHECK_TOOL([OBJCOPY], [objcopy])
if test "x$OBJCOPY" = x ; then
	AC_MSG_FAILURE([objcopy not found!])
fi

# GDB
AC_CHECK_PROG([GDB], [gdb-multiarch], [gdb-multiarch], [no])
if test "x$GDB" = xno ; then
	AC_CHECK_TOOL([GDB], [gdb], [no])
	if test "x$GDB" = xno ; then
		AC_CHECK_PROG([GDB], [gdb], [gdb], [no])
		if test "x$GDB" != xno ; then
			AC_MSG_WARN([Using plain gdb: beware it may not support $host!])
		fi
	fi
fi
AM_CONDITIONAL([HAS_GDB], [test "x$GDB" != xno])
AC_SUBST([GDB], [$GDB])

##
## Checks for libraries.
##

# eglib
ORIGINAL_CFLAGS="$CFLAGS" ; CFLAGS="$CFLAGS_EGLIB $CFLAGS"
ORIGINAL_LDFLAGS="$LDFLAGS" ; LDFLAGS="$LDFLAGS_EGLIB $LDFLAGS"
ORIGINAL_LIBS="$LIBS" ; LIBS="$LIBS_NOSYS_SPECS $LIBS"
AC_CHECK_LIB([eglib], [eglib_Init], [], AC_MSG_WARN([eglib not found! Please build it before building here!]))
CFLAGS="$ORIGINAL_CFLAGS"
LDFLAGS="$ORIGINAL_LDFLAGS"
LIBS="$ORIGINAL_LIBS"

# opencm3_stm32f4
ORIGINAL_CFLAGS="$CFLAGS" ; CFLAGS="$CFLAGS_OPENCM3 $CFLAGS"
ORIGINAL_LDFLAGS="$LDFLAGS" ; LDFLAGS="$LDFLAGS_OPENCM3 $LDFLAGS"
ORIGINAL_LIBS="$LIBS" ; LIBS="$LIBS_NOSYS_SPECS $LIBS_OPENCM3 $LIBS"
AC_CHECK_LIB([opencm3_stm32f4], [rcc_clock_setup_pll], [], AC_MSG_FAILURE([opencm3_stm32f4 not found!]))
CFLAGS="$ORIGINAL_CFLAGS"
LDFLAGS="$ORIGINAL_LDFLAGS"
LIBS="$ORIGINAL_LIBS"

##
## Checks for header files.
##

ORIGINAL_CPPFLAGS="$CPPFLAGS" ; CPPFLAGS="$CPPFLAGS_EGLIB $CPPFLAGS"
AC_CHECK_HEADERS([eglib.h], [], AC_MSG_FAILURE([eglib.h not found!]))
CPPFLAGS="$ORIGINAL_CPPFLAGS"
ORIGINAL_CPPFLAGS="$CPPFLAGS" ; CPPFLAGS="$CPPFLAGS_OPENCM3 $CPPFLAGS"
AC_CHECK_HEADERS([libopencm3/stm32/rcc.h], [], AC_MSG_FAILURE([libopencm3/stm32/rcc.h not found!]))
CPPFLAGS="$ORIGINAL_CPPFLAGS"

##
## Checks for typedefs, structures, and compiler characteristics.
##

AC_CHECK_HEADER_STDBOOL
if test "x$ac_cv_header_stdbool_h" != xyes ; then
	AC_MSG_FAILURE([stdbool.h not found!])
fi
AC_TYPE_UINT16_T
AC_TYPE_UINT8_T

##
## Checks for library functions.
##

# N/A

AC_CONFIG_FILES([Makefile])
AC_OUTPUT