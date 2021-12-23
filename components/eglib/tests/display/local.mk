if NOT_CROSS_COMPILE

%D%/display.c: eglib/drawing/fonts.h

SH1106_EXPECTATIONS = %D%/sh1106.four_wire_spi.init
SH1106_EXPECTATIONS += %D%/sh1106.four_wire_spi.sleep_in
SH1106_EXPECTATIONS += %D%/sh1106.four_wire_spi.sleep_out
SH1106_EXPECTATIONS += %D%/sh1106.four_wire_spi.get_dimension
SH1106_EXPECTATIONS += %D%/sh1106.four_wire_spi.get_pixel_format
SH1106_EXPECTATIONS += %D%/sh1106.four_wire_spi.draw_pixel_color
SH1106_EXPECTATIONS += %D%/sh1106.four_wire_spi.draw_line
SH1106_EXPECTATIONS += %D%/sh1106.four_wire_spi.send_buffer
SH1106_EXPECTATIONS += %D%/sh1106.four_wire_spi.refresh
SH1106_EXPECTATIONS += %D%/sh1106.four_wire_spi.EntireDisplayOn
SH1106_EXPECTATIONS += %D%/sh1106.four_wire_spi.Reverse
SH1106_EXPECTATIONS += %D%/sh1106.four_wire_spi.SetContrast
SH1106_EXPECTATIONS += %D%/sh1106.four_wire_spi.SetStartLine
SH1106_EXPECTATIONS += %D%/sh1106.i2c.init
SH1106_EXPECTATIONS += %D%/sh1106.i2c.sleep_in
SH1106_EXPECTATIONS += %D%/sh1106.i2c.sleep_out
SH1106_EXPECTATIONS += %D%/sh1106.i2c.get_dimension
SH1106_EXPECTATIONS += %D%/sh1106.i2c.get_pixel_format
SH1106_EXPECTATIONS += %D%/sh1106.i2c.draw_pixel_color
SH1106_EXPECTATIONS += %D%/sh1106.i2c.draw_line
SH1106_EXPECTATIONS += %D%/sh1106.i2c.send_buffer
SH1106_EXPECTATIONS += %D%/sh1106.i2c.refresh
SH1106_EXPECTATIONS += %D%/sh1106.i2c.EntireDisplayOn
SH1106_EXPECTATIONS += %D%/sh1106.i2c.Reverse
SH1106_EXPECTATIONS += %D%/sh1106.i2c.SetContrast
SH1106_EXPECTATIONS += %D%/sh1106.i2c.SetStartLine
EXTRA_DIST += $(SH1106_EXPECTATIONS)
check_PROGRAMS += %D%/sh1106.test
TESTS += %D%/sh1106.test
tests_display_sh1106_test_CFLAGS = @CHECK_CFLAGS@ $(CFLAGS_WARNINGS) -DRELDIR=\"%D%\"
tests_display_sh1106_test_LDFLAGS = $(LDFLAGS_EGLIB)
tests_display_sh1106_test_LDADD = tests/libcommon.a $(LIBS_EGLIB) @CHECK_LIBS@
tests_display_sh1106_test_CPPFLAGS = $(CPPFLAGS_EGLIB)
tests_display_sh1106_test_SOURCES = %D%/display.c %D%/sh1106.c
EXTRA_tests_display_sh1106_test_DEPENDENCIES = $(SH1106_EXPECTATIONS)

SSD1331_EXPECTATIONS = %D%/ssd1331.four_wire_spi.init
SSD1331_EXPECTATIONS += %D%/ssd1331.four_wire_spi.sleep_in
SSD1331_EXPECTATIONS += %D%/ssd1331.four_wire_spi.sleep_out
SSD1331_EXPECTATIONS += %D%/ssd1331.four_wire_spi.get_dimension
SSD1331_EXPECTATIONS += %D%/ssd1331.four_wire_spi.get_pixel_format
SSD1331_EXPECTATIONS += %D%/ssd1331.four_wire_spi.draw_pixel_color
SSD1331_EXPECTATIONS += %D%/ssd1331.four_wire_spi.draw_line
SSD1331_EXPECTATIONS += %D%/ssd1331.four_wire_spi.send_buffer
SSD1331_EXPECTATIONS += %D%/ssd1331.four_wire_spi.refresh
SSD1331_EXPECTATIONS += %D%/ssd1331.four_wire_spi.DrawBox
SSD1331_EXPECTATIONS += %D%/ssd1331.four_wire_spi.DrawFrame
SSD1331_EXPECTATIONS += %D%/ssd1331.four_wire_spi.DrawLine
SSD1331_EXPECTATIONS += %D%/ssd1331.four_wire_spi.SetDimMode
SSD1331_EXPECTATIONS += %D%/ssd1331.four_wire_spi.SetDisplayMode
SSD1331_EXPECTATIONS += %D%/ssd1331.four_wire_spi.SetDisplayStartLine
SSD1331_EXPECTATIONS += %D%/ssd1331.four_wire_spi.SetMasterCurrent
EXTRA_DIST += $(SSD1331_EXPECTATIONS)
check_PROGRAMS += %D%/ssd1331.test
TESTS += %D%/ssd1331.test
tests_display_ssd1331_test_CFLAGS = @CHECK_CFLAGS@ $(CFLAGS_WARNINGS) -DRELDIR=\"%D%\"
tests_display_ssd1331_test_LDFLAGS = $(LDFLAGS_EGLIB)
tests_display_ssd1331_test_LDADD = tests/libcommon.a $(LIBS_EGLIB) @CHECK_LIBS@
tests_display_ssd1331_test_CPPFLAGS = $(CPPFLAGS_EGLIB)
tests_display_ssd1331_test_SOURCES = %D%/display.c %D%/ssd1331.c
EXTRA_tests_display_ssd1331_test_DEPENDENCIES = $(SSD1331_EXPECTATIONS)

