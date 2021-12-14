sphinx_conf = %D%/conf.py
EXTRA_DIST += $(sphinx_conf)

sphinx_static_files =
sphinx_static_files += %D%/_static/logo.png
EXTRA_DIST += $(sphinx_static_files)

sphinx_template_files =
sphinx_template_files += %D%/_templates/badges.html
EXTRA_DIST += $(sphinx_template_files)

sphinx_rst_files =
sphinx_rst_files += %D%/api_reference/display/drivers/frame_buffer.rst
sphinx_rst_files += %D%/api_reference/display/drivers/index.rst
sphinx_rst_files += %D%/api_reference/display/drivers/sh1106.rst
sphinx_rst_files += %D%/api_reference/display/drivers/ssd1331.rst
sphinx_rst_files += %D%/api_reference/display/drivers/ssd1675a.rst
sphinx_rst_files += %D%/api_reference/display/drivers/st7789.rst
sphinx_rst_files += %D%/api_reference/display/drivers/tga.rst
sphinx_rst_files += %D%/api_reference/display/index.rst
sphinx_rst_files += %D%/api_reference/display/interface.rst
sphinx_rst_files += %D%/api_reference/drawing/fonts.rst
sphinx_rst_files += %D%/api_reference/drawing/fonts/adobe.rst
sphinx_rst_files += %D%/api_reference/drawing/fonts/freefont.rst
sphinx_rst_files += %D%/api_reference/drawing/fonts/liberation.rst
sphinx_rst_files += %D%/api_reference/drawing/functions.rst
sphinx_rst_files += %D%/api_reference/drawing/index.rst
sphinx_rst_files += %D%/api_reference/eglib.rst
sphinx_rst_files += %D%/api_reference/hal/four_wire_spi/index.rst
sphinx_rst_files += %D%/api_reference/hal/four_wire_spi/libopencm3_stm32f4.rst
sphinx_rst_files += %D%/api_reference/hal/four_wire_spi/none.rst
sphinx_rst_files += %D%/api_reference/hal/four_wire_spi/stream.rst
sphinx_rst_files += %D%/api_reference/hal/i2c/index.rst
sphinx_rst_files += %D%/api_reference/hal/i2c/libopencm3_stm32f4.rst
sphinx_rst_files += %D%/api_reference/hal/i2c/stream.rst
sphinx_rst_files += %D%/api_reference/hal/index.rst
sphinx_rst_files += %D%/api_reference/hal/interface.rst
sphinx_rst_files += %D%/api_reference/hal/parallel_8_bit_8080/index.rst
sphinx_rst_files += %D%/api_reference/hal/parallel_8_bit_8080/libopencm3_stm32f4.rst
sphinx_rst_files += %D%/api_reference/hal/parallel_8_bit_8080/stream.rst
sphinx_rst_files += %D%/api_reference/index.rst
sphinx_rst_files += %D%/driver_development/display.rst
sphinx_rst_files += %D%/driver_development/hal.rst
sphinx_rst_files += %D%/driver_development/index.rst
sphinx_rst_files += %D%/index.rst
EXTRA_DIST += $(sphinx_rst_files)

sphinx_deps = $(sphinx_conf) $(sphinx_static_files) $(sphinx_template_files) $(sphinx_rst_files)

EXTRA_DIST += %D%/requirements.txt

.PHONY: %D%/html/
.PHONY: html-local
.PHONY: install-html-local
.PHONY: update-html
.PHONY: uninstall-local

clean-local: clean-html

if BUILD_HTML

CLEANFILES += top_srcdir
top_srcdir:
	$(LN_S) $(abs_top_srcdir) $@

CLEANFILES += opencm3_dir
opencm3_dir:
	$(LN_S) $(OPENCM3_DIR) $@

%D%/root/.ok: opencm3_dir opencm3_dir $(EXPECTATION_PNGS_TARGETS) $(sphinx_deps) $(example_c_files) $(tests_drawing_test_sources)
	rm -rf %D%/root/
	mkdir -p %D%/root/
	cp -a $(abs_top_srcdir)/%D%/conf.py %D%/root/
	cp -a $(top_srcdir)/%D%/_static/ %D%/root/_static/
	cp -a $(top_srcdir)/%D%/_templates/ %D%/root/_templates/
	{ cd $(top_srcdir)/%D%/ && find . -name \*.rst ; } | while read rst ; do mkdir -p %D%/root/$$(dirname $$rst) && cp -a $(top_srcdir)/%D%/$$rst %D%/root/$$rst ; done
	for f in $(example_c_files) ; do mkdir -p %D%/root/$$(dirname $$f) && cp -a $(top_srcdir)/$$f %D%/root/$$(dirname $$f) ; done
	mkdir -p %D%/root/api_reference/drawing/fonts/adobe/
	cp -a tests/fonts/adobe/*.png %D%/root/api_reference/drawing/fonts/adobe/
	mkdir -p %D%/root/api_reference/drawing/fonts/freefont/
	cp -a tests/fonts/freefont/*.png %D%/root/api_reference/drawing/fonts/freefont/
	mkdir -p %D%/root/api_reference/drawing/fonts/liberation/
	cp -a tests/fonts/liberation/*.png %D%/root/api_reference/drawing/fonts/liberation/
	for f in $(tests_drawing_test_sources) ; do cp -a $(top_srcdir)/$$f %D%/root/api_reference/drawing/ ; done
	cp -a tests/drawing/*.png %D%/root/api_reference/drawing/

%D%/html/: eglib/drawing/fonts.h top_srcdir %D%/root/.ok
	TOP_SRCDIR=$(top_srcdir) TOP_BUILDDIR=$(top_builddir) sphinx-build -b html -W %D%/root/ $@
	rm -rf $@/.buildinfo $@/.doctrees $@/_sources

clean-html:
	rm -rf %D%/html/ %D%/root/

html-local: %D%/html/

install-html-local: html-local
	cd %D% && find html/ -type f -exec $(install_sh_DATA) "{}" $(DESTDIR)$(docdir)/"{}" \;

update-html: html-local
	rm -rf $(top_srcdir)/docs
	cp -a %D%/html $(top_srcdir)/docs
	touch $(top_srcdir)/docs/.nojekyll

uninstall-local:
	-rm -rf $(DESTDIR)$(docdir)/html

check: html-local

else

clean-html:

endif