#include "../../common.h"
#include "../unicode_block.h"
#include <check.h>
#include <eglib/display.h>
#include <libgen.h>
#include <stdlib.h>

struct font_t *font;
const struct glyph_unicode_block_t *unicode_block;
char *expectation_name;
char *reldir = RELDIR;

Suite * build_suite(void);

START_TEST(Adobe_Courier_10px_BasicLatin) {
	font = &font_Adobe_Courier_10px;
	unicode_block = &unicode_block_Adobe_Courier_10px_BasicLatin;
	expectation_name = "Adobe_Courier_10px_BasicLatin";
}END_TEST

START_TEST(Adobe_Courier_10px_Latin1Supplement) {
	font = &font_Adobe_Courier_10px;
	unicode_block = &unicode_block_Adobe_Courier_10px_Latin1Supplement;
	expectation_name = "Adobe_Courier_10px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_Courier_10px_LatinExtendedA) {
	font = &font_Adobe_Courier_10px;
	unicode_block = &unicode_block_Adobe_Courier_10px_LatinExtendedA;
	expectation_name = "Adobe_Courier_10px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_Courier_11px_BasicLatin) {
	font = &font_Adobe_Courier_11px;
	unicode_block = &unicode_block_Adobe_Courier_11px_BasicLatin;
	expectation_name = "Adobe_Courier_11px_BasicLatin";
}END_TEST

START_TEST(Adobe_Courier_11px_Latin1Supplement) {
	font = &font_Adobe_Courier_11px;
	unicode_block = &unicode_block_Adobe_Courier_11px_Latin1Supplement;
	expectation_name = "Adobe_Courier_11px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_Courier_11px_LatinExtendedA) {
	font = &font_Adobe_Courier_11px;
	unicode_block = &unicode_block_Adobe_Courier_11px_LatinExtendedA;
	expectation_name = "Adobe_Courier_11px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_Courier_12px_BasicLatin) {
	font = &font_Adobe_Courier_12px;
	unicode_block = &unicode_block_Adobe_Courier_12px_BasicLatin;
	expectation_name = "Adobe_Courier_12px_BasicLatin";
}END_TEST

START_TEST(Adobe_Courier_12px_Latin1Supplement) {
	font = &font_Adobe_Courier_12px;
	unicode_block = &unicode_block_Adobe_Courier_12px_Latin1Supplement;
	expectation_name = "Adobe_Courier_12px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_Courier_12px_LatinExtendedA) {
	font = &font_Adobe_Courier_12px;
	unicode_block = &unicode_block_Adobe_Courier_12px_LatinExtendedA;
	expectation_name = "Adobe_Courier_12px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_Courier_14px_BasicLatin) {
	font = &font_Adobe_Courier_14px;
	unicode_block = &unicode_block_Adobe_Courier_14px_BasicLatin;
	expectation_name = "Adobe_Courier_14px_BasicLatin";
}END_TEST

START_TEST(Adobe_Courier_14px_Latin1Supplement) {
	font = &font_Adobe_Courier_14px;
	unicode_block = &unicode_block_Adobe_Courier_14px_Latin1Supplement;
	expectation_name = "Adobe_Courier_14px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_Courier_14px_LatinExtendedA) {
	font = &font_Adobe_Courier_14px;
	unicode_block = &unicode_block_Adobe_Courier_14px_LatinExtendedA;
	expectation_name = "Adobe_Courier_14px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_Courier_17px_BasicLatin) {
	font = &font_Adobe_Courier_17px;
	unicode_block = &unicode_block_Adobe_Courier_17px_BasicLatin;
	expectation_name = "Adobe_Courier_17px_BasicLatin";
}END_TEST

START_TEST(Adobe_Courier_17px_Latin1Supplement) {
	font = &font_Adobe_Courier_17px;
	unicode_block = &unicode_block_Adobe_Courier_17px_Latin1Supplement;
	expectation_name = "Adobe_Courier_17px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_Courier_17px_LatinExtendedA) {
	font = &font_Adobe_Courier_17px;
	unicode_block = &unicode_block_Adobe_Courier_17px_LatinExtendedA;
	expectation_name = "Adobe_Courier_17px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_Courier_18px_BasicLatin) {
	font = &font_Adobe_Courier_18px;
	unicode_block = &unicode_block_Adobe_Courier_18px_BasicLatin;
	expectation_name = "Adobe_Courier_18px_BasicLatin";
}END_TEST

START_TEST(Adobe_Courier_18px_Latin1Supplement) {
	font = &font_Adobe_Courier_18px;
	unicode_block = &unicode_block_Adobe_Courier_18px_Latin1Supplement;
	expectation_name = "Adobe_Courier_18px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_Courier_18px_LatinExtendedA) {
	font = &font_Adobe_Courier_18px;
	unicode_block = &unicode_block_Adobe_Courier_18px_LatinExtendedA;
	expectation_name = "Adobe_Courier_18px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_Courier_20px_BasicLatin) {
	font = &font_Adobe_Courier_20px;
	unicode_block = &unicode_block_Adobe_Courier_20px_BasicLatin;
	expectation_name = "Adobe_Courier_20px_BasicLatin";
}END_TEST

START_TEST(Adobe_Courier_20px_Latin1Supplement) {
	font = &font_Adobe_Courier_20px;
	unicode_block = &unicode_block_Adobe_Courier_20px_Latin1Supplement;
	expectation_name = "Adobe_Courier_20px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_Courier_20px_LatinExtendedA) {
	font = &font_Adobe_Courier_20px;
	unicode_block = &unicode_block_Adobe_Courier_20px_LatinExtendedA;
	expectation_name = "Adobe_Courier_20px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_Courier_24px_BasicLatin) {
	font = &font_Adobe_Courier_24px;
	unicode_block = &unicode_block_Adobe_Courier_24px_BasicLatin;
	expectation_name = "Adobe_Courier_24px_BasicLatin";
}END_TEST

START_TEST(Adobe_Courier_24px_Latin1Supplement) {
	font = &font_Adobe_Courier_24px;
	unicode_block = &unicode_block_Adobe_Courier_24px_Latin1Supplement;
	expectation_name = "Adobe_Courier_24px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_Courier_24px_LatinExtendedA) {
	font = &font_Adobe_Courier_24px;
	unicode_block = &unicode_block_Adobe_Courier_24px_LatinExtendedA;
	expectation_name = "Adobe_Courier_24px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_Courier_25px_BasicLatin) {
	font = &font_Adobe_Courier_25px;
	unicode_block = &unicode_block_Adobe_Courier_25px_BasicLatin;
	expectation_name = "Adobe_Courier_25px_BasicLatin";
}END_TEST

START_TEST(Adobe_Courier_25px_Latin1Supplement) {
	font = &font_Adobe_Courier_25px;
	unicode_block = &unicode_block_Adobe_Courier_25px_Latin1Supplement;
	expectation_name = "Adobe_Courier_25px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_Courier_25px_LatinExtendedA) {
	font = &font_Adobe_Courier_25px;
	unicode_block = &unicode_block_Adobe_Courier_25px_LatinExtendedA;
	expectation_name = "Adobe_Courier_25px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_Courier_34px_BasicLatin) {
	font = &font_Adobe_Courier_34px;
	unicode_block = &unicode_block_Adobe_Courier_34px_BasicLatin;
	expectation_name = "Adobe_Courier_34px_BasicLatin";
}END_TEST

START_TEST(Adobe_Courier_34px_Latin1Supplement) {
	font = &font_Adobe_Courier_34px;
	unicode_block = &unicode_block_Adobe_Courier_34px_Latin1Supplement;
	expectation_name = "Adobe_Courier_34px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_Courier_34px_LatinExtendedA) {
	font = &font_Adobe_Courier_34px;
	unicode_block = &unicode_block_Adobe_Courier_34px_LatinExtendedA;
	expectation_name = "Adobe_Courier_34px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_Courier_8px_BasicLatin) {
	font = &font_Adobe_Courier_8px;
	unicode_block = &unicode_block_Adobe_Courier_8px_BasicLatin;
	expectation_name = "Adobe_Courier_8px_BasicLatin";
}END_TEST

START_TEST(Adobe_Courier_8px_Latin1Supplement) {
	font = &font_Adobe_Courier_8px;
	unicode_block = &unicode_block_Adobe_Courier_8px_Latin1Supplement;
	expectation_name = "Adobe_Courier_8px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_Courier_8px_LatinExtendedA) {
	font = &font_Adobe_Courier_8px;
	unicode_block = &unicode_block_Adobe_Courier_8px_LatinExtendedA;
	expectation_name = "Adobe_Courier_8px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierBold_10px_BasicLatin) {
	font = &font_Adobe_CourierBold_10px;
	unicode_block = &unicode_block_Adobe_CourierBold_10px_BasicLatin;
	expectation_name = "Adobe_CourierBold_10px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierBold_10px_Latin1Supplement) {
	font = &font_Adobe_CourierBold_10px;
	unicode_block = &unicode_block_Adobe_CourierBold_10px_Latin1Supplement;
	expectation_name = "Adobe_CourierBold_10px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierBold_10px_LatinExtendedA) {
	font = &font_Adobe_CourierBold_10px;
	unicode_block = &unicode_block_Adobe_CourierBold_10px_LatinExtendedA;
	expectation_name = "Adobe_CourierBold_10px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierBold_11px_BasicLatin) {
	font = &font_Adobe_CourierBold_11px;
	unicode_block = &unicode_block_Adobe_CourierBold_11px_BasicLatin;
	expectation_name = "Adobe_CourierBold_11px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierBold_11px_Latin1Supplement) {
	font = &font_Adobe_CourierBold_11px;
	unicode_block = &unicode_block_Adobe_CourierBold_11px_Latin1Supplement;
	expectation_name = "Adobe_CourierBold_11px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierBold_11px_LatinExtendedA) {
	font = &font_Adobe_CourierBold_11px;
	unicode_block = &unicode_block_Adobe_CourierBold_11px_LatinExtendedA;
	expectation_name = "Adobe_CourierBold_11px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierBold_12px_BasicLatin) {
	font = &font_Adobe_CourierBold_12px;
	unicode_block = &unicode_block_Adobe_CourierBold_12px_BasicLatin;
	expectation_name = "Adobe_CourierBold_12px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierBold_12px_Latin1Supplement) {
	font = &font_Adobe_CourierBold_12px;
	unicode_block = &unicode_block_Adobe_CourierBold_12px_Latin1Supplement;
	expectation_name = "Adobe_CourierBold_12px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierBold_12px_LatinExtendedA) {
	font = &font_Adobe_CourierBold_12px;
	unicode_block = &unicode_block_Adobe_CourierBold_12px_LatinExtendedA;
	expectation_name = "Adobe_CourierBold_12px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierBold_14px_BasicLatin) {
	font = &font_Adobe_CourierBold_14px;
	unicode_block = &unicode_block_Adobe_CourierBold_14px_BasicLatin;
	expectation_name = "Adobe_CourierBold_14px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierBold_14px_Latin1Supplement) {
	font = &font_Adobe_CourierBold_14px;
	unicode_block = &unicode_block_Adobe_CourierBold_14px_Latin1Supplement;
	expectation_name = "Adobe_CourierBold_14px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierBold_14px_LatinExtendedA) {
	font = &font_Adobe_CourierBold_14px;
	unicode_block = &unicode_block_Adobe_CourierBold_14px_LatinExtendedA;
	expectation_name = "Adobe_CourierBold_14px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierBold_17px_BasicLatin) {
	font = &font_Adobe_CourierBold_17px;
	unicode_block = &unicode_block_Adobe_CourierBold_17px_BasicLatin;
	expectation_name = "Adobe_CourierBold_17px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierBold_17px_Latin1Supplement) {
	font = &font_Adobe_CourierBold_17px;
	unicode_block = &unicode_block_Adobe_CourierBold_17px_Latin1Supplement;
	expectation_name = "Adobe_CourierBold_17px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierBold_17px_LatinExtendedA) {
	font = &font_Adobe_CourierBold_17px;
	unicode_block = &unicode_block_Adobe_CourierBold_17px_LatinExtendedA;
	expectation_name = "Adobe_CourierBold_17px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierBold_18px_BasicLatin) {
	font = &font_Adobe_CourierBold_18px;
	unicode_block = &unicode_block_Adobe_CourierBold_18px_BasicLatin;
	expectation_name = "Adobe_CourierBold_18px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierBold_18px_Latin1Supplement) {
	font = &font_Adobe_CourierBold_18px;
	unicode_block = &unicode_block_Adobe_CourierBold_18px_Latin1Supplement;
	expectation_name = "Adobe_CourierBold_18px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierBold_18px_LatinExtendedA) {
	font = &font_Adobe_CourierBold_18px;
	unicode_block = &unicode_block_Adobe_CourierBold_18px_LatinExtendedA;
	expectation_name = "Adobe_CourierBold_18px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierBold_20px_BasicLatin) {
	font = &font_Adobe_CourierBold_20px;
	unicode_block = &unicode_block_Adobe_CourierBold_20px_BasicLatin;
	expectation_name = "Adobe_CourierBold_20px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierBold_20px_Latin1Supplement) {
	font = &font_Adobe_CourierBold_20px;
	unicode_block = &unicode_block_Adobe_CourierBold_20px_Latin1Supplement;
	expectation_name = "Adobe_CourierBold_20px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierBold_20px_LatinExtendedA) {
	font = &font_Adobe_CourierBold_20px;
	unicode_block = &unicode_block_Adobe_CourierBold_20px_LatinExtendedA;
	expectation_name = "Adobe_CourierBold_20px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierBold_24px_BasicLatin) {
	font = &font_Adobe_CourierBold_24px;
	unicode_block = &unicode_block_Adobe_CourierBold_24px_BasicLatin;
	expectation_name = "Adobe_CourierBold_24px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierBold_24px_Latin1Supplement) {
	font = &font_Adobe_CourierBold_24px;
	unicode_block = &unicode_block_Adobe_CourierBold_24px_Latin1Supplement;
	expectation_name = "Adobe_CourierBold_24px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierBold_24px_LatinExtendedA) {
	font = &font_Adobe_CourierBold_24px;
	unicode_block = &unicode_block_Adobe_CourierBold_24px_LatinExtendedA;
	expectation_name = "Adobe_CourierBold_24px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierBold_25px_BasicLatin) {
	font = &font_Adobe_CourierBold_25px;
	unicode_block = &unicode_block_Adobe_CourierBold_25px_BasicLatin;
	expectation_name = "Adobe_CourierBold_25px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierBold_25px_Latin1Supplement) {
	font = &font_Adobe_CourierBold_25px;
	unicode_block = &unicode_block_Adobe_CourierBold_25px_Latin1Supplement;
	expectation_name = "Adobe_CourierBold_25px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierBold_25px_LatinExtendedA) {
	font = &font_Adobe_CourierBold_25px;
	unicode_block = &unicode_block_Adobe_CourierBold_25px_LatinExtendedA;
	expectation_name = "Adobe_CourierBold_25px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierBold_34px_BasicLatin) {
	font = &font_Adobe_CourierBold_34px;
	unicode_block = &unicode_block_Adobe_CourierBold_34px_BasicLatin;
	expectation_name = "Adobe_CourierBold_34px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierBold_34px_Latin1Supplement) {
	font = &font_Adobe_CourierBold_34px;
	unicode_block = &unicode_block_Adobe_CourierBold_34px_Latin1Supplement;
	expectation_name = "Adobe_CourierBold_34px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierBold_34px_LatinExtendedA) {
	font = &font_Adobe_CourierBold_34px;
	unicode_block = &unicode_block_Adobe_CourierBold_34px_LatinExtendedA;
	expectation_name = "Adobe_CourierBold_34px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierBold_8px_BasicLatin) {
	font = &font_Adobe_CourierBold_8px;
	unicode_block = &unicode_block_Adobe_CourierBold_8px_BasicLatin;
	expectation_name = "Adobe_CourierBold_8px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierBold_8px_Latin1Supplement) {
	font = &font_Adobe_CourierBold_8px;
	unicode_block = &unicode_block_Adobe_CourierBold_8px_Latin1Supplement;
	expectation_name = "Adobe_CourierBold_8px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierBold_8px_LatinExtendedA) {
	font = &font_Adobe_CourierBold_8px;
	unicode_block = &unicode_block_Adobe_CourierBold_8px_LatinExtendedA;
	expectation_name = "Adobe_CourierBold_8px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_10px_BasicLatin) {
	font = &font_Adobe_CourierBoldOblique_10px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_10px_BasicLatin;
	expectation_name = "Adobe_CourierBoldOblique_10px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_10px_Latin1Supplement) {
	font = &font_Adobe_CourierBoldOblique_10px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_10px_Latin1Supplement;
	expectation_name = "Adobe_CourierBoldOblique_10px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_10px_LatinExtendedA) {
	font = &font_Adobe_CourierBoldOblique_10px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_10px_LatinExtendedA;
	expectation_name = "Adobe_CourierBoldOblique_10px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_11px_BasicLatin) {
	font = &font_Adobe_CourierBoldOblique_11px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_11px_BasicLatin;
	expectation_name = "Adobe_CourierBoldOblique_11px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_11px_Latin1Supplement) {
	font = &font_Adobe_CourierBoldOblique_11px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_11px_Latin1Supplement;
	expectation_name = "Adobe_CourierBoldOblique_11px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_11px_LatinExtendedA) {
	font = &font_Adobe_CourierBoldOblique_11px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_11px_LatinExtendedA;
	expectation_name = "Adobe_CourierBoldOblique_11px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_12px_BasicLatin) {
	font = &font_Adobe_CourierBoldOblique_12px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_12px_BasicLatin;
	expectation_name = "Adobe_CourierBoldOblique_12px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_12px_Latin1Supplement) {
	font = &font_Adobe_CourierBoldOblique_12px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_12px_Latin1Supplement;
	expectation_name = "Adobe_CourierBoldOblique_12px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_12px_LatinExtendedA) {
	font = &font_Adobe_CourierBoldOblique_12px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_12px_LatinExtendedA;
	expectation_name = "Adobe_CourierBoldOblique_12px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_14px_BasicLatin) {
	font = &font_Adobe_CourierBoldOblique_14px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_14px_BasicLatin;
	expectation_name = "Adobe_CourierBoldOblique_14px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_14px_Latin1Supplement) {
	font = &font_Adobe_CourierBoldOblique_14px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_14px_Latin1Supplement;
	expectation_name = "Adobe_CourierBoldOblique_14px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_14px_LatinExtendedA) {
	font = &font_Adobe_CourierBoldOblique_14px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_14px_LatinExtendedA;
	expectation_name = "Adobe_CourierBoldOblique_14px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_17px_BasicLatin) {
	font = &font_Adobe_CourierBoldOblique_17px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_17px_BasicLatin;
	expectation_name = "Adobe_CourierBoldOblique_17px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_17px_Latin1Supplement) {
	font = &font_Adobe_CourierBoldOblique_17px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_17px_Latin1Supplement;
	expectation_name = "Adobe_CourierBoldOblique_17px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_17px_LatinExtendedA) {
	font = &font_Adobe_CourierBoldOblique_17px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_17px_LatinExtendedA;
	expectation_name = "Adobe_CourierBoldOblique_17px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_18px_BasicLatin) {
	font = &font_Adobe_CourierBoldOblique_18px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_18px_BasicLatin;
	expectation_name = "Adobe_CourierBoldOblique_18px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_18px_Latin1Supplement) {
	font = &font_Adobe_CourierBoldOblique_18px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_18px_Latin1Supplement;
	expectation_name = "Adobe_CourierBoldOblique_18px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_18px_LatinExtendedA) {
	font = &font_Adobe_CourierBoldOblique_18px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_18px_LatinExtendedA;
	expectation_name = "Adobe_CourierBoldOblique_18px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_20px_BasicLatin) {
	font = &font_Adobe_CourierBoldOblique_20px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_20px_BasicLatin;
	expectation_name = "Adobe_CourierBoldOblique_20px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_20px_Latin1Supplement) {
	font = &font_Adobe_CourierBoldOblique_20px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_20px_Latin1Supplement;
	expectation_name = "Adobe_CourierBoldOblique_20px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_20px_LatinExtendedA) {
	font = &font_Adobe_CourierBoldOblique_20px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_20px_LatinExtendedA;
	expectation_name = "Adobe_CourierBoldOblique_20px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_24px_BasicLatin) {
	font = &font_Adobe_CourierBoldOblique_24px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_24px_BasicLatin;
	expectation_name = "Adobe_CourierBoldOblique_24px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_24px_Latin1Supplement) {
	font = &font_Adobe_CourierBoldOblique_24px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_24px_Latin1Supplement;
	expectation_name = "Adobe_CourierBoldOblique_24px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_24px_LatinExtendedA) {
	font = &font_Adobe_CourierBoldOblique_24px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_24px_LatinExtendedA;
	expectation_name = "Adobe_CourierBoldOblique_24px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_25px_BasicLatin) {
	font = &font_Adobe_CourierBoldOblique_25px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_25px_BasicLatin;
	expectation_name = "Adobe_CourierBoldOblique_25px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_25px_Latin1Supplement) {
	font = &font_Adobe_CourierBoldOblique_25px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_25px_Latin1Supplement;
	expectation_name = "Adobe_CourierBoldOblique_25px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_25px_LatinExtendedA) {
	font = &font_Adobe_CourierBoldOblique_25px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_25px_LatinExtendedA;
	expectation_name = "Adobe_CourierBoldOblique_25px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_34px_BasicLatin) {
	font = &font_Adobe_CourierBoldOblique_34px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_34px_BasicLatin;
	expectation_name = "Adobe_CourierBoldOblique_34px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_34px_Latin1Supplement) {
	font = &font_Adobe_CourierBoldOblique_34px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_34px_Latin1Supplement;
	expectation_name = "Adobe_CourierBoldOblique_34px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_34px_LatinExtendedA) {
	font = &font_Adobe_CourierBoldOblique_34px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_34px_LatinExtendedA;
	expectation_name = "Adobe_CourierBoldOblique_34px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_8px_BasicLatin) {
	font = &font_Adobe_CourierBoldOblique_8px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_8px_BasicLatin;
	expectation_name = "Adobe_CourierBoldOblique_8px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_8px_Latin1Supplement) {
	font = &font_Adobe_CourierBoldOblique_8px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_8px_Latin1Supplement;
	expectation_name = "Adobe_CourierBoldOblique_8px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierBoldOblique_8px_LatinExtendedA) {
	font = &font_Adobe_CourierBoldOblique_8px;
	unicode_block = &unicode_block_Adobe_CourierBoldOblique_8px_LatinExtendedA;
	expectation_name = "Adobe_CourierBoldOblique_8px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierOblique_10px_BasicLatin) {
	font = &font_Adobe_CourierOblique_10px;
	unicode_block = &unicode_block_Adobe_CourierOblique_10px_BasicLatin;
	expectation_name = "Adobe_CourierOblique_10px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierOblique_10px_Latin1Supplement) {
	font = &font_Adobe_CourierOblique_10px;
	unicode_block = &unicode_block_Adobe_CourierOblique_10px_Latin1Supplement;
	expectation_name = "Adobe_CourierOblique_10px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierOblique_10px_LatinExtendedA) {
	font = &font_Adobe_CourierOblique_10px;
	unicode_block = &unicode_block_Adobe_CourierOblique_10px_LatinExtendedA;
	expectation_name = "Adobe_CourierOblique_10px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierOblique_11px_BasicLatin) {
	font = &font_Adobe_CourierOblique_11px;
	unicode_block = &unicode_block_Adobe_CourierOblique_11px_BasicLatin;
	expectation_name = "Adobe_CourierOblique_11px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierOblique_11px_Latin1Supplement) {
	font = &font_Adobe_CourierOblique_11px;
	unicode_block = &unicode_block_Adobe_CourierOblique_11px_Latin1Supplement;
	expectation_name = "Adobe_CourierOblique_11px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierOblique_11px_LatinExtendedA) {
	font = &font_Adobe_CourierOblique_11px;
	unicode_block = &unicode_block_Adobe_CourierOblique_11px_LatinExtendedA;
	expectation_name = "Adobe_CourierOblique_11px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierOblique_12px_BasicLatin) {
	font = &font_Adobe_CourierOblique_12px;
	unicode_block = &unicode_block_Adobe_CourierOblique_12px_BasicLatin;
	expectation_name = "Adobe_CourierOblique_12px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierOblique_12px_Latin1Supplement) {
	font = &font_Adobe_CourierOblique_12px;
	unicode_block = &unicode_block_Adobe_CourierOblique_12px_Latin1Supplement;
	expectation_name = "Adobe_CourierOblique_12px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierOblique_12px_LatinExtendedA) {
	font = &font_Adobe_CourierOblique_12px;
	unicode_block = &unicode_block_Adobe_CourierOblique_12px_LatinExtendedA;
	expectation_name = "Adobe_CourierOblique_12px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierOblique_14px_BasicLatin) {
	font = &font_Adobe_CourierOblique_14px;
	unicode_block = &unicode_block_Adobe_CourierOblique_14px_BasicLatin;
	expectation_name = "Adobe_CourierOblique_14px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierOblique_14px_Latin1Supplement) {
	font = &font_Adobe_CourierOblique_14px;
	unicode_block = &unicode_block_Adobe_CourierOblique_14px_Latin1Supplement;
	expectation_name = "Adobe_CourierOblique_14px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierOblique_14px_LatinExtendedA) {
	font = &font_Adobe_CourierOblique_14px;
	unicode_block = &unicode_block_Adobe_CourierOblique_14px_LatinExtendedA;
	expectation_name = "Adobe_CourierOblique_14px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierOblique_17px_BasicLatin) {
	font = &font_Adobe_CourierOblique_17px;
	unicode_block = &unicode_block_Adobe_CourierOblique_17px_BasicLatin;
	expectation_name = "Adobe_CourierOblique_17px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierOblique_17px_Latin1Supplement) {
	font = &font_Adobe_CourierOblique_17px;
	unicode_block = &unicode_block_Adobe_CourierOblique_17px_Latin1Supplement;
	expectation_name = "Adobe_CourierOblique_17px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierOblique_17px_LatinExtendedA) {
	font = &font_Adobe_CourierOblique_17px;
	unicode_block = &unicode_block_Adobe_CourierOblique_17px_LatinExtendedA;
	expectation_name = "Adobe_CourierOblique_17px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierOblique_18px_BasicLatin) {
	font = &font_Adobe_CourierOblique_18px;
	unicode_block = &unicode_block_Adobe_CourierOblique_18px_BasicLatin;
	expectation_name = "Adobe_CourierOblique_18px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierOblique_18px_Latin1Supplement) {
	font = &font_Adobe_CourierOblique_18px;
	unicode_block = &unicode_block_Adobe_CourierOblique_18px_Latin1Supplement;
	expectation_name = "Adobe_CourierOblique_18px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierOblique_18px_LatinExtendedA) {
	font = &font_Adobe_CourierOblique_18px;
	unicode_block = &unicode_block_Adobe_CourierOblique_18px_LatinExtendedA;
	expectation_name = "Adobe_CourierOblique_18px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierOblique_20px_BasicLatin) {
	font = &font_Adobe_CourierOblique_20px;
	unicode_block = &unicode_block_Adobe_CourierOblique_20px_BasicLatin;
	expectation_name = "Adobe_CourierOblique_20px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierOblique_20px_Latin1Supplement) {
	font = &font_Adobe_CourierOblique_20px;
	unicode_block = &unicode_block_Adobe_CourierOblique_20px_Latin1Supplement;
	expectation_name = "Adobe_CourierOblique_20px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierOblique_20px_LatinExtendedA) {
	font = &font_Adobe_CourierOblique_20px;
	unicode_block = &unicode_block_Adobe_CourierOblique_20px_LatinExtendedA;
	expectation_name = "Adobe_CourierOblique_20px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierOblique_24px_BasicLatin) {
	font = &font_Adobe_CourierOblique_24px;
	unicode_block = &unicode_block_Adobe_CourierOblique_24px_BasicLatin;
	expectation_name = "Adobe_CourierOblique_24px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierOblique_24px_Latin1Supplement) {
	font = &font_Adobe_CourierOblique_24px;
	unicode_block = &unicode_block_Adobe_CourierOblique_24px_Latin1Supplement;
	expectation_name = "Adobe_CourierOblique_24px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierOblique_24px_LatinExtendedA) {
	font = &font_Adobe_CourierOblique_24px;
	unicode_block = &unicode_block_Adobe_CourierOblique_24px_LatinExtendedA;
	expectation_name = "Adobe_CourierOblique_24px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierOblique_25px_BasicLatin) {
	font = &font_Adobe_CourierOblique_25px;
	unicode_block = &unicode_block_Adobe_CourierOblique_25px_BasicLatin;
	expectation_name = "Adobe_CourierOblique_25px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierOblique_25px_Latin1Supplement) {
	font = &font_Adobe_CourierOblique_25px;
	unicode_block = &unicode_block_Adobe_CourierOblique_25px_Latin1Supplement;
	expectation_name = "Adobe_CourierOblique_25px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierOblique_25px_LatinExtendedA) {
	font = &font_Adobe_CourierOblique_25px;
	unicode_block = &unicode_block_Adobe_CourierOblique_25px_LatinExtendedA;
	expectation_name = "Adobe_CourierOblique_25px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierOblique_34px_BasicLatin) {
	font = &font_Adobe_CourierOblique_34px;
	unicode_block = &unicode_block_Adobe_CourierOblique_34px_BasicLatin;
	expectation_name = "Adobe_CourierOblique_34px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierOblique_34px_Latin1Supplement) {
	font = &font_Adobe_CourierOblique_34px;
	unicode_block = &unicode_block_Adobe_CourierOblique_34px_Latin1Supplement;
	expectation_name = "Adobe_CourierOblique_34px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierOblique_34px_LatinExtendedA) {
	font = &font_Adobe_CourierOblique_34px;
	unicode_block = &unicode_block_Adobe_CourierOblique_34px_LatinExtendedA;
	expectation_name = "Adobe_CourierOblique_34px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_CourierOblique_8px_BasicLatin) {
	font = &font_Adobe_CourierOblique_8px;
	unicode_block = &unicode_block_Adobe_CourierOblique_8px_BasicLatin;
	expectation_name = "Adobe_CourierOblique_8px_BasicLatin";
}END_TEST

