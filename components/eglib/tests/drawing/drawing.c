#include "../common.h"
#include <check.h>
#include <eglib.h>
#include <eglib/display/tga.h>
#include <eglib/hal/four_wire_spi/none.h>
#include <libgen.h>
#include <stdlib.h>

eglib_t eglib;
tga_config_t tga_config = {
	.width = 100,
	.height = 100,
};
char *expectation_name;

void setup(void);
void teardown(void);
Suite * build_suite(void);
char *reldir = RELDIR;

void setup(void) {
	eglib_Init(&eglib, &four_wire_spi_none, NULL, &tga, &tga_config);
}

START_TEST(test_eglib_AddUnicodeBlockToFont) {
	expectation_name = "eglib_AddUnicodeBlockToFont";
	#include "eglib_AddUnicodeBlockToFont.c"
}END_TEST

START_TEST(test_eglib_ClearScreen) {
	expectation_name = "eglib_ClearScreen";
	#include "eglib_ClearScreen.c"
}END_TEST

START_TEST(test_eglib_DrawArc) {
	expectation_name = "eglib_DrawArc";
	#include "eglib_DrawArc.c"
}END_TEST

START_TEST(test_eglib_DrawBitmap) {
	expectation_name = "eglib_DrawBitmap";
	#include "eglib_DrawBitmap.c"
}END_TEST

START_TEST(test_eglib_DrawBox) {
	expectation_name = "eglib_DrawBox";
	#include "eglib_DrawBox.c"
}END_TEST

START_TEST(test_eglib_DrawCircle) {
	expectation_name = "eglib_DrawCircle";
	#include "eglib_DrawCircle.c"
}END_TEST

START_TEST(test_eglib_DrawDisc) {
	expectation_name = "eglib_DrawDisc";
	#include "eglib_DrawDisc.c"
}END_TEST

START_TEST(test_eglib_DrawFilledArc) {
	expectation_name = "eglib_DrawFilledArc";
	#include "eglib_DrawFilledArc.c"
}END_TEST

START_TEST(test_eglib_DrawFrame) {
	expectation_name = "eglib_DrawFrame";
	#include "eglib_DrawFrame.c"
}END_TEST

START_TEST(test_eglib_DrawGradientArc) {
	expectation_name = "eglib_DrawGradientArc";
	#include "eglib_DrawGradientArc.c"
}END_TEST

START_TEST(test_eglib_DrawGradientBox) {
	expectation_name = "eglib_DrawGradientBox";
	#include "eglib_DrawGradientBox.c"
}END_TEST

START_TEST(test_eglib_DrawGradientDisc) {
	expectation_name = "eglib_DrawGradientDisc";
	#include "eglib_DrawGradientDisc.c"
}END_TEST

START_TEST(test_eglib_DrawGradientFilledArc) {
	expectation_name = "eglib_DrawGradientFilledArc";
	#include "eglib_DrawGradientFilledArc.c"
}END_TEST

START_TEST(test_eglib_DrawGradientFrame) {
	expectation_name = "eglib_DrawGradientFrame";
	#include "eglib_DrawGradientFrame.c"
}END_TEST

START_TEST(test_eglib_DrawGradientHLine) {
	expectation_name = "eglib_DrawGradientHLine";
	#include "eglib_DrawGradientHLine.c"
}END_TEST

START_TEST(test_eglib_DrawGradientLine) {
	expectation_name = "eglib_DrawGradientLine";
	#include "eglib_DrawGradientLine.c"
}END_TEST

START_TEST(test_eglib_DrawGradientVLine) {
	expectation_name = "eglib_DrawGradientVLine";
	#include "eglib_DrawGradientVLine.c"
}END_TEST

START_TEST(test_eglib_DrawHLine) {
	expectation_name = "eglib_DrawHLine";
	#include "eglib_DrawHLine.c"
}END_TEST

START_TEST(test_eglib_DrawLine) {
	expectation_name = "eglib_DrawLine";
	#include "eglib_DrawLine.c"
}END_TEST

