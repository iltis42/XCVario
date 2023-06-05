libopencm3_stm32f4_sources =
libopencm3_stm32f4_sources += %D%/libopencm3_stm32f4/build-aux/libopencm3.mk
libopencm3_stm32f4_sources += %D%/libopencm3_stm32f4/configure.ac
libopencm3_stm32f4_sources += %D%/libopencm3_stm32f4/gdb.init
libopencm3_stm32f4_sources += %D%/libopencm3_stm32f4/Makefile.am

example_c_files =
example_c_files += %D%/libopencm3_stm32f4/sh1106-four_wire_spi.c
example_c_files += %D%/libopencm3_stm32f4/sh1106-i2c.c
example_c_files += %D%/libopencm3_stm32f4/ssd1331-four_wire_spi.c
example_c_files += %D%/libopencm3_stm32f4/ssd1675a-four_wire_spi.c
example_c_files += %D%/libopencm3_stm32f4/st7789-four_wire_spi.c
example_c_files += %D%/libopencm3_stm32f4/st7789-parallel_8_bit_8080.c
libopencm3_stm32f4_sources += $(example_c_files)

libopencm3_stm32f4_configury =
libopencm3_stm32f4_configury += %D%/libopencm3_stm32f4/aclocal.m4
libopencm3_stm32f4_configury += %D%/libopencm3_stm32f4/build-aux/
libopencm3_stm32f4_configury += %D%/libopencm3_stm32f4/build-aux/compile
libopencm3_stm32f4_configury += %D%/libopencm3_stm32f4/build-aux/config.guess
libopencm3_stm32f4_configury += %D%/libopencm3_stm32f4/build-aux/config.sub
libopencm3_stm32f4_configury += %D%/libopencm3_stm32f4/build-aux/depcomp
libopencm3_stm32f4_configury += %D%/libopencm3_stm32f4/build-aux/install-sh
libopencm3_stm32f4_configury += %D%/libopencm3_stm32f4/build-aux/missing
libopencm3_stm32f4_configury += %D%/libopencm3_stm32f4/config.h.in
libopencm3_stm32f4_configury += %D%/libopencm3_stm32f4/configure
libopencm3_stm32f4_configury += %D%/libopencm3_stm32f4/gdb.init
libopencm3_stm32f4_configury += %D%/libopencm3_stm32f4/Makefile.in

dist_noinst_DATA = $(libopencm3_stm32f4_sources) $(libopencm3_stm32f4_configury)
nodist_doc_DATA = %D%/eglib-example-libopencm3_stm32f4-$(VERSION).tar.gz

CLEAN_EGLIB_EXAMPLE_LIBOPENCM3_STM32F4_CMD = if test -d %D%/eglib-example-libopencm3_stm32f4-$(VERSION) ; then chmod u+w -R %D%/eglib-example-libopencm3_stm32f4-$(VERSION) && rm -rf %D%/eglib-example-libopencm3_stm32f4-$(VERSION) ; fi

.PHONY: clean-eglib-example-libopencm3_stm32f4-$(VERSION)
clean-eglib-example-libopencm3_stm32f4-$(VERSION):
	$(CLEAN_EGLIB_EXAMPLE_LIBOPENCM3_STM32F4_CMD)

clean-local: clean-eglib-example-libopencm3_stm32f4-$(VERSION)

%D%/eglib-example-libopencm3_stm32f4-$(VERSION).tar.gz: $(libopencm3_stm32f4_sources) $(libopencm3_stm32f4_configury)
	$(CLEAN_EGLIB_EXAMPLE_LIBOPENCM3_STM32F4_CMD)
	mkdir -p %D%/eglib-example-libopencm3_stm32f4-$(VERSION)
	cp -a $? %D%/eglib-example-libopencm3_stm32f4-$(VERSION)/
	tar zcf $@ -C %D% eglib-example-libopencm3_stm32f4-$(VERSION)/
	chmod u+w -R %D%/eglib-example-libopencm3_stm32f4-$(VERSION)/
	rm -rf %D%/eglib-example-libopencm3_stm32f4-$(VERSION)

CLEANFILES += %D%/eglib-example-libopencm3_stm32f4-$(VERSION).tar.gz

.PHONY: check-eglib-example-libopencm3_stm32f4-$(VERSION).tar.gz

if OPENCM3_STM32F4

check-eglib-example-libopencm3_stm32f4-$(VERSION).tar.gz: %D%/eglib-example-libopencm3_stm32f4-$(VERSION).tar.gz libeglib.a
	$(MAKE) $(AM_MAKEFLAGS) clean-eglib-example-libopencm3_stm32f4-$(VERSION)
	tar zxf %D%/eglib-example-libopencm3_stm32f4-$(VERSION).tar.gz -C %D%
	$(MAKE) $(AM_MAKEFLAGS) -C $(abs_top_builddir) install-data DESTDIR=%D%/eglib-example-libopencm3_stm32f4-$(VERSION)/_eglib/
	cd %D%/eglib-example-libopencm3_stm32f4-$(VERSION)/ \
		&& if test -e _build/ ; then  chmod u+w -R _build/ && rm -rf _build/ ; fi \
		&& mkdir _build \
		&& cd _build/ \
		&& ../configure \
			--host=$(host) \
			--with-libopencm3="$(OPENCM3_DIR)" \
			--with-eglib-libdir="$(abs_top_builddir)" \
			--with-eglib-includedir=$(abs_builddir)/%D%/eglib-example-libopencm3_stm32f4-$(VERSION)/_eglib/$(includedir) \
			CC="$(CC)" \
			CFLAGS="$(CFLAGS)" \
			LDFLAGS="$(LDFLAGS)" \
			LIBS="$(LIBS)" \
			CPPFLAGS="$(CPPFLAGS)" \
			CPP="$(CPP)" \
		&& $(MAKE) $(AM_MAKEFLAGS) distcheck

check: check-eglib-example-libopencm3_stm32f4-$(VERSION).tar.gz

endif