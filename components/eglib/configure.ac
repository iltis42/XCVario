#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(
	[eglib],
	m4_esyscmd([build-aux/git-version-gen .tarball-version]),
	[https://github.com/fornellas/eglib/issues],
	[eglib],
	[https://fornellas.github.io/eglib/])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AM_SILENT_RULES([yes])
AC_CONFIG_SRCDIR([eglib.c])
AC_CONFIG_MACRO_DIR([/usr/share/aclocal])
AC_REQUIRE_AUX_FILE([tap-driver.sh])
AC_REQUIRE_AUX_FILE([libopencm3.mk])
AC_CONFIG_HEADERS([eglib/config.h])

##
## Canonical
##

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AM_CONDITIONAL([NOT_CROSS_COMPILE], [test x$build = x$host])

##
## libopencm3
##

# --with-libopencm3
AC_ARG_WITH(
	[libopencm3],
	[AS_HELP_STRING([--with-libopencm3], [http://libopencm3.org/ built sources path @<:@default=no@:>@])],
	[OPENCM3_DIR="$(cd "$with_libopencm3" && pwd)"],
	[with_libopencm3=no])
if test "x$with_libopencm3" != xno ; then
	AC_CHECK_FILE([$OPENCM3_DIR/mk/genlink-config.mk], [:], AC_MSG_ERROR([File not found: $OPENCM3_DIR/mk/genlink-config.mk]))
	AC_SUBST([OPENCM3_DIR], "$OPENCM3_DIR")
fi

# --enable-libopencm3_stm32f4
AC_ARG_ENABLE(
	[libopencm3_stm32f4],
	[AS_HELP_STRING([--enable-libopencm3_stm32f4], [Build for STM32F4 host. @<:@default=no@:>@])],
	[],
	[enable_libopencm3_stm32f4=no])
AM_CONDITIONAL([OPENCM3_STM32F4], [test "x$enable_libopencm3_stm32f4" != xno])
if test "x$enable_libopencm3_stm32f4" != xno ; then
	if test "x$with_libopencm3" = xno ; then
		AC_MSG_ERROR([Missing --with-libopencm3=\$OPENCM3_DIR parameter!])
	fi
	if test "x$host" != xarm-none-eabi ; then
		AC_MSG_ERROR([Missing --host=arm-none-eabi parameter to enable cross-compilation!])
	fi
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
fi

##
## eglib
##

# LDFLAGS
LDFLAGS_EGLIB="-L$(cd "${builddir}" && pwd)"
AC_SUBST([LDFLAGS_EGLIB], [$LDFLAGS_EGLIB])
# LIBS
LIBS_EGLIB="-leglib -Wl,--start-group -lm -Wl,--end-group"
AC_SUBST([LIBS_EGLIB], [$LIBS_EGLIB])
# CPPFLAGS
CPPFLAGS_EGLIB="-I$(cd "${builddir}" && pwd) -I$(cd "${srcdir}" && pwd)"
AC_SUBST([CPPFLAGS_EGLIB], [$CPPFLAGS_EGLIB])

##
## html
##

# TODO Python
if test "x$with_libopencm3" != xno ; then
	BUILD_HTML=yes
else
	BUILD_HTML=no
fi
AM_CONDITIONAL([BUILD_HTML], [test "x$BUILD_HTML" != xno])

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
ORIGINAL_CPPFLAGS="$CPPFLAGS"
if test "x$host" = xarm-none-eabi ; then
	# AC_PROG_CC calls exit(3) from its test: we temporarily add nosys.specs
	# to avoid "undefined reference to `_exit'" error.
	CPPFLAGS="$CPPFLAGS  -specs=nosys.specs"
fi
AC_PROG_CC
CPPFLAGS="$ORIGINAL_CPPFLAGS"


# CC
: ${CFLAGS="-g -O3"}
ORIGINAL_LIBS="$LIBS"
LIBS="$LIBS_NOSYS_SPECS $LIBS"
AC_PROG_CC
LIBS="$ORIGINAL_LIBS"

AM_PROG_AR

AC_PROG_RANLIB

AC_PROG_LN_S

# Valgrind
AX_VALGRIND_DFLT([helgrind],[off])
AX_VALGRIND_DFLT([drd],[off])
AX_VALGRIND_DFLT([sgcheck],[off])
AX_VALGRIND_DFLT([memcheck],[on])
AX_VALGRIND_CHECK

# ImageMagick
if test "x$build" = "x$host" ; then
	AC_CHECK_PROG([IMAGE_MAGICK_COMPARE], [compare], [yes], [no])
	if test "x$IMAGE_MAGICK_COMPARE" = xno
	then
		AC_MSG_FAILURE([ImageMagick 'compare' command not found!])
	fi
	AC_CHECK_PROG([IMAGE_MAGICK_CONVERT], [convert], [yes], [no])
	if test "x$IMAGE_MAGICK_COMPARE" = xno
	then
		AC_MSG_FAILURE([ImageMagick 'convert' command not found!])
	fi
fi

##
## Checks for libraries.
##

# check
if test "x$build" = "x$host" ; then
	PKG_CHECK_MODULES([CHECK], [check >= 0.10.0])
fi

# opencm3_stm32f4
if test "x$enable_libopencm3_stm32f4" != xno ; then
	ORIGINAL_CFLAGS="$CFLAGS" ; CFLAGS="$CFLAGS_OPENCM3 $CFLAGS"
	ORIGINAL_LDFLAGS="$LDFLAGS" ; LDFLAGS="$LDFLAGS_OPENCM3 $LDFLAGS"
	ORIGINAL_LIBS="$LIBS" ; LIBS="$LIBS_NOSYS_SPECS $LIBS_OPENCM3 $LIBS"
	AC_CHECK_LIB(
		[opencm3_stm32f4],
		[rcc_clock_setup_pll],
		[],
		AC_MSG_WARN(
			[opencm3_stm32f4 not found! Please make sure to 'make -C $OPENCM3_DIR' before calling make here!]))
	CFLAGS="$ORIGINAL_CFLAGS"
	LDFLAGS="$ORIGINAL_LDFLAGS"
	LIBS="$ORIGINAL_LIBS"
fi

##
## Checks for header files.
##

AC_CHECK_HEADERS([stddef.h], [], AC_MSG_FAILURE([stddef.h not found!]))
AC_CHECK_HEADERS([stdint.h], [], AC_MSG_FAILURE([stdint.h not found!]))
AC_CHECK_HEADERS([stdlib.h], [], AC_MSG_FAILURE([stdlib.h not found!]))
AC_CHECK_HEADERS([wchar.h], [], AC_MSG_FAILURE([wchar.h not found!]))
if test "x$build" = "x$host" ; then
	AC_CHECK_HEADERS([fcntl.h], [], AC_MSG_FAILURE([fcntl.h not found!]))
	AC_CHECK_HEADERS([string.h], [], AC_MSG_FAILURE([string.h not found!]))
	AC_CHECK_HEADERS([unistd.h], [], AC_MSG_FAILURE([unistd.h not found!]))
fi
if test "x$enable_libopencm3_stm32f4" != xno ; then
	ORIGINAL_CPPFLAGS="$CPPFLAGS" ; CPPFLAGS="$CPPFLAGS_OPENCM3 $CPPFLAGS"
	AC_CHECK_HEADERS([libopencm3/stm32/rcc.h], [], AC_MSG_FAILURE([libopencm3/stm32/rcc.h not found!]))
	CPPFLAGS="$ORIGINAL_CPPFLAGS"
fi

##
## Checks for typedefs, structures, and compiler characteristics.
##

AC_CHECK_HEADER_STDBOOL
if ! test "x$ac_cv_header_stdbool_h" = xyes ; then
	AC_MSG_FAILURE([stdbool.h not found!])
fi
AC_C_INLINE
if test "x$inline" != x ; then
	AC_MSG_FAILURE([C does not support inline keyword!])
fi
AC_TYPE_INT16_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T
if test "x$build" = "x$host" ; then
	AC_TYPE_SSIZE_T
fi

##
## Checks for library functions.
##

if test "x$build" = "x$host" ; then
	AC_CHECK_FUNCS([munmap], [], AC_MSG_FAILURE([Function munmap not found!]))
	AC_CHECK_FUNCS([strerror], [], AC_MSG_FAILURE([Function strerror not found!]))
	AC_FUNC_MMAP
	if ! test "x$ac_cv_func_mmap_fixed_mapped" = xyes
	then
		AC_MSG_FAILURE([Function mmap not found!])
	fi
fi

AC_CONFIG_FILES([
	Makefile])

##
## Subdirectory packages
##

# font_generator
ORIGINAL_CC="$CC"
unset CC
ORIGINAL_CFLAGS="$CFLAGS"
unset CFLAGS
ORIGINAL_LDFLAGS="$LDFLAGS"
unset LDFLAGS
ORIGINAL_LIBS="$LIBS"
unset LIBS
ORIGINAL_CPPFLAGS="$CPPFLAGS"
unset CPPFLAGS
ORIGINAL_CPP="$CPP"
unset CPP
AX_SUBDIRS_CONFIGURE(
	[font_generator],
	[],
	[],
	[[--build="$build"], [--host="$build"], [host_alias="$build"]],
	[[CC=.*], [CFLAGS=.*], [LDFLAGS=.*], [LIBS=.*], [CPPFLAGS=.*], [CPP=.*]])
CC="$ORIGINAL_CC"
CFLAGS="$ORIGINAL_CFLAGS"
LDFLAGS="$ORIGINAL_LDFLAGS"
LIBS="$ORIGINAL_LIBS"
CPPFLAGS="$ORIGINAL_CPPFLAGS"
CPP="$ORIGINAL_CPP"

##
## Output
##

AC_OUTPUT

AC_MSG_NOTICE([Summary:])
if test "x$build" = "x$host" ; then
	AC_MSG_NOTICE([  - Cross compile: no])
	AC_MSG_NOTICE([  - Tests: enabled])
else
	AC_MSG_NOTICE([  - Cross compile: $host])
	AC_MSG_NOTICE([  - Tests: disabled (due to cross-compilation)])
fi
if test "x$OPENCM3_DIR" = x
then
 AC_MSG_NOTICE([  - libopencm3: no])
else
 AC_MSG_NOTICE([  - libopencm3: $OPENCM3_DIR])
fi
AC_MSG_NOTICE([  - enable_libopencm3_stm32f4: $enable_libopencm3_stm32f4])
AC_MSG_NOTICE([  - HTML documentation: $BUILD_HTML])