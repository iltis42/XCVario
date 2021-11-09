EXTRA_DIST += %D%/expectation_pngs.tar

%D%/.expectations: $(top_srcdir)/%D%/expectation_pngs.tar
	tar xf $< -C %D%/
	touch $@

.PHONY: clean-tests-fonts-freefont
clean-local: clean-tests-fonts-freefont
clean-tests-fonts-freefont:
	rm -f %D%/*.tga %D%/*.png %D%/.expectations

EXPECTATION_PNGS_TARGETS += %D%/.expectations

if NOT_CROSS_COMPILE

check_PROGRAMS += %D%/freefont.test
TESTS += %D%/freefont.test
tests_fonts_freefont_freefont_test_CFLAGS = @CHECK_CFLAGS@ $(CFLAGS_WARNINGS) -DRELDIR=\"%D%\"
tests_fonts_freefont_freefont_test_LDFLAGS = $(LDFLAGS_EGLIB)
tests_fonts_freefont_freefont_test_LDADD = tests/libcommon.a tests/fonts/libunicode_block.a $(LIBS_EGLIB) @CHECK_LIBS@
tests_fonts_freefont_freefont_test_CPPFLAGS = $(CPPFLAGS_EGLIB)
tests_fonts_freefont_freefont_test_SOURCES = %D%/freefont.c
EXTRA_tests_fonts_freefont_freefont_test_DEPENDENCIES = %D%/.expectations

endif