bin_PROGRAMS = eglib-font_generator
eglib_font_generator_SOURCES = font_generator.c
eglib_font_generator_CFLAGS = $(FREETYPE2_CFLAGS) $(AM_CFLAGS)
eglib_font_generator_LDADD = $(FREETYPE2_LIBS)

check-valgrind:
check-valgrind-memcheck: