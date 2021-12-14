#include "../unicode_block.h"
#include <check.h>
#include <eglib/display.h>

struct font_t *font;
const struct glyph_unicode_block_t *unicode_block;
char *expectation_name;
char *reldir = RELDIR;

Suite * build_suite(void);

START_TEST(FreeFont_FreeMono_7px_BasicLatin) {
	font = &font_FreeFont_FreeMono_7px;
	unicode_block = &unicode_block_FreeFont_FreeMono_7px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_7px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_7px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_7px;
	unicode_block = &unicode_block_FreeFont_FreeMono_7px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_7px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_7px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_7px;
	unicode_block = &unicode_block_FreeFont_FreeMono_7px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_7px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_7px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_7px;
	unicode_block = &unicode_block_FreeFont_FreeMono_7px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_7px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMono_8px_BasicLatin) {
	font = &font_FreeFont_FreeMono_8px;
	unicode_block = &unicode_block_FreeFont_FreeMono_8px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_8px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_8px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_8px;
	unicode_block = &unicode_block_FreeFont_FreeMono_8px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_8px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_8px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_8px;
	unicode_block = &unicode_block_FreeFont_FreeMono_8px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_8px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_8px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_8px;
	unicode_block = &unicode_block_FreeFont_FreeMono_8px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_8px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMono_9px_BasicLatin) {
	font = &font_FreeFont_FreeMono_9px;
	unicode_block = &unicode_block_FreeFont_FreeMono_9px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_9px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_9px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_9px;
	unicode_block = &unicode_block_FreeFont_FreeMono_9px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_9px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_9px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_9px;
	unicode_block = &unicode_block_FreeFont_FreeMono_9px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_9px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_9px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_9px;
	unicode_block = &unicode_block_FreeFont_FreeMono_9px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_9px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMono_10px_BasicLatin) {
	font = &font_FreeFont_FreeMono_10px;
	unicode_block = &unicode_block_FreeFont_FreeMono_10px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_10px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_10px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_10px;
	unicode_block = &unicode_block_FreeFont_FreeMono_10px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_10px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_10px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_10px;
	unicode_block = &unicode_block_FreeFont_FreeMono_10px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_10px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_10px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_10px;
	unicode_block = &unicode_block_FreeFont_FreeMono_10px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_10px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMono_11px_BasicLatin) {
	font = &font_FreeFont_FreeMono_11px;
	unicode_block = &unicode_block_FreeFont_FreeMono_11px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_11px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_11px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_11px;
	unicode_block = &unicode_block_FreeFont_FreeMono_11px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_11px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_11px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_11px;
	unicode_block = &unicode_block_FreeFont_FreeMono_11px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_11px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_11px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_11px;
	unicode_block = &unicode_block_FreeFont_FreeMono_11px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_11px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMono_12px_BasicLatin) {
	font = &font_FreeFont_FreeMono_12px;
	unicode_block = &unicode_block_FreeFont_FreeMono_12px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_12px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_12px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_12px;
	unicode_block = &unicode_block_FreeFont_FreeMono_12px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_12px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_12px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_12px;
	unicode_block = &unicode_block_FreeFont_FreeMono_12px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_12px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_12px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_12px;
	unicode_block = &unicode_block_FreeFont_FreeMono_12px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_12px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMono_13px_BasicLatin) {
	font = &font_FreeFont_FreeMono_13px;
	unicode_block = &unicode_block_FreeFont_FreeMono_13px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_13px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_13px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_13px;
	unicode_block = &unicode_block_FreeFont_FreeMono_13px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_13px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_13px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_13px;
	unicode_block = &unicode_block_FreeFont_FreeMono_13px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_13px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_13px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_13px;
	unicode_block = &unicode_block_FreeFont_FreeMono_13px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_13px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMono_14px_BasicLatin) {
	font = &font_FreeFont_FreeMono_14px;
	unicode_block = &unicode_block_FreeFont_FreeMono_14px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_14px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_14px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_14px;
	unicode_block = &unicode_block_FreeFont_FreeMono_14px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_14px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_14px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_14px;
	unicode_block = &unicode_block_FreeFont_FreeMono_14px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_14px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_14px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_14px;
	unicode_block = &unicode_block_FreeFont_FreeMono_14px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_14px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMono_15px_BasicLatin) {
	font = &font_FreeFont_FreeMono_15px;
	unicode_block = &unicode_block_FreeFont_FreeMono_15px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_15px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_15px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_15px;
	unicode_block = &unicode_block_FreeFont_FreeMono_15px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_15px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_15px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_15px;
	unicode_block = &unicode_block_FreeFont_FreeMono_15px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_15px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_15px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_15px;
	unicode_block = &unicode_block_FreeFont_FreeMono_15px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_15px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMono_16px_BasicLatin) {
	font = &font_FreeFont_FreeMono_16px;
	unicode_block = &unicode_block_FreeFont_FreeMono_16px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_16px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_16px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_16px;
	unicode_block = &unicode_block_FreeFont_FreeMono_16px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_16px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_16px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_16px;
	unicode_block = &unicode_block_FreeFont_FreeMono_16px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_16px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_16px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_16px;
	unicode_block = &unicode_block_FreeFont_FreeMono_16px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_16px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMono_18px_BasicLatin) {
	font = &font_FreeFont_FreeMono_18px;
	unicode_block = &unicode_block_FreeFont_FreeMono_18px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_18px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_18px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_18px;
	unicode_block = &unicode_block_FreeFont_FreeMono_18px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_18px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_18px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_18px;
	unicode_block = &unicode_block_FreeFont_FreeMono_18px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_18px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_18px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_18px;
	unicode_block = &unicode_block_FreeFont_FreeMono_18px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_18px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMono_20px_BasicLatin) {
	font = &font_FreeFont_FreeMono_20px;
	unicode_block = &unicode_block_FreeFont_FreeMono_20px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_20px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_20px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_20px;
	unicode_block = &unicode_block_FreeFont_FreeMono_20px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_20px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_20px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_20px;
	unicode_block = &unicode_block_FreeFont_FreeMono_20px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_20px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_20px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_20px;
	unicode_block = &unicode_block_FreeFont_FreeMono_20px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_20px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMono_22px_BasicLatin) {
	font = &font_FreeFont_FreeMono_22px;
	unicode_block = &unicode_block_FreeFont_FreeMono_22px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_22px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_22px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_22px;
	unicode_block = &unicode_block_FreeFont_FreeMono_22px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_22px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_22px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_22px;
	unicode_block = &unicode_block_FreeFont_FreeMono_22px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_22px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_22px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_22px;
	unicode_block = &unicode_block_FreeFont_FreeMono_22px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_22px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMono_24px_BasicLatin) {
	font = &font_FreeFont_FreeMono_24px;
	unicode_block = &unicode_block_FreeFont_FreeMono_24px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_24px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_24px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_24px;
	unicode_block = &unicode_block_FreeFont_FreeMono_24px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_24px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_24px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_24px;
	unicode_block = &unicode_block_FreeFont_FreeMono_24px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_24px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_24px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_24px;
	unicode_block = &unicode_block_FreeFont_FreeMono_24px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_24px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMono_26px_BasicLatin) {
	font = &font_FreeFont_FreeMono_26px;
	unicode_block = &unicode_block_FreeFont_FreeMono_26px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_26px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_26px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_26px;
	unicode_block = &unicode_block_FreeFont_FreeMono_26px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_26px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_26px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_26px;
	unicode_block = &unicode_block_FreeFont_FreeMono_26px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_26px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_26px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_26px;
	unicode_block = &unicode_block_FreeFont_FreeMono_26px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_26px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMono_28px_BasicLatin) {
	font = &font_FreeFont_FreeMono_28px;
	unicode_block = &unicode_block_FreeFont_FreeMono_28px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_28px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_28px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_28px;
	unicode_block = &unicode_block_FreeFont_FreeMono_28px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_28px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_28px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_28px;
	unicode_block = &unicode_block_FreeFont_FreeMono_28px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_28px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_28px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_28px;
	unicode_block = &unicode_block_FreeFont_FreeMono_28px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_28px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMono_32px_BasicLatin) {
	font = &font_FreeFont_FreeMono_32px;
	unicode_block = &unicode_block_FreeFont_FreeMono_32px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_32px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_32px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_32px;
	unicode_block = &unicode_block_FreeFont_FreeMono_32px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_32px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_32px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_32px;
	unicode_block = &unicode_block_FreeFont_FreeMono_32px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_32px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_32px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_32px;
	unicode_block = &unicode_block_FreeFont_FreeMono_32px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_32px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMono_36px_BasicLatin) {
	font = &font_FreeFont_FreeMono_36px;
	unicode_block = &unicode_block_FreeFont_FreeMono_36px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_36px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_36px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_36px;
	unicode_block = &unicode_block_FreeFont_FreeMono_36px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_36px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_36px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_36px;
	unicode_block = &unicode_block_FreeFont_FreeMono_36px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_36px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_36px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_36px;
	unicode_block = &unicode_block_FreeFont_FreeMono_36px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_36px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMono_40px_BasicLatin) {
	font = &font_FreeFont_FreeMono_40px;
	unicode_block = &unicode_block_FreeFont_FreeMono_40px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_40px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_40px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_40px;
	unicode_block = &unicode_block_FreeFont_FreeMono_40px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_40px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_40px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_40px;
	unicode_block = &unicode_block_FreeFont_FreeMono_40px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_40px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_40px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_40px;
	unicode_block = &unicode_block_FreeFont_FreeMono_40px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_40px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMono_44px_BasicLatin) {
	font = &font_FreeFont_FreeMono_44px;
	unicode_block = &unicode_block_FreeFont_FreeMono_44px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_44px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_44px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_44px;
	unicode_block = &unicode_block_FreeFont_FreeMono_44px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_44px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_44px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_44px;
	unicode_block = &unicode_block_FreeFont_FreeMono_44px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_44px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_44px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_44px;
	unicode_block = &unicode_block_FreeFont_FreeMono_44px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_44px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMono_48px_BasicLatin) {
	font = &font_FreeFont_FreeMono_48px;
	unicode_block = &unicode_block_FreeFont_FreeMono_48px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_48px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_48px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_48px;
	unicode_block = &unicode_block_FreeFont_FreeMono_48px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_48px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_48px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_48px;
	unicode_block = &unicode_block_FreeFont_FreeMono_48px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_48px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_48px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_48px;
	unicode_block = &unicode_block_FreeFont_FreeMono_48px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_48px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMono_54px_BasicLatin) {
	font = &font_FreeFont_FreeMono_54px;
	unicode_block = &unicode_block_FreeFont_FreeMono_54px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_54px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_54px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_54px;
	unicode_block = &unicode_block_FreeFont_FreeMono_54px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_54px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_54px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_54px;
	unicode_block = &unicode_block_FreeFont_FreeMono_54px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_54px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_54px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_54px;
	unicode_block = &unicode_block_FreeFont_FreeMono_54px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_54px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMono_60px_BasicLatin) {
	font = &font_FreeFont_FreeMono_60px;
	unicode_block = &unicode_block_FreeFont_FreeMono_60px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_60px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_60px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_60px;
	unicode_block = &unicode_block_FreeFont_FreeMono_60px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_60px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_60px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_60px;
	unicode_block = &unicode_block_FreeFont_FreeMono_60px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_60px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_60px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_60px;
	unicode_block = &unicode_block_FreeFont_FreeMono_60px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_60px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMono_66px_BasicLatin) {
	font = &font_FreeFont_FreeMono_66px;
	unicode_block = &unicode_block_FreeFont_FreeMono_66px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_66px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_66px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_66px;
	unicode_block = &unicode_block_FreeFont_FreeMono_66px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_66px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_66px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_66px;
	unicode_block = &unicode_block_FreeFont_FreeMono_66px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_66px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_66px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_66px;
	unicode_block = &unicode_block_FreeFont_FreeMono_66px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_66px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMono_72px_BasicLatin) {
	font = &font_FreeFont_FreeMono_72px;
	unicode_block = &unicode_block_FreeFont_FreeMono_72px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_72px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_72px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_72px;
	unicode_block = &unicode_block_FreeFont_FreeMono_72px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_72px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_72px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_72px;
	unicode_block = &unicode_block_FreeFont_FreeMono_72px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_72px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_72px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_72px;
	unicode_block = &unicode_block_FreeFont_FreeMono_72px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_72px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMono_80px_BasicLatin) {
	font = &font_FreeFont_FreeMono_80px;
	unicode_block = &unicode_block_FreeFont_FreeMono_80px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_80px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_80px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_80px;
	unicode_block = &unicode_block_FreeFont_FreeMono_80px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_80px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_80px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_80px;
	unicode_block = &unicode_block_FreeFont_FreeMono_80px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_80px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_80px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_80px;
	unicode_block = &unicode_block_FreeFont_FreeMono_80px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_80px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMono_83px_BasicLatin) {
	font = &font_FreeFont_FreeMono_83px;
	unicode_block = &unicode_block_FreeFont_FreeMono_83px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_83px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_83px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_83px;
	unicode_block = &unicode_block_FreeFont_FreeMono_83px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_83px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_83px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_83px;
	unicode_block = &unicode_block_FreeFont_FreeMono_83px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_83px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_83px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_83px;
	unicode_block = &unicode_block_FreeFont_FreeMono_83px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_83px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMono_88px_BasicLatin) {
	font = &font_FreeFont_FreeMono_88px;
	unicode_block = &unicode_block_FreeFont_FreeMono_88px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_88px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_88px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_88px;
	unicode_block = &unicode_block_FreeFont_FreeMono_88px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_88px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_88px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_88px;
	unicode_block = &unicode_block_FreeFont_FreeMono_88px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_88px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_88px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_88px;
	unicode_block = &unicode_block_FreeFont_FreeMono_88px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_88px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMono_96px_BasicLatin) {
	font = &font_FreeFont_FreeMono_96px;
	unicode_block = &unicode_block_FreeFont_FreeMono_96px_BasicLatin;
	expectation_name = "FreeFont_FreeMono_96px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMono_96px_Latin1Supplement) {
	font = &font_FreeFont_FreeMono_96px;
	unicode_block = &unicode_block_FreeFont_FreeMono_96px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMono_96px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMono_96px_LatinExtendedA) {
	font = &font_FreeFont_FreeMono_96px;
	unicode_block = &unicode_block_FreeFont_FreeMono_96px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMono_96px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMono_96px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMono_96px;
	unicode_block = &unicode_block_FreeFont_FreeMono_96px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMono_96px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_7px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_7px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_7px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_7px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_7px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_7px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_7px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_7px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_7px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_7px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_7px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_7px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_7px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_7px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_7px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_7px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_8px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_8px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_8px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_8px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_8px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_8px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_8px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_8px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_8px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_8px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_8px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_8px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_8px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_8px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_8px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_8px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_9px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_9px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_9px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_9px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_9px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_9px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_9px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_9px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_9px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_9px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_9px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_9px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_9px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_9px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_9px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_9px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_10px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_10px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_10px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_10px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_10px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_10px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_10px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_10px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_10px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_10px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_10px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_10px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_10px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_10px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_10px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_10px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_11px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_11px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_11px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_11px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_11px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_11px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_11px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_11px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_11px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_11px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_11px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_11px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_11px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_11px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_11px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_11px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_12px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_12px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_12px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_12px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_12px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_12px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_12px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_12px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_12px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_12px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_12px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_12px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_12px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_12px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_12px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_12px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_13px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_13px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_13px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_13px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_13px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_13px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_13px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_13px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_13px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_13px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_13px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_13px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_13px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_13px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_13px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_13px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_14px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_14px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_14px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_14px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_14px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_14px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_14px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_14px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_14px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_14px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_14px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_14px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_14px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_14px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_14px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_14px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_15px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_15px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_15px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_15px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_15px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_15px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_15px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_15px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_15px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_15px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_15px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_15px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_15px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_15px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_15px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_15px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_16px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_16px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_16px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_16px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_16px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_16px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_16px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_16px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_16px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_16px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_16px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_16px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_16px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_16px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_16px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_16px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_18px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_18px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_18px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_18px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_18px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_18px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_18px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_18px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_18px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_18px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_18px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_18px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_18px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_18px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_18px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_18px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_20px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_20px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_20px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_20px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_20px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_20px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_20px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_20px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_20px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_20px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_20px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_20px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_20px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_20px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_20px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_20px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_22px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_22px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_22px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_22px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_22px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_22px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_22px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_22px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_22px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_22px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_22px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_22px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_22px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_22px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_22px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_22px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_24px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_24px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_24px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_24px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_24px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_24px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_24px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_24px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_24px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_24px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_24px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_24px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_24px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_24px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_24px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_24px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_26px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_26px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_26px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_26px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_26px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_26px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_26px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_26px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_26px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_26px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_26px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_26px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_26px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_26px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_26px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_26px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_28px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_28px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_28px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_28px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_28px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_28px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_28px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_28px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_28px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_28px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_28px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_28px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_28px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_28px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_28px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_28px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_32px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_32px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_32px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_32px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_32px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_32px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_32px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_32px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_32px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_32px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_32px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_32px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_32px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_32px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_32px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_32px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_36px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_36px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_36px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_36px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_36px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_36px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_36px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_36px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_36px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_36px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_36px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_36px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_36px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_36px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_36px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_36px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_40px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_40px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_40px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_40px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_40px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_40px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_40px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_40px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_40px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_40px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_40px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_40px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_40px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_40px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_40px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_40px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_44px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_44px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_44px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_44px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_44px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_44px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_44px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_44px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_44px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_44px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_44px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_44px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_44px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_44px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_44px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_44px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_48px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_48px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_48px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_48px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_48px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_48px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_48px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_48px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_48px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_48px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_48px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_48px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_48px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_48px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_48px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_48px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_54px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_54px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_54px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_54px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_54px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_54px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_54px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_54px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_54px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_54px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_54px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_54px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_54px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_54px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_54px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_54px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_60px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_60px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_60px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_60px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_60px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_60px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_60px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_60px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_60px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_60px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_60px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_60px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_60px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_60px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_60px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_60px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_66px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_66px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_66px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_66px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_66px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_66px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_66px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_66px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_66px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_66px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_66px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_66px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_66px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_66px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_66px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_66px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_72px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_72px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_72px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_72px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_72px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_72px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_72px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_72px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_72px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_72px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_72px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_72px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_72px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_72px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_72px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_72px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_80px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_80px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_80px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_80px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_80px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_80px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_80px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_80px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_80px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_80px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_80px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_80px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_80px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_80px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_80px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_80px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_83px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_83px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_83px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_83px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_83px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_83px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_83px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_83px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_83px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_83px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_83px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_83px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_83px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_83px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_83px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_83px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_88px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_88px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_88px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_88px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_88px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_88px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_88px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_88px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_88px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_88px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_88px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_88px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_88px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_88px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_88px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_88px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_96px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBold_96px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_96px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBold_96px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_96px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBold_96px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_96px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBold_96px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_96px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBold_96px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_96px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBold_96px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBold_96px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBold_96px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBold_96px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBold_96px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_7px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_7px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_7px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_7px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_7px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_7px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_7px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_7px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_7px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_7px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_7px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_7px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_7px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_7px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_7px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_7px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_8px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_8px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_8px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_8px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_8px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_8px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_8px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_8px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_8px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_8px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_8px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_8px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_8px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_8px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_8px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_8px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_9px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_9px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_9px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_9px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_9px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_9px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_9px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_9px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_9px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_9px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_9px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_9px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_9px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_9px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_9px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_9px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_10px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_10px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_10px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_10px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_10px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_10px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_10px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_10px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_10px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_10px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_10px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_10px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_10px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_10px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_10px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_10px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_11px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_11px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_11px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_11px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_11px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_11px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_11px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_11px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_11px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_11px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_11px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_11px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_11px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_11px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_11px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_11px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_12px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_12px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_12px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_12px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_12px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_12px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_12px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_12px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_12px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_12px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_12px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_12px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_12px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_12px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_12px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_12px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_13px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_13px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_13px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_13px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_13px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_13px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_13px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_13px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_13px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_13px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_13px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_13px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_13px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_13px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_13px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_13px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_14px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_14px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_14px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_14px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_14px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_14px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_14px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_14px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_14px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_14px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_14px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_14px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_14px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_14px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_14px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_14px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_15px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_15px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_15px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_15px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_15px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_15px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_15px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_15px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_15px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_15px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_15px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_15px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_15px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_15px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_15px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_15px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_16px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_16px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_16px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_16px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_16px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_16px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_16px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_16px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_16px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_16px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_16px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_16px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_16px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_16px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_16px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_16px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_18px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_18px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_18px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_18px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_18px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_18px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_18px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_18px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_18px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_18px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_18px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_18px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_18px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_18px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_18px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_18px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_20px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_20px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_20px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_20px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_20px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_20px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_20px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_20px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_20px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_20px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_20px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_20px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_20px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_20px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_20px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_20px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_22px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_22px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_22px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_22px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_22px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_22px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_22px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_22px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_22px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_22px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_22px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_22px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_22px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_22px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_22px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_22px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_24px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_24px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_24px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_24px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_24px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_24px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_24px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_24px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_24px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_24px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_24px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_24px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_24px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_24px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_24px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_24px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_26px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_26px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_26px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_26px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_26px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_26px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_26px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_26px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_26px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_26px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_26px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_26px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_26px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_26px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_26px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_26px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_28px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_28px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_28px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_28px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_28px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_28px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_28px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_28px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_28px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_28px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_28px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_28px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_28px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_28px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_28px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_28px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_32px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_32px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_32px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_32px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_32px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_32px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_32px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_32px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_32px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_32px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_32px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_32px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_32px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_32px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_32px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_32px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_36px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_36px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_36px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_36px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_36px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_36px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_36px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_36px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_36px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_36px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_36px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_36px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_36px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_36px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_36px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_36px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_40px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_40px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_40px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_40px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_40px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_40px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_40px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_40px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_40px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_40px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_40px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_40px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_40px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_40px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_40px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_40px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_44px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_44px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_44px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_44px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_44px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_44px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_44px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_44px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_44px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_44px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_44px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_44px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_44px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_44px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_44px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_44px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_48px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_48px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_48px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_48px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_48px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_48px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_48px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_48px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_48px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_48px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_48px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_48px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_48px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_48px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_48px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_48px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_54px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_54px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_54px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_54px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_54px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_54px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_54px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_54px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_54px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_54px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_54px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_54px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_54px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_54px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_54px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_54px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_60px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_60px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_60px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_60px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_60px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_60px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_60px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_60px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_60px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_60px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_60px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_60px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_60px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_60px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_60px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_60px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_66px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_66px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_66px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_66px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_66px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_66px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_66px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_66px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_66px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_66px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_66px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_66px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_66px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_66px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_66px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_66px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_72px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_72px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_72px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_72px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_72px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_72px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_72px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_72px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_72px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_72px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_72px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_72px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_72px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_72px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_72px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_72px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_80px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_80px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_80px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_80px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_80px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_80px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_80px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_80px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_80px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_80px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_80px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_80px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_80px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_80px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_80px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_80px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_83px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_83px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_83px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_83px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_83px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_83px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_83px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_83px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_83px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_83px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_83px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_83px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_83px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_83px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_83px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_83px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_88px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_88px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_88px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_88px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_88px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_88px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_88px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_88px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_88px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_88px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_88px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_88px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_88px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_88px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_88px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_88px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_96px_BasicLatin) {
	font = &font_FreeFont_FreeMonoBoldOblique_96px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_96px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoBoldOblique_96px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_96px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoBoldOblique_96px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_96px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoBoldOblique_96px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_96px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoBoldOblique_96px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_96px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoBoldOblique_96px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoBoldOblique_96px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoBoldOblique_96px;
	unicode_block = &unicode_block_FreeFont_FreeMonoBoldOblique_96px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoBoldOblique_96px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_7px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_7px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_7px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_7px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_7px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_7px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_7px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_7px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_7px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_7px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_7px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_7px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_7px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_7px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_7px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_7px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_8px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_8px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_8px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_8px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_8px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_8px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_8px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_8px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_8px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_8px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_8px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_8px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_8px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_8px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_8px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_8px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_9px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_9px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_9px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_9px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_9px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_9px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_9px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_9px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_9px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_9px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_9px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_9px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_9px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_9px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_9px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_9px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_10px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_10px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_10px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_10px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_10px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_10px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_10px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_10px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_10px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_10px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_10px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_10px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_10px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_10px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_10px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_10px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_11px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_11px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_11px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_11px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_11px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_11px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_11px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_11px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_11px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_11px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_11px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_11px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_11px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_11px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_11px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_11px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_12px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_12px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_12px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_12px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_12px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_12px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_12px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_12px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_12px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_12px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_12px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_12px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_12px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_12px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_12px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_12px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_13px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_13px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_13px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_13px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_13px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_13px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_13px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_13px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_13px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_13px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_13px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_13px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_13px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_13px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_13px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_13px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_14px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_14px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_14px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_14px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_14px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_14px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_14px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_14px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_14px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_14px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_14px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_14px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_14px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_14px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_14px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_14px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_15px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_15px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_15px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_15px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_15px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_15px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_15px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_15px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_15px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_15px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_15px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_15px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_15px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_15px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_15px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_15px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_16px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_16px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_16px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_16px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_16px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_16px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_16px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_16px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_16px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_16px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_16px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_16px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_16px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_16px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_16px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_16px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_18px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_18px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_18px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_18px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_18px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_18px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_18px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_18px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_18px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_18px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_18px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_18px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_18px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_18px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_18px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_18px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_20px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_20px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_20px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_20px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_20px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_20px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_20px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_20px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_20px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_20px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_20px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_20px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_20px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_20px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_20px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_20px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_22px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_22px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_22px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_22px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_22px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_22px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_22px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_22px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_22px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_22px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_22px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_22px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_22px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_22px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_22px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_22px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_24px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_24px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_24px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_24px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_24px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_24px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_24px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_24px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_24px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_24px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_24px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_24px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_24px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_24px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_24px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_24px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_26px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_26px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_26px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_26px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_26px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_26px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_26px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_26px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_26px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_26px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_26px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_26px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_26px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_26px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_26px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_26px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_28px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_28px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_28px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_28px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_28px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_28px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_28px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_28px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_28px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_28px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_28px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_28px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_28px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_28px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_28px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_28px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_32px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_32px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_32px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_32px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_32px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_32px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_32px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_32px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_32px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_32px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_32px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_32px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_32px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_32px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_32px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_32px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_36px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_36px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_36px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_36px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_36px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_36px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_36px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_36px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_36px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_36px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_36px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_36px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_36px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_36px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_36px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_36px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_40px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_40px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_40px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_40px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_40px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_40px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_40px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_40px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_40px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_40px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_40px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_40px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_40px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_40px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_40px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_40px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_44px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_44px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_44px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_44px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_44px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_44px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_44px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_44px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_44px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_44px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_44px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_44px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_44px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_44px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_44px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_44px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_48px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_48px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_48px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_48px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_48px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_48px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_48px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_48px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_48px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_48px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_48px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_48px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_48px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_48px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_48px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_48px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_54px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_54px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_54px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_54px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_54px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_54px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_54px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_54px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_54px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_54px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_54px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_54px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_54px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_54px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_54px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_54px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_60px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_60px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_60px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_60px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_60px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_60px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_60px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_60px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_60px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_60px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_60px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_60px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_60px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_60px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_60px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_60px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_66px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_66px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_66px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_66px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_66px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_66px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_66px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_66px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_66px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_66px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_66px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_66px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_66px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_66px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_66px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_66px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_72px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_72px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_72px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_72px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_72px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_72px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_72px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_72px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_72px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_72px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_72px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_72px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_72px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_72px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_72px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_72px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_80px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_80px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_80px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_80px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_80px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_80px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_80px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_80px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_80px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_80px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_80px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_80px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_80px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_80px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_80px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_80px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_83px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_83px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_83px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_83px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_83px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_83px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_83px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_83px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_83px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_83px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_83px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_83px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_83px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_83px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_83px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_83px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_88px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_88px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_88px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_88px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_88px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_88px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_88px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_88px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_88px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_88px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_88px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_88px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_88px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_88px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_88px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_88px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_96px_BasicLatin) {
	font = &font_FreeFont_FreeMonoOblique_96px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_96px_BasicLatin;
	expectation_name = "FreeFont_FreeMonoOblique_96px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_96px_Latin1Supplement) {
	font = &font_FreeFont_FreeMonoOblique_96px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_96px_Latin1Supplement;
	expectation_name = "FreeFont_FreeMonoOblique_96px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_96px_LatinExtendedA) {
	font = &font_FreeFont_FreeMonoOblique_96px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_96px_LatinExtendedA;
	expectation_name = "FreeFont_FreeMonoOblique_96px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeMonoOblique_96px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeMonoOblique_96px;
	unicode_block = &unicode_block_FreeFont_FreeMonoOblique_96px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeMonoOblique_96px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_7px_BasicLatin) {
	font = &font_FreeFont_FreeSans_7px;
	unicode_block = &unicode_block_FreeFont_FreeSans_7px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_7px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_7px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_7px;
	unicode_block = &unicode_block_FreeFont_FreeSans_7px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_7px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_7px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_7px;
	unicode_block = &unicode_block_FreeFont_FreeSans_7px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_7px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_7px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_7px;
	unicode_block = &unicode_block_FreeFont_FreeSans_7px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_7px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_8px_BasicLatin) {
	font = &font_FreeFont_FreeSans_8px;
	unicode_block = &unicode_block_FreeFont_FreeSans_8px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_8px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_8px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_8px;
	unicode_block = &unicode_block_FreeFont_FreeSans_8px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_8px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_8px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_8px;
	unicode_block = &unicode_block_FreeFont_FreeSans_8px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_8px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_8px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_8px;
	unicode_block = &unicode_block_FreeFont_FreeSans_8px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_8px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_9px_BasicLatin) {
	font = &font_FreeFont_FreeSans_9px;
	unicode_block = &unicode_block_FreeFont_FreeSans_9px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_9px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_9px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_9px;
	unicode_block = &unicode_block_FreeFont_FreeSans_9px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_9px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_9px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_9px;
	unicode_block = &unicode_block_FreeFont_FreeSans_9px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_9px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_9px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_9px;
	unicode_block = &unicode_block_FreeFont_FreeSans_9px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_9px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_10px_BasicLatin) {
	font = &font_FreeFont_FreeSans_10px;
	unicode_block = &unicode_block_FreeFont_FreeSans_10px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_10px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_10px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_10px;
	unicode_block = &unicode_block_FreeFont_FreeSans_10px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_10px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_10px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_10px;
	unicode_block = &unicode_block_FreeFont_FreeSans_10px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_10px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_10px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_10px;
	unicode_block = &unicode_block_FreeFont_FreeSans_10px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_10px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_11px_BasicLatin) {
	font = &font_FreeFont_FreeSans_11px;
	unicode_block = &unicode_block_FreeFont_FreeSans_11px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_11px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_11px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_11px;
	unicode_block = &unicode_block_FreeFont_FreeSans_11px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_11px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_11px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_11px;
	unicode_block = &unicode_block_FreeFont_FreeSans_11px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_11px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_11px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_11px;
	unicode_block = &unicode_block_FreeFont_FreeSans_11px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_11px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_12px_BasicLatin) {
	font = &font_FreeFont_FreeSans_12px;
	unicode_block = &unicode_block_FreeFont_FreeSans_12px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_12px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_12px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_12px;
	unicode_block = &unicode_block_FreeFont_FreeSans_12px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_12px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_12px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_12px;
	unicode_block = &unicode_block_FreeFont_FreeSans_12px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_12px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_12px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_12px;
	unicode_block = &unicode_block_FreeFont_FreeSans_12px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_12px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_13px_BasicLatin) {
	font = &font_FreeFont_FreeSans_13px;
	unicode_block = &unicode_block_FreeFont_FreeSans_13px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_13px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_13px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_13px;
	unicode_block = &unicode_block_FreeFont_FreeSans_13px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_13px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_13px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_13px;
	unicode_block = &unicode_block_FreeFont_FreeSans_13px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_13px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_13px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_13px;
	unicode_block = &unicode_block_FreeFont_FreeSans_13px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_13px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_14px_BasicLatin) {
	font = &font_FreeFont_FreeSans_14px;
	unicode_block = &unicode_block_FreeFont_FreeSans_14px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_14px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_14px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_14px;
	unicode_block = &unicode_block_FreeFont_FreeSans_14px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_14px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_14px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_14px;
	unicode_block = &unicode_block_FreeFont_FreeSans_14px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_14px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_14px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_14px;
	unicode_block = &unicode_block_FreeFont_FreeSans_14px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_14px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_15px_BasicLatin) {
	font = &font_FreeFont_FreeSans_15px;
	unicode_block = &unicode_block_FreeFont_FreeSans_15px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_15px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_15px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_15px;
	unicode_block = &unicode_block_FreeFont_FreeSans_15px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_15px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_15px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_15px;
	unicode_block = &unicode_block_FreeFont_FreeSans_15px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_15px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_15px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_15px;
	unicode_block = &unicode_block_FreeFont_FreeSans_15px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_15px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_16px_BasicLatin) {
	font = &font_FreeFont_FreeSans_16px;
	unicode_block = &unicode_block_FreeFont_FreeSans_16px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_16px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_16px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_16px;
	unicode_block = &unicode_block_FreeFont_FreeSans_16px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_16px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_16px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_16px;
	unicode_block = &unicode_block_FreeFont_FreeSans_16px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_16px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_16px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_16px;
	unicode_block = &unicode_block_FreeFont_FreeSans_16px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_16px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_18px_BasicLatin) {
	font = &font_FreeFont_FreeSans_18px;
	unicode_block = &unicode_block_FreeFont_FreeSans_18px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_18px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_18px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_18px;
	unicode_block = &unicode_block_FreeFont_FreeSans_18px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_18px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_18px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_18px;
	unicode_block = &unicode_block_FreeFont_FreeSans_18px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_18px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_18px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_18px;
	unicode_block = &unicode_block_FreeFont_FreeSans_18px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_18px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_20px_BasicLatin) {
	font = &font_FreeFont_FreeSans_20px;
	unicode_block = &unicode_block_FreeFont_FreeSans_20px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_20px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_20px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_20px;
	unicode_block = &unicode_block_FreeFont_FreeSans_20px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_20px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_20px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_20px;
	unicode_block = &unicode_block_FreeFont_FreeSans_20px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_20px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_20px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_20px;
	unicode_block = &unicode_block_FreeFont_FreeSans_20px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_20px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_22px_BasicLatin) {
	font = &font_FreeFont_FreeSans_22px;
	unicode_block = &unicode_block_FreeFont_FreeSans_22px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_22px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_22px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_22px;
	unicode_block = &unicode_block_FreeFont_FreeSans_22px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_22px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_22px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_22px;
	unicode_block = &unicode_block_FreeFont_FreeSans_22px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_22px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_22px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_22px;
	unicode_block = &unicode_block_FreeFont_FreeSans_22px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_22px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_24px_BasicLatin) {
	font = &font_FreeFont_FreeSans_24px;
	unicode_block = &unicode_block_FreeFont_FreeSans_24px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_24px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_24px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_24px;
	unicode_block = &unicode_block_FreeFont_FreeSans_24px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_24px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_24px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_24px;
	unicode_block = &unicode_block_FreeFont_FreeSans_24px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_24px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_24px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_24px;
	unicode_block = &unicode_block_FreeFont_FreeSans_24px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_24px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_26px_BasicLatin) {
	font = &font_FreeFont_FreeSans_26px;
	unicode_block = &unicode_block_FreeFont_FreeSans_26px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_26px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_26px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_26px;
	unicode_block = &unicode_block_FreeFont_FreeSans_26px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_26px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_26px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_26px;
	unicode_block = &unicode_block_FreeFont_FreeSans_26px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_26px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_26px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_26px;
	unicode_block = &unicode_block_FreeFont_FreeSans_26px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_26px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_28px_BasicLatin) {
	font = &font_FreeFont_FreeSans_28px;
	unicode_block = &unicode_block_FreeFont_FreeSans_28px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_28px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_28px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_28px;
	unicode_block = &unicode_block_FreeFont_FreeSans_28px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_28px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_28px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_28px;
	unicode_block = &unicode_block_FreeFont_FreeSans_28px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_28px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_28px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_28px;
	unicode_block = &unicode_block_FreeFont_FreeSans_28px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_28px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_32px_BasicLatin) {
	font = &font_FreeFont_FreeSans_32px;
	unicode_block = &unicode_block_FreeFont_FreeSans_32px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_32px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_32px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_32px;
	unicode_block = &unicode_block_FreeFont_FreeSans_32px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_32px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_32px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_32px;
	unicode_block = &unicode_block_FreeFont_FreeSans_32px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_32px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_32px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_32px;
	unicode_block = &unicode_block_FreeFont_FreeSans_32px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_32px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_36px_BasicLatin) {
	font = &font_FreeFont_FreeSans_36px;
	unicode_block = &unicode_block_FreeFont_FreeSans_36px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_36px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_36px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_36px;
	unicode_block = &unicode_block_FreeFont_FreeSans_36px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_36px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_36px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_36px;
	unicode_block = &unicode_block_FreeFont_FreeSans_36px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_36px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_36px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_36px;
	unicode_block = &unicode_block_FreeFont_FreeSans_36px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_36px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_40px_BasicLatin) {
	font = &font_FreeFont_FreeSans_40px;
	unicode_block = &unicode_block_FreeFont_FreeSans_40px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_40px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_40px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_40px;
	unicode_block = &unicode_block_FreeFont_FreeSans_40px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_40px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_40px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_40px;
	unicode_block = &unicode_block_FreeFont_FreeSans_40px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_40px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_40px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_40px;
	unicode_block = &unicode_block_FreeFont_FreeSans_40px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_40px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_44px_BasicLatin) {
	font = &font_FreeFont_FreeSans_44px;
	unicode_block = &unicode_block_FreeFont_FreeSans_44px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_44px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_44px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_44px;
	unicode_block = &unicode_block_FreeFont_FreeSans_44px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_44px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_44px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_44px;
	unicode_block = &unicode_block_FreeFont_FreeSans_44px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_44px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_44px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_44px;
	unicode_block = &unicode_block_FreeFont_FreeSans_44px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_44px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_48px_BasicLatin) {
	font = &font_FreeFont_FreeSans_48px;
	unicode_block = &unicode_block_FreeFont_FreeSans_48px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_48px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_48px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_48px;
	unicode_block = &unicode_block_FreeFont_FreeSans_48px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_48px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_48px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_48px;
	unicode_block = &unicode_block_FreeFont_FreeSans_48px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_48px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_48px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_48px;
	unicode_block = &unicode_block_FreeFont_FreeSans_48px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_48px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_54px_BasicLatin) {
	font = &font_FreeFont_FreeSans_54px;
	unicode_block = &unicode_block_FreeFont_FreeSans_54px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_54px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_54px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_54px;
	unicode_block = &unicode_block_FreeFont_FreeSans_54px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_54px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_54px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_54px;
	unicode_block = &unicode_block_FreeFont_FreeSans_54px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_54px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_54px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_54px;
	unicode_block = &unicode_block_FreeFont_FreeSans_54px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_54px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_60px_BasicLatin) {
	font = &font_FreeFont_FreeSans_60px;
	unicode_block = &unicode_block_FreeFont_FreeSans_60px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_60px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_60px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_60px;
	unicode_block = &unicode_block_FreeFont_FreeSans_60px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_60px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_60px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_60px;
	unicode_block = &unicode_block_FreeFont_FreeSans_60px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_60px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_60px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_60px;
	unicode_block = &unicode_block_FreeFont_FreeSans_60px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_60px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_66px_BasicLatin) {
	font = &font_FreeFont_FreeSans_66px;
	unicode_block = &unicode_block_FreeFont_FreeSans_66px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_66px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_66px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_66px;
	unicode_block = &unicode_block_FreeFont_FreeSans_66px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_66px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_66px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_66px;
	unicode_block = &unicode_block_FreeFont_FreeSans_66px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_66px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_66px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_66px;
	unicode_block = &unicode_block_FreeFont_FreeSans_66px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_66px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_72px_BasicLatin) {
	font = &font_FreeFont_FreeSans_72px;
	unicode_block = &unicode_block_FreeFont_FreeSans_72px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_72px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_72px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_72px;
	unicode_block = &unicode_block_FreeFont_FreeSans_72px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_72px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_72px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_72px;
	unicode_block = &unicode_block_FreeFont_FreeSans_72px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_72px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_72px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_72px;
	unicode_block = &unicode_block_FreeFont_FreeSans_72px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_72px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_80px_BasicLatin) {
	font = &font_FreeFont_FreeSans_80px;
	unicode_block = &unicode_block_FreeFont_FreeSans_80px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_80px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_80px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_80px;
	unicode_block = &unicode_block_FreeFont_FreeSans_80px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_80px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_80px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_80px;
	unicode_block = &unicode_block_FreeFont_FreeSans_80px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_80px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_80px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_80px;
	unicode_block = &unicode_block_FreeFont_FreeSans_80px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_80px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_83px_BasicLatin) {
	font = &font_FreeFont_FreeSans_83px;
	unicode_block = &unicode_block_FreeFont_FreeSans_83px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_83px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_83px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_83px;
	unicode_block = &unicode_block_FreeFont_FreeSans_83px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_83px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_83px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_83px;
	unicode_block = &unicode_block_FreeFont_FreeSans_83px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_83px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_83px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_83px;
	unicode_block = &unicode_block_FreeFont_FreeSans_83px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_83px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_88px_BasicLatin) {
	font = &font_FreeFont_FreeSans_88px;
	unicode_block = &unicode_block_FreeFont_FreeSans_88px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_88px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_88px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_88px;
	unicode_block = &unicode_block_FreeFont_FreeSans_88px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_88px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_88px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_88px;
	unicode_block = &unicode_block_FreeFont_FreeSans_88px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_88px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_88px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_88px;
	unicode_block = &unicode_block_FreeFont_FreeSans_88px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_88px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSans_96px_BasicLatin) {
	font = &font_FreeFont_FreeSans_96px;
	unicode_block = &unicode_block_FreeFont_FreeSans_96px_BasicLatin;
	expectation_name = "FreeFont_FreeSans_96px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSans_96px_Latin1Supplement) {
	font = &font_FreeFont_FreeSans_96px;
	unicode_block = &unicode_block_FreeFont_FreeSans_96px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSans_96px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSans_96px_LatinExtendedA) {
	font = &font_FreeFont_FreeSans_96px;
	unicode_block = &unicode_block_FreeFont_FreeSans_96px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSans_96px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSans_96px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSans_96px;
	unicode_block = &unicode_block_FreeFont_FreeSans_96px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSans_96px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_7px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_7px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_7px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_7px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_7px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_7px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_7px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_7px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_7px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_7px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_7px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_7px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_7px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_7px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_7px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_7px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_8px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_8px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_8px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_8px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_8px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_8px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_8px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_8px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_8px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_8px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_8px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_8px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_8px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_8px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_8px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_8px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_9px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_9px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_9px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_9px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_9px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_9px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_9px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_9px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_9px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_9px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_9px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_9px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_9px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_9px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_9px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_9px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_10px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_10px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_10px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_10px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_10px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_10px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_10px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_10px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_10px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_10px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_10px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_10px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_10px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_10px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_10px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_10px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_11px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_11px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_11px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_11px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_11px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_11px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_11px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_11px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_11px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_11px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_11px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_11px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_11px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_11px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_11px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_11px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_12px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_12px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_12px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_12px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_12px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_12px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_12px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_12px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_12px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_12px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_12px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_12px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_12px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_12px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_12px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_12px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_13px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_13px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_13px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_13px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_13px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_13px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_13px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_13px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_13px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_13px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_13px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_13px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_13px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_13px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_13px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_13px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_14px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_14px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_14px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_14px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_14px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_14px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_14px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_14px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_14px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_14px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_14px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_14px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_14px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_14px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_14px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_14px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_15px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_15px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_15px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_15px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_15px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_15px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_15px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_15px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_15px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_15px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_15px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_15px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_15px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_15px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_15px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_15px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_16px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_16px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_16px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_16px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_16px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_16px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_16px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_16px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_16px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_16px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_16px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_16px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_16px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_16px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_16px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_16px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_18px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_18px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_18px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_18px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_18px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_18px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_18px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_18px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_18px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_18px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_18px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_18px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_18px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_18px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_18px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_18px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_20px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_20px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_20px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_20px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_20px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_20px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_20px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_20px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_20px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_20px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_20px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_20px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_20px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_20px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_20px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_20px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_22px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_22px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_22px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_22px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_22px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_22px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_22px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_22px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_22px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_22px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_22px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_22px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_22px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_22px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_22px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_22px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_24px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_24px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_24px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_24px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_24px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_24px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_24px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_24px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_24px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_24px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_24px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_24px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_24px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_24px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_24px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_24px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_26px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_26px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_26px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_26px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_26px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_26px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_26px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_26px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_26px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_26px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_26px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_26px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_26px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_26px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_26px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_26px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_28px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_28px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_28px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_28px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_28px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_28px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_28px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_28px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_28px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_28px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_28px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_28px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_28px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_28px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_28px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_28px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_32px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_32px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_32px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_32px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_32px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_32px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_32px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_32px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_32px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_32px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_32px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_32px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_32px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_32px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_32px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_32px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_36px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_36px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_36px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_36px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_36px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_36px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_36px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_36px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_36px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_36px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_36px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_36px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_36px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_36px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_36px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_36px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_40px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_40px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_40px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_40px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_40px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_40px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_40px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_40px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_40px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_40px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_40px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_40px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_40px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_40px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_40px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_40px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_44px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_44px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_44px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_44px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_44px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_44px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_44px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_44px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_44px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_44px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_44px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_44px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_44px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_44px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_44px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_44px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_48px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_48px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_48px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_48px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_48px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_48px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_48px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_48px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_48px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_48px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_48px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_48px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_48px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_48px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_48px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_48px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_54px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_54px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_54px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_54px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_54px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_54px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_54px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_54px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_54px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_54px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_54px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_54px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_54px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_54px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_54px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_54px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_60px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_60px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_60px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_60px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_60px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_60px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_60px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_60px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_60px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_60px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_60px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_60px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_60px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_60px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_60px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_60px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_66px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_66px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_66px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_66px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_66px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_66px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_66px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_66px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_66px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_66px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_66px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_66px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_66px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_66px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_66px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_66px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_72px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_72px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_72px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_72px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_72px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_72px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_72px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_72px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_72px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_72px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_72px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_72px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_72px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_72px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_72px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_72px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_80px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_80px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_80px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_80px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_80px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_80px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_80px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_80px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_80px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_80px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_80px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_80px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_80px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_80px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_80px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_80px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_83px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_83px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_83px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_83px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_83px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_83px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_83px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_83px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_83px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_83px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_83px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_83px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_83px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_83px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_83px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_83px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_88px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_88px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_88px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_88px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_88px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_88px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_88px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_88px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_88px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_88px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_88px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_88px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_88px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_88px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_88px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_88px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBold_96px_BasicLatin) {
	font = &font_FreeFont_FreeSansBold_96px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_96px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBold_96px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBold_96px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBold_96px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_96px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBold_96px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBold_96px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBold_96px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_96px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBold_96px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBold_96px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBold_96px;
	unicode_block = &unicode_block_FreeFont_FreeSansBold_96px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBold_96px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_7px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_7px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_7px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_7px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_7px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_7px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_7px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_7px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_7px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_7px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_7px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_7px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_7px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_7px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_7px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_7px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_8px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_8px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_8px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_8px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_8px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_8px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_8px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_8px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_8px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_8px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_8px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_8px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_8px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_8px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_8px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_8px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_9px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_9px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_9px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_9px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_9px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_9px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_9px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_9px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_9px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_9px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_9px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_9px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_9px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_9px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_9px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_9px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_10px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_10px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_10px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_10px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_10px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_10px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_10px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_10px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_10px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_10px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_10px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_10px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_10px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_10px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_10px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_10px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_11px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_11px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_11px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_11px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_11px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_11px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_11px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_11px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_11px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_11px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_11px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_11px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_11px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_11px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_11px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_11px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_12px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_12px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_12px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_12px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_12px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_12px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_12px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_12px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_12px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_12px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_12px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_12px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_12px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_12px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_12px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_12px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_13px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_13px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_13px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_13px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_13px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_13px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_13px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_13px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_13px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_13px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_13px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_13px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_13px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_13px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_13px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_13px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_14px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_14px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_14px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_14px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_14px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_14px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_14px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_14px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_14px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_14px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_14px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_14px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_14px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_14px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_14px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_14px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_15px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_15px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_15px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_15px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_15px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_15px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_15px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_15px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_15px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_15px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_15px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_15px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_15px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_15px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_15px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_15px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_16px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_16px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_16px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_16px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_16px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_16px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_16px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_16px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_16px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_16px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_16px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_16px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_16px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_16px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_16px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_16px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_18px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_18px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_18px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_18px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_18px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_18px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_18px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_18px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_18px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_18px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_18px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_18px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_18px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_18px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_18px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_18px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_20px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_20px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_20px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_20px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_20px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_20px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_20px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_20px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_20px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_20px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_20px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_20px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_20px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_20px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_20px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_20px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_22px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_22px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_22px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_22px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_22px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_22px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_22px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_22px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_22px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_22px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_22px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_22px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_22px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_22px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_22px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_22px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_24px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_24px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_24px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_24px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_24px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_24px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_24px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_24px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_24px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_24px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_24px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_24px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_24px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_24px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_24px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_24px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_26px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_26px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_26px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_26px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_26px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_26px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_26px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_26px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_26px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_26px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_26px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_26px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_26px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_26px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_26px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_26px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_28px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_28px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_28px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_28px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_28px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_28px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_28px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_28px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_28px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_28px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_28px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_28px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_28px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_28px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_28px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_28px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_32px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_32px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_32px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_32px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_32px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_32px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_32px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_32px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_32px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_32px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_32px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_32px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_32px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_32px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_32px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_32px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_36px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_36px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_36px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_36px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_36px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_36px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_36px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_36px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_36px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_36px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_36px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_36px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_36px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_36px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_36px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_36px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_40px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_40px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_40px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_40px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_40px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_40px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_40px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_40px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_40px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_40px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_40px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_40px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_40px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_40px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_40px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_40px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_44px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_44px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_44px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_44px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_44px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_44px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_44px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_44px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_44px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_44px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_44px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_44px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_44px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_44px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_44px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_44px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_48px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_48px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_48px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_48px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_48px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_48px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_48px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_48px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_48px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_48px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_48px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_48px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_48px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_48px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_48px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_48px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_54px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_54px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_54px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_54px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_54px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_54px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_54px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_54px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_54px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_54px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_54px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_54px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_54px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_54px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_54px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_54px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_60px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_60px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_60px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_60px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_60px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_60px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_60px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_60px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_60px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_60px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_60px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_60px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_60px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_60px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_60px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_60px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_66px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_66px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_66px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_66px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_66px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_66px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_66px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_66px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_66px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_66px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_66px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_66px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_66px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_66px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_66px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_66px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_72px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_72px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_72px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_72px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_72px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_72px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_72px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_72px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_72px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_72px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_72px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_72px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_72px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_72px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_72px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_72px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_80px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_80px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_80px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_80px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_80px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_80px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_80px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_80px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_80px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_80px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_80px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_80px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_80px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_80px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_80px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_80px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_83px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_83px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_83px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_83px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_83px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_83px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_83px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_83px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_83px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_83px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_83px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_83px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_83px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_83px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_83px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_83px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_88px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_88px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_88px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_88px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_88px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_88px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_88px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_88px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_88px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_88px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_88px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_88px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_88px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_88px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_88px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_88px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_96px_BasicLatin) {
	font = &font_FreeFont_FreeSansBoldOblique_96px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_96px_BasicLatin;
	expectation_name = "FreeFont_FreeSansBoldOblique_96px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_96px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansBoldOblique_96px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_96px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansBoldOblique_96px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_96px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansBoldOblique_96px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_96px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansBoldOblique_96px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansBoldOblique_96px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansBoldOblique_96px;
	unicode_block = &unicode_block_FreeFont_FreeSansBoldOblique_96px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansBoldOblique_96px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_7px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_7px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_7px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_7px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_7px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_7px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_7px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_7px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_7px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_7px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_7px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_7px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_7px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_7px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_7px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_7px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_8px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_8px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_8px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_8px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_8px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_8px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_8px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_8px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_8px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_8px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_8px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_8px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_8px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_8px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_8px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_8px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_9px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_9px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_9px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_9px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_9px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_9px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_9px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_9px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_9px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_9px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_9px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_9px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_9px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_9px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_9px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_9px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_10px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_10px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_10px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_10px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_10px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_10px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_10px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_10px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_10px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_10px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_10px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_10px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_10px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_10px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_10px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_10px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_11px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_11px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_11px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_11px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_11px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_11px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_11px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_11px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_11px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_11px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_11px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_11px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_11px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_11px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_11px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_11px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_12px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_12px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_12px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_12px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_12px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_12px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_12px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_12px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_12px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_12px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_12px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_12px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_12px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_12px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_12px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_12px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_13px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_13px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_13px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_13px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_13px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_13px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_13px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_13px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_13px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_13px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_13px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_13px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_13px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_13px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_13px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_13px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_14px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_14px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_14px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_14px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_14px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_14px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_14px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_14px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_14px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_14px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_14px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_14px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_14px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_14px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_14px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_14px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_15px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_15px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_15px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_15px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_15px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_15px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_15px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_15px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_15px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_15px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_15px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_15px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_15px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_15px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_15px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_15px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_16px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_16px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_16px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_16px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_16px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_16px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_16px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_16px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_16px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_16px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_16px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_16px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_16px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_16px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_16px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_16px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_18px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_18px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_18px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_18px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_18px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_18px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_18px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_18px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_18px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_18px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_18px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_18px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_18px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_18px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_18px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_18px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_20px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_20px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_20px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_20px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_20px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_20px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_20px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_20px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_20px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_20px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_20px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_20px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_20px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_20px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_20px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_20px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_22px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_22px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_22px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_22px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_22px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_22px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_22px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_22px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_22px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_22px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_22px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_22px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_22px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_22px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_22px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_22px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_24px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_24px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_24px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_24px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_24px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_24px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_24px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_24px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_24px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_24px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_24px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_24px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_24px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_24px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_24px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_24px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_26px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_26px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_26px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_26px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_26px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_26px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_26px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_26px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_26px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_26px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_26px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_26px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_26px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_26px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_26px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_26px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_28px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_28px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_28px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_28px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_28px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_28px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_28px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_28px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_28px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_28px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_28px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_28px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_28px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_28px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_28px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_28px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_32px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_32px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_32px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_32px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_32px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_32px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_32px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_32px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_32px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_32px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_32px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_32px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_32px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_32px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_32px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_32px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_36px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_36px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_36px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_36px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_36px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_36px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_36px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_36px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_36px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_36px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_36px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_36px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_36px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_36px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_36px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_36px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_40px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_40px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_40px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_40px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_40px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_40px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_40px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_40px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_40px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_40px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_40px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_40px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_40px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_40px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_40px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_40px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_44px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_44px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_44px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_44px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_44px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_44px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_44px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_44px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_44px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_44px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_44px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_44px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_44px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_44px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_44px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_44px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_48px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_48px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_48px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_48px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_48px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_48px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_48px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_48px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_48px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_48px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_48px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_48px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_48px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_48px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_48px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_48px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_54px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_54px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_54px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_54px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_54px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_54px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_54px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_54px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_54px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_54px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_54px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_54px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_54px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_54px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_54px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_54px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_60px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_60px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_60px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_60px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_60px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_60px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_60px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_60px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_60px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_60px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_60px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_60px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_60px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_60px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_60px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_60px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_66px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_66px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_66px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_66px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_66px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_66px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_66px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_66px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_66px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_66px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_66px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_66px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_66px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_66px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_66px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_66px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_72px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_72px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_72px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_72px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_72px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_72px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_72px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_72px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_72px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_72px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_72px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_72px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_72px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_72px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_72px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_72px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_80px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_80px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_80px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_80px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_80px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_80px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_80px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_80px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_80px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_80px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_80px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_80px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_80px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_80px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_80px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_80px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_83px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_83px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_83px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_83px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_83px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_83px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_83px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_83px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_83px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_83px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_83px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_83px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_83px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_83px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_83px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_83px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_88px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_88px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_88px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_88px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_88px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_88px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_88px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_88px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_88px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_88px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_88px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_88px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_88px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_88px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_88px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_88px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_96px_BasicLatin) {
	font = &font_FreeFont_FreeSansOblique_96px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_96px_BasicLatin;
	expectation_name = "FreeFont_FreeSansOblique_96px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_96px_Latin1Supplement) {
	font = &font_FreeFont_FreeSansOblique_96px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_96px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSansOblique_96px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_96px_LatinExtendedA) {
	font = &font_FreeFont_FreeSansOblique_96px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_96px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSansOblique_96px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSansOblique_96px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSansOblique_96px;
	unicode_block = &unicode_block_FreeFont_FreeSansOblique_96px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSansOblique_96px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_7px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_7px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_7px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_7px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_7px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_7px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_7px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_7px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_7px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_7px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_7px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_7px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_7px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_7px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_7px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_7px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_8px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_8px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_8px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_8px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_8px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_8px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_8px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_8px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_8px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_8px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_8px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_8px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_8px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_8px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_8px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_8px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_9px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_9px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_9px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_9px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_9px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_9px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_9px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_9px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_9px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_9px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_9px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_9px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_9px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_9px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_9px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_9px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_10px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_10px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_10px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_10px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_10px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_10px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_10px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_10px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_10px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_10px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_10px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_10px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_10px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_10px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_10px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_10px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_11px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_11px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_11px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_11px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_11px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_11px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_11px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_11px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_11px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_11px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_11px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_11px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_11px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_11px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_11px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_11px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_12px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_12px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_12px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_12px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_12px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_12px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_12px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_12px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_12px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_12px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_12px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_12px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_12px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_12px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_12px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_12px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_13px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_13px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_13px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_13px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_13px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_13px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_13px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_13px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_13px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_13px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_13px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_13px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_13px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_13px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_13px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_13px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_14px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_14px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_14px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_14px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_14px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_14px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_14px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_14px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_14px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_14px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_14px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_14px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_14px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_14px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_14px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_14px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_15px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_15px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_15px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_15px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_15px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_15px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_15px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_15px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_15px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_15px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_15px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_15px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_15px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_15px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_15px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_15px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_16px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_16px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_16px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_16px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_16px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_16px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_16px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_16px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_16px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_16px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_16px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_16px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_16px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_16px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_16px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_16px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_18px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_18px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_18px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_18px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_18px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_18px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_18px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_18px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_18px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_18px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_18px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_18px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_18px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_18px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_18px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_18px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_20px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_20px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_20px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_20px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_20px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_20px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_20px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_20px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_20px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_20px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_20px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_20px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_20px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_20px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_20px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_20px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_22px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_22px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_22px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_22px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_22px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_22px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_22px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_22px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_22px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_22px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_22px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_22px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_22px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_22px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_22px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_22px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_24px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_24px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_24px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_24px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_24px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_24px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_24px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_24px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_24px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_24px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_24px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_24px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_24px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_24px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_24px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_24px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_26px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_26px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_26px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_26px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_26px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_26px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_26px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_26px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_26px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_26px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_26px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_26px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_26px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_26px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_26px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_26px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_28px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_28px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_28px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_28px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_28px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_28px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_28px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_28px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_28px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_28px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_28px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_28px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_28px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_28px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_28px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_28px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_32px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_32px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_32px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_32px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_32px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_32px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_32px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_32px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_32px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_32px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_32px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_32px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_32px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_32px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_32px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_32px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_36px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_36px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_36px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_36px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_36px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_36px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_36px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_36px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_36px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_36px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_36px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_36px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_36px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_36px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_36px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_36px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_40px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_40px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_40px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_40px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_40px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_40px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_40px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_40px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_40px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_40px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_40px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_40px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_40px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_40px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_40px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_40px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_44px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_44px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_44px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_44px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_44px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_44px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_44px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_44px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_44px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_44px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_44px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_44px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_44px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_44px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_44px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_44px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_48px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_48px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_48px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_48px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_48px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_48px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_48px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_48px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_48px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_48px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_48px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_48px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_48px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_48px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_48px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_48px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_54px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_54px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_54px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_54px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_54px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_54px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_54px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_54px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_54px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_54px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_54px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_54px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_54px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_54px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_54px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_54px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_60px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_60px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_60px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_60px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_60px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_60px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_60px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_60px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_60px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_60px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_60px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_60px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_60px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_60px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_60px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_60px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_66px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_66px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_66px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_66px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_66px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_66px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_66px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_66px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_66px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_66px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_66px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_66px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_66px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_66px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_66px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_66px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_72px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_72px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_72px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_72px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_72px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_72px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_72px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_72px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_72px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_72px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_72px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_72px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_72px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_72px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_72px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_72px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_80px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_80px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_80px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_80px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_80px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_80px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_80px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_80px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_80px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_80px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_80px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_80px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_80px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_80px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_80px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_80px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_83px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_83px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_83px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_83px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_83px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_83px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_83px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_83px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_83px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_83px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_83px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_83px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_83px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_83px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_83px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_83px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_88px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_88px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_88px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_88px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_88px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_88px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_88px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_88px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_88px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_88px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_88px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_88px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_88px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_88px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_88px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_88px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerif_96px_BasicLatin) {
	font = &font_FreeFont_FreeSerif_96px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_96px_BasicLatin;
	expectation_name = "FreeFont_FreeSerif_96px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerif_96px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerif_96px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_96px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerif_96px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerif_96px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerif_96px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_96px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerif_96px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerif_96px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerif_96px;
	unicode_block = &unicode_block_FreeFont_FreeSerif_96px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerif_96px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_7px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_7px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_7px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_7px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_7px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_7px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_7px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_7px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_7px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_7px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_7px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_7px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_7px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_7px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_7px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_7px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_8px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_8px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_8px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_8px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_8px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_8px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_8px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_8px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_8px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_8px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_8px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_8px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_8px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_8px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_8px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_8px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_9px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_9px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_9px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_9px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_9px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_9px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_9px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_9px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_9px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_9px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_9px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_9px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_9px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_9px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_9px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_9px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_10px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_10px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_10px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_10px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_10px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_10px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_10px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_10px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_10px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_10px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_10px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_10px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_10px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_10px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_10px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_10px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_11px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_11px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_11px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_11px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_11px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_11px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_11px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_11px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_11px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_11px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_11px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_11px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_11px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_11px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_11px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_11px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_12px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_12px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_12px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_12px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_12px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_12px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_12px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_12px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_12px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_12px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_12px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_12px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_12px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_12px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_12px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_12px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_13px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_13px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_13px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_13px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_13px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_13px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_13px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_13px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_13px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_13px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_13px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_13px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_13px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_13px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_13px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_13px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_14px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_14px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_14px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_14px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_14px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_14px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_14px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_14px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_14px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_14px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_14px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_14px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_14px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_14px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_14px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_14px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_15px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_15px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_15px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_15px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_15px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_15px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_15px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_15px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_15px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_15px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_15px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_15px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_15px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_15px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_15px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_15px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_16px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_16px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_16px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_16px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_16px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_16px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_16px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_16px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_16px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_16px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_16px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_16px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_16px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_16px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_16px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_16px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_18px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_18px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_18px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_18px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_18px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_18px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_18px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_18px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_18px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_18px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_18px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_18px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_18px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_18px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_18px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_18px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_20px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_20px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_20px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_20px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_20px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_20px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_20px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_20px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_20px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_20px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_20px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_20px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_20px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_20px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_20px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_20px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_22px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_22px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_22px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_22px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_22px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_22px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_22px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_22px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_22px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_22px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_22px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_22px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_22px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_22px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_22px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_22px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_24px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_24px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_24px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_24px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_24px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_24px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_24px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_24px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_24px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_24px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_24px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_24px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_24px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_24px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_24px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_24px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_26px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_26px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_26px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_26px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_26px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_26px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_26px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_26px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_26px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_26px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_26px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_26px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_26px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_26px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_26px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_26px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_28px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_28px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_28px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_28px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_28px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_28px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_28px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_28px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_28px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_28px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_28px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_28px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_28px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_28px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_28px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_28px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_32px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_32px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_32px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_32px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_32px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_32px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_32px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_32px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_32px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_32px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_32px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_32px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_32px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_32px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_32px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_32px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_36px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_36px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_36px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_36px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_36px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_36px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_36px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_36px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_36px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_36px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_36px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_36px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_36px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_36px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_36px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_36px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_40px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_40px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_40px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_40px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_40px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_40px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_40px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_40px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_40px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_40px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_40px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_40px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_40px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_40px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_40px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_40px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_44px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_44px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_44px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_44px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_44px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_44px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_44px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_44px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_44px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_44px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_44px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_44px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_44px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_44px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_44px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_44px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_48px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_48px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_48px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_48px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_48px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_48px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_48px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_48px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_48px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_48px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_48px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_48px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_48px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_48px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_48px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_48px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_54px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_54px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_54px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_54px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_54px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_54px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_54px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_54px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_54px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_54px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_54px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_54px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_54px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_54px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_54px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_54px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_60px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_60px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_60px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_60px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_60px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_60px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_60px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_60px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_60px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_60px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_60px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_60px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_60px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_60px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_60px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_60px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_66px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_66px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_66px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_66px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_66px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_66px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_66px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_66px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_66px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_66px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_66px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_66px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_66px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_66px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_66px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_66px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_72px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_72px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_72px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_72px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_72px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_72px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_72px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_72px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_72px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_72px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_72px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_72px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_72px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_72px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_72px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_72px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_80px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_80px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_80px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_80px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_80px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_80px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_80px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_80px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_80px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_80px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_80px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_80px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_80px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_80px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_80px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_80px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_83px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_83px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_83px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_83px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_83px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_83px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_83px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_83px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_83px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_83px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_83px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_83px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_83px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_83px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_83px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_83px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_88px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_88px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_88px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_88px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_88px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_88px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_88px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_88px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_88px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_88px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_88px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_88px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_88px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_88px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_88px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_88px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_96px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBold_96px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_96px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBold_96px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_96px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBold_96px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_96px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBold_96px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_96px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBold_96px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_96px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBold_96px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBold_96px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBold_96px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBold_96px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBold_96px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_7px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_7px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_7px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_7px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_7px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_7px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_7px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_7px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_7px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_7px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_7px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_7px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_7px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_7px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_7px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_7px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_8px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_8px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_8px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_8px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_8px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_8px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_8px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_8px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_8px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_8px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_8px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_8px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_8px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_8px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_8px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_8px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_9px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_9px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_9px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_9px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_9px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_9px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_9px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_9px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_9px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_9px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_9px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_9px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_9px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_9px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_9px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_9px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_10px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_10px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_10px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_10px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_10px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_10px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_10px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_10px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_10px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_10px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_10px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_10px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_10px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_10px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_10px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_10px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_11px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_11px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_11px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_11px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_11px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_11px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_11px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_11px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_11px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_11px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_11px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_11px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_11px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_11px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_11px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_11px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_12px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_12px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_12px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_12px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_12px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_12px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_12px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_12px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_12px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_12px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_12px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_12px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_12px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_12px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_12px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_12px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_13px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_13px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_13px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_13px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_13px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_13px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_13px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_13px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_13px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_13px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_13px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_13px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_13px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_13px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_13px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_13px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_14px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_14px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_14px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_14px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_14px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_14px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_14px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_14px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_14px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_14px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_14px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_14px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_14px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_14px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_14px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_14px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_15px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_15px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_15px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_15px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_15px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_15px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_15px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_15px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_15px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_15px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_15px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_15px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_15px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_15px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_15px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_15px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_16px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_16px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_16px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_16px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_16px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_16px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_16px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_16px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_16px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_16px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_16px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_16px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_16px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_16px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_16px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_16px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_18px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_18px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_18px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_18px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_18px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_18px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_18px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_18px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_18px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_18px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_18px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_18px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_18px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_18px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_18px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_18px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_20px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_20px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_20px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_20px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_20px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_20px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_20px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_20px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_20px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_20px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_20px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_20px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_20px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_20px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_20px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_20px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_22px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_22px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_22px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_22px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_22px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_22px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_22px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_22px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_22px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_22px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_22px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_22px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_22px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_22px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_22px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_22px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_24px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_24px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_24px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_24px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_24px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_24px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_24px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_24px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_24px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_24px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_24px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_24px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_24px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_24px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_24px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_24px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_26px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_26px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_26px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_26px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_26px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_26px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_26px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_26px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_26px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_26px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_26px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_26px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_26px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_26px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_26px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_26px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_28px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_28px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_28px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_28px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_28px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_28px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_28px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_28px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_28px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_28px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_28px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_28px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_28px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_28px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_28px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_28px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_32px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_32px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_32px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_32px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_32px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_32px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_32px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_32px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_32px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_32px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_32px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_32px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_32px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_32px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_32px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_32px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_36px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_36px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_36px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_36px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_36px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_36px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_36px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_36px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_36px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_36px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_36px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_36px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_36px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_36px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_36px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_36px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_40px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_40px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_40px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_40px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_40px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_40px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_40px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_40px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_40px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_40px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_40px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_40px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_40px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_40px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_40px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_40px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_44px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_44px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_44px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_44px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_44px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_44px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_44px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_44px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_44px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_44px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_44px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_44px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_44px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_44px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_44px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_44px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_48px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_48px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_48px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_48px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_48px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_48px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_48px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_48px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_48px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_48px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_48px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_48px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_48px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_48px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_48px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_48px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_54px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_54px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_54px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_54px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_54px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_54px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_54px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_54px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_54px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_54px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_54px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_54px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_54px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_54px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_54px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_54px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_60px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_60px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_60px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_60px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_60px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_60px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_60px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_60px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_60px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_60px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_60px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_60px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_60px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_60px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_60px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_60px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_66px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_66px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_66px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_66px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_66px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_66px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_66px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_66px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_66px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_66px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_66px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_66px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_66px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_66px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_66px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_66px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_72px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_72px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_72px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_72px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_72px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_72px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_72px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_72px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_72px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_72px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_72px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_72px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_72px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_72px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_72px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_72px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_80px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_80px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_80px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_80px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_80px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_80px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_80px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_80px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_80px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_80px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_80px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_80px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_80px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_80px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_80px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_80px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_83px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_83px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_83px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_83px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_83px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_83px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_83px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_83px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_83px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_83px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_83px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_83px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_83px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_83px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_83px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_83px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_88px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_88px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_88px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_88px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_88px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_88px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_88px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_88px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_88px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_88px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_88px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_88px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_88px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_88px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_88px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_88px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_96px_BasicLatin) {
	font = &font_FreeFont_FreeSerifBoldItalic_96px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_96px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifBoldItalic_96px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_96px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifBoldItalic_96px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_96px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifBoldItalic_96px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_96px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifBoldItalic_96px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_96px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifBoldItalic_96px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifBoldItalic_96px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifBoldItalic_96px;
	unicode_block = &unicode_block_FreeFont_FreeSerifBoldItalic_96px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifBoldItalic_96px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_7px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_7px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_7px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_7px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_7px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_7px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_7px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_7px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_7px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_7px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_7px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_7px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_7px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_7px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_7px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_7px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_8px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_8px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_8px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_8px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_8px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_8px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_8px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_8px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_8px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_8px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_8px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_8px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_8px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_8px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_8px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_8px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_9px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_9px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_9px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_9px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_9px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_9px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_9px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_9px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_9px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_9px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_9px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_9px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_9px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_9px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_9px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_9px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_10px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_10px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_10px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_10px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_10px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_10px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_10px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_10px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_10px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_10px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_10px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_10px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_10px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_10px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_10px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_10px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_11px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_11px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_11px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_11px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_11px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_11px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_11px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_11px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_11px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_11px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_11px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_11px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_11px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_11px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_11px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_11px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_12px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_12px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_12px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_12px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_12px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_12px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_12px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_12px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_12px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_12px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_12px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_12px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_12px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_12px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_12px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_12px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_13px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_13px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_13px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_13px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_13px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_13px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_13px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_13px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_13px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_13px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_13px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_13px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_13px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_13px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_13px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_13px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_14px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_14px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_14px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_14px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_14px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_14px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_14px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_14px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_14px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_14px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_14px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_14px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_14px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_14px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_14px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_14px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_15px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_15px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_15px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_15px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_15px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_15px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_15px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_15px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_15px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_15px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_15px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_15px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_15px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_15px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_15px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_15px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_16px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_16px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_16px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_16px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_16px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_16px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_16px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_16px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_16px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_16px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_16px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_16px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_16px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_16px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_16px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_16px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_18px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_18px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_18px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_18px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_18px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_18px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_18px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_18px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_18px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_18px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_18px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_18px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_18px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_18px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_18px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_18px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_20px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_20px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_20px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_20px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_20px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_20px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_20px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_20px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_20px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_20px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_20px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_20px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_20px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_20px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_20px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_20px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_22px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_22px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_22px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_22px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_22px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_22px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_22px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_22px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_22px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_22px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_22px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_22px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_22px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_22px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_22px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_22px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_24px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_24px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_24px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_24px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_24px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_24px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_24px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_24px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_24px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_24px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_24px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_24px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_24px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_24px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_24px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_24px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_26px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_26px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_26px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_26px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_26px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_26px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_26px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_26px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_26px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_26px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_26px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_26px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_26px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_26px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_26px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_26px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_28px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_28px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_28px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_28px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_28px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_28px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_28px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_28px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_28px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_28px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_28px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_28px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_28px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_28px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_28px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_28px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_32px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_32px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_32px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_32px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_32px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_32px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_32px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_32px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_32px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_32px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_32px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_32px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_32px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_32px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_32px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_32px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_36px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_36px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_36px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_36px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_36px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_36px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_36px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_36px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_36px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_36px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_36px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_36px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_36px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_36px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_36px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_36px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_40px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_40px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_40px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_40px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_40px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_40px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_40px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_40px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_40px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_40px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_40px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_40px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_40px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_40px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_40px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_40px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_44px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_44px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_44px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_44px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_44px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_44px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_44px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_44px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_44px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_44px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_44px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_44px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_44px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_44px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_44px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_44px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_48px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_48px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_48px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_48px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_48px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_48px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_48px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_48px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_48px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_48px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_48px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_48px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_48px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_48px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_48px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_48px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_54px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_54px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_54px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_54px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_54px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_54px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_54px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_54px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_54px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_54px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_54px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_54px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_54px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_54px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_54px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_54px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_60px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_60px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_60px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_60px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_60px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_60px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_60px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_60px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_60px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_60px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_60px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_60px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_60px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_60px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_60px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_60px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_66px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_66px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_66px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_66px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_66px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_66px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_66px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_66px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_66px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_66px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_66px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_66px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_66px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_66px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_66px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_66px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_72px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_72px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_72px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_72px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_72px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_72px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_72px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_72px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_72px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_72px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_72px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_72px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_72px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_72px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_72px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_72px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_80px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_80px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_80px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_80px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_80px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_80px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_80px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_80px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_80px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_80px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_80px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_80px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_80px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_80px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_80px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_80px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_83px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_83px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_83px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_83px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_83px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_83px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_83px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_83px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_83px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_83px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_83px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_83px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_83px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_83px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_83px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_83px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_88px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_88px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_88px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_88px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_88px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_88px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_88px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_88px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_88px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_88px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_88px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_88px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_88px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_88px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_88px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_88px_SuperscriptsAndSubscripts";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_96px_BasicLatin) {
	font = &font_FreeFont_FreeSerifItalic_96px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_96px_BasicLatin;
	expectation_name = "FreeFont_FreeSerifItalic_96px_BasicLatin";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_96px_Latin1Supplement) {
	font = &font_FreeFont_FreeSerifItalic_96px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_96px_Latin1Supplement;
	expectation_name = "FreeFont_FreeSerifItalic_96px_Latin1Supplement";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_96px_LatinExtendedA) {
	font = &font_FreeFont_FreeSerifItalic_96px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_96px_LatinExtendedA;
	expectation_name = "FreeFont_FreeSerifItalic_96px_LatinExtendedA";
}END_TEST

