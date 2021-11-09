#include <check.h>
#include <eglib/display.h>
#include <eglib/display/ssd1331.h>

const display_t *display_driver = &ssd1331;
void *display_config_ptr = &ssd1331_config_adafruit_65k_colors;
coordinate_t width = 96;
coordinate_t height = 64;
enum pixel_format_t pixel_format = PIXEL_FORMAT_16BIT_RGB;
char driver_name[] = "ssd1331";

extern eglib_t eglib;
extern char *test_name;

void tcase_add_extra_tests(TCase *tcase);

START_TEST(SetMasterCurrent) {
	test_name = "SetMasterCurrent";
	ssd1331_SetMasterCurrent(&eglib, 128);
}END_TEST

START_TEST(SetDisplayStartLine) {
	test_name = "SetDisplayStartLine";
	ssd1331_SetDisplayStartLine(&eglib, eglib_GetHeight(&eglib) / 2);
}END_TEST

START_TEST(SetDisplayMode) {
	test_name = "SetDisplayMode";
	ssd1331_SetDisplayMode(&eglib, SSD1331_DISPLAY_MODE_ENTIRE_DISPLAY_ON);
}END_TEST

START_TEST(SetDimMode) {
	test_name = "SetDimMode";
	ssd1331_SetDimMode(
		&eglib,
		64,
		128,
		192,
		128
	);
}END_TEST

START_TEST(DrawLine) {
	test_name = "DrawLine";
	ssd1331_DrawLine(
		&eglib,
		0, 0,
		eglib_GetWidth(&eglib), eglib_GetHeight(&eglib)
	);
}END_TEST

START_TEST(DrawFrame) {
	test_name = "DrawFrame";
	ssd1331_DrawFrame(
		&eglib,
		0, 0,
		eglib_GetWidth(&eglib), eglib_GetHeight(&eglib)
	);
}END_TEST

START_TEST(DrawBox) {
	test_name = "DrawBox";
	ssd1331_DrawBox(
		&eglib,
		0, 0,
		eglib_GetWidth(&eglib), eglib_GetHeight(&eglib)
	);
}END_TEST

void tcase_add_extra_tests(TCase *tcase) {
	tcase_add_test(tcase, SetMasterCurrent);
	tcase_add_test(tcase, SetDisplayStartLine);
	tcase_add_test(tcase, SetDisplayMode);
	tcase_add_test(tcase, SetDimMode);
	tcase_add_test(tcase, DrawLine);
	tcase_add_test(tcase, DrawFrame);
	tcase_add_test(tcase, DrawBox);
}