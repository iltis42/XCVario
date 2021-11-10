if NOT_CROSS_COMPILE

check_HEADERS += %D%/unicode_block.h
check_LIBRARIES += %D%/libunicode_block.a
tests_fonts_libunicode_block_a_CFLAGS = @CHECK_CFLAGS@ $(CFLAGS_WARNINGS)
tests_fonts_libunicode_block_a_CPPFLAGS = $(CPPFLAGS_EGLIB)
tests_fonts_libunicode_block_a_SOURCES = %D%/unicode_block.c

endif