START_TEST(Adobe_CourierOblique_8px_Latin1Supplement) {
	font = &font_Adobe_CourierOblique_8px;
	unicode_block = &unicode_block_Adobe_CourierOblique_8px_Latin1Supplement;
	expectation_name = "Adobe_CourierOblique_8px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_CourierOblique_8px_LatinExtendedA) {
	font = &font_Adobe_CourierOblique_8px;
	unicode_block = &unicode_block_Adobe_CourierOblique_8px_LatinExtendedA;
	expectation_name = "Adobe_CourierOblique_8px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_Helvetica_10px_BasicLatin) {
	font = &font_Adobe_Helvetica_10px;
	unicode_block = &unicode_block_Adobe_Helvetica_10px_BasicLatin;
	expectation_name = "Adobe_Helvetica_10px_BasicLatin";
}END_TEST

START_TEST(Adobe_Helvetica_10px_Latin1Supplement) {
	font = &font_Adobe_Helvetica_10px;
	unicode_block = &unicode_block_Adobe_Helvetica_10px_Latin1Supplement;
	expectation_name = "Adobe_Helvetica_10px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_Helvetica_10px_LatinExtendedA) {
	font = &font_Adobe_Helvetica_10px;
	unicode_block = &unicode_block_Adobe_Helvetica_10px_LatinExtendedA;
	expectation_name = "Adobe_Helvetica_10px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_Helvetica_11px_BasicLatin) {
	font = &font_Adobe_Helvetica_11px;
	unicode_block = &unicode_block_Adobe_Helvetica_11px_BasicLatin;
	expectation_name = "Adobe_Helvetica_11px_BasicLatin";
}END_TEST

START_TEST(Adobe_Helvetica_11px_Latin1Supplement) {
	font = &font_Adobe_Helvetica_11px;
	unicode_block = &unicode_block_Adobe_Helvetica_11px_Latin1Supplement;
	expectation_name = "Adobe_Helvetica_11px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_Helvetica_11px_LatinExtendedA) {
	font = &font_Adobe_Helvetica_11px;
	unicode_block = &unicode_block_Adobe_Helvetica_11px_LatinExtendedA;
	expectation_name = "Adobe_Helvetica_11px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_Helvetica_12px_BasicLatin) {
	font = &font_Adobe_Helvetica_12px;
	unicode_block = &unicode_block_Adobe_Helvetica_12px_BasicLatin;
	expectation_name = "Adobe_Helvetica_12px_BasicLatin";
}END_TEST

START_TEST(Adobe_Helvetica_12px_Latin1Supplement) {
	font = &font_Adobe_Helvetica_12px;
	unicode_block = &unicode_block_Adobe_Helvetica_12px_Latin1Supplement;
	expectation_name = "Adobe_Helvetica_12px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_Helvetica_12px_LatinExtendedA) {
	font = &font_Adobe_Helvetica_12px;
	unicode_block = &unicode_block_Adobe_Helvetica_12px_LatinExtendedA;
	expectation_name = "Adobe_Helvetica_12px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_Helvetica_14px_BasicLatin) {
	font = &font_Adobe_Helvetica_14px;
	unicode_block = &unicode_block_Adobe_Helvetica_14px_BasicLatin;
	expectation_name = "Adobe_Helvetica_14px_BasicLatin";
}END_TEST

START_TEST(Adobe_Helvetica_14px_Latin1Supplement) {
	font = &font_Adobe_Helvetica_14px;
	unicode_block = &unicode_block_Adobe_Helvetica_14px_Latin1Supplement;
	expectation_name = "Adobe_Helvetica_14px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_Helvetica_14px_LatinExtendedA) {
	font = &font_Adobe_Helvetica_14px;
	unicode_block = &unicode_block_Adobe_Helvetica_14px_LatinExtendedA;
	expectation_name = "Adobe_Helvetica_14px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_Helvetica_17px_BasicLatin) {
	font = &font_Adobe_Helvetica_17px;
	unicode_block = &unicode_block_Adobe_Helvetica_17px_BasicLatin;
	expectation_name = "Adobe_Helvetica_17px_BasicLatin";
}END_TEST

START_TEST(Adobe_Helvetica_17px_Latin1Supplement) {
	font = &font_Adobe_Helvetica_17px;
	unicode_block = &unicode_block_Adobe_Helvetica_17px_Latin1Supplement;
	expectation_name = "Adobe_Helvetica_17px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_Helvetica_17px_LatinExtendedA) {
	font = &font_Adobe_Helvetica_17px;
	unicode_block = &unicode_block_Adobe_Helvetica_17px_LatinExtendedA;
	expectation_name = "Adobe_Helvetica_17px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_Helvetica_18px_BasicLatin) {
	font = &font_Adobe_Helvetica_18px;
	unicode_block = &unicode_block_Adobe_Helvetica_18px_BasicLatin;
	expectation_name = "Adobe_Helvetica_18px_BasicLatin";
}END_TEST

START_TEST(Adobe_Helvetica_18px_Latin1Supplement) {
	font = &font_Adobe_Helvetica_18px;
	unicode_block = &unicode_block_Adobe_Helvetica_18px_Latin1Supplement;
	expectation_name = "Adobe_Helvetica_18px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_Helvetica_18px_LatinExtendedA) {
	font = &font_Adobe_Helvetica_18px;
	unicode_block = &unicode_block_Adobe_Helvetica_18px_LatinExtendedA;
	expectation_name = "Adobe_Helvetica_18px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_Helvetica_20px_BasicLatin) {
	font = &font_Adobe_Helvetica_20px;
	unicode_block = &unicode_block_Adobe_Helvetica_20px_BasicLatin;
	expectation_name = "Adobe_Helvetica_20px_BasicLatin";
}END_TEST

START_TEST(Adobe_Helvetica_20px_Latin1Supplement) {
	font = &font_Adobe_Helvetica_20px;
	unicode_block = &unicode_block_Adobe_Helvetica_20px_Latin1Supplement;
	expectation_name = "Adobe_Helvetica_20px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_Helvetica_20px_LatinExtendedA) {
	font = &font_Adobe_Helvetica_20px;
	unicode_block = &unicode_block_Adobe_Helvetica_20px_LatinExtendedA;
	expectation_name = "Adobe_Helvetica_20px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_Helvetica_24px_BasicLatin) {
	font = &font_Adobe_Helvetica_24px;
	unicode_block = &unicode_block_Adobe_Helvetica_24px_BasicLatin;
	expectation_name = "Adobe_Helvetica_24px_BasicLatin";
}END_TEST

START_TEST(Adobe_Helvetica_24px_Latin1Supplement) {
	font = &font_Adobe_Helvetica_24px;
	unicode_block = &unicode_block_Adobe_Helvetica_24px_Latin1Supplement;
	expectation_name = "Adobe_Helvetica_24px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_Helvetica_24px_LatinExtendedA) {
	font = &font_Adobe_Helvetica_24px;
	unicode_block = &unicode_block_Adobe_Helvetica_24px_LatinExtendedA;
	expectation_name = "Adobe_Helvetica_24px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_Helvetica_25px_BasicLatin) {
	font = &font_Adobe_Helvetica_25px;
	unicode_block = &unicode_block_Adobe_Helvetica_25px_BasicLatin;
	expectation_name = "Adobe_Helvetica_25px_BasicLatin";
}END_TEST

START_TEST(Adobe_Helvetica_25px_Latin1Supplement) {
	font = &font_Adobe_Helvetica_25px;
	unicode_block = &unicode_block_Adobe_Helvetica_25px_Latin1Supplement;
	expectation_name = "Adobe_Helvetica_25px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_Helvetica_25px_LatinExtendedA) {
	font = &font_Adobe_Helvetica_25px;
	unicode_block = &unicode_block_Adobe_Helvetica_25px_LatinExtendedA;
	expectation_name = "Adobe_Helvetica_25px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_Helvetica_34px_BasicLatin) {
	font = &font_Adobe_Helvetica_34px;
	unicode_block = &unicode_block_Adobe_Helvetica_34px_BasicLatin;
	expectation_name = "Adobe_Helvetica_34px_BasicLatin";
}END_TEST

START_TEST(Adobe_Helvetica_34px_Latin1Supplement) {
	font = &font_Adobe_Helvetica_34px;
	unicode_block = &unicode_block_Adobe_Helvetica_34px_Latin1Supplement;
	expectation_name = "Adobe_Helvetica_34px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_Helvetica_34px_LatinExtendedA) {
	font = &font_Adobe_Helvetica_34px;
	unicode_block = &unicode_block_Adobe_Helvetica_34px_LatinExtendedA;
	expectation_name = "Adobe_Helvetica_34px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_Helvetica_8px_BasicLatin) {
	font = &font_Adobe_Helvetica_8px;
	unicode_block = &unicode_block_Adobe_Helvetica_8px_BasicLatin;
	expectation_name = "Adobe_Helvetica_8px_BasicLatin";
}END_TEST

