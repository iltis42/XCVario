#include <check.h>
#include <eglib/display/st7789.h>

const display_t *display_driver = &st7789;
void *display_config_ptr = &(st7789_config_t){
	.width = 240,
	.height = 240,
	.color = ST7789_COLOR_16_BIT,
	.page_address = ST7789_PAGE_ADDRESS_TOP_TO_BOTTOM,
	.colum_address = ST7789_COLUMN_ADDRESS_LEFT_TO_RIGHT,
	.page_column_order = ST7789_PAGE_COLUMN_ORDER_NORMAL,
	.vertical_refresh = ST7789_VERTICAL_REFRESH_TOP_TO_BOTTOM,
	.horizontal_refresh = ST7789_HORIZONTAL_REFRESH_LEFT_TO_RIGHT,
};
coordinate_t width = 240;
coordinate_t height = 240;
enum pixel_format_t pixel_format = PIXEL_FORMAT_16BIT_RGB;
char driver_name[] = "st7789";

extern eglib_t eglib;
extern char *test_name;

void tcase_add_extra_tests(TCase *tcase);

START_TEST(SetDisplayInversion) {
	test_name = "SetDisplayInversion";
	st7789_SetDisplayInversion(&eglib, true);
}END_TEST;

START_TEST(SetIdleMode) {
	test_name = "SetIdleMode";
	st7789_SetIdleMode(&eglib, true);
}END_TEST;

void tcase_add_extra_tests(TCase *tcase) {
	tcase_add_test(tcase, SetDisplayInversion);
	tcase_add_test(tcase, SetIdleMode);
}