START_TEST(test_eglib_DrawPixelColor) {
	expectation_name = "eglib_DrawPixelColor";
	#include "eglib_DrawPixelColor.c"
}END_TEST

START_TEST(test_eglib_DrawPixel) {
	expectation_name = "eglib_DrawPixel";
	#include "eglib_DrawPixel.c"
}END_TEST

START_TEST(test_eglib_DrawRoundBox) {
	expectation_name = "eglib_DrawRoundBox";
	#include "eglib_DrawRoundBox.c"
}END_TEST

START_TEST(test_eglib_DrawRoundFrame) {
	expectation_name = "eglib_DrawRoundFrame";
	#include "eglib_DrawRoundFrame.c"
}END_TEST

START_TEST(test_eglib_DrawText) {
	expectation_name = "eglib_DrawText";
	#include "eglib_DrawText.c"
}END_TEST

START_TEST(test_eglib_DrawTextCentered) {
	expectation_name = "eglib_DrawTextCentered";
	#include "eglib_DrawTextCentered.c"
}END_TEST

START_TEST(test_eglib_DrawTriangle) {
	expectation_name = "eglib_DrawTriangle";
	#include "eglib_DrawTriangle.c"
}END_TEST

START_TEST(test_eglib_DrawVLine) {
	expectation_name = "eglib_DrawVLine";
	#include "eglib_DrawVLine.c"
}END_TEST

START_TEST(test_eglib_DrawWChar) {
	expectation_name = "eglib_DrawWChar";
	#include "eglib_DrawWChar.c"
}END_TEST

void teardown(void) {
	compare_expectation(expectation_name, &eglib);
}

Suite * build_suite(void) {
	Suite *suite;
	TCase *tcase;

	suite = suite_create("drawing");

	tcase = tcase_create("functions");
	tcase_add_checked_fixture(tcase, setup, teardown);
	tcase_add_test(tcase, test_eglib_AddUnicodeBlockToFont);
	tcase_add_test(tcase, test_eglib_ClearScreen);
	tcase_add_test(tcase, test_eglib_DrawArc);
	tcase_add_test(tcase, test_eglib_DrawBitmap);
	tcase_add_test(tcase, test_eglib_DrawBox);
	tcase_add_test(tcase, test_eglib_DrawCircle);
	tcase_add_test(tcase, test_eglib_DrawDisc);
	tcase_add_test(tcase, test_eglib_DrawFilledArc);
	tcase_add_test(tcase, test_eglib_DrawFrame);
	tcase_add_test(tcase, test_eglib_DrawGradientArc);
	tcase_add_test(tcase, test_eglib_DrawGradientBox);
	tcase_add_test(tcase, test_eglib_DrawGradientDisc);
	tcase_add_test(tcase, test_eglib_DrawGradientFilledArc);
	tcase_add_test(tcase, test_eglib_DrawGradientFrame);
	tcase_add_test(tcase, test_eglib_DrawGradientHLine);
	tcase_add_test(tcase, test_eglib_DrawGradientLine);
	tcase_add_test(tcase, test_eglib_DrawGradientVLine);
	tcase_add_test(tcase, test_eglib_DrawHLine);
	tcase_add_test(tcase, test_eglib_DrawLine);
	tcase_add_test(tcase, test_eglib_DrawPixelColor);
	tcase_add_test(tcase, test_eglib_DrawPixel);
	tcase_add_test(tcase, test_eglib_DrawRoundBox);
	tcase_add_test(tcase, test_eglib_DrawRoundFrame);
	tcase_add_test(tcase, test_eglib_DrawText);
	tcase_add_test(tcase, test_eglib_DrawTextCentered);
	tcase_add_test(tcase, test_eglib_DrawTriangle);
	tcase_add_test(tcase, test_eglib_DrawVLine);
	tcase_add_test(tcase, test_eglib_DrawWChar);

	suite_add_tcase(suite, tcase);

	return suite;
}