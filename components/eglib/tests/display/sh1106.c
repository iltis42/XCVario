#include <check.h>
#include <eglib/display/sh1106.h>

const display_t *display_driver = &sh1106_vdd1_2_4_v;
void *display_config_ptr = &sh1106_config_sparkfun_micro_oled;
coordinate_t width = 64;
coordinate_t height = 48;
enum pixel_format_t pixel_format = PIXEL_FORMAT_1BIT_BW_PAGED;
char driver_name[] = "sh1106";

extern eglib_t eglib;
extern char *test_name;

void tcase_add_extra_tests(TCase *tcase);

START_TEST(SetStartLine) {
	test_name = "SetStartLine";
	sh1106_SetStartLine(&eglib, eglib_GetHeight(&eglib) / 2);
}END_TEST

START_TEST(SetContrast) {
	test_name = "SetContrast";
	sh1106_SetContrast(&eglib, 128);
}END_TEST

START_TEST(EntireDisplayOn) {
	test_name = "EntireDisplayOn";
	sh1106_EntireDisplayOn(&eglib, true);
}END_TEST

START_TEST(Reverse) {
	test_name = "Reverse";
	sh1106_Reverse(&eglib, true);
}END_TEST

void tcase_add_extra_tests(TCase *tcase) {
	tcase_add_test(tcase, SetStartLine);
	tcase_add_test(tcase, SetContrast);
	tcase_add_test(tcase, EntireDisplayOn);
	tcase_add_test(tcase, Reverse);
}