EXPECTATION_PNGS_TARGETS =

if NOT_CROSS_COMPILE

TEST_LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) $(top_srcdir)/build-aux/tap-driver.sh

check_PROGRAMS =
TESTS =

check_HEADERS = %D%/common.h
check_LIBRARIES = %D%/libcommon.a
tests_libcommon_a_CFLAGS = @CHECK_CFLAGS@ $(CFLAGS_WARNINGS) -DTOP_SRCDIR=\"$(abs_top_srcdir)\" -DTOP_BUILDDIR=\"$(abs_top_builddir)\"
tests_libcommon_a_CPPFLAGS = $(CPPFLAGS_EGLIB)
tests_libcommon_a_SOURCES = %D%/common.c
EXTRA_tests_libcommon_a_DEPENDENCIES = libeglib.a

endif