START_TEST(Adobe_Helvetica_8px_Latin1Supplement) {
	font = &font_Adobe_Helvetica_8px;
	unicode_block = &unicode_block_Adobe_Helvetica_8px_Latin1Supplement;
	expectation_name = "Adobe_Helvetica_8px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_Helvetica_8px_LatinExtendedA) {
	font = &font_Adobe_Helvetica_8px;
	unicode_block = &unicode_block_Adobe_Helvetica_8px_LatinExtendedA;
	expectation_name = "Adobe_Helvetica_8px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaBold_10px_BasicLatin) {
	font = &font_Adobe_HelveticaBold_10px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_10px_BasicLatin;
	expectation_name = "Adobe_HelveticaBold_10px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaBold_10px_Latin1Supplement) {
	font = &font_Adobe_HelveticaBold_10px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_10px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaBold_10px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaBold_10px_LatinExtendedA) {
	font = &font_Adobe_HelveticaBold_10px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_10px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaBold_10px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaBold_11px_BasicLatin) {
	font = &font_Adobe_HelveticaBold_11px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_11px_BasicLatin;
	expectation_name = "Adobe_HelveticaBold_11px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaBold_11px_Latin1Supplement) {
	font = &font_Adobe_HelveticaBold_11px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_11px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaBold_11px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaBold_11px_LatinExtendedA) {
	font = &font_Adobe_HelveticaBold_11px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_11px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaBold_11px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaBold_12px_BasicLatin) {
	font = &font_Adobe_HelveticaBold_12px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_12px_BasicLatin;
	expectation_name = "Adobe_HelveticaBold_12px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaBold_12px_Latin1Supplement) {
	font = &font_Adobe_HelveticaBold_12px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_12px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaBold_12px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaBold_12px_LatinExtendedA) {
	font = &font_Adobe_HelveticaBold_12px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_12px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaBold_12px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaBold_14px_BasicLatin) {
	font = &font_Adobe_HelveticaBold_14px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_14px_BasicLatin;
	expectation_name = "Adobe_HelveticaBold_14px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaBold_14px_Latin1Supplement) {
	font = &font_Adobe_HelveticaBold_14px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_14px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaBold_14px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaBold_14px_LatinExtendedA) {
	font = &font_Adobe_HelveticaBold_14px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_14px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaBold_14px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaBold_17px_BasicLatin) {
	font = &font_Adobe_HelveticaBold_17px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_17px_BasicLatin;
	expectation_name = "Adobe_HelveticaBold_17px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaBold_17px_Latin1Supplement) {
	font = &font_Adobe_HelveticaBold_17px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_17px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaBold_17px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaBold_17px_LatinExtendedA) {
	font = &font_Adobe_HelveticaBold_17px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_17px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaBold_17px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaBold_18px_BasicLatin) {
	font = &font_Adobe_HelveticaBold_18px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_18px_BasicLatin;
	expectation_name = "Adobe_HelveticaBold_18px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaBold_18px_Latin1Supplement) {
	font = &font_Adobe_HelveticaBold_18px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_18px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaBold_18px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaBold_18px_LatinExtendedA) {
	font = &font_Adobe_HelveticaBold_18px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_18px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaBold_18px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaBold_20px_BasicLatin) {
	font = &font_Adobe_HelveticaBold_20px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_20px_BasicLatin;
	expectation_name = "Adobe_HelveticaBold_20px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaBold_20px_Latin1Supplement) {
	font = &font_Adobe_HelveticaBold_20px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_20px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaBold_20px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaBold_20px_LatinExtendedA) {
	font = &font_Adobe_HelveticaBold_20px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_20px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaBold_20px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaBold_24px_BasicLatin) {
	font = &font_Adobe_HelveticaBold_24px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_24px_BasicLatin;
	expectation_name = "Adobe_HelveticaBold_24px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaBold_24px_Latin1Supplement) {
	font = &font_Adobe_HelveticaBold_24px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_24px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaBold_24px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaBold_24px_LatinExtendedA) {
	font = &font_Adobe_HelveticaBold_24px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_24px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaBold_24px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaBold_25px_BasicLatin) {
	font = &font_Adobe_HelveticaBold_25px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_25px_BasicLatin;
	expectation_name = "Adobe_HelveticaBold_25px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaBold_25px_Latin1Supplement) {
	font = &font_Adobe_HelveticaBold_25px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_25px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaBold_25px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaBold_25px_LatinExtendedA) {
	font = &font_Adobe_HelveticaBold_25px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_25px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaBold_25px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaBold_34px_BasicLatin) {
	font = &font_Adobe_HelveticaBold_34px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_34px_BasicLatin;
	expectation_name = "Adobe_HelveticaBold_34px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaBold_34px_Latin1Supplement) {
	font = &font_Adobe_HelveticaBold_34px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_34px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaBold_34px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaBold_34px_LatinExtendedA) {
	font = &font_Adobe_HelveticaBold_34px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_34px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaBold_34px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaBold_8px_BasicLatin) {
	font = &font_Adobe_HelveticaBold_8px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_8px_BasicLatin;
	expectation_name = "Adobe_HelveticaBold_8px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaBold_8px_Latin1Supplement) {
	font = &font_Adobe_HelveticaBold_8px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_8px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaBold_8px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaBold_8px_LatinExtendedA) {
	font = &font_Adobe_HelveticaBold_8px;
	unicode_block = &unicode_block_Adobe_HelveticaBold_8px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaBold_8px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_10px_BasicLatin) {
	font = &font_Adobe_HelveticaBoldOblique_10px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_10px_BasicLatin;
	expectation_name = "Adobe_HelveticaBoldOblique_10px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_10px_Latin1Supplement) {
	font = &font_Adobe_HelveticaBoldOblique_10px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_10px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaBoldOblique_10px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_10px_LatinExtendedA) {
	font = &font_Adobe_HelveticaBoldOblique_10px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_10px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaBoldOblique_10px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_11px_BasicLatin) {
	font = &font_Adobe_HelveticaBoldOblique_11px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_11px_BasicLatin;
	expectation_name = "Adobe_HelveticaBoldOblique_11px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_11px_Latin1Supplement) {
	font = &font_Adobe_HelveticaBoldOblique_11px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_11px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaBoldOblique_11px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_11px_LatinExtendedA) {
	font = &font_Adobe_HelveticaBoldOblique_11px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_11px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaBoldOblique_11px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_12px_BasicLatin) {
	font = &font_Adobe_HelveticaBoldOblique_12px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_12px_BasicLatin;
	expectation_name = "Adobe_HelveticaBoldOblique_12px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_12px_Latin1Supplement) {
	font = &font_Adobe_HelveticaBoldOblique_12px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_12px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaBoldOblique_12px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_12px_LatinExtendedA) {
	font = &font_Adobe_HelveticaBoldOblique_12px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_12px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaBoldOblique_12px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_14px_BasicLatin) {
	font = &font_Adobe_HelveticaBoldOblique_14px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_14px_BasicLatin;
	expectation_name = "Adobe_HelveticaBoldOblique_14px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_14px_Latin1Supplement) {
	font = &font_Adobe_HelveticaBoldOblique_14px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_14px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaBoldOblique_14px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_14px_LatinExtendedA) {
	font = &font_Adobe_HelveticaBoldOblique_14px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_14px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaBoldOblique_14px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_17px_BasicLatin) {
	font = &font_Adobe_HelveticaBoldOblique_17px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_17px_BasicLatin;
	expectation_name = "Adobe_HelveticaBoldOblique_17px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_17px_Latin1Supplement) {
	font = &font_Adobe_HelveticaBoldOblique_17px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_17px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaBoldOblique_17px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_17px_LatinExtendedA) {
	font = &font_Adobe_HelveticaBoldOblique_17px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_17px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaBoldOblique_17px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_18px_BasicLatin) {
	font = &font_Adobe_HelveticaBoldOblique_18px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_18px_BasicLatin;
	expectation_name = "Adobe_HelveticaBoldOblique_18px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_18px_Latin1Supplement) {
	font = &font_Adobe_HelveticaBoldOblique_18px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_18px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaBoldOblique_18px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_18px_LatinExtendedA) {
	font = &font_Adobe_HelveticaBoldOblique_18px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_18px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaBoldOblique_18px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_20px_BasicLatin) {
	font = &font_Adobe_HelveticaBoldOblique_20px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_20px_BasicLatin;
	expectation_name = "Adobe_HelveticaBoldOblique_20px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_20px_Latin1Supplement) {
	font = &font_Adobe_HelveticaBoldOblique_20px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_20px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaBoldOblique_20px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_20px_LatinExtendedA) {
	font = &font_Adobe_HelveticaBoldOblique_20px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_20px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaBoldOblique_20px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_24px_BasicLatin) {
	font = &font_Adobe_HelveticaBoldOblique_24px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_24px_BasicLatin;
	expectation_name = "Adobe_HelveticaBoldOblique_24px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_24px_Latin1Supplement) {
	font = &font_Adobe_HelveticaBoldOblique_24px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_24px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaBoldOblique_24px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_24px_LatinExtendedA) {
	font = &font_Adobe_HelveticaBoldOblique_24px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_24px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaBoldOblique_24px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_25px_BasicLatin) {
	font = &font_Adobe_HelveticaBoldOblique_25px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_25px_BasicLatin;
	expectation_name = "Adobe_HelveticaBoldOblique_25px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_25px_Latin1Supplement) {
	font = &font_Adobe_HelveticaBoldOblique_25px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_25px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaBoldOblique_25px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_25px_LatinExtendedA) {
	font = &font_Adobe_HelveticaBoldOblique_25px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_25px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaBoldOblique_25px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_34px_BasicLatin) {
	font = &font_Adobe_HelveticaBoldOblique_34px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_34px_BasicLatin;
	expectation_name = "Adobe_HelveticaBoldOblique_34px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_34px_Latin1Supplement) {
	font = &font_Adobe_HelveticaBoldOblique_34px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_34px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaBoldOblique_34px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_34px_LatinExtendedA) {
	font = &font_Adobe_HelveticaBoldOblique_34px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_34px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaBoldOblique_34px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_8px_BasicLatin) {
	font = &font_Adobe_HelveticaBoldOblique_8px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_8px_BasicLatin;
	expectation_name = "Adobe_HelveticaBoldOblique_8px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_8px_Latin1Supplement) {
	font = &font_Adobe_HelveticaBoldOblique_8px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_8px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaBoldOblique_8px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaBoldOblique_8px_LatinExtendedA) {
	font = &font_Adobe_HelveticaBoldOblique_8px;
	unicode_block = &unicode_block_Adobe_HelveticaBoldOblique_8px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaBoldOblique_8px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaOblique_10px_BasicLatin) {
	font = &font_Adobe_HelveticaOblique_10px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_10px_BasicLatin;
	expectation_name = "Adobe_HelveticaOblique_10px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaOblique_10px_Latin1Supplement) {
	font = &font_Adobe_HelveticaOblique_10px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_10px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaOblique_10px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaOblique_10px_LatinExtendedA) {
	font = &font_Adobe_HelveticaOblique_10px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_10px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaOblique_10px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaOblique_11px_BasicLatin) {
	font = &font_Adobe_HelveticaOblique_11px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_11px_BasicLatin;
	expectation_name = "Adobe_HelveticaOblique_11px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaOblique_11px_Latin1Supplement) {
	font = &font_Adobe_HelveticaOblique_11px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_11px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaOblique_11px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaOblique_11px_LatinExtendedA) {
	font = &font_Adobe_HelveticaOblique_11px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_11px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaOblique_11px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaOblique_12px_BasicLatin) {
	font = &font_Adobe_HelveticaOblique_12px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_12px_BasicLatin;
	expectation_name = "Adobe_HelveticaOblique_12px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaOblique_12px_Latin1Supplement) {
	font = &font_Adobe_HelveticaOblique_12px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_12px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaOblique_12px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaOblique_12px_LatinExtendedA) {
	font = &font_Adobe_HelveticaOblique_12px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_12px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaOblique_12px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaOblique_14px_BasicLatin) {
	font = &font_Adobe_HelveticaOblique_14px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_14px_BasicLatin;
	expectation_name = "Adobe_HelveticaOblique_14px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaOblique_14px_Latin1Supplement) {
	font = &font_Adobe_HelveticaOblique_14px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_14px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaOblique_14px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaOblique_14px_LatinExtendedA) {
	font = &font_Adobe_HelveticaOblique_14px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_14px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaOblique_14px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaOblique_17px_BasicLatin) {
	font = &font_Adobe_HelveticaOblique_17px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_17px_BasicLatin;
	expectation_name = "Adobe_HelveticaOblique_17px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaOblique_17px_Latin1Supplement) {
	font = &font_Adobe_HelveticaOblique_17px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_17px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaOblique_17px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaOblique_17px_LatinExtendedA) {
	font = &font_Adobe_HelveticaOblique_17px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_17px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaOblique_17px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaOblique_18px_BasicLatin) {
	font = &font_Adobe_HelveticaOblique_18px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_18px_BasicLatin;
	expectation_name = "Adobe_HelveticaOblique_18px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaOblique_18px_Latin1Supplement) {
	font = &font_Adobe_HelveticaOblique_18px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_18px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaOblique_18px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaOblique_18px_LatinExtendedA) {
	font = &font_Adobe_HelveticaOblique_18px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_18px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaOblique_18px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaOblique_20px_BasicLatin) {
	font = &font_Adobe_HelveticaOblique_20px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_20px_BasicLatin;
	expectation_name = "Adobe_HelveticaOblique_20px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaOblique_20px_Latin1Supplement) {
	font = &font_Adobe_HelveticaOblique_20px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_20px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaOblique_20px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaOblique_20px_LatinExtendedA) {
	font = &font_Adobe_HelveticaOblique_20px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_20px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaOblique_20px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaOblique_24px_BasicLatin) {
	font = &font_Adobe_HelveticaOblique_24px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_24px_BasicLatin;
	expectation_name = "Adobe_HelveticaOblique_24px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaOblique_24px_Latin1Supplement) {
	font = &font_Adobe_HelveticaOblique_24px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_24px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaOblique_24px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaOblique_24px_LatinExtendedA) {
	font = &font_Adobe_HelveticaOblique_24px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_24px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaOblique_24px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaOblique_25px_BasicLatin) {
	font = &font_Adobe_HelveticaOblique_25px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_25px_BasicLatin;
	expectation_name = "Adobe_HelveticaOblique_25px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaOblique_25px_Latin1Supplement) {
	font = &font_Adobe_HelveticaOblique_25px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_25px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaOblique_25px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaOblique_25px_LatinExtendedA) {
	font = &font_Adobe_HelveticaOblique_25px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_25px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaOblique_25px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaOblique_34px_BasicLatin) {
	font = &font_Adobe_HelveticaOblique_34px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_34px_BasicLatin;
	expectation_name = "Adobe_HelveticaOblique_34px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaOblique_34px_Latin1Supplement) {
	font = &font_Adobe_HelveticaOblique_34px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_34px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaOblique_34px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaOblique_34px_LatinExtendedA) {
	font = &font_Adobe_HelveticaOblique_34px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_34px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaOblique_34px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_HelveticaOblique_8px_BasicLatin) {
	font = &font_Adobe_HelveticaOblique_8px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_8px_BasicLatin;
	expectation_name = "Adobe_HelveticaOblique_8px_BasicLatin";
}END_TEST

