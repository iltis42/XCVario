#include <check.h>
#include <eglib/display/ssd1675a.h>

const display_t *display_driver = &ssd1675a;
void *display_config_ptr = &ssd1675a_config_buydisplay_red_2_9_inch_e_ink_display_module;
coordinate_t width = 128;
coordinate_t height = 296;
enum pixel_format_t pixel_format = PIXEL_FORMAT_2BIT_EPD;
char driver_name[] = "ssd1675a";

extern eglib_t eglib;
extern char *test_name;

void tcase_add_extra_tests(TCase *tcase);

void tcase_add_extra_tests(TCase *tcase) {
	(void)tcase;
}