START_TEST(FreeFont_FreeSerifItalic_96px_SuperscriptsAndSubscripts) {
	font = &font_FreeFont_FreeSerifItalic_96px;
	unicode_block = &unicode_block_FreeFont_FreeSerifItalic_96px_SuperscriptsAndSubscripts;
	expectation_name = "FreeFont_FreeSerifItalic_96px_SuperscriptsAndSubscripts";
}END_TEST

Suite * build_suite(void) {
	Suite *suite;
	TCase *tcase;

	suite = suite_create("liberation");

	tcase = tcase_create("liberation");
	tcase_add_checked_fixture(tcase, setup, teardown);

	tcase_add_test(tcase, FreeFont_FreeMono_7px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_7px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_7px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_7px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMono_8px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_8px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_8px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_8px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMono_9px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_9px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_9px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_9px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMono_10px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_10px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_10px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_10px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMono_11px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_11px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_11px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_11px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMono_12px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_12px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_12px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_12px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMono_13px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_13px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_13px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_13px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMono_14px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_14px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_14px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_14px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMono_15px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_15px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_15px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_15px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMono_16px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_16px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_16px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_16px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMono_18px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_18px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_18px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_18px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMono_20px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_20px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_20px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_20px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMono_22px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_22px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_22px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_22px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMono_24px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_24px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_24px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_24px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMono_26px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_26px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_26px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_26px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMono_28px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_28px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_28px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_28px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMono_32px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_32px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_32px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_32px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMono_36px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_36px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_36px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_36px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMono_40px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_40px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_40px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_40px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMono_44px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_44px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_44px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_44px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMono_48px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_48px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_48px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_48px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMono_54px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_54px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_54px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_54px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMono_60px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_60px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_60px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_60px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMono_66px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_66px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_66px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_66px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMono_72px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_72px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_72px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_72px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMono_80px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_80px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_80px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_80px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMono_83px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_83px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_83px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_83px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMono_88px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_88px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_88px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_88px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMono_96px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMono_96px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMono_96px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMono_96px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_7px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_7px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_7px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_7px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_8px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_8px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_8px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_8px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_9px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_9px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_9px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_9px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_10px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_10px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_10px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_10px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_11px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_11px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_11px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_11px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_12px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_12px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_12px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_12px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_13px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_13px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_13px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_13px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_14px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_14px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_14px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_14px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_15px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_15px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_15px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_15px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_16px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_16px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_16px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_16px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_18px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_18px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_18px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_18px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_20px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_20px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_20px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_20px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_22px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_22px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_22px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_22px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_24px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_24px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_24px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_24px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_26px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_26px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_26px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_26px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_28px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_28px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_28px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_28px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_32px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_32px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_32px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_32px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_36px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_36px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_36px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_36px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_40px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_40px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_40px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_40px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_44px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_44px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_44px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_44px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_48px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_48px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_48px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_48px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_54px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_54px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_54px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_54px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_60px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_60px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_60px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_60px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_66px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_66px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_66px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_66px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_72px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_72px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_72px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_72px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_80px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_80px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_80px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_80px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_83px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_83px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_83px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_83px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_88px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_88px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_88px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_88px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_96px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_96px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_96px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBold_96px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_7px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_7px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_7px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_7px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_8px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_8px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_8px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_8px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_9px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_9px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_9px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_9px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_10px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_10px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_10px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_10px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_11px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_11px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_11px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_11px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_12px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_12px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_12px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_12px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_13px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_13px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_13px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_13px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_14px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_14px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_14px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_14px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_15px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_15px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_15px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_15px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_16px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_16px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_16px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_16px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_18px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_18px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_18px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_18px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_20px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_20px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_20px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_20px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_22px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_22px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_22px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_22px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_24px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_24px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_24px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_24px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_26px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_26px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_26px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_26px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_28px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_28px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_28px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_28px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_32px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_32px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_32px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_32px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_36px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_36px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_36px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_36px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_40px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_40px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_40px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_40px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_44px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_44px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_44px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_44px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_48px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_48px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_48px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_48px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_54px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_54px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_54px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_54px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_60px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_60px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_60px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_60px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_66px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_66px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_66px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_66px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_72px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_72px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_72px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_72px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_80px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_80px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_80px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_80px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_83px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_83px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_83px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_83px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_88px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_88px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_88px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_88px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_96px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_96px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_96px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoBoldOblique_96px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_7px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_7px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_7px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_7px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_8px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_8px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_8px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_8px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_9px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_9px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_9px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_9px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_10px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_10px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_10px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_10px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_11px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_11px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_11px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_11px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_12px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_12px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_12px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_12px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_13px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_13px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_13px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_13px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_14px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_14px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_14px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_14px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_15px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_15px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_15px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_15px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_16px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_16px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_16px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_16px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_18px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_18px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_18px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_18px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_20px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_20px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_20px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_20px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_22px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_22px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_22px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_22px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_24px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_24px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_24px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_24px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_26px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_26px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_26px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_26px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_28px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_28px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_28px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_28px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_32px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_32px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_32px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_32px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_36px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_36px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_36px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_36px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_40px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_40px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_40px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_40px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_44px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_44px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_44px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_44px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_48px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_48px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_48px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_48px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_54px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_54px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_54px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_54px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_60px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_60px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_60px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_60px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_66px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_66px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_66px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_66px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_72px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_72px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_72px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_72px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_80px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_80px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_80px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_80px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_83px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_83px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_83px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_83px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_88px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_88px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_88px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_88px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_96px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_96px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_96px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeMonoOblique_96px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_7px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_7px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_7px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_7px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_8px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_8px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_8px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_8px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_9px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_9px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_9px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_9px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_10px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_10px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_10px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_10px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_11px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_11px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_11px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_11px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_12px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_12px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_12px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_12px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_13px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_13px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_13px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_13px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_14px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_14px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_14px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_14px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_15px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_15px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_15px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_15px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_16px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_16px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_16px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_16px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_18px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_18px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_18px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_18px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_20px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_20px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_20px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_20px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_22px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_22px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_22px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_22px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_24px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_24px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_24px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_24px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_26px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_26px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_26px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_26px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_28px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_28px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_28px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_28px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_32px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_32px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_32px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_32px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_36px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_36px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_36px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_36px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_40px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_40px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_40px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_40px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_44px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_44px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_44px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_44px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_48px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_48px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_48px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_48px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_54px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_54px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_54px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_54px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_60px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_60px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_60px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_60px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_66px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_66px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_66px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_66px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_72px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_72px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_72px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_72px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_80px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_80px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_80px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_80px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_83px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_83px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_83px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_83px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_88px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_88px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_88px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_88px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSans_96px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSans_96px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSans_96px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSans_96px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_7px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_7px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_7px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_7px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_8px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_8px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_8px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_8px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_9px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_9px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_9px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_9px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_10px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_10px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_10px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_10px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_11px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_11px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_11px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_11px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_12px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_12px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_12px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_12px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_13px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_13px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_13px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_13px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_14px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_14px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_14px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_14px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_15px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_15px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_15px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_15px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_16px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_16px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_16px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_16px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_18px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_18px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_18px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_18px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_20px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_20px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_20px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_20px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_22px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_22px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_22px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_22px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_24px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_24px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_24px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_24px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_26px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_26px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_26px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_26px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_28px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_28px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_28px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_28px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_32px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_32px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_32px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_32px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_36px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_36px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_36px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_36px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_40px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_40px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_40px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_40px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_44px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_44px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_44px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_44px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_48px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_48px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_48px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_48px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_54px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_54px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_54px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_54px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_60px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_60px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_60px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_60px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_66px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_66px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_66px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_66px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_72px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_72px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_72px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_72px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_80px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_80px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_80px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_80px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_83px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_83px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_83px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_83px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_88px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_88px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_88px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_88px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBold_96px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBold_96px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBold_96px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBold_96px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_7px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_7px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_7px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_7px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_8px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_8px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_8px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_8px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_9px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_9px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_9px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_9px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_10px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_10px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_10px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_10px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_11px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_11px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_11px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_11px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_12px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_12px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_12px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_12px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_13px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_13px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_13px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_13px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_14px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_14px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_14px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_14px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_15px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_15px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_15px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_15px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_16px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_16px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_16px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_16px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_18px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_18px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_18px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_18px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_20px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_20px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_20px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_20px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_22px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_22px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_22px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_22px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_24px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_24px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_24px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_24px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_26px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_26px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_26px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_26px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_28px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_28px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_28px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_28px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_32px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_32px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_32px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_32px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_36px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_36px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_36px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_36px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_40px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_40px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_40px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_40px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_44px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_44px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_44px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_44px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_48px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_48px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_48px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_48px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_54px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_54px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_54px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_54px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_60px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_60px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_60px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_60px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_66px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_66px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_66px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_66px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_72px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_72px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_72px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_72px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_80px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_80px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_80px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_80px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_83px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_83px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_83px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_83px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_88px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_88px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_88px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_88px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_96px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_96px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_96px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansBoldOblique_96px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_7px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_7px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_7px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_7px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_8px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_8px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_8px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_8px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_9px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_9px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_9px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_9px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_10px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_10px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_10px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_10px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_11px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_11px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_11px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_11px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_12px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_12px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_12px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_12px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_13px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_13px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_13px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_13px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_14px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_14px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_14px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_14px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_15px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_15px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_15px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_15px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_16px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_16px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_16px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_16px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_18px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_18px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_18px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_18px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_20px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_20px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_20px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_20px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_22px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_22px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_22px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_22px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_24px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_24px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_24px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_24px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_26px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_26px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_26px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_26px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_28px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_28px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_28px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_28px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_32px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_32px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_32px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_32px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_36px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_36px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_36px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_36px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_40px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_40px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_40px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_40px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_44px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_44px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_44px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_44px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_48px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_48px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_48px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_48px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_54px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_54px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_54px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_54px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_60px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_60px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_60px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_60px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_66px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_66px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_66px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_66px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_72px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_72px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_72px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_72px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_80px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_80px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_80px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_80px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_83px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_83px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_83px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_83px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_88px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_88px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_88px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_88px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_96px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_96px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_96px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSansOblique_96px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_7px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_7px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_7px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_7px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_8px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_8px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_8px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_8px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_9px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_9px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_9px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_9px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_10px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_10px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_10px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_10px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_11px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_11px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_11px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_11px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_12px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_12px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_12px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_12px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_13px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_13px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_13px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_13px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_14px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_14px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_14px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_14px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_15px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_15px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_15px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_15px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_16px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_16px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_16px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_16px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_18px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_18px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_18px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_18px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_20px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_20px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_20px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_20px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_22px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_22px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_22px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_22px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_24px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_24px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_24px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_24px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_26px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_26px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_26px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_26px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_28px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_28px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_28px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_28px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_32px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_32px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_32px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_32px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_36px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_36px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_36px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_36px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_40px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_40px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_40px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_40px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_44px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_44px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_44px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_44px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_48px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_48px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_48px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_48px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_54px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_54px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_54px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_54px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_60px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_60px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_60px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_60px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_66px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_66px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_66px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_66px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_72px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_72px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_72px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_72px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_80px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_80px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_80px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_80px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_83px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_83px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_83px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_83px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_88px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_88px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_88px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_88px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerif_96px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerif_96px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerif_96px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerif_96px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_7px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_7px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_7px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_7px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_8px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_8px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_8px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_8px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_9px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_9px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_9px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_9px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_10px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_10px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_10px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_10px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_11px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_11px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_11px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_11px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_12px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_12px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_12px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_12px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_13px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_13px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_13px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_13px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_14px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_14px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_14px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_14px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_15px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_15px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_15px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_15px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_16px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_16px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_16px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_16px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_18px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_18px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_18px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_18px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_20px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_20px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_20px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_20px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_22px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_22px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_22px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_22px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_24px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_24px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_24px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_24px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_26px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_26px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_26px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_26px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_28px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_28px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_28px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_28px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_32px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_32px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_32px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_32px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_36px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_36px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_36px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_36px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_40px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_40px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_40px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_40px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_44px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_44px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_44px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_44px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_48px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_48px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_48px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_48px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_54px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_54px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_54px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_54px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_60px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_60px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_60px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_60px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_66px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_66px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_66px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_66px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_72px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_72px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_72px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_72px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_80px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_80px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_80px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_80px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_83px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_83px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_83px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_83px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_88px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_88px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_88px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_88px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_96px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_96px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_96px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBold_96px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_7px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_7px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_7px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_7px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_8px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_8px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_8px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_8px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_9px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_9px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_9px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_9px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_10px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_10px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_10px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_10px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_11px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_11px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_11px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_11px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_12px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_12px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_12px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_12px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_13px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_13px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_13px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_13px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_14px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_14px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_14px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_14px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_15px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_15px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_15px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_15px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_16px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_16px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_16px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_16px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_18px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_18px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_18px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_18px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_20px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_20px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_20px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_20px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_22px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_22px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_22px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_22px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_24px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_24px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_24px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_24px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_26px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_26px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_26px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_26px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_28px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_28px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_28px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_28px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_32px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_32px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_32px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_32px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_36px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_36px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_36px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_36px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_40px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_40px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_40px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_40px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_44px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_44px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_44px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_44px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_48px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_48px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_48px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_48px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_54px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_54px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_54px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_54px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_60px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_60px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_60px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_60px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_66px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_66px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_66px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_66px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_72px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_72px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_72px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_72px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_80px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_80px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_80px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_80px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_83px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_83px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_83px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_83px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_88px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_88px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_88px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_88px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_96px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_96px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_96px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifBoldItalic_96px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_7px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_7px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_7px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_7px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_8px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_8px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_8px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_8px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_9px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_9px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_9px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_9px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_10px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_10px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_10px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_10px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_11px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_11px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_11px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_11px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_12px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_12px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_12px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_12px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_13px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_13px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_13px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_13px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_14px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_14px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_14px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_14px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_15px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_15px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_15px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_15px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_16px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_16px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_16px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_16px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_18px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_18px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_18px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_18px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_20px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_20px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_20px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_20px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_22px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_22px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_22px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_22px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_24px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_24px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_24px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_24px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_26px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_26px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_26px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_26px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_28px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_28px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_28px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_28px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_32px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_32px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_32px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_32px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_36px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_36px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_36px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_36px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_40px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_40px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_40px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_40px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_44px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_44px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_44px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_44px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_48px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_48px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_48px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_48px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_54px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_54px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_54px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_54px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_60px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_60px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_60px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_60px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_66px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_66px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_66px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_66px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_72px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_72px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_72px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_72px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_80px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_80px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_80px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_80px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_83px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_83px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_83px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_83px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_88px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_88px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_88px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_88px_SuperscriptsAndSubscripts);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_96px_BasicLatin);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_96px_Latin1Supplement);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_96px_LatinExtendedA);
	tcase_add_test(tcase, FreeFont_FreeSerifItalic_96px_SuperscriptsAndSubscripts);

	suite_add_tcase(suite, tcase);

	return suite;
}