START_TEST(Adobe_HelveticaOblique_8px_Latin1Supplement) {
	font = &font_Adobe_HelveticaOblique_8px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_8px_Latin1Supplement;
	expectation_name = "Adobe_HelveticaOblique_8px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_HelveticaOblique_8px_LatinExtendedA) {
	font = &font_Adobe_HelveticaOblique_8px;
	unicode_block = &unicode_block_Adobe_HelveticaOblique_8px_LatinExtendedA;
	expectation_name = "Adobe_HelveticaOblique_8px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_10px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookBold_10px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_10px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookBold_10px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_10px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookBold_10px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_10px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookBold_10px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_10px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookBold_10px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_10px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookBold_10px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_11px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookBold_11px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_11px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookBold_11px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_11px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookBold_11px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_11px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookBold_11px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_11px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookBold_11px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_11px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookBold_11px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_14px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookBold_14px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_14px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookBold_14px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_14px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookBold_14px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_14px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookBold_14px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_14px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookBold_14px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_14px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookBold_14px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_17px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookBold_17px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_17px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookBold_17px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_17px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookBold_17px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_17px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookBold_17px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_17px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookBold_17px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_17px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookBold_17px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_18px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookBold_18px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_18px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookBold_18px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_18px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookBold_18px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_18px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookBold_18px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_18px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookBold_18px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_18px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookBold_18px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_20px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookBold_20px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_20px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookBold_20px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_20px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookBold_20px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_20px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookBold_20px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_20px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookBold_20px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_20px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookBold_20px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_24px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookBold_24px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_24px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookBold_24px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_24px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookBold_24px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_24px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookBold_24px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_24px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookBold_24px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_24px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookBold_24px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_25px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookBold_25px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_25px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookBold_25px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_25px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookBold_25px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_25px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookBold_25px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_25px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookBold_25px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_25px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookBold_25px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_34px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookBold_34px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_34px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookBold_34px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_34px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookBold_34px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_34px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookBold_34px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_34px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookBold_34px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_34px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookBold_34px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_8px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookBold_8px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_8px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookBold_8px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_8px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookBold_8px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_8px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookBold_8px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBold_8px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookBold_8px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBold_8px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookBold_8px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_10px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_10px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_10px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_10px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_10px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_10px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_10px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_10px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_10px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_10px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_10px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_10px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_11px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_11px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_11px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_11px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_11px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_11px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_11px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_11px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_11px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_11px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_11px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_11px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_12px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_12px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_12px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_12px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_12px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_12px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_12px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_12px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_12px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_12px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_12px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_12px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_14px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_14px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_14px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_14px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_14px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_14px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_14px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_14px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_14px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_14px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_14px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_14px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_17px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_17px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_17px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_17px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_17px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_17px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_17px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_17px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_17px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_17px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_17px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_17px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_18px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_18px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_18px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_18px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_18px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_18px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_18px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_18px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_18px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_18px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_18px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_18px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_20px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_20px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_20px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_20px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_20px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_20px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_20px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_20px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_20px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_20px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_20px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_20px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_24px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_24px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_24px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_24px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_24px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_24px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_24px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_24px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_24px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_24px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_24px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_24px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_25px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_25px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_25px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_25px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_25px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_25px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_25px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_25px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_25px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_25px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_25px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_25px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_34px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_34px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_34px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_34px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_34px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_34px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_34px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_34px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_34px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_34px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_34px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_34px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_8px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_8px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_8px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_8px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_8px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_8px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_8px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_8px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookBoldItalic_8px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookBoldItalic_8px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookBoldItalic_8px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookBoldItalic_8px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_10px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookItalic_10px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_10px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_10px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_10px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookItalic_10px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_10px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_10px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_10px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookItalic_10px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_10px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_10px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_11px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookItalic_11px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_11px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_11px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_11px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookItalic_11px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_11px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_11px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_11px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookItalic_11px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_11px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_11px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_12px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookItalic_12px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_12px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_12px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_12px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookItalic_12px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_12px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_12px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_12px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookItalic_12px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_12px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_12px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_14px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookItalic_14px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_14px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_14px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_14px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookItalic_14px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_14px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_14px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_14px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookItalic_14px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_14px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_14px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_17px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookItalic_17px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_17px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_17px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_17px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookItalic_17px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_17px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_17px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_17px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookItalic_17px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_17px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_17px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_18px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookItalic_18px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_18px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_18px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_18px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookItalic_18px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_18px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_18px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_18px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookItalic_18px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_18px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_18px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_20px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookItalic_20px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_20px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_20px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_20px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookItalic_20px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_20px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_20px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_20px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookItalic_20px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_20px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_20px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_24px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookItalic_24px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_24px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_24px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_24px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookItalic_24px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_24px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_24px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_24px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookItalic_24px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_24px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_24px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_25px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookItalic_25px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_25px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_25px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_25px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookItalic_25px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_25px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_25px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_25px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookItalic_25px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_25px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_25px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_34px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookItalic_34px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_34px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_34px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_34px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookItalic_34px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_34px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_34px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_34px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookItalic_34px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_34px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_34px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_8px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookItalic_8px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_8px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_8px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_8px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookItalic_8px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_8px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_8px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookItalic_8px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookItalic_8px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookItalic_8px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookItalic_8px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_10px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookRoman_10px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_10px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_10px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_10px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookRoman_10px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_10px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_10px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_10px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookRoman_10px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_10px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_10px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_11px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookRoman_11px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_11px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_11px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_11px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookRoman_11px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_11px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_11px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_11px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookRoman_11px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_11px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_11px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_14px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookRoman_14px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_14px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_14px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_14px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookRoman_14px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_14px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_14px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_14px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookRoman_14px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_14px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_14px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_17px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookRoman_17px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_17px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_17px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_17px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookRoman_17px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_17px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_17px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_17px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookRoman_17px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_17px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_17px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_18px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookRoman_18px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_18px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_18px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_18px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookRoman_18px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_18px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_18px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_18px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookRoman_18px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_18px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_18px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_20px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookRoman_20px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_20px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_20px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_20px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookRoman_20px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_20px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_20px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_20px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookRoman_20px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_20px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_20px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_24px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookRoman_24px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_24px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_24px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_24px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookRoman_24px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_24px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_24px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_24px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookRoman_24px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_24px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_24px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_25px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookRoman_25px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_25px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_25px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_25px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookRoman_25px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_25px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_25px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_25px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookRoman_25px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_25px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_25px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_34px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookRoman_34px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_34px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_34px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_34px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookRoman_34px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_34px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_34px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_34px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookRoman_34px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_34px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_34px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_8px_BasicLatin) {
	font = &font_Adobe_NewCenturySchoolbookRoman_8px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_8px_BasicLatin;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_8px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_8px_Latin1Supplement) {
	font = &font_Adobe_NewCenturySchoolbookRoman_8px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_8px_Latin1Supplement;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_8px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenturySchoolbookRoman_8px_LatinExtendedA) {
	font = &font_Adobe_NewCenturySchoolbookRoman_8px;
	unicode_block = &unicode_block_Adobe_NewCenturySchoolbookRoman_8px_LatinExtendedA;
	expectation_name = "Adobe_NewCenturySchoolbookRoman_8px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCentuySchoolbookBold_12px_BasicLatin) {
	font = &font_Adobe_NewCentuySchoolbookBold_12px;
	unicode_block = &unicode_block_Adobe_NewCentuySchoolbookBold_12px_BasicLatin;
	expectation_name = "Adobe_NewCentuySchoolbookBold_12px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCentuySchoolbookBold_12px_Latin1Supplement) {
	font = &font_Adobe_NewCentuySchoolbookBold_12px;
	unicode_block = &unicode_block_Adobe_NewCentuySchoolbookBold_12px_Latin1Supplement;
	expectation_name = "Adobe_NewCentuySchoolbookBold_12px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCentuySchoolbookBold_12px_LatinExtendedA) {
	font = &font_Adobe_NewCentuySchoolbookBold_12px;
	unicode_block = &unicode_block_Adobe_NewCentuySchoolbookBold_12px_LatinExtendedA;
	expectation_name = "Adobe_NewCentuySchoolbookBold_12px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_NewCenurySchoolbookRoman_12px_BasicLatin) {
	font = &font_Adobe_NewCenurySchoolbookRoman_12px;
	unicode_block = &unicode_block_Adobe_NewCenurySchoolbookRoman_12px_BasicLatin;
	expectation_name = "Adobe_NewCenurySchoolbookRoman_12px_BasicLatin";
}END_TEST