SSD1675A_EXPECTATIONS = %D%/ssd1675a.four_wire_spi.init
SSD1675A_EXPECTATIONS += %D%/ssd1675a.four_wire_spi.sleep_in
SSD1675A_EXPECTATIONS += %D%/ssd1675a.four_wire_spi.sleep_out
SSD1675A_EXPECTATIONS += %D%/ssd1675a.four_wire_spi.get_dimension
SSD1675A_EXPECTATIONS += %D%/ssd1675a.four_wire_spi.get_pixel_format
SSD1675A_EXPECTATIONS += %D%/ssd1675a.four_wire_spi.draw_pixel_color
SSD1675A_EXPECTATIONS += %D%/ssd1675a.four_wire_spi.draw_line
SSD1675A_EXPECTATIONS += %D%/ssd1675a.four_wire_spi.send_buffer
SSD1675A_EXPECTATIONS += %D%/ssd1675a.four_wire_spi.refresh
SSD1675A_EXPECTATIONS += %D%/ssd1675a.four_wire_spi.DrawBox
SSD1675A_EXPECTATIONS += %D%/ssd1675a.four_wire_spi.DrawFrame
SSD1675A_EXPECTATIONS += %D%/ssd1675a.four_wire_spi.DrawLine
SSD1675A_EXPECTATIONS += %D%/ssd1675a.four_wire_spi.SetDimMode
SSD1675A_EXPECTATIONS += %D%/ssd1675a.four_wire_spi.SetDisplayMode
SSD1675A_EXPECTATIONS += %D%/ssd1675a.four_wire_spi.SetDisplayStartLine
SSD1675A_EXPECTATIONS += %D%/ssd1675a.four_wire_spi.SetMasterCurrent
EXTRA_DIST += $(SSD1675A_EXPECTATIONS)
check_PROGRAMS += %D%/ssd1675a.test
TESTS += %D%/ssd1675a.test
tests_display_ssd1675a_test_CFLAGS = @CHECK_CFLAGS@ $(CFLAGS_WARNINGS) -DRELDIR=\"%D%\"
tests_display_ssd1675a_test_LDFLAGS = $(LDFLAGS_EGLIB)
tests_display_ssd1675a_test_LDADD = tests/libcommon.a $(LIBS_EGLIB) @CHECK_LIBS@
tests_display_ssd1675a_test_CPPFLAGS = $(CPPFLAGS_EGLIB)
tests_display_ssd1675a_test_SOURCES = %D%/display.c %D%/ssd1675a.c
EXTRA_tests_display_ssd1675a_test_DEPENDENCIES = $(SSD1675A_EXPECTATIONS)

ST7789_EXPECTATIONS = %D%/st7789.four_wire_spi.init
ST7789_EXPECTATIONS += %D%/st7789.four_wire_spi.sleep_in
ST7789_EXPECTATIONS += %D%/st7789.four_wire_spi.sleep_out
ST7789_EXPECTATIONS += %D%/st7789.four_wire_spi.get_dimension
ST7789_EXPECTATIONS += %D%/st7789.four_wire_spi.get_pixel_format
ST7789_EXPECTATIONS += %D%/st7789.four_wire_spi.draw_pixel_color
ST7789_EXPECTATIONS += %D%/st7789.four_wire_spi.draw_line
ST7789_EXPECTATIONS += %D%/st7789.four_wire_spi.send_buffer
ST7789_EXPECTATIONS += %D%/st7789.four_wire_spi.refresh
ST7789_EXPECTATIONS += %D%/st7789.four_wire_spi.SetDisplayInversion
ST7789_EXPECTATIONS += %D%/st7789.four_wire_spi.SetIdleMode
ST7789_EXPECTATIONS += %D%/st7789.parallel_8_bit_8080.init
ST7789_EXPECTATIONS += %D%/st7789.parallel_8_bit_8080.sleep_in
ST7789_EXPECTATIONS += %D%/st7789.parallel_8_bit_8080.sleep_out
ST7789_EXPECTATIONS += %D%/st7789.parallel_8_bit_8080.get_dimension
ST7789_EXPECTATIONS += %D%/st7789.parallel_8_bit_8080.get_pixel_format
ST7789_EXPECTATIONS += %D%/st7789.parallel_8_bit_8080.draw_pixel_color
ST7789_EXPECTATIONS += %D%/st7789.parallel_8_bit_8080.draw_line
ST7789_EXPECTATIONS += %D%/st7789.parallel_8_bit_8080.send_buffer
ST7789_EXPECTATIONS += %D%/st7789.parallel_8_bit_8080.refresh
ST7789_EXPECTATIONS += %D%/st7789.parallel_8_bit_8080.SetDisplayInversion
ST7789_EXPECTATIONS += %D%/st7789.parallel_8_bit_8080.SetIdleMode
EXTRA_DIST += $(ST7789_EXPECTATIONS)
check_PROGRAMS += %D%/st7789.test
TESTS += %D%/st7789.test
tests_display_st7789_test_CFLAGS = @CHECK_CFLAGS@ $(CFLAGS_WARNINGS) -DRELDIR=\"%D%\"
tests_display_st7789_test_LDFLAGS = $(LDFLAGS_EGLIB)
tests_display_st7789_test_LDADD = tests/libcommon.a $(LIBS_EGLIB) @CHECK_LIBS@
tests_display_st7789_test_CPPFLAGS = $(CPPFLAGS_EGLIB)
tests_display_st7789_test_SOURCES = %D%/display.c %D%/st7789.c
EXTRA_tests_display_st7789_test_DEPENDENCIES = $(ST7789_EXPECTATIONS)

endif