#include <check.h>
#include <eglib.h>
#include <eglib/display/frame_buffer.h>
#include <eglib/display/tga.h>
#include <eglib/hal/four_wire_spi/none.h>
#include "../common.h"

eglib_t eglib;
frame_buffer_config_t frame_buffer_config;
tga_config_t tga_config = {
	.width = 100,
	.height = 100,
};
eglib_t *eglib_tga;
char *expectation_name;
char *reldir = RELDIR;

void setup(void);
void teardown(void);
Suite * build_suite(void);

void setup(void) {
	eglib_tga = eglib_Init_FrameBuffer(
		&eglib, &frame_buffer_config,
		&four_wire_spi_none, NULL,
		&tga, &tga_config
	);
}

START_TEST(test_eglib_FrameBuffer_Send) {
	coordinate_t width, height;

	expectation_name = "eglib_FrameBuffer_Send";

	width = eglib_GetWidth(&eglib);
	height = eglib_GetHeight(&eglib);

	eglib_SetIndexColor(&eglib, 0, 0, 0, 0);
	for(coordinate_t v=0 ; (v < width) && (v < height) ; v++ )
		eglib_DrawPixel(&eglib, v, v);

	eglib_SetIndexColor(&eglib, 0, 255, 0, 0);
	eglib_DrawLine(&eglib, 0, 0, width - 1, 0);

	eglib_SetIndexColor(&eglib, 0, 0, 255, 0);
	eglib_DrawLine(&eglib, 0, height - 1, 0, 0);

	eglib_SetIndexColor(&eglib, 0, 0, 0, 255);
	eglib_DrawLine(&eglib, 0, 0, width - 1, height - 1);

	eglib_FrameBuffer_Send(&eglib);
}END_TEST

START_TEST(test_eglib_FrameBuffer_SendPartial) {
	coordinate_t width, height;

	expectation_name = "eglib_FrameBuffer_SendPartial";

	width = eglib_GetWidth(&eglib);
	height = eglib_GetHeight(&eglib);
	eglib_SetIndexColor(&eglib, 0, 0, 0, 0);
	for(coordinate_t v=0 ; (v < width) && (v < height) ; v++ )
		eglib_DrawPixel(&eglib, v, v);

	eglib_SetIndexColor(&eglib, 0, 255, 0, 0);
	eglib_DrawLine(&eglib, 0, 0, width - 1, 0);

	eglib_SetIndexColor(&eglib, 0, 0, 255, 0);
	eglib_DrawLine(&eglib, 0, height - 1, 0, 0);

	eglib_SetIndexColor(&eglib, 0, 0, 0, 255);
	eglib_DrawLine(&eglib, 0, 0, width - 1, height - 1);

	eglib_FrameBuffer_SendPartial(&eglib, 0, 0, 50, 50);
}END_TEST

START_TEST(test_eglib_FrameBuffer_SendUpdated) {
	expectation_name = "eglib_FrameBuffer_SendUpdated";

	eglib_SetIndexColor(&eglib, 0, 255, 255, 255);
	eglib_DrawBox(&eglib, 25, 25, 50, 50);

	eglib_FrameBuffer_SendUpdated(&eglib);
}END_TEST

void teardown(void) {
	compare_expectation(expectation_name, eglib_tga);
}

Suite * build_suite(void) {
	Suite *suite;
	TCase *tcase;

	suite = suite_create("display");

	tcase = tcase_create("frame_buffer");
	tcase_add_checked_fixture(tcase, setup, teardown);
	tcase_add_test(tcase, test_eglib_FrameBuffer_Send);
	tcase_add_test(tcase, test_eglib_FrameBuffer_SendPartial);
	tcase_add_test(tcase, test_eglib_FrameBuffer_SendUpdated);

	suite_add_tcase(suite, tcase);

	return suite;
}