START_TEST(Adobe_NewCenurySchoolbookRoman_12px_Latin1Supplement) {
	font = &font_Adobe_NewCenurySchoolbookRoman_12px;
	unicode_block = &unicode_block_Adobe_NewCenurySchoolbookRoman_12px_Latin1Supplement;
	expectation_name = "Adobe_NewCenurySchoolbookRoman_12px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_NewCenurySchoolbookRoman_12px_LatinExtendedA) {
	font = &font_Adobe_NewCenurySchoolbookRoman_12px;
	unicode_block = &unicode_block_Adobe_NewCenurySchoolbookRoman_12px_LatinExtendedA;
	expectation_name = "Adobe_NewCenurySchoolbookRoman_12px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesBold_10px_BasicLatin) {
	font = &font_Adobe_TimesBold_10px;
	unicode_block = &unicode_block_Adobe_TimesBold_10px_BasicLatin;
	expectation_name = "Adobe_TimesBold_10px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesBold_10px_Latin1Supplement) {
	font = &font_Adobe_TimesBold_10px;
	unicode_block = &unicode_block_Adobe_TimesBold_10px_Latin1Supplement;
	expectation_name = "Adobe_TimesBold_10px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesBold_10px_LatinExtendedA) {
	font = &font_Adobe_TimesBold_10px;
	unicode_block = &unicode_block_Adobe_TimesBold_10px_LatinExtendedA;
	expectation_name = "Adobe_TimesBold_10px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesBold_11px_BasicLatin) {
	font = &font_Adobe_TimesBold_11px;
	unicode_block = &unicode_block_Adobe_TimesBold_11px_BasicLatin;
	expectation_name = "Adobe_TimesBold_11px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesBold_11px_Latin1Supplement) {
	font = &font_Adobe_TimesBold_11px;
	unicode_block = &unicode_block_Adobe_TimesBold_11px_Latin1Supplement;
	expectation_name = "Adobe_TimesBold_11px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesBold_11px_LatinExtendedA) {
	font = &font_Adobe_TimesBold_11px;
	unicode_block = &unicode_block_Adobe_TimesBold_11px_LatinExtendedA;
	expectation_name = "Adobe_TimesBold_11px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesBold_12px_BasicLatin) {
	font = &font_Adobe_TimesBold_12px;
	unicode_block = &unicode_block_Adobe_TimesBold_12px_BasicLatin;
	expectation_name = "Adobe_TimesBold_12px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesBold_12px_Latin1Supplement) {
	font = &font_Adobe_TimesBold_12px;
	unicode_block = &unicode_block_Adobe_TimesBold_12px_Latin1Supplement;
	expectation_name = "Adobe_TimesBold_12px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesBold_12px_LatinExtendedA) {
	font = &font_Adobe_TimesBold_12px;
	unicode_block = &unicode_block_Adobe_TimesBold_12px_LatinExtendedA;
	expectation_name = "Adobe_TimesBold_12px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesBold_14px_BasicLatin) {
	font = &font_Adobe_TimesBold_14px;
	unicode_block = &unicode_block_Adobe_TimesBold_14px_BasicLatin;
	expectation_name = "Adobe_TimesBold_14px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesBold_14px_Latin1Supplement) {
	font = &font_Adobe_TimesBold_14px;
	unicode_block = &unicode_block_Adobe_TimesBold_14px_Latin1Supplement;
	expectation_name = "Adobe_TimesBold_14px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesBold_14px_LatinExtendedA) {
	font = &font_Adobe_TimesBold_14px;
	unicode_block = &unicode_block_Adobe_TimesBold_14px_LatinExtendedA;
	expectation_name = "Adobe_TimesBold_14px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesBold_17px_BasicLatin) {
	font = &font_Adobe_TimesBold_17px;
	unicode_block = &unicode_block_Adobe_TimesBold_17px_BasicLatin;
	expectation_name = "Adobe_TimesBold_17px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesBold_17px_Latin1Supplement) {
	font = &font_Adobe_TimesBold_17px;
	unicode_block = &unicode_block_Adobe_TimesBold_17px_Latin1Supplement;
	expectation_name = "Adobe_TimesBold_17px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesBold_17px_LatinExtendedA) {
	font = &font_Adobe_TimesBold_17px;
	unicode_block = &unicode_block_Adobe_TimesBold_17px_LatinExtendedA;
	expectation_name = "Adobe_TimesBold_17px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesBold_18px_BasicLatin) {
	font = &font_Adobe_TimesBold_18px;
	unicode_block = &unicode_block_Adobe_TimesBold_18px_BasicLatin;
	expectation_name = "Adobe_TimesBold_18px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesBold_18px_Latin1Supplement) {
	font = &font_Adobe_TimesBold_18px;
	unicode_block = &unicode_block_Adobe_TimesBold_18px_Latin1Supplement;
	expectation_name = "Adobe_TimesBold_18px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesBold_18px_LatinExtendedA) {
	font = &font_Adobe_TimesBold_18px;
	unicode_block = &unicode_block_Adobe_TimesBold_18px_LatinExtendedA;
	expectation_name = "Adobe_TimesBold_18px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesBold_20px_BasicLatin) {
	font = &font_Adobe_TimesBold_20px;
	unicode_block = &unicode_block_Adobe_TimesBold_20px_BasicLatin;
	expectation_name = "Adobe_TimesBold_20px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesBold_20px_Latin1Supplement) {
	font = &font_Adobe_TimesBold_20px;
	unicode_block = &unicode_block_Adobe_TimesBold_20px_Latin1Supplement;
	expectation_name = "Adobe_TimesBold_20px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesBold_20px_LatinExtendedA) {
	font = &font_Adobe_TimesBold_20px;
	unicode_block = &unicode_block_Adobe_TimesBold_20px_LatinExtendedA;
	expectation_name = "Adobe_TimesBold_20px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesBold_24px_BasicLatin) {
	font = &font_Adobe_TimesBold_24px;
	unicode_block = &unicode_block_Adobe_TimesBold_24px_BasicLatin;
	expectation_name = "Adobe_TimesBold_24px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesBold_24px_Latin1Supplement) {
	font = &font_Adobe_TimesBold_24px;
	unicode_block = &unicode_block_Adobe_TimesBold_24px_Latin1Supplement;
	expectation_name = "Adobe_TimesBold_24px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesBold_24px_LatinExtendedA) {
	font = &font_Adobe_TimesBold_24px;
	unicode_block = &unicode_block_Adobe_TimesBold_24px_LatinExtendedA;
	expectation_name = "Adobe_TimesBold_24px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesBold_25px_BasicLatin) {
	font = &font_Adobe_TimesBold_25px;
	unicode_block = &unicode_block_Adobe_TimesBold_25px_BasicLatin;
	expectation_name = "Adobe_TimesBold_25px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesBold_25px_Latin1Supplement) {
	font = &font_Adobe_TimesBold_25px;
	unicode_block = &unicode_block_Adobe_TimesBold_25px_Latin1Supplement;
	expectation_name = "Adobe_TimesBold_25px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesBold_25px_LatinExtendedA) {
	font = &font_Adobe_TimesBold_25px;
	unicode_block = &unicode_block_Adobe_TimesBold_25px_LatinExtendedA;
	expectation_name = "Adobe_TimesBold_25px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesBold_34px_BasicLatin) {
	font = &font_Adobe_TimesBold_34px;
	unicode_block = &unicode_block_Adobe_TimesBold_34px_BasicLatin;
	expectation_name = "Adobe_TimesBold_34px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesBold_34px_Latin1Supplement) {
	font = &font_Adobe_TimesBold_34px;
	unicode_block = &unicode_block_Adobe_TimesBold_34px_Latin1Supplement;
	expectation_name = "Adobe_TimesBold_34px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesBold_34px_LatinExtendedA) {
	font = &font_Adobe_TimesBold_34px;
	unicode_block = &unicode_block_Adobe_TimesBold_34px_LatinExtendedA;
	expectation_name = "Adobe_TimesBold_34px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesBold_8px_BasicLatin) {
	font = &font_Adobe_TimesBold_8px;
	unicode_block = &unicode_block_Adobe_TimesBold_8px_BasicLatin;
	expectation_name = "Adobe_TimesBold_8px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesBold_8px_Latin1Supplement) {
	font = &font_Adobe_TimesBold_8px;
	unicode_block = &unicode_block_Adobe_TimesBold_8px_Latin1Supplement;
	expectation_name = "Adobe_TimesBold_8px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesBold_8px_LatinExtendedA) {
	font = &font_Adobe_TimesBold_8px;
	unicode_block = &unicode_block_Adobe_TimesBold_8px_LatinExtendedA;
	expectation_name = "Adobe_TimesBold_8px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_10px_BasicLatin) {
	font = &font_Adobe_TimesBoldItalic_10px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_10px_BasicLatin;
	expectation_name = "Adobe_TimesBoldItalic_10px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_10px_Latin1Supplement) {
	font = &font_Adobe_TimesBoldItalic_10px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_10px_Latin1Supplement;
	expectation_name = "Adobe_TimesBoldItalic_10px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_10px_LatinExtendedA) {
	font = &font_Adobe_TimesBoldItalic_10px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_10px_LatinExtendedA;
	expectation_name = "Adobe_TimesBoldItalic_10px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_11px_BasicLatin) {
	font = &font_Adobe_TimesBoldItalic_11px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_11px_BasicLatin;
	expectation_name = "Adobe_TimesBoldItalic_11px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_11px_Latin1Supplement) {
	font = &font_Adobe_TimesBoldItalic_11px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_11px_Latin1Supplement;
	expectation_name = "Adobe_TimesBoldItalic_11px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_11px_LatinExtendedA) {
	font = &font_Adobe_TimesBoldItalic_11px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_11px_LatinExtendedA;
	expectation_name = "Adobe_TimesBoldItalic_11px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_12px_BasicLatin) {
	font = &font_Adobe_TimesBoldItalic_12px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_12px_BasicLatin;
	expectation_name = "Adobe_TimesBoldItalic_12px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_12px_Latin1Supplement) {
	font = &font_Adobe_TimesBoldItalic_12px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_12px_Latin1Supplement;
	expectation_name = "Adobe_TimesBoldItalic_12px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_12px_LatinExtendedA) {
	font = &font_Adobe_TimesBoldItalic_12px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_12px_LatinExtendedA;
	expectation_name = "Adobe_TimesBoldItalic_12px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_14px_BasicLatin) {
	font = &font_Adobe_TimesBoldItalic_14px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_14px_BasicLatin;
	expectation_name = "Adobe_TimesBoldItalic_14px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_14px_Latin1Supplement) {
	font = &font_Adobe_TimesBoldItalic_14px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_14px_Latin1Supplement;
	expectation_name = "Adobe_TimesBoldItalic_14px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_14px_LatinExtendedA) {
	font = &font_Adobe_TimesBoldItalic_14px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_14px_LatinExtendedA;
	expectation_name = "Adobe_TimesBoldItalic_14px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_17px_BasicLatin) {
	font = &font_Adobe_TimesBoldItalic_17px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_17px_BasicLatin;
	expectation_name = "Adobe_TimesBoldItalic_17px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_17px_Latin1Supplement) {
	font = &font_Adobe_TimesBoldItalic_17px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_17px_Latin1Supplement;
	expectation_name = "Adobe_TimesBoldItalic_17px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_17px_LatinExtendedA) {
	font = &font_Adobe_TimesBoldItalic_17px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_17px_LatinExtendedA;
	expectation_name = "Adobe_TimesBoldItalic_17px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_18px_BasicLatin) {
	font = &font_Adobe_TimesBoldItalic_18px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_18px_BasicLatin;
	expectation_name = "Adobe_TimesBoldItalic_18px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_18px_Latin1Supplement) {
	font = &font_Adobe_TimesBoldItalic_18px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_18px_Latin1Supplement;
	expectation_name = "Adobe_TimesBoldItalic_18px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_18px_LatinExtendedA) {
	font = &font_Adobe_TimesBoldItalic_18px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_18px_LatinExtendedA;
	expectation_name = "Adobe_TimesBoldItalic_18px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_20px_BasicLatin) {
	font = &font_Adobe_TimesBoldItalic_20px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_20px_BasicLatin;
	expectation_name = "Adobe_TimesBoldItalic_20px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_20px_Latin1Supplement) {
	font = &font_Adobe_TimesBoldItalic_20px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_20px_Latin1Supplement;
	expectation_name = "Adobe_TimesBoldItalic_20px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_20px_LatinExtendedA) {
	font = &font_Adobe_TimesBoldItalic_20px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_20px_LatinExtendedA;
	expectation_name = "Adobe_TimesBoldItalic_20px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_24px_BasicLatin) {
	font = &font_Adobe_TimesBoldItalic_24px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_24px_BasicLatin;
	expectation_name = "Adobe_TimesBoldItalic_24px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_24px_Latin1Supplement) {
	font = &font_Adobe_TimesBoldItalic_24px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_24px_Latin1Supplement;
	expectation_name = "Adobe_TimesBoldItalic_24px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_24px_LatinExtendedA) {
	font = &font_Adobe_TimesBoldItalic_24px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_24px_LatinExtendedA;
	expectation_name = "Adobe_TimesBoldItalic_24px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_25px_BasicLatin) {
	font = &font_Adobe_TimesBoldItalic_25px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_25px_BasicLatin;
	expectation_name = "Adobe_TimesBoldItalic_25px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_25px_Latin1Supplement) {
	font = &font_Adobe_TimesBoldItalic_25px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_25px_Latin1Supplement;
	expectation_name = "Adobe_TimesBoldItalic_25px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_25px_LatinExtendedA) {
	font = &font_Adobe_TimesBoldItalic_25px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_25px_LatinExtendedA;
	expectation_name = "Adobe_TimesBoldItalic_25px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_34px_BasicLatin) {
	font = &font_Adobe_TimesBoldItalic_34px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_34px_BasicLatin;
	expectation_name = "Adobe_TimesBoldItalic_34px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_34px_Latin1Supplement) {
	font = &font_Adobe_TimesBoldItalic_34px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_34px_Latin1Supplement;
	expectation_name = "Adobe_TimesBoldItalic_34px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_34px_LatinExtendedA) {
	font = &font_Adobe_TimesBoldItalic_34px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_34px_LatinExtendedA;
	expectation_name = "Adobe_TimesBoldItalic_34px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_8px_BasicLatin) {
	font = &font_Adobe_TimesBoldItalic_8px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_8px_BasicLatin;
	expectation_name = "Adobe_TimesBoldItalic_8px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_8px_Latin1Supplement) {
	font = &font_Adobe_TimesBoldItalic_8px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_8px_Latin1Supplement;
	expectation_name = "Adobe_TimesBoldItalic_8px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesBoldItalic_8px_LatinExtendedA) {
	font = &font_Adobe_TimesBoldItalic_8px;
	unicode_block = &unicode_block_Adobe_TimesBoldItalic_8px_LatinExtendedA;
	expectation_name = "Adobe_TimesBoldItalic_8px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesItalic_10px_BasicLatin) {
	font = &font_Adobe_TimesItalic_10px;
	unicode_block = &unicode_block_Adobe_TimesItalic_10px_BasicLatin;
	expectation_name = "Adobe_TimesItalic_10px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesItalic_10px_Latin1Supplement) {
	font = &font_Adobe_TimesItalic_10px;
	unicode_block = &unicode_block_Adobe_TimesItalic_10px_Latin1Supplement;
	expectation_name = "Adobe_TimesItalic_10px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesItalic_10px_LatinExtendedA) {
	font = &font_Adobe_TimesItalic_10px;
	unicode_block = &unicode_block_Adobe_TimesItalic_10px_LatinExtendedA;
	expectation_name = "Adobe_TimesItalic_10px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesItalic_11px_BasicLatin) {
	font = &font_Adobe_TimesItalic_11px;
	unicode_block = &unicode_block_Adobe_TimesItalic_11px_BasicLatin;
	expectation_name = "Adobe_TimesItalic_11px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesItalic_11px_Latin1Supplement) {
	font = &font_Adobe_TimesItalic_11px;
	unicode_block = &unicode_block_Adobe_TimesItalic_11px_Latin1Supplement;
	expectation_name = "Adobe_TimesItalic_11px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesItalic_11px_LatinExtendedA) {
	font = &font_Adobe_TimesItalic_11px;
	unicode_block = &unicode_block_Adobe_TimesItalic_11px_LatinExtendedA;
	expectation_name = "Adobe_TimesItalic_11px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesItalic_12px_BasicLatin) {
	font = &font_Adobe_TimesItalic_12px;
	unicode_block = &unicode_block_Adobe_TimesItalic_12px_BasicLatin;
	expectation_name = "Adobe_TimesItalic_12px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesItalic_12px_Latin1Supplement) {
	font = &font_Adobe_TimesItalic_12px;
	unicode_block = &unicode_block_Adobe_TimesItalic_12px_Latin1Supplement;
	expectation_name = "Adobe_TimesItalic_12px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesItalic_12px_LatinExtendedA) {
	font = &font_Adobe_TimesItalic_12px;
	unicode_block = &unicode_block_Adobe_TimesItalic_12px_LatinExtendedA;
	expectation_name = "Adobe_TimesItalic_12px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesItalic_14px_BasicLatin) {
	font = &font_Adobe_TimesItalic_14px;
	unicode_block = &unicode_block_Adobe_TimesItalic_14px_BasicLatin;
	expectation_name = "Adobe_TimesItalic_14px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesItalic_14px_Latin1Supplement) {
	font = &font_Adobe_TimesItalic_14px;
	unicode_block = &unicode_block_Adobe_TimesItalic_14px_Latin1Supplement;
	expectation_name = "Adobe_TimesItalic_14px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesItalic_14px_LatinExtendedA) {
	font = &font_Adobe_TimesItalic_14px;
	unicode_block = &unicode_block_Adobe_TimesItalic_14px_LatinExtendedA;
	expectation_name = "Adobe_TimesItalic_14px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesItalic_17px_BasicLatin) {
	font = &font_Adobe_TimesItalic_17px;
	unicode_block = &unicode_block_Adobe_TimesItalic_17px_BasicLatin;
	expectation_name = "Adobe_TimesItalic_17px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesItalic_17px_Latin1Supplement) {
	font = &font_Adobe_TimesItalic_17px;
	unicode_block = &unicode_block_Adobe_TimesItalic_17px_Latin1Supplement;
	expectation_name = "Adobe_TimesItalic_17px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesItalic_17px_LatinExtendedA) {
	font = &font_Adobe_TimesItalic_17px;
	unicode_block = &unicode_block_Adobe_TimesItalic_17px_LatinExtendedA;
	expectation_name = "Adobe_TimesItalic_17px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesItalic_18px_BasicLatin) {
	font = &font_Adobe_TimesItalic_18px;
	unicode_block = &unicode_block_Adobe_TimesItalic_18px_BasicLatin;
	expectation_name = "Adobe_TimesItalic_18px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesItalic_18px_Latin1Supplement) {
	font = &font_Adobe_TimesItalic_18px;
	unicode_block = &unicode_block_Adobe_TimesItalic_18px_Latin1Supplement;
	expectation_name = "Adobe_TimesItalic_18px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesItalic_18px_LatinExtendedA) {
	font = &font_Adobe_TimesItalic_18px;
	unicode_block = &unicode_block_Adobe_TimesItalic_18px_LatinExtendedA;
	expectation_name = "Adobe_TimesItalic_18px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesItalic_20px_BasicLatin) {
	font = &font_Adobe_TimesItalic_20px;
	unicode_block = &unicode_block_Adobe_TimesItalic_20px_BasicLatin;
	expectation_name = "Adobe_TimesItalic_20px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesItalic_20px_Latin1Supplement) {
	font = &font_Adobe_TimesItalic_20px;
	unicode_block = &unicode_block_Adobe_TimesItalic_20px_Latin1Supplement;
	expectation_name = "Adobe_TimesItalic_20px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesItalic_20px_LatinExtendedA) {
	font = &font_Adobe_TimesItalic_20px;
	unicode_block = &unicode_block_Adobe_TimesItalic_20px_LatinExtendedA;
	expectation_name = "Adobe_TimesItalic_20px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesItalic_24px_BasicLatin) {
	font = &font_Adobe_TimesItalic_24px;
	unicode_block = &unicode_block_Adobe_TimesItalic_24px_BasicLatin;
	expectation_name = "Adobe_TimesItalic_24px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesItalic_24px_Latin1Supplement) {
	font = &font_Adobe_TimesItalic_24px;
	unicode_block = &unicode_block_Adobe_TimesItalic_24px_Latin1Supplement;
	expectation_name = "Adobe_TimesItalic_24px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesItalic_24px_LatinExtendedA) {
	font = &font_Adobe_TimesItalic_24px;
	unicode_block = &unicode_block_Adobe_TimesItalic_24px_LatinExtendedA;
	expectation_name = "Adobe_TimesItalic_24px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesItalic_25px_BasicLatin) {
	font = &font_Adobe_TimesItalic_25px;
	unicode_block = &unicode_block_Adobe_TimesItalic_25px_BasicLatin;
	expectation_name = "Adobe_TimesItalic_25px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesItalic_25px_Latin1Supplement) {
	font = &font_Adobe_TimesItalic_25px;
	unicode_block = &unicode_block_Adobe_TimesItalic_25px_Latin1Supplement;
	expectation_name = "Adobe_TimesItalic_25px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesItalic_25px_LatinExtendedA) {
	font = &font_Adobe_TimesItalic_25px;
	unicode_block = &unicode_block_Adobe_TimesItalic_25px_LatinExtendedA;
	expectation_name = "Adobe_TimesItalic_25px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesItalic_34px_BasicLatin) {
	font = &font_Adobe_TimesItalic_34px;
	unicode_block = &unicode_block_Adobe_TimesItalic_34px_BasicLatin;
	expectation_name = "Adobe_TimesItalic_34px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesItalic_34px_Latin1Supplement) {
	font = &font_Adobe_TimesItalic_34px;
	unicode_block = &unicode_block_Adobe_TimesItalic_34px_Latin1Supplement;
	expectation_name = "Adobe_TimesItalic_34px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesItalic_34px_LatinExtendedA) {
	font = &font_Adobe_TimesItalic_34px;
	unicode_block = &unicode_block_Adobe_TimesItalic_34px_LatinExtendedA;
	expectation_name = "Adobe_TimesItalic_34px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesItalic_8px_BasicLatin) {
	font = &font_Adobe_TimesItalic_8px;
	unicode_block = &unicode_block_Adobe_TimesItalic_8px_BasicLatin;
	expectation_name = "Adobe_TimesItalic_8px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesItalic_8px_Latin1Supplement) {
	font = &font_Adobe_TimesItalic_8px;
	unicode_block = &unicode_block_Adobe_TimesItalic_8px_Latin1Supplement;
	expectation_name = "Adobe_TimesItalic_8px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesItalic_8px_LatinExtendedA) {
	font = &font_Adobe_TimesItalic_8px;
	unicode_block = &unicode_block_Adobe_TimesItalic_8px_LatinExtendedA;
	expectation_name = "Adobe_TimesItalic_8px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesRoman_10px_BasicLatin) {
	font = &font_Adobe_TimesRoman_10px;
	unicode_block = &unicode_block_Adobe_TimesRoman_10px_BasicLatin;
	expectation_name = "Adobe_TimesRoman_10px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesRoman_10px_Latin1Supplement) {
	font = &font_Adobe_TimesRoman_10px;
	unicode_block = &unicode_block_Adobe_TimesRoman_10px_Latin1Supplement;
	expectation_name = "Adobe_TimesRoman_10px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesRoman_10px_LatinExtendedA) {
	font = &font_Adobe_TimesRoman_10px;
	unicode_block = &unicode_block_Adobe_TimesRoman_10px_LatinExtendedA;
	expectation_name = "Adobe_TimesRoman_10px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesRoman_11px_BasicLatin) {
	font = &font_Adobe_TimesRoman_11px;
	unicode_block = &unicode_block_Adobe_TimesRoman_11px_BasicLatin;
	expectation_name = "Adobe_TimesRoman_11px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesRoman_11px_Latin1Supplement) {
	font = &font_Adobe_TimesRoman_11px;
	unicode_block = &unicode_block_Adobe_TimesRoman_11px_Latin1Supplement;
	expectation_name = "Adobe_TimesRoman_11px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesRoman_11px_LatinExtendedA) {
	font = &font_Adobe_TimesRoman_11px;
	unicode_block = &unicode_block_Adobe_TimesRoman_11px_LatinExtendedA;
	expectation_name = "Adobe_TimesRoman_11px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesRoman_12px_BasicLatin) {
	font = &font_Adobe_TimesRoman_12px;
	unicode_block = &unicode_block_Adobe_TimesRoman_12px_BasicLatin;
	expectation_name = "Adobe_TimesRoman_12px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesRoman_12px_Latin1Supplement) {
	font = &font_Adobe_TimesRoman_12px;
	unicode_block = &unicode_block_Adobe_TimesRoman_12px_Latin1Supplement;
	expectation_name = "Adobe_TimesRoman_12px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesRoman_12px_LatinExtendedA) {
	font = &font_Adobe_TimesRoman_12px;
	unicode_block = &unicode_block_Adobe_TimesRoman_12px_LatinExtendedA;
	expectation_name = "Adobe_TimesRoman_12px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesRoman_14px_BasicLatin) {
	font = &font_Adobe_TimesRoman_14px;
	unicode_block = &unicode_block_Adobe_TimesRoman_14px_BasicLatin;
	expectation_name = "Adobe_TimesRoman_14px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesRoman_14px_Latin1Supplement) {
	font = &font_Adobe_TimesRoman_14px;
	unicode_block = &unicode_block_Adobe_TimesRoman_14px_Latin1Supplement;
	expectation_name = "Adobe_TimesRoman_14px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesRoman_14px_LatinExtendedA) {
	font = &font_Adobe_TimesRoman_14px;
	unicode_block = &unicode_block_Adobe_TimesRoman_14px_LatinExtendedA;
	expectation_name = "Adobe_TimesRoman_14px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesRoman_17px_BasicLatin) {
	font = &font_Adobe_TimesRoman_17px;
	unicode_block = &unicode_block_Adobe_TimesRoman_17px_BasicLatin;
	expectation_name = "Adobe_TimesRoman_17px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesRoman_17px_Latin1Supplement) {
	font = &font_Adobe_TimesRoman_17px;
	unicode_block = &unicode_block_Adobe_TimesRoman_17px_Latin1Supplement;
	expectation_name = "Adobe_TimesRoman_17px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesRoman_17px_LatinExtendedA) {
	font = &font_Adobe_TimesRoman_17px;
	unicode_block = &unicode_block_Adobe_TimesRoman_17px_LatinExtendedA;
	expectation_name = "Adobe_TimesRoman_17px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesRoman_18px_BasicLatin) {
	font = &font_Adobe_TimesRoman_18px;
	unicode_block = &unicode_block_Adobe_TimesRoman_18px_BasicLatin;
	expectation_name = "Adobe_TimesRoman_18px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesRoman_18px_Latin1Supplement) {
	font = &font_Adobe_TimesRoman_18px;
	unicode_block = &unicode_block_Adobe_TimesRoman_18px_Latin1Supplement;
	expectation_name = "Adobe_TimesRoman_18px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesRoman_18px_LatinExtendedA) {
	font = &font_Adobe_TimesRoman_18px;
	unicode_block = &unicode_block_Adobe_TimesRoman_18px_LatinExtendedA;
	expectation_name = "Adobe_TimesRoman_18px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesRoman_20px_BasicLatin) {
	font = &font_Adobe_TimesRoman_20px;
	unicode_block = &unicode_block_Adobe_TimesRoman_20px_BasicLatin;
	expectation_name = "Adobe_TimesRoman_20px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesRoman_20px_Latin1Supplement) {
	font = &font_Adobe_TimesRoman_20px;
	unicode_block = &unicode_block_Adobe_TimesRoman_20px_Latin1Supplement;
	expectation_name = "Adobe_TimesRoman_20px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesRoman_20px_LatinExtendedA) {
	font = &font_Adobe_TimesRoman_20px;
	unicode_block = &unicode_block_Adobe_TimesRoman_20px_LatinExtendedA;
	expectation_name = "Adobe_TimesRoman_20px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesRoman_24px_BasicLatin) {
	font = &font_Adobe_TimesRoman_24px;
	unicode_block = &unicode_block_Adobe_TimesRoman_24px_BasicLatin;
	expectation_name = "Adobe_TimesRoman_24px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesRoman_24px_Latin1Supplement) {
	font = &font_Adobe_TimesRoman_24px;
	unicode_block = &unicode_block_Adobe_TimesRoman_24px_Latin1Supplement;
	expectation_name = "Adobe_TimesRoman_24px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesRoman_24px_LatinExtendedA) {
	font = &font_Adobe_TimesRoman_24px;
	unicode_block = &unicode_block_Adobe_TimesRoman_24px_LatinExtendedA;
	expectation_name = "Adobe_TimesRoman_24px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesRoman_25px_BasicLatin) {
	font = &font_Adobe_TimesRoman_25px;
	unicode_block = &unicode_block_Adobe_TimesRoman_25px_BasicLatin;
	expectation_name = "Adobe_TimesRoman_25px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesRoman_25px_Latin1Supplement) {
	font = &font_Adobe_TimesRoman_25px;
	unicode_block = &unicode_block_Adobe_TimesRoman_25px_Latin1Supplement;
	expectation_name = "Adobe_TimesRoman_25px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesRoman_25px_LatinExtendedA) {
	font = &font_Adobe_TimesRoman_25px;
	unicode_block = &unicode_block_Adobe_TimesRoman_25px_LatinExtendedA;
	expectation_name = "Adobe_TimesRoman_25px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesRoman_34px_BasicLatin) {
	font = &font_Adobe_TimesRoman_34px;
	unicode_block = &unicode_block_Adobe_TimesRoman_34px_BasicLatin;
	expectation_name = "Adobe_TimesRoman_34px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesRoman_34px_Latin1Supplement) {
	font = &font_Adobe_TimesRoman_34px;
	unicode_block = &unicode_block_Adobe_TimesRoman_34px_Latin1Supplement;
	expectation_name = "Adobe_TimesRoman_34px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesRoman_34px_LatinExtendedA) {
	font = &font_Adobe_TimesRoman_34px;
	unicode_block = &unicode_block_Adobe_TimesRoman_34px_LatinExtendedA;
	expectation_name = "Adobe_TimesRoman_34px_LatinExtendedA";
}END_TEST

START_TEST(Adobe_TimesRoman_8px_BasicLatin) {
	font = &font_Adobe_TimesRoman_8px;
	unicode_block = &unicode_block_Adobe_TimesRoman_8px_BasicLatin;
	expectation_name = "Adobe_TimesRoman_8px_BasicLatin";
}END_TEST

START_TEST(Adobe_TimesRoman_8px_Latin1Supplement) {
	font = &font_Adobe_TimesRoman_8px;
	unicode_block = &unicode_block_Adobe_TimesRoman_8px_Latin1Supplement;
	expectation_name = "Adobe_TimesRoman_8px_Latin1Supplement";
}END_TEST

START_TEST(Adobe_TimesRoman_8px_LatinExtendedA) {
	font = &font_Adobe_TimesRoman_8px;
	unicode_block = &unicode_block_Adobe_TimesRoman_8px_LatinExtendedA;
	expectation_name = "Adobe_TimesRoman_8px_LatinExtendedA";
}END_TEST

Suite * build_suite(void) {
	Suite *suite;
	TCase *tcase;

	suite = suite_create("adobe");

	tcase = tcase_create("adobe");
	tcase_add_checked_fixture(tcase, setup, teardown);
	tcase_add_test(tcase, Adobe_Courier_10px_BasicLatin);
	tcase_add_test(tcase, Adobe_Courier_10px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_Courier_10px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_Courier_11px_BasicLatin);
	tcase_add_test(tcase, Adobe_Courier_11px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_Courier_11px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_Courier_12px_BasicLatin);
	tcase_add_test(tcase, Adobe_Courier_12px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_Courier_12px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_Courier_14px_BasicLatin);
	tcase_add_test(tcase, Adobe_Courier_14px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_Courier_14px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_Courier_17px_BasicLatin);
	tcase_add_test(tcase, Adobe_Courier_17px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_Courier_17px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_Courier_18px_BasicLatin);
	tcase_add_test(tcase, Adobe_Courier_18px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_Courier_18px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_Courier_20px_BasicLatin);
	tcase_add_test(tcase, Adobe_Courier_20px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_Courier_20px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_Courier_24px_BasicLatin);
	tcase_add_test(tcase, Adobe_Courier_24px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_Courier_24px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_Courier_25px_BasicLatin);
	tcase_add_test(tcase, Adobe_Courier_25px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_Courier_25px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_Courier_34px_BasicLatin);
	tcase_add_test(tcase, Adobe_Courier_34px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_Courier_34px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_Courier_8px_BasicLatin);
	tcase_add_test(tcase, Adobe_Courier_8px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_Courier_8px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierBold_10px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierBold_10px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierBold_10px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierBold_11px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierBold_11px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierBold_11px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierBold_12px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierBold_12px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierBold_12px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierBold_14px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierBold_14px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierBold_14px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierBold_17px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierBold_17px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierBold_17px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierBold_18px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierBold_18px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierBold_18px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierBold_20px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierBold_20px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierBold_20px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierBold_24px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierBold_24px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierBold_24px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierBold_25px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierBold_25px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierBold_25px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierBold_34px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierBold_34px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierBold_34px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierBold_8px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierBold_8px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierBold_8px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_10px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_10px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_10px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_11px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_11px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_11px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_12px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_12px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_12px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_14px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_14px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_14px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_17px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_17px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_17px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_18px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_18px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_18px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_20px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_20px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_20px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_24px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_24px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_24px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_25px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_25px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_25px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_34px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_34px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_34px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_8px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_8px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierBoldOblique_8px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierOblique_10px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierOblique_10px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierOblique_10px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierOblique_11px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierOblique_11px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierOblique_11px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierOblique_12px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierOblique_12px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierOblique_12px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierOblique_14px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierOblique_14px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierOblique_14px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierOblique_17px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierOblique_17px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierOblique_17px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierOblique_18px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierOblique_18px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierOblique_18px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierOblique_20px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierOblique_20px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierOblique_20px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierOblique_24px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierOblique_24px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierOblique_24px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierOblique_25px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierOblique_25px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierOblique_25px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierOblique_34px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierOblique_34px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierOblique_34px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_CourierOblique_8px_BasicLatin);
	tcase_add_test(tcase, Adobe_CourierOblique_8px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_CourierOblique_8px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_Helvetica_10px_BasicLatin);
	tcase_add_test(tcase, Adobe_Helvetica_10px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_Helvetica_10px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_Helvetica_11px_BasicLatin);
	tcase_add_test(tcase, Adobe_Helvetica_11px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_Helvetica_11px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_Helvetica_12px_BasicLatin);
	tcase_add_test(tcase, Adobe_Helvetica_12px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_Helvetica_12px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_Helvetica_14px_BasicLatin);
	tcase_add_test(tcase, Adobe_Helvetica_14px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_Helvetica_14px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_Helvetica_17px_BasicLatin);
	tcase_add_test(tcase, Adobe_Helvetica_17px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_Helvetica_17px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_Helvetica_18px_BasicLatin);
	tcase_add_test(tcase, Adobe_Helvetica_18px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_Helvetica_18px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_Helvetica_20px_BasicLatin);
	tcase_add_test(tcase, Adobe_Helvetica_20px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_Helvetica_20px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_Helvetica_24px_BasicLatin);
	tcase_add_test(tcase, Adobe_Helvetica_24px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_Helvetica_24px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_Helvetica_25px_BasicLatin);
	tcase_add_test(tcase, Adobe_Helvetica_25px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_Helvetica_25px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_Helvetica_34px_BasicLatin);
	tcase_add_test(tcase, Adobe_Helvetica_34px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_Helvetica_34px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_Helvetica_8px_BasicLatin);
	tcase_add_test(tcase, Adobe_Helvetica_8px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_Helvetica_8px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaBold_10px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaBold_10px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaBold_10px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaBold_11px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaBold_11px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaBold_11px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaBold_12px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaBold_12px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaBold_12px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaBold_14px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaBold_14px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaBold_14px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaBold_17px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaBold_17px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaBold_17px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaBold_18px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaBold_18px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaBold_18px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaBold_20px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaBold_20px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaBold_20px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaBold_24px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaBold_24px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaBold_24px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaBold_25px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaBold_25px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaBold_25px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaBold_34px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaBold_34px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaBold_34px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaBold_8px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaBold_8px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaBold_8px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_10px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_10px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_10px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_11px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_11px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_11px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_12px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_12px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_12px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_14px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_14px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_14px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_17px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_17px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_17px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_18px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_18px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_18px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_20px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_20px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_20px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_24px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_24px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_24px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_25px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_25px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_25px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_34px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_34px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_34px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_8px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_8px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaBoldOblique_8px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaOblique_10px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaOblique_10px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaOblique_10px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaOblique_11px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaOblique_11px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaOblique_11px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaOblique_12px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaOblique_12px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaOblique_12px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaOblique_14px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaOblique_14px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaOblique_14px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaOblique_17px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaOblique_17px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaOblique_17px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaOblique_18px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaOblique_18px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaOblique_18px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaOblique_20px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaOblique_20px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaOblique_20px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaOblique_24px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaOblique_24px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaOblique_24px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaOblique_25px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaOblique_25px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaOblique_25px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaOblique_34px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaOblique_34px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaOblique_34px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_HelveticaOblique_8px_BasicLatin);
	tcase_add_test(tcase, Adobe_HelveticaOblique_8px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_HelveticaOblique_8px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_10px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_10px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_10px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_11px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_11px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_11px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_14px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_14px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_14px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_17px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_17px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_17px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_18px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_18px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_18px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_20px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_20px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_20px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_24px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_24px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_24px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_25px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_25px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_25px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_34px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_34px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_34px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_8px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_8px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBold_8px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_10px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_10px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_10px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_11px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_11px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_11px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_12px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_12px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_12px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_14px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_14px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_14px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_17px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_17px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_17px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_18px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_18px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_18px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_20px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_20px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_20px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_24px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_24px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_24px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_25px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_25px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_25px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_34px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_34px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_34px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_8px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_8px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookBoldItalic_8px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_10px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_10px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_10px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_11px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_11px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_11px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_12px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_12px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_12px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_14px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_14px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_14px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_17px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_17px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_17px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_18px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_18px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_18px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_20px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_20px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_20px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_24px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_24px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_24px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_25px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_25px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_25px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_34px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_34px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_34px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_8px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_8px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookItalic_8px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_10px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_10px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_10px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_11px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_11px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_11px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_14px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_14px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_14px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_17px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_17px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_17px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_18px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_18px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_18px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_20px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_20px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_20px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_24px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_24px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_24px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_25px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_25px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_25px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_34px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_34px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_34px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_8px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_8px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenturySchoolbookRoman_8px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCentuySchoolbookBold_12px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCentuySchoolbookBold_12px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCentuySchoolbookBold_12px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_NewCenurySchoolbookRoman_12px_BasicLatin);
	tcase_add_test(tcase, Adobe_NewCenurySchoolbookRoman_12px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_NewCenurySchoolbookRoman_12px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesBold_10px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesBold_10px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesBold_10px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesBold_11px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesBold_11px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesBold_11px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesBold_12px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesBold_12px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesBold_12px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesBold_14px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesBold_14px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesBold_14px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesBold_17px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesBold_17px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesBold_17px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesBold_18px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesBold_18px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesBold_18px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesBold_20px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesBold_20px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesBold_20px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesBold_24px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesBold_24px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesBold_24px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesBold_25px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesBold_25px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesBold_25px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesBold_34px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesBold_34px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesBold_34px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesBold_8px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesBold_8px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesBold_8px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_10px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_10px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_10px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_11px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_11px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_11px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_12px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_12px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_12px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_14px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_14px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_14px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_17px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_17px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_17px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_18px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_18px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_18px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_20px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_20px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_20px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_24px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_24px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_24px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_25px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_25px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_25px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_34px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_34px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_34px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_8px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_8px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesBoldItalic_8px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesItalic_10px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesItalic_10px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesItalic_10px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesItalic_11px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesItalic_11px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesItalic_11px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesItalic_12px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesItalic_12px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesItalic_12px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesItalic_14px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesItalic_14px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesItalic_14px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesItalic_17px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesItalic_17px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesItalic_17px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesItalic_18px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesItalic_18px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesItalic_18px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesItalic_20px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesItalic_20px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesItalic_20px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesItalic_24px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesItalic_24px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesItalic_24px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesItalic_25px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesItalic_25px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesItalic_25px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesItalic_34px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesItalic_34px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesItalic_34px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesItalic_8px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesItalic_8px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesItalic_8px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesRoman_10px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesRoman_10px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesRoman_10px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesRoman_11px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesRoman_11px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesRoman_11px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesRoman_12px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesRoman_12px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesRoman_12px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesRoman_14px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesRoman_14px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesRoman_14px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesRoman_17px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesRoman_17px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesRoman_17px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesRoman_18px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesRoman_18px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesRoman_18px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesRoman_20px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesRoman_20px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesRoman_20px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesRoman_24px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesRoman_24px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesRoman_24px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesRoman_25px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesRoman_25px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesRoman_25px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesRoman_34px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesRoman_34px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesRoman_34px_LatinExtendedA);
	tcase_add_test(tcase, Adobe_TimesRoman_8px_BasicLatin);
	tcase_add_test(tcase, Adobe_TimesRoman_8px_Latin1Supplement);
	tcase_add_test(tcase, Adobe_TimesRoman_8px_LatinExtendedA);

	suite_add_tcase(suite, tcase);

	return suite;
}