#include "../unicode_block.h"
#include <check.h>
#include <eglib/display.h>

struct font_t *font;
const struct glyph_unicode_block_t *unicode_block;
char *expectation_name;
char *reldir = RELDIR;

Suite * build_suite(void);

START_TEST(Liberation_MonoBold_10px_BasicLatin) {
	font = &font_Liberation_MonoBold_10px;
	unicode_block = &unicode_block_Liberation_MonoBold_10px_BasicLatin;
	expectation_name = "Liberation_MonoBold_10px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_10px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_10px;
	unicode_block = &unicode_block_Liberation_MonoBold_10px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_10px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_10px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_10px;
	unicode_block = &unicode_block_Liberation_MonoBold_10px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_10px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBold_11px_BasicLatin) {
	font = &font_Liberation_MonoBold_11px;
	unicode_block = &unicode_block_Liberation_MonoBold_11px_BasicLatin;
	expectation_name = "Liberation_MonoBold_11px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_11px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_11px;
	unicode_block = &unicode_block_Liberation_MonoBold_11px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_11px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_11px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_11px;
	unicode_block = &unicode_block_Liberation_MonoBold_11px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_11px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBold_12px_BasicLatin) {
	font = &font_Liberation_MonoBold_12px;
	unicode_block = &unicode_block_Liberation_MonoBold_12px_BasicLatin;
	expectation_name = "Liberation_MonoBold_12px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_12px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_12px;
	unicode_block = &unicode_block_Liberation_MonoBold_12px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_12px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_12px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_12px;
	unicode_block = &unicode_block_Liberation_MonoBold_12px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_12px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBold_13px_BasicLatin) {
	font = &font_Liberation_MonoBold_13px;
	unicode_block = &unicode_block_Liberation_MonoBold_13px_BasicLatin;
	expectation_name = "Liberation_MonoBold_13px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_13px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_13px;
	unicode_block = &unicode_block_Liberation_MonoBold_13px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_13px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_13px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_13px;
	unicode_block = &unicode_block_Liberation_MonoBold_13px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_13px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBold_14px_BasicLatin) {
	font = &font_Liberation_MonoBold_14px;
	unicode_block = &unicode_block_Liberation_MonoBold_14px_BasicLatin;
	expectation_name = "Liberation_MonoBold_14px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_14px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_14px;
	unicode_block = &unicode_block_Liberation_MonoBold_14px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_14px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_14px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_14px;
	unicode_block = &unicode_block_Liberation_MonoBold_14px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_14px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBold_15px_BasicLatin) {
	font = &font_Liberation_MonoBold_15px;
	unicode_block = &unicode_block_Liberation_MonoBold_15px_BasicLatin;
	expectation_name = "Liberation_MonoBold_15px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_15px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_15px;
	unicode_block = &unicode_block_Liberation_MonoBold_15px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_15px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_15px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_15px;
	unicode_block = &unicode_block_Liberation_MonoBold_15px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_15px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBold_16px_BasicLatin) {
	font = &font_Liberation_MonoBold_16px;
	unicode_block = &unicode_block_Liberation_MonoBold_16px_BasicLatin;
	expectation_name = "Liberation_MonoBold_16px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_16px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_16px;
	unicode_block = &unicode_block_Liberation_MonoBold_16px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_16px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_16px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_16px;
	unicode_block = &unicode_block_Liberation_MonoBold_16px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_16px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBold_18px_BasicLatin) {
	font = &font_Liberation_MonoBold_18px;
	unicode_block = &unicode_block_Liberation_MonoBold_18px_BasicLatin;
	expectation_name = "Liberation_MonoBold_18px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_18px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_18px;
	unicode_block = &unicode_block_Liberation_MonoBold_18px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_18px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_18px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_18px;
	unicode_block = &unicode_block_Liberation_MonoBold_18px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_18px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBold_20px_BasicLatin) {
	font = &font_Liberation_MonoBold_20px;
	unicode_block = &unicode_block_Liberation_MonoBold_20px_BasicLatin;
	expectation_name = "Liberation_MonoBold_20px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_20px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_20px;
	unicode_block = &unicode_block_Liberation_MonoBold_20px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_20px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_20px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_20px;
	unicode_block = &unicode_block_Liberation_MonoBold_20px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_20px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBold_22px_BasicLatin) {
	font = &font_Liberation_MonoBold_22px;
	unicode_block = &unicode_block_Liberation_MonoBold_22px_BasicLatin;
	expectation_name = "Liberation_MonoBold_22px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_22px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_22px;
	unicode_block = &unicode_block_Liberation_MonoBold_22px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_22px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_22px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_22px;
	unicode_block = &unicode_block_Liberation_MonoBold_22px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_22px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBold_24px_BasicLatin) {
	font = &font_Liberation_MonoBold_24px;
	unicode_block = &unicode_block_Liberation_MonoBold_24px_BasicLatin;
	expectation_name = "Liberation_MonoBold_24px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_24px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_24px;
	unicode_block = &unicode_block_Liberation_MonoBold_24px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_24px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_24px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_24px;
	unicode_block = &unicode_block_Liberation_MonoBold_24px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_24px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBold_26px_BasicLatin) {
	font = &font_Liberation_MonoBold_26px;
	unicode_block = &unicode_block_Liberation_MonoBold_26px_BasicLatin;
	expectation_name = "Liberation_MonoBold_26px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_26px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_26px;
	unicode_block = &unicode_block_Liberation_MonoBold_26px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_26px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_26px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_26px;
	unicode_block = &unicode_block_Liberation_MonoBold_26px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_26px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBold_28px_BasicLatin) {
	font = &font_Liberation_MonoBold_28px;
	unicode_block = &unicode_block_Liberation_MonoBold_28px_BasicLatin;
	expectation_name = "Liberation_MonoBold_28px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_28px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_28px;
	unicode_block = &unicode_block_Liberation_MonoBold_28px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_28px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_28px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_28px;
	unicode_block = &unicode_block_Liberation_MonoBold_28px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_28px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBold_32px_BasicLatin) {
	font = &font_Liberation_MonoBold_32px;
	unicode_block = &unicode_block_Liberation_MonoBold_32px_BasicLatin;
	expectation_name = "Liberation_MonoBold_32px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_32px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_32px;
	unicode_block = &unicode_block_Liberation_MonoBold_32px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_32px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_32px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_32px;
	unicode_block = &unicode_block_Liberation_MonoBold_32px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_32px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBold_36px_BasicLatin) {
	font = &font_Liberation_MonoBold_36px;
	unicode_block = &unicode_block_Liberation_MonoBold_36px_BasicLatin;
	expectation_name = "Liberation_MonoBold_36px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_36px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_36px;
	unicode_block = &unicode_block_Liberation_MonoBold_36px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_36px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_36px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_36px;
	unicode_block = &unicode_block_Liberation_MonoBold_36px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_36px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBold_40px_BasicLatin) {
	font = &font_Liberation_MonoBold_40px;
	unicode_block = &unicode_block_Liberation_MonoBold_40px_BasicLatin;
	expectation_name = "Liberation_MonoBold_40px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_40px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_40px;
	unicode_block = &unicode_block_Liberation_MonoBold_40px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_40px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_40px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_40px;
	unicode_block = &unicode_block_Liberation_MonoBold_40px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_40px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBold_44px_BasicLatin) {
	font = &font_Liberation_MonoBold_44px;
	unicode_block = &unicode_block_Liberation_MonoBold_44px_BasicLatin;
	expectation_name = "Liberation_MonoBold_44px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_44px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_44px;
	unicode_block = &unicode_block_Liberation_MonoBold_44px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_44px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_44px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_44px;
	unicode_block = &unicode_block_Liberation_MonoBold_44px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_44px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBold_48px_BasicLatin) {
	font = &font_Liberation_MonoBold_48px;
	unicode_block = &unicode_block_Liberation_MonoBold_48px_BasicLatin;
	expectation_name = "Liberation_MonoBold_48px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_48px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_48px;
	unicode_block = &unicode_block_Liberation_MonoBold_48px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_48px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_48px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_48px;
	unicode_block = &unicode_block_Liberation_MonoBold_48px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_48px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBold_54px_BasicLatin) {
	font = &font_Liberation_MonoBold_54px;
	unicode_block = &unicode_block_Liberation_MonoBold_54px_BasicLatin;
	expectation_name = "Liberation_MonoBold_54px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_54px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_54px;
	unicode_block = &unicode_block_Liberation_MonoBold_54px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_54px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_54px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_54px;
	unicode_block = &unicode_block_Liberation_MonoBold_54px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_54px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBold_60px_BasicLatin) {
	font = &font_Liberation_MonoBold_60px;
	unicode_block = &unicode_block_Liberation_MonoBold_60px_BasicLatin;
	expectation_name = "Liberation_MonoBold_60px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_60px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_60px;
	unicode_block = &unicode_block_Liberation_MonoBold_60px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_60px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_60px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_60px;
	unicode_block = &unicode_block_Liberation_MonoBold_60px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_60px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBold_66px_BasicLatin) {
	font = &font_Liberation_MonoBold_66px;
	unicode_block = &unicode_block_Liberation_MonoBold_66px_BasicLatin;
	expectation_name = "Liberation_MonoBold_66px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_66px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_66px;
	unicode_block = &unicode_block_Liberation_MonoBold_66px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_66px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_66px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_66px;
	unicode_block = &unicode_block_Liberation_MonoBold_66px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_66px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBold_72px_BasicLatin) {
	font = &font_Liberation_MonoBold_72px;
	unicode_block = &unicode_block_Liberation_MonoBold_72px_BasicLatin;
	expectation_name = "Liberation_MonoBold_72px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_72px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_72px;
	unicode_block = &unicode_block_Liberation_MonoBold_72px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_72px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_72px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_72px;
	unicode_block = &unicode_block_Liberation_MonoBold_72px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_72px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBold_7px_BasicLatin) {
	font = &font_Liberation_MonoBold_7px;
	unicode_block = &unicode_block_Liberation_MonoBold_7px_BasicLatin;
	expectation_name = "Liberation_MonoBold_7px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_7px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_7px;
	unicode_block = &unicode_block_Liberation_MonoBold_7px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_7px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_7px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_7px;
	unicode_block = &unicode_block_Liberation_MonoBold_7px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_7px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBold_80px_BasicLatin) {
	font = &font_Liberation_MonoBold_80px;
	unicode_block = &unicode_block_Liberation_MonoBold_80px_BasicLatin;
	expectation_name = "Liberation_MonoBold_80px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_80px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_80px;
	unicode_block = &unicode_block_Liberation_MonoBold_80px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_80px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_80px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_80px;
	unicode_block = &unicode_block_Liberation_MonoBold_80px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_80px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBold_83px_BasicLatin) {
	font = &font_Liberation_MonoBold_83px;
	unicode_block = &unicode_block_Liberation_MonoBold_83px_BasicLatin;
	expectation_name = "Liberation_MonoBold_83px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_83px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_83px;
	unicode_block = &unicode_block_Liberation_MonoBold_83px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_83px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_83px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_83px;
	unicode_block = &unicode_block_Liberation_MonoBold_83px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_83px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBold_88px_BasicLatin) {
	font = &font_Liberation_MonoBold_88px;
	unicode_block = &unicode_block_Liberation_MonoBold_88px_BasicLatin;
	expectation_name = "Liberation_MonoBold_88px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_88px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_88px;
	unicode_block = &unicode_block_Liberation_MonoBold_88px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_88px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_88px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_88px;
	unicode_block = &unicode_block_Liberation_MonoBold_88px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_88px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBold_8px_BasicLatin) {
	font = &font_Liberation_MonoBold_8px;
	unicode_block = &unicode_block_Liberation_MonoBold_8px_BasicLatin;
	expectation_name = "Liberation_MonoBold_8px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_8px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_8px;
	unicode_block = &unicode_block_Liberation_MonoBold_8px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_8px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_8px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_8px;
	unicode_block = &unicode_block_Liberation_MonoBold_8px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_8px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBold_96px_BasicLatin) {
	font = &font_Liberation_MonoBold_96px;
	unicode_block = &unicode_block_Liberation_MonoBold_96px_BasicLatin;
	expectation_name = "Liberation_MonoBold_96px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_96px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_96px;
	unicode_block = &unicode_block_Liberation_MonoBold_96px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_96px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_96px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_96px;
	unicode_block = &unicode_block_Liberation_MonoBold_96px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_96px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBold_9px_BasicLatin) {
	font = &font_Liberation_MonoBold_9px;
	unicode_block = &unicode_block_Liberation_MonoBold_9px_BasicLatin;
	expectation_name = "Liberation_MonoBold_9px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBold_9px_Latin1Supplement) {
	font = &font_Liberation_MonoBold_9px;
	unicode_block = &unicode_block_Liberation_MonoBold_9px_Latin1Supplement;
	expectation_name = "Liberation_MonoBold_9px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBold_9px_LatinExtendedA) {
	font = &font_Liberation_MonoBold_9px;
	unicode_block = &unicode_block_Liberation_MonoBold_9px_LatinExtendedA;
	expectation_name = "Liberation_MonoBold_9px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_10px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_10px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_10px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_10px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_10px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_10px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_10px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_10px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_10px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_10px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_10px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_10px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_11px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_11px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_11px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_11px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_11px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_11px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_11px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_11px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_11px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_11px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_11px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_11px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_12px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_12px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_12px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_12px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_12px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_12px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_12px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_12px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_12px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_12px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_12px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_12px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_13px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_13px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_13px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_13px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_13px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_13px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_13px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_13px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_13px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_13px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_13px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_13px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_14px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_14px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_14px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_14px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_14px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_14px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_14px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_14px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_14px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_14px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_14px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_14px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_15px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_15px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_15px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_15px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_15px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_15px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_15px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_15px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_15px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_15px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_15px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_15px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_16px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_16px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_16px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_16px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_16px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_16px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_16px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_16px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_16px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_16px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_16px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_16px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_18px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_18px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_18px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_18px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_18px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_18px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_18px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_18px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_18px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_18px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_18px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_18px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_20px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_20px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_20px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_20px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_20px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_20px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_20px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_20px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_20px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_20px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_20px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_20px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_22px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_22px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_22px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_22px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_22px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_22px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_22px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_22px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_22px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_22px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_22px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_22px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_24px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_24px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_24px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_24px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_24px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_24px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_24px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_24px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_24px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_24px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_24px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_24px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_26px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_26px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_26px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_26px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_26px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_26px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_26px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_26px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_26px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_26px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_26px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_26px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_28px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_28px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_28px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_28px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_28px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_28px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_28px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_28px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_28px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_28px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_28px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_28px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_32px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_32px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_32px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_32px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_32px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_32px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_32px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_32px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_32px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_32px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_32px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_32px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_36px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_36px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_36px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_36px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_36px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_36px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_36px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_36px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_36px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_36px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_36px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_36px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_40px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_40px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_40px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_40px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_40px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_40px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_40px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_40px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_40px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_40px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_40px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_40px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_44px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_44px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_44px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_44px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_44px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_44px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_44px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_44px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_44px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_44px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_44px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_44px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_48px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_48px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_48px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_48px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_48px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_48px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_48px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_48px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_48px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_48px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_48px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_48px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_54px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_54px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_54px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_54px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_54px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_54px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_54px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_54px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_54px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_54px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_54px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_54px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_60px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_60px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_60px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_60px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_60px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_60px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_60px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_60px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_60px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_60px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_60px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_60px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_66px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_66px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_66px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_66px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_66px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_66px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_66px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_66px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_66px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_66px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_66px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_66px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_72px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_72px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_72px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_72px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_72px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_72px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_72px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_72px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_72px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_72px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_72px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_72px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_7px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_7px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_7px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_7px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_7px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_7px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_7px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_7px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_7px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_7px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_7px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_7px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_80px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_80px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_80px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_80px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_80px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_80px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_80px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_80px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_80px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_80px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_80px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_80px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_83px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_83px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_83px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_83px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_83px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_83px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_83px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_83px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_83px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_83px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_83px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_83px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_88px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_88px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_88px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_88px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_88px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_88px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_88px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_88px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_88px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_88px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_88px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_88px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_8px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_8px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_8px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_8px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_8px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_8px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_8px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_8px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_8px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_8px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_8px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_8px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_96px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_96px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_96px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_96px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_96px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_96px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_96px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_96px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_96px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_96px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_96px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_96px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_9px_BasicLatin) {
	font = &font_Liberation_MonoBoldItalic_9px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_9px_BasicLatin;
	expectation_name = "Liberation_MonoBoldItalic_9px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_9px_Latin1Supplement) {
	font = &font_Liberation_MonoBoldItalic_9px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_9px_Latin1Supplement;
	expectation_name = "Liberation_MonoBoldItalic_9px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoBoldItalic_9px_LatinExtendedA) {
	font = &font_Liberation_MonoBoldItalic_9px;
	unicode_block = &unicode_block_Liberation_MonoBoldItalic_9px_LatinExtendedA;
	expectation_name = "Liberation_MonoBoldItalic_9px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_10px_BasicLatin) {
	font = &font_Liberation_MonoItalic_10px;
	unicode_block = &unicode_block_Liberation_MonoItalic_10px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_10px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_10px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_10px;
	unicode_block = &unicode_block_Liberation_MonoItalic_10px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_10px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_10px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_10px;
	unicode_block = &unicode_block_Liberation_MonoItalic_10px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_10px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_11px_BasicLatin) {
	font = &font_Liberation_MonoItalic_11px;
	unicode_block = &unicode_block_Liberation_MonoItalic_11px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_11px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_11px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_11px;
	unicode_block = &unicode_block_Liberation_MonoItalic_11px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_11px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_11px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_11px;
	unicode_block = &unicode_block_Liberation_MonoItalic_11px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_11px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_12px_BasicLatin) {
	font = &font_Liberation_MonoItalic_12px;
	unicode_block = &unicode_block_Liberation_MonoItalic_12px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_12px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_12px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_12px;
	unicode_block = &unicode_block_Liberation_MonoItalic_12px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_12px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_12px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_12px;
	unicode_block = &unicode_block_Liberation_MonoItalic_12px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_12px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_13px_BasicLatin) {
	font = &font_Liberation_MonoItalic_13px;
	unicode_block = &unicode_block_Liberation_MonoItalic_13px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_13px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_13px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_13px;
	unicode_block = &unicode_block_Liberation_MonoItalic_13px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_13px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_13px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_13px;
	unicode_block = &unicode_block_Liberation_MonoItalic_13px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_13px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_14px_BasicLatin) {
	font = &font_Liberation_MonoItalic_14px;
	unicode_block = &unicode_block_Liberation_MonoItalic_14px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_14px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_14px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_14px;
	unicode_block = &unicode_block_Liberation_MonoItalic_14px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_14px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_14px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_14px;
	unicode_block = &unicode_block_Liberation_MonoItalic_14px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_14px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_15px_BasicLatin) {
	font = &font_Liberation_MonoItalic_15px;
	unicode_block = &unicode_block_Liberation_MonoItalic_15px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_15px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_15px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_15px;
	unicode_block = &unicode_block_Liberation_MonoItalic_15px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_15px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_15px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_15px;
	unicode_block = &unicode_block_Liberation_MonoItalic_15px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_15px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_16px_BasicLatin) {
	font = &font_Liberation_MonoItalic_16px;
	unicode_block = &unicode_block_Liberation_MonoItalic_16px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_16px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_16px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_16px;
	unicode_block = &unicode_block_Liberation_MonoItalic_16px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_16px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_16px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_16px;
	unicode_block = &unicode_block_Liberation_MonoItalic_16px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_16px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_18px_BasicLatin) {
	font = &font_Liberation_MonoItalic_18px;
	unicode_block = &unicode_block_Liberation_MonoItalic_18px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_18px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_18px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_18px;
	unicode_block = &unicode_block_Liberation_MonoItalic_18px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_18px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_18px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_18px;
	unicode_block = &unicode_block_Liberation_MonoItalic_18px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_18px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_20px_BasicLatin) {
	font = &font_Liberation_MonoItalic_20px;
	unicode_block = &unicode_block_Liberation_MonoItalic_20px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_20px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_20px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_20px;
	unicode_block = &unicode_block_Liberation_MonoItalic_20px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_20px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_20px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_20px;
	unicode_block = &unicode_block_Liberation_MonoItalic_20px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_20px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_22px_BasicLatin) {
	font = &font_Liberation_MonoItalic_22px;
	unicode_block = &unicode_block_Liberation_MonoItalic_22px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_22px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_22px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_22px;
	unicode_block = &unicode_block_Liberation_MonoItalic_22px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_22px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_22px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_22px;
	unicode_block = &unicode_block_Liberation_MonoItalic_22px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_22px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_24px_BasicLatin) {
	font = &font_Liberation_MonoItalic_24px;
	unicode_block = &unicode_block_Liberation_MonoItalic_24px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_24px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_24px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_24px;
	unicode_block = &unicode_block_Liberation_MonoItalic_24px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_24px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_24px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_24px;
	unicode_block = &unicode_block_Liberation_MonoItalic_24px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_24px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_26px_BasicLatin) {
	font = &font_Liberation_MonoItalic_26px;
	unicode_block = &unicode_block_Liberation_MonoItalic_26px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_26px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_26px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_26px;
	unicode_block = &unicode_block_Liberation_MonoItalic_26px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_26px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_26px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_26px;
	unicode_block = &unicode_block_Liberation_MonoItalic_26px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_26px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_28px_BasicLatin) {
	font = &font_Liberation_MonoItalic_28px;
	unicode_block = &unicode_block_Liberation_MonoItalic_28px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_28px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_28px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_28px;
	unicode_block = &unicode_block_Liberation_MonoItalic_28px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_28px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_28px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_28px;
	unicode_block = &unicode_block_Liberation_MonoItalic_28px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_28px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_32px_BasicLatin) {
	font = &font_Liberation_MonoItalic_32px;
	unicode_block = &unicode_block_Liberation_MonoItalic_32px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_32px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_32px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_32px;
	unicode_block = &unicode_block_Liberation_MonoItalic_32px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_32px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_32px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_32px;
	unicode_block = &unicode_block_Liberation_MonoItalic_32px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_32px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_36px_BasicLatin) {
	font = &font_Liberation_MonoItalic_36px;
	unicode_block = &unicode_block_Liberation_MonoItalic_36px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_36px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_36px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_36px;
	unicode_block = &unicode_block_Liberation_MonoItalic_36px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_36px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_36px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_36px;
	unicode_block = &unicode_block_Liberation_MonoItalic_36px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_36px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_40px_BasicLatin) {
	font = &font_Liberation_MonoItalic_40px;
	unicode_block = &unicode_block_Liberation_MonoItalic_40px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_40px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_40px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_40px;
	unicode_block = &unicode_block_Liberation_MonoItalic_40px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_40px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_40px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_40px;
	unicode_block = &unicode_block_Liberation_MonoItalic_40px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_40px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_44px_BasicLatin) {
	font = &font_Liberation_MonoItalic_44px;
	unicode_block = &unicode_block_Liberation_MonoItalic_44px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_44px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_44px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_44px;
	unicode_block = &unicode_block_Liberation_MonoItalic_44px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_44px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_44px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_44px;
	unicode_block = &unicode_block_Liberation_MonoItalic_44px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_44px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_48px_BasicLatin) {
	font = &font_Liberation_MonoItalic_48px;
	unicode_block = &unicode_block_Liberation_MonoItalic_48px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_48px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_48px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_48px;
	unicode_block = &unicode_block_Liberation_MonoItalic_48px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_48px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_48px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_48px;
	unicode_block = &unicode_block_Liberation_MonoItalic_48px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_48px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_54px_BasicLatin) {
	font = &font_Liberation_MonoItalic_54px;
	unicode_block = &unicode_block_Liberation_MonoItalic_54px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_54px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_54px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_54px;
	unicode_block = &unicode_block_Liberation_MonoItalic_54px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_54px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_54px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_54px;
	unicode_block = &unicode_block_Liberation_MonoItalic_54px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_54px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_60px_BasicLatin) {
	font = &font_Liberation_MonoItalic_60px;
	unicode_block = &unicode_block_Liberation_MonoItalic_60px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_60px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_60px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_60px;
	unicode_block = &unicode_block_Liberation_MonoItalic_60px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_60px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_60px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_60px;
	unicode_block = &unicode_block_Liberation_MonoItalic_60px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_60px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_66px_BasicLatin) {
	font = &font_Liberation_MonoItalic_66px;
	unicode_block = &unicode_block_Liberation_MonoItalic_66px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_66px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_66px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_66px;
	unicode_block = &unicode_block_Liberation_MonoItalic_66px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_66px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_66px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_66px;
	unicode_block = &unicode_block_Liberation_MonoItalic_66px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_66px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_72px_BasicLatin) {
	font = &font_Liberation_MonoItalic_72px;
	unicode_block = &unicode_block_Liberation_MonoItalic_72px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_72px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_72px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_72px;
	unicode_block = &unicode_block_Liberation_MonoItalic_72px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_72px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_72px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_72px;
	unicode_block = &unicode_block_Liberation_MonoItalic_72px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_72px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_7px_BasicLatin) {
	font = &font_Liberation_MonoItalic_7px;
	unicode_block = &unicode_block_Liberation_MonoItalic_7px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_7px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_7px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_7px;
	unicode_block = &unicode_block_Liberation_MonoItalic_7px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_7px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_7px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_7px;
	unicode_block = &unicode_block_Liberation_MonoItalic_7px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_7px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_80px_BasicLatin) {
	font = &font_Liberation_MonoItalic_80px;
	unicode_block = &unicode_block_Liberation_MonoItalic_80px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_80px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_80px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_80px;
	unicode_block = &unicode_block_Liberation_MonoItalic_80px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_80px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_80px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_80px;
	unicode_block = &unicode_block_Liberation_MonoItalic_80px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_80px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_83px_BasicLatin) {
	font = &font_Liberation_MonoItalic_83px;
	unicode_block = &unicode_block_Liberation_MonoItalic_83px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_83px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_83px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_83px;
	unicode_block = &unicode_block_Liberation_MonoItalic_83px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_83px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_83px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_83px;
	unicode_block = &unicode_block_Liberation_MonoItalic_83px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_83px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_88px_BasicLatin) {
	font = &font_Liberation_MonoItalic_88px;
	unicode_block = &unicode_block_Liberation_MonoItalic_88px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_88px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_88px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_88px;
	unicode_block = &unicode_block_Liberation_MonoItalic_88px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_88px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_88px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_88px;
	unicode_block = &unicode_block_Liberation_MonoItalic_88px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_88px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_8px_BasicLatin) {
	font = &font_Liberation_MonoItalic_8px;
	unicode_block = &unicode_block_Liberation_MonoItalic_8px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_8px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_8px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_8px;
	unicode_block = &unicode_block_Liberation_MonoItalic_8px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_8px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_8px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_8px;
	unicode_block = &unicode_block_Liberation_MonoItalic_8px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_8px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_96px_BasicLatin) {
	font = &font_Liberation_MonoItalic_96px;
	unicode_block = &unicode_block_Liberation_MonoItalic_96px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_96px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_96px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_96px;
	unicode_block = &unicode_block_Liberation_MonoItalic_96px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_96px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_96px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_96px;
	unicode_block = &unicode_block_Liberation_MonoItalic_96px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_96px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoItalic_9px_BasicLatin) {
	font = &font_Liberation_MonoItalic_9px;
	unicode_block = &unicode_block_Liberation_MonoItalic_9px_BasicLatin;
	expectation_name = "Liberation_MonoItalic_9px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoItalic_9px_Latin1Supplement) {
	font = &font_Liberation_MonoItalic_9px;
	unicode_block = &unicode_block_Liberation_MonoItalic_9px_Latin1Supplement;
	expectation_name = "Liberation_MonoItalic_9px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoItalic_9px_LatinExtendedA) {
	font = &font_Liberation_MonoItalic_9px;
	unicode_block = &unicode_block_Liberation_MonoItalic_9px_LatinExtendedA;
	expectation_name = "Liberation_MonoItalic_9px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_10px_BasicLatin) {
	font = &font_Liberation_MonoRegular_10px;
	unicode_block = &unicode_block_Liberation_MonoRegular_10px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_10px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_10px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_10px;
	unicode_block = &unicode_block_Liberation_MonoRegular_10px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_10px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_10px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_10px;
	unicode_block = &unicode_block_Liberation_MonoRegular_10px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_10px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_11px_BasicLatin) {
	font = &font_Liberation_MonoRegular_11px;
	unicode_block = &unicode_block_Liberation_MonoRegular_11px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_11px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_11px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_11px;
	unicode_block = &unicode_block_Liberation_MonoRegular_11px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_11px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_11px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_11px;
	unicode_block = &unicode_block_Liberation_MonoRegular_11px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_11px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_12px_BasicLatin) {
	font = &font_Liberation_MonoRegular_12px;
	unicode_block = &unicode_block_Liberation_MonoRegular_12px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_12px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_12px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_12px;
	unicode_block = &unicode_block_Liberation_MonoRegular_12px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_12px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_12px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_12px;
	unicode_block = &unicode_block_Liberation_MonoRegular_12px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_12px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_13px_BasicLatin) {
	font = &font_Liberation_MonoRegular_13px;
	unicode_block = &unicode_block_Liberation_MonoRegular_13px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_13px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_13px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_13px;
	unicode_block = &unicode_block_Liberation_MonoRegular_13px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_13px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_13px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_13px;
	unicode_block = &unicode_block_Liberation_MonoRegular_13px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_13px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_14px_BasicLatin) {
	font = &font_Liberation_MonoRegular_14px;
	unicode_block = &unicode_block_Liberation_MonoRegular_14px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_14px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_14px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_14px;
	unicode_block = &unicode_block_Liberation_MonoRegular_14px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_14px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_14px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_14px;
	unicode_block = &unicode_block_Liberation_MonoRegular_14px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_14px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_15px_BasicLatin) {
	font = &font_Liberation_MonoRegular_15px;
	unicode_block = &unicode_block_Liberation_MonoRegular_15px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_15px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_15px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_15px;
	unicode_block = &unicode_block_Liberation_MonoRegular_15px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_15px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_15px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_15px;
	unicode_block = &unicode_block_Liberation_MonoRegular_15px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_15px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_16px_BasicLatin) {
	font = &font_Liberation_MonoRegular_16px;
	unicode_block = &unicode_block_Liberation_MonoRegular_16px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_16px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_16px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_16px;
	unicode_block = &unicode_block_Liberation_MonoRegular_16px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_16px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_16px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_16px;
	unicode_block = &unicode_block_Liberation_MonoRegular_16px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_16px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_18px_BasicLatin) {
	font = &font_Liberation_MonoRegular_18px;
	unicode_block = &unicode_block_Liberation_MonoRegular_18px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_18px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_18px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_18px;
	unicode_block = &unicode_block_Liberation_MonoRegular_18px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_18px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_18px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_18px;
	unicode_block = &unicode_block_Liberation_MonoRegular_18px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_18px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_20px_BasicLatin) {
	font = &font_Liberation_MonoRegular_20px;
	unicode_block = &unicode_block_Liberation_MonoRegular_20px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_20px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_20px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_20px;
	unicode_block = &unicode_block_Liberation_MonoRegular_20px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_20px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_20px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_20px;
	unicode_block = &unicode_block_Liberation_MonoRegular_20px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_20px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_22px_BasicLatin) {
	font = &font_Liberation_MonoRegular_22px;
	unicode_block = &unicode_block_Liberation_MonoRegular_22px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_22px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_22px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_22px;
	unicode_block = &unicode_block_Liberation_MonoRegular_22px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_22px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_22px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_22px;
	unicode_block = &unicode_block_Liberation_MonoRegular_22px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_22px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_24px_BasicLatin) {
	font = &font_Liberation_MonoRegular_24px;
	unicode_block = &unicode_block_Liberation_MonoRegular_24px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_24px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_24px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_24px;
	unicode_block = &unicode_block_Liberation_MonoRegular_24px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_24px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_24px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_24px;
	unicode_block = &unicode_block_Liberation_MonoRegular_24px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_24px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_26px_BasicLatin) {
	font = &font_Liberation_MonoRegular_26px;
	unicode_block = &unicode_block_Liberation_MonoRegular_26px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_26px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_26px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_26px;
	unicode_block = &unicode_block_Liberation_MonoRegular_26px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_26px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_26px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_26px;
	unicode_block = &unicode_block_Liberation_MonoRegular_26px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_26px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_28px_BasicLatin) {
	font = &font_Liberation_MonoRegular_28px;
	unicode_block = &unicode_block_Liberation_MonoRegular_28px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_28px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_28px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_28px;
	unicode_block = &unicode_block_Liberation_MonoRegular_28px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_28px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_28px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_28px;
	unicode_block = &unicode_block_Liberation_MonoRegular_28px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_28px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_32px_BasicLatin) {
	font = &font_Liberation_MonoRegular_32px;
	unicode_block = &unicode_block_Liberation_MonoRegular_32px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_32px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_32px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_32px;
	unicode_block = &unicode_block_Liberation_MonoRegular_32px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_32px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_32px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_32px;
	unicode_block = &unicode_block_Liberation_MonoRegular_32px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_32px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_36px_BasicLatin) {
	font = &font_Liberation_MonoRegular_36px;
	unicode_block = &unicode_block_Liberation_MonoRegular_36px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_36px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_36px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_36px;
	unicode_block = &unicode_block_Liberation_MonoRegular_36px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_36px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_36px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_36px;
	unicode_block = &unicode_block_Liberation_MonoRegular_36px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_36px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_40px_BasicLatin) {
	font = &font_Liberation_MonoRegular_40px;
	unicode_block = &unicode_block_Liberation_MonoRegular_40px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_40px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_40px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_40px;
	unicode_block = &unicode_block_Liberation_MonoRegular_40px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_40px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_40px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_40px;
	unicode_block = &unicode_block_Liberation_MonoRegular_40px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_40px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_44px_BasicLatin) {
	font = &font_Liberation_MonoRegular_44px;
	unicode_block = &unicode_block_Liberation_MonoRegular_44px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_44px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_44px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_44px;
	unicode_block = &unicode_block_Liberation_MonoRegular_44px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_44px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_44px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_44px;
	unicode_block = &unicode_block_Liberation_MonoRegular_44px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_44px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_48px_BasicLatin) {
	font = &font_Liberation_MonoRegular_48px;
	unicode_block = &unicode_block_Liberation_MonoRegular_48px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_48px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_48px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_48px;
	unicode_block = &unicode_block_Liberation_MonoRegular_48px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_48px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_48px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_48px;
	unicode_block = &unicode_block_Liberation_MonoRegular_48px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_48px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_54px_BasicLatin) {
	font = &font_Liberation_MonoRegular_54px;
	unicode_block = &unicode_block_Liberation_MonoRegular_54px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_54px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_54px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_54px;
	unicode_block = &unicode_block_Liberation_MonoRegular_54px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_54px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_54px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_54px;
	unicode_block = &unicode_block_Liberation_MonoRegular_54px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_54px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_60px_BasicLatin) {
	font = &font_Liberation_MonoRegular_60px;
	unicode_block = &unicode_block_Liberation_MonoRegular_60px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_60px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_60px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_60px;
	unicode_block = &unicode_block_Liberation_MonoRegular_60px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_60px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_60px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_60px;
	unicode_block = &unicode_block_Liberation_MonoRegular_60px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_60px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_66px_BasicLatin) {
	font = &font_Liberation_MonoRegular_66px;
	unicode_block = &unicode_block_Liberation_MonoRegular_66px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_66px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_66px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_66px;
	unicode_block = &unicode_block_Liberation_MonoRegular_66px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_66px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_66px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_66px;
	unicode_block = &unicode_block_Liberation_MonoRegular_66px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_66px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_72px_BasicLatin) {
	font = &font_Liberation_MonoRegular_72px;
	unicode_block = &unicode_block_Liberation_MonoRegular_72px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_72px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_72px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_72px;
	unicode_block = &unicode_block_Liberation_MonoRegular_72px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_72px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_72px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_72px;
	unicode_block = &unicode_block_Liberation_MonoRegular_72px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_72px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_7px_BasicLatin) {
	font = &font_Liberation_MonoRegular_7px;
	unicode_block = &unicode_block_Liberation_MonoRegular_7px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_7px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_7px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_7px;
	unicode_block = &unicode_block_Liberation_MonoRegular_7px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_7px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_7px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_7px;
	unicode_block = &unicode_block_Liberation_MonoRegular_7px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_7px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_80px_BasicLatin) {
	font = &font_Liberation_MonoRegular_80px;
	unicode_block = &unicode_block_Liberation_MonoRegular_80px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_80px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_80px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_80px;
	unicode_block = &unicode_block_Liberation_MonoRegular_80px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_80px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_80px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_80px;
	unicode_block = &unicode_block_Liberation_MonoRegular_80px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_80px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_83px_BasicLatin) {
	font = &font_Liberation_MonoRegular_83px;
	unicode_block = &unicode_block_Liberation_MonoRegular_83px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_83px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_83px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_83px;
	unicode_block = &unicode_block_Liberation_MonoRegular_83px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_83px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_83px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_83px;
	unicode_block = &unicode_block_Liberation_MonoRegular_83px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_83px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_88px_BasicLatin) {
	font = &font_Liberation_MonoRegular_88px;
	unicode_block = &unicode_block_Liberation_MonoRegular_88px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_88px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_88px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_88px;
	unicode_block = &unicode_block_Liberation_MonoRegular_88px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_88px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_88px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_88px;
	unicode_block = &unicode_block_Liberation_MonoRegular_88px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_88px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_8px_BasicLatin) {
	font = &font_Liberation_MonoRegular_8px;
	unicode_block = &unicode_block_Liberation_MonoRegular_8px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_8px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_8px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_8px;
	unicode_block = &unicode_block_Liberation_MonoRegular_8px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_8px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_8px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_8px;
	unicode_block = &unicode_block_Liberation_MonoRegular_8px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_8px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_96px_BasicLatin) {
	font = &font_Liberation_MonoRegular_96px;
	unicode_block = &unicode_block_Liberation_MonoRegular_96px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_96px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_96px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_96px;
	unicode_block = &unicode_block_Liberation_MonoRegular_96px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_96px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_96px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_96px;
	unicode_block = &unicode_block_Liberation_MonoRegular_96px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_96px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_MonoRegular_9px_BasicLatin) {
	font = &font_Liberation_MonoRegular_9px;
	unicode_block = &unicode_block_Liberation_MonoRegular_9px_BasicLatin;
	expectation_name = "Liberation_MonoRegular_9px_BasicLatin";
}END_TEST

START_TEST(Liberation_MonoRegular_9px_Latin1Supplement) {
	font = &font_Liberation_MonoRegular_9px;
	unicode_block = &unicode_block_Liberation_MonoRegular_9px_Latin1Supplement;
	expectation_name = "Liberation_MonoRegular_9px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_MonoRegular_9px_LatinExtendedA) {
	font = &font_Liberation_MonoRegular_9px;
	unicode_block = &unicode_block_Liberation_MonoRegular_9px_LatinExtendedA;
	expectation_name = "Liberation_MonoRegular_9px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_10px_BasicLatin) {
	font = &font_Liberation_SansBold_10px;
	unicode_block = &unicode_block_Liberation_SansBold_10px_BasicLatin;
	expectation_name = "Liberation_SansBold_10px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_10px_Latin1Supplement) {
	font = &font_Liberation_SansBold_10px;
	unicode_block = &unicode_block_Liberation_SansBold_10px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_10px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_10px_LatinExtendedA) {
	font = &font_Liberation_SansBold_10px;
	unicode_block = &unicode_block_Liberation_SansBold_10px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_10px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_11px_BasicLatin) {
	font = &font_Liberation_SansBold_11px;
	unicode_block = &unicode_block_Liberation_SansBold_11px_BasicLatin;
	expectation_name = "Liberation_SansBold_11px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_11px_Latin1Supplement) {
	font = &font_Liberation_SansBold_11px;
	unicode_block = &unicode_block_Liberation_SansBold_11px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_11px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_11px_LatinExtendedA) {
	font = &font_Liberation_SansBold_11px;
	unicode_block = &unicode_block_Liberation_SansBold_11px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_11px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_12px_BasicLatin) {
	font = &font_Liberation_SansBold_12px;
	unicode_block = &unicode_block_Liberation_SansBold_12px_BasicLatin;
	expectation_name = "Liberation_SansBold_12px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_12px_Latin1Supplement) {
	font = &font_Liberation_SansBold_12px;
	unicode_block = &unicode_block_Liberation_SansBold_12px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_12px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_12px_LatinExtendedA) {
	font = &font_Liberation_SansBold_12px;
	unicode_block = &unicode_block_Liberation_SansBold_12px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_12px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_13px_BasicLatin) {
	font = &font_Liberation_SansBold_13px;
	unicode_block = &unicode_block_Liberation_SansBold_13px_BasicLatin;
	expectation_name = "Liberation_SansBold_13px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_13px_Latin1Supplement) {
	font = &font_Liberation_SansBold_13px;
	unicode_block = &unicode_block_Liberation_SansBold_13px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_13px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_13px_LatinExtendedA) {
	font = &font_Liberation_SansBold_13px;
	unicode_block = &unicode_block_Liberation_SansBold_13px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_13px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_14px_BasicLatin) {
	font = &font_Liberation_SansBold_14px;
	unicode_block = &unicode_block_Liberation_SansBold_14px_BasicLatin;
	expectation_name = "Liberation_SansBold_14px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_14px_Latin1Supplement) {
	font = &font_Liberation_SansBold_14px;
	unicode_block = &unicode_block_Liberation_SansBold_14px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_14px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_14px_LatinExtendedA) {
	font = &font_Liberation_SansBold_14px;
	unicode_block = &unicode_block_Liberation_SansBold_14px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_14px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_15px_BasicLatin) {
	font = &font_Liberation_SansBold_15px;
	unicode_block = &unicode_block_Liberation_SansBold_15px_BasicLatin;
	expectation_name = "Liberation_SansBold_15px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_15px_Latin1Supplement) {
	font = &font_Liberation_SansBold_15px;
	unicode_block = &unicode_block_Liberation_SansBold_15px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_15px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_15px_LatinExtendedA) {
	font = &font_Liberation_SansBold_15px;
	unicode_block = &unicode_block_Liberation_SansBold_15px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_15px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_16px_BasicLatin) {
	font = &font_Liberation_SansBold_16px;
	unicode_block = &unicode_block_Liberation_SansBold_16px_BasicLatin;
	expectation_name = "Liberation_SansBold_16px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_16px_Latin1Supplement) {
	font = &font_Liberation_SansBold_16px;
	unicode_block = &unicode_block_Liberation_SansBold_16px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_16px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_16px_LatinExtendedA) {
	font = &font_Liberation_SansBold_16px;
	unicode_block = &unicode_block_Liberation_SansBold_16px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_16px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_18px_BasicLatin) {
	font = &font_Liberation_SansBold_18px;
	unicode_block = &unicode_block_Liberation_SansBold_18px_BasicLatin;
	expectation_name = "Liberation_SansBold_18px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_18px_Latin1Supplement) {
	font = &font_Liberation_SansBold_18px;
	unicode_block = &unicode_block_Liberation_SansBold_18px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_18px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_18px_LatinExtendedA) {
	font = &font_Liberation_SansBold_18px;
	unicode_block = &unicode_block_Liberation_SansBold_18px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_18px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_20px_BasicLatin) {
	font = &font_Liberation_SansBold_20px;
	unicode_block = &unicode_block_Liberation_SansBold_20px_BasicLatin;
	expectation_name = "Liberation_SansBold_20px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_20px_Latin1Supplement) {
	font = &font_Liberation_SansBold_20px;
	unicode_block = &unicode_block_Liberation_SansBold_20px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_20px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_20px_LatinExtendedA) {
	font = &font_Liberation_SansBold_20px;
	unicode_block = &unicode_block_Liberation_SansBold_20px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_20px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_22px_BasicLatin) {
	font = &font_Liberation_SansBold_22px;
	unicode_block = &unicode_block_Liberation_SansBold_22px_BasicLatin;
	expectation_name = "Liberation_SansBold_22px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_22px_Latin1Supplement) {
	font = &font_Liberation_SansBold_22px;
	unicode_block = &unicode_block_Liberation_SansBold_22px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_22px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_22px_LatinExtendedA) {
	font = &font_Liberation_SansBold_22px;
	unicode_block = &unicode_block_Liberation_SansBold_22px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_22px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_24px_BasicLatin) {
	font = &font_Liberation_SansBold_24px;
	unicode_block = &unicode_block_Liberation_SansBold_24px_BasicLatin;
	expectation_name = "Liberation_SansBold_24px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_24px_Latin1Supplement) {
	font = &font_Liberation_SansBold_24px;
	unicode_block = &unicode_block_Liberation_SansBold_24px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_24px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_24px_LatinExtendedA) {
	font = &font_Liberation_SansBold_24px;
	unicode_block = &unicode_block_Liberation_SansBold_24px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_24px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_26px_BasicLatin) {
	font = &font_Liberation_SansBold_26px;
	unicode_block = &unicode_block_Liberation_SansBold_26px_BasicLatin;
	expectation_name = "Liberation_SansBold_26px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_26px_Latin1Supplement) {
	font = &font_Liberation_SansBold_26px;
	unicode_block = &unicode_block_Liberation_SansBold_26px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_26px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_26px_LatinExtendedA) {
	font = &font_Liberation_SansBold_26px;
	unicode_block = &unicode_block_Liberation_SansBold_26px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_26px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_28px_BasicLatin) {
	font = &font_Liberation_SansBold_28px;
	unicode_block = &unicode_block_Liberation_SansBold_28px_BasicLatin;
	expectation_name = "Liberation_SansBold_28px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_28px_Latin1Supplement) {
	font = &font_Liberation_SansBold_28px;
	unicode_block = &unicode_block_Liberation_SansBold_28px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_28px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_28px_LatinExtendedA) {
	font = &font_Liberation_SansBold_28px;
	unicode_block = &unicode_block_Liberation_SansBold_28px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_28px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_32px_BasicLatin) {
	font = &font_Liberation_SansBold_32px;
	unicode_block = &unicode_block_Liberation_SansBold_32px_BasicLatin;
	expectation_name = "Liberation_SansBold_32px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_32px_Latin1Supplement) {
	font = &font_Liberation_SansBold_32px;
	unicode_block = &unicode_block_Liberation_SansBold_32px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_32px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_32px_LatinExtendedA) {
	font = &font_Liberation_SansBold_32px;
	unicode_block = &unicode_block_Liberation_SansBold_32px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_32px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_36px_BasicLatin) {
	font = &font_Liberation_SansBold_36px;
	unicode_block = &unicode_block_Liberation_SansBold_36px_BasicLatin;
	expectation_name = "Liberation_SansBold_36px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_36px_Latin1Supplement) {
	font = &font_Liberation_SansBold_36px;
	unicode_block = &unicode_block_Liberation_SansBold_36px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_36px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_36px_LatinExtendedA) {
	font = &font_Liberation_SansBold_36px;
	unicode_block = &unicode_block_Liberation_SansBold_36px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_36px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_40px_BasicLatin) {
	font = &font_Liberation_SansBold_40px;
	unicode_block = &unicode_block_Liberation_SansBold_40px_BasicLatin;
	expectation_name = "Liberation_SansBold_40px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_40px_Latin1Supplement) {
	font = &font_Liberation_SansBold_40px;
	unicode_block = &unicode_block_Liberation_SansBold_40px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_40px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_40px_LatinExtendedA) {
	font = &font_Liberation_SansBold_40px;
	unicode_block = &unicode_block_Liberation_SansBold_40px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_40px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_44px_BasicLatin) {
	font = &font_Liberation_SansBold_44px;
	unicode_block = &unicode_block_Liberation_SansBold_44px_BasicLatin;
	expectation_name = "Liberation_SansBold_44px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_44px_Latin1Supplement) {
	font = &font_Liberation_SansBold_44px;
	unicode_block = &unicode_block_Liberation_SansBold_44px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_44px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_44px_LatinExtendedA) {
	font = &font_Liberation_SansBold_44px;
	unicode_block = &unicode_block_Liberation_SansBold_44px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_44px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_48px_BasicLatin) {
	font = &font_Liberation_SansBold_48px;
	unicode_block = &unicode_block_Liberation_SansBold_48px_BasicLatin;
	expectation_name = "Liberation_SansBold_48px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_48px_Latin1Supplement) {
	font = &font_Liberation_SansBold_48px;
	unicode_block = &unicode_block_Liberation_SansBold_48px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_48px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_48px_LatinExtendedA) {
	font = &font_Liberation_SansBold_48px;
	unicode_block = &unicode_block_Liberation_SansBold_48px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_48px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_54px_BasicLatin) {
	font = &font_Liberation_SansBold_54px;
	unicode_block = &unicode_block_Liberation_SansBold_54px_BasicLatin;
	expectation_name = "Liberation_SansBold_54px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_54px_Latin1Supplement) {
	font = &font_Liberation_SansBold_54px;
	unicode_block = &unicode_block_Liberation_SansBold_54px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_54px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_54px_LatinExtendedA) {
	font = &font_Liberation_SansBold_54px;
	unicode_block = &unicode_block_Liberation_SansBold_54px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_54px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_60px_BasicLatin) {
	font = &font_Liberation_SansBold_60px;
	unicode_block = &unicode_block_Liberation_SansBold_60px_BasicLatin;
	expectation_name = "Liberation_SansBold_60px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_60px_Latin1Supplement) {
	font = &font_Liberation_SansBold_60px;
	unicode_block = &unicode_block_Liberation_SansBold_60px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_60px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_60px_LatinExtendedA) {
	font = &font_Liberation_SansBold_60px;
	unicode_block = &unicode_block_Liberation_SansBold_60px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_60px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_66px_BasicLatin) {
	font = &font_Liberation_SansBold_66px;
	unicode_block = &unicode_block_Liberation_SansBold_66px_BasicLatin;
	expectation_name = "Liberation_SansBold_66px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_66px_Latin1Supplement) {
	font = &font_Liberation_SansBold_66px;
	unicode_block = &unicode_block_Liberation_SansBold_66px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_66px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_66px_LatinExtendedA) {
	font = &font_Liberation_SansBold_66px;
	unicode_block = &unicode_block_Liberation_SansBold_66px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_66px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_72px_BasicLatin) {
	font = &font_Liberation_SansBold_72px;
	unicode_block = &unicode_block_Liberation_SansBold_72px_BasicLatin;
	expectation_name = "Liberation_SansBold_72px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_72px_Latin1Supplement) {
	font = &font_Liberation_SansBold_72px;
	unicode_block = &unicode_block_Liberation_SansBold_72px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_72px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_72px_LatinExtendedA) {
	font = &font_Liberation_SansBold_72px;
	unicode_block = &unicode_block_Liberation_SansBold_72px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_72px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_7px_BasicLatin) {
	font = &font_Liberation_SansBold_7px;
	unicode_block = &unicode_block_Liberation_SansBold_7px_BasicLatin;
	expectation_name = "Liberation_SansBold_7px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_7px_Latin1Supplement) {
	font = &font_Liberation_SansBold_7px;
	unicode_block = &unicode_block_Liberation_SansBold_7px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_7px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_7px_LatinExtendedA) {
	font = &font_Liberation_SansBold_7px;
	unicode_block = &unicode_block_Liberation_SansBold_7px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_7px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_80px_BasicLatin) {
	font = &font_Liberation_SansBold_80px;
	unicode_block = &unicode_block_Liberation_SansBold_80px_BasicLatin;
	expectation_name = "Liberation_SansBold_80px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_80px_Latin1Supplement) {
	font = &font_Liberation_SansBold_80px;
	unicode_block = &unicode_block_Liberation_SansBold_80px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_80px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_80px_LatinExtendedA) {
	font = &font_Liberation_SansBold_80px;
	unicode_block = &unicode_block_Liberation_SansBold_80px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_80px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_83px_BasicLatin) {
	font = &font_Liberation_SansBold_83px;
	unicode_block = &unicode_block_Liberation_SansBold_83px_BasicLatin;
	expectation_name = "Liberation_SansBold_83px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_83px_Latin1Supplement) {
	font = &font_Liberation_SansBold_83px;
	unicode_block = &unicode_block_Liberation_SansBold_83px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_83px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_83px_LatinExtendedA) {
	font = &font_Liberation_SansBold_83px;
	unicode_block = &unicode_block_Liberation_SansBold_83px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_83px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_88px_BasicLatin) {
	font = &font_Liberation_SansBold_88px;
	unicode_block = &unicode_block_Liberation_SansBold_88px_BasicLatin;
	expectation_name = "Liberation_SansBold_88px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_88px_Latin1Supplement) {
	font = &font_Liberation_SansBold_88px;
	unicode_block = &unicode_block_Liberation_SansBold_88px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_88px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_88px_LatinExtendedA) {
	font = &font_Liberation_SansBold_88px;
	unicode_block = &unicode_block_Liberation_SansBold_88px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_88px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_8px_BasicLatin) {
	font = &font_Liberation_SansBold_8px;
	unicode_block = &unicode_block_Liberation_SansBold_8px_BasicLatin;
	expectation_name = "Liberation_SansBold_8px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_8px_Latin1Supplement) {
	font = &font_Liberation_SansBold_8px;
	unicode_block = &unicode_block_Liberation_SansBold_8px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_8px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_8px_LatinExtendedA) {
	font = &font_Liberation_SansBold_8px;
	unicode_block = &unicode_block_Liberation_SansBold_8px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_8px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_96px_BasicLatin) {
	font = &font_Liberation_SansBold_96px;
	unicode_block = &unicode_block_Liberation_SansBold_96px_BasicLatin;
	expectation_name = "Liberation_SansBold_96px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_96px_Latin1Supplement) {
	font = &font_Liberation_SansBold_96px;
	unicode_block = &unicode_block_Liberation_SansBold_96px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_96px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_96px_LatinExtendedA) {
	font = &font_Liberation_SansBold_96px;
	unicode_block = &unicode_block_Liberation_SansBold_96px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_96px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBold_9px_BasicLatin) {
	font = &font_Liberation_SansBold_9px;
	unicode_block = &unicode_block_Liberation_SansBold_9px_BasicLatin;
	expectation_name = "Liberation_SansBold_9px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBold_9px_Latin1Supplement) {
	font = &font_Liberation_SansBold_9px;
	unicode_block = &unicode_block_Liberation_SansBold_9px_Latin1Supplement;
	expectation_name = "Liberation_SansBold_9px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBold_9px_LatinExtendedA) {
	font = &font_Liberation_SansBold_9px;
	unicode_block = &unicode_block_Liberation_SansBold_9px_LatinExtendedA;
	expectation_name = "Liberation_SansBold_9px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_10px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_10px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_10px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_10px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_10px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_10px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_10px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_10px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_10px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_10px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_10px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_10px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_11px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_11px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_11px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_11px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_11px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_11px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_11px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_11px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_11px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_11px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_11px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_11px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_12px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_12px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_12px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_12px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_12px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_12px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_12px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_12px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_12px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_12px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_12px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_12px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_13px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_13px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_13px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_13px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_13px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_13px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_13px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_13px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_13px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_13px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_13px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_13px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_14px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_14px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_14px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_14px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_14px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_14px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_14px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_14px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_14px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_14px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_14px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_14px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_15px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_15px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_15px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_15px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_15px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_15px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_15px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_15px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_15px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_15px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_15px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_15px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_16px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_16px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_16px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_16px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_16px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_16px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_16px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_16px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_16px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_16px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_16px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_16px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_18px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_18px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_18px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_18px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_18px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_18px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_18px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_18px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_18px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_18px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_18px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_18px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_20px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_20px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_20px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_20px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_20px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_20px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_20px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_20px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_20px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_20px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_20px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_20px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_22px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_22px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_22px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_22px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_22px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_22px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_22px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_22px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_22px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_22px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_22px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_22px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_24px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_24px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_24px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_24px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_24px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_24px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_24px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_24px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_24px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_24px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_24px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_24px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_26px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_26px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_26px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_26px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_26px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_26px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_26px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_26px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_26px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_26px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_26px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_26px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_28px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_28px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_28px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_28px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_28px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_28px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_28px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_28px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_28px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_28px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_28px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_28px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_32px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_32px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_32px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_32px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_32px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_32px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_32px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_32px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_32px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_32px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_32px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_32px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_36px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_36px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_36px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_36px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_36px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_36px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_36px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_36px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_36px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_36px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_36px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_36px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_40px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_40px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_40px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_40px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_40px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_40px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_40px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_40px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_40px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_40px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_40px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_40px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_44px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_44px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_44px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_44px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_44px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_44px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_44px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_44px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_44px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_44px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_44px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_44px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_48px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_48px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_48px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_48px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_48px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_48px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_48px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_48px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_48px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_48px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_48px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_48px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_54px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_54px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_54px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_54px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_54px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_54px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_54px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_54px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_54px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_54px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_54px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_54px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_60px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_60px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_60px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_60px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_60px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_60px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_60px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_60px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_60px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_60px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_60px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_60px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_66px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_66px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_66px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_66px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_66px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_66px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_66px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_66px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_66px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_66px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_66px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_66px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_72px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_72px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_72px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_72px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_72px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_72px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_72px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_72px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_72px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_72px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_72px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_72px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_7px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_7px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_7px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_7px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_7px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_7px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_7px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_7px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_7px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_7px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_7px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_7px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_80px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_80px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_80px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_80px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_80px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_80px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_80px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_80px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_80px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_80px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_80px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_80px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_83px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_83px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_83px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_83px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_83px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_83px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_83px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_83px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_83px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_83px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_83px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_83px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_88px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_88px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_88px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_88px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_88px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_88px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_88px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_88px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_88px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_88px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_88px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_88px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_8px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_8px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_8px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_8px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_8px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_8px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_8px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_8px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_8px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_8px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_8px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_8px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_96px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_96px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_96px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_96px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_96px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_96px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_96px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_96px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_96px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_96px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_96px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_96px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansBoldItalic_9px_BasicLatin) {
	font = &font_Liberation_SansBoldItalic_9px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_9px_BasicLatin;
	expectation_name = "Liberation_SansBoldItalic_9px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansBoldItalic_9px_Latin1Supplement) {
	font = &font_Liberation_SansBoldItalic_9px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_9px_Latin1Supplement;
	expectation_name = "Liberation_SansBoldItalic_9px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansBoldItalic_9px_LatinExtendedA) {
	font = &font_Liberation_SansBoldItalic_9px;
	unicode_block = &unicode_block_Liberation_SansBoldItalic_9px_LatinExtendedA;
	expectation_name = "Liberation_SansBoldItalic_9px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_10px_BasicLatin) {
	font = &font_Liberation_SansItalic_10px;
	unicode_block = &unicode_block_Liberation_SansItalic_10px_BasicLatin;
	expectation_name = "Liberation_SansItalic_10px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_10px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_10px;
	unicode_block = &unicode_block_Liberation_SansItalic_10px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_10px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_10px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_10px;
	unicode_block = &unicode_block_Liberation_SansItalic_10px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_10px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_11px_BasicLatin) {
	font = &font_Liberation_SansItalic_11px;
	unicode_block = &unicode_block_Liberation_SansItalic_11px_BasicLatin;
	expectation_name = "Liberation_SansItalic_11px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_11px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_11px;
	unicode_block = &unicode_block_Liberation_SansItalic_11px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_11px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_11px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_11px;
	unicode_block = &unicode_block_Liberation_SansItalic_11px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_11px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_12px_BasicLatin) {
	font = &font_Liberation_SansItalic_12px;
	unicode_block = &unicode_block_Liberation_SansItalic_12px_BasicLatin;
	expectation_name = "Liberation_SansItalic_12px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_12px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_12px;
	unicode_block = &unicode_block_Liberation_SansItalic_12px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_12px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_12px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_12px;
	unicode_block = &unicode_block_Liberation_SansItalic_12px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_12px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_13px_BasicLatin) {
	font = &font_Liberation_SansItalic_13px;
	unicode_block = &unicode_block_Liberation_SansItalic_13px_BasicLatin;
	expectation_name = "Liberation_SansItalic_13px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_13px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_13px;
	unicode_block = &unicode_block_Liberation_SansItalic_13px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_13px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_13px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_13px;
	unicode_block = &unicode_block_Liberation_SansItalic_13px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_13px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_14px_BasicLatin) {
	font = &font_Liberation_SansItalic_14px;
	unicode_block = &unicode_block_Liberation_SansItalic_14px_BasicLatin;
	expectation_name = "Liberation_SansItalic_14px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_14px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_14px;
	unicode_block = &unicode_block_Liberation_SansItalic_14px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_14px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_14px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_14px;
	unicode_block = &unicode_block_Liberation_SansItalic_14px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_14px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_15px_BasicLatin) {
	font = &font_Liberation_SansItalic_15px;
	unicode_block = &unicode_block_Liberation_SansItalic_15px_BasicLatin;
	expectation_name = "Liberation_SansItalic_15px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_15px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_15px;
	unicode_block = &unicode_block_Liberation_SansItalic_15px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_15px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_15px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_15px;
	unicode_block = &unicode_block_Liberation_SansItalic_15px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_15px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_16px_BasicLatin) {
	font = &font_Liberation_SansItalic_16px;
	unicode_block = &unicode_block_Liberation_SansItalic_16px_BasicLatin;
	expectation_name = "Liberation_SansItalic_16px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_16px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_16px;
	unicode_block = &unicode_block_Liberation_SansItalic_16px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_16px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_16px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_16px;
	unicode_block = &unicode_block_Liberation_SansItalic_16px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_16px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_18px_BasicLatin) {
	font = &font_Liberation_SansItalic_18px;
	unicode_block = &unicode_block_Liberation_SansItalic_18px_BasicLatin;
	expectation_name = "Liberation_SansItalic_18px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_18px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_18px;
	unicode_block = &unicode_block_Liberation_SansItalic_18px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_18px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_18px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_18px;
	unicode_block = &unicode_block_Liberation_SansItalic_18px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_18px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_20px_BasicLatin) {
	font = &font_Liberation_SansItalic_20px;
	unicode_block = &unicode_block_Liberation_SansItalic_20px_BasicLatin;
	expectation_name = "Liberation_SansItalic_20px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_20px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_20px;
	unicode_block = &unicode_block_Liberation_SansItalic_20px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_20px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_20px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_20px;
	unicode_block = &unicode_block_Liberation_SansItalic_20px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_20px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_22px_BasicLatin) {
	font = &font_Liberation_SansItalic_22px;
	unicode_block = &unicode_block_Liberation_SansItalic_22px_BasicLatin;
	expectation_name = "Liberation_SansItalic_22px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_22px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_22px;
	unicode_block = &unicode_block_Liberation_SansItalic_22px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_22px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_22px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_22px;
	unicode_block = &unicode_block_Liberation_SansItalic_22px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_22px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_24px_BasicLatin) {
	font = &font_Liberation_SansItalic_24px;
	unicode_block = &unicode_block_Liberation_SansItalic_24px_BasicLatin;
	expectation_name = "Liberation_SansItalic_24px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_24px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_24px;
	unicode_block = &unicode_block_Liberation_SansItalic_24px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_24px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_24px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_24px;
	unicode_block = &unicode_block_Liberation_SansItalic_24px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_24px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_26px_BasicLatin) {
	font = &font_Liberation_SansItalic_26px;
	unicode_block = &unicode_block_Liberation_SansItalic_26px_BasicLatin;
	expectation_name = "Liberation_SansItalic_26px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_26px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_26px;
	unicode_block = &unicode_block_Liberation_SansItalic_26px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_26px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_26px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_26px;
	unicode_block = &unicode_block_Liberation_SansItalic_26px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_26px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_28px_BasicLatin) {
	font = &font_Liberation_SansItalic_28px;
	unicode_block = &unicode_block_Liberation_SansItalic_28px_BasicLatin;
	expectation_name = "Liberation_SansItalic_28px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_28px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_28px;
	unicode_block = &unicode_block_Liberation_SansItalic_28px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_28px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_28px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_28px;
	unicode_block = &unicode_block_Liberation_SansItalic_28px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_28px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_32px_BasicLatin) {
	font = &font_Liberation_SansItalic_32px;
	unicode_block = &unicode_block_Liberation_SansItalic_32px_BasicLatin;
	expectation_name = "Liberation_SansItalic_32px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_32px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_32px;
	unicode_block = &unicode_block_Liberation_SansItalic_32px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_32px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_32px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_32px;
	unicode_block = &unicode_block_Liberation_SansItalic_32px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_32px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_36px_BasicLatin) {
	font = &font_Liberation_SansItalic_36px;
	unicode_block = &unicode_block_Liberation_SansItalic_36px_BasicLatin;
	expectation_name = "Liberation_SansItalic_36px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_36px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_36px;
	unicode_block = &unicode_block_Liberation_SansItalic_36px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_36px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_36px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_36px;
	unicode_block = &unicode_block_Liberation_SansItalic_36px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_36px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_40px_BasicLatin) {
	font = &font_Liberation_SansItalic_40px;
	unicode_block = &unicode_block_Liberation_SansItalic_40px_BasicLatin;
	expectation_name = "Liberation_SansItalic_40px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_40px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_40px;
	unicode_block = &unicode_block_Liberation_SansItalic_40px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_40px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_40px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_40px;
	unicode_block = &unicode_block_Liberation_SansItalic_40px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_40px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_44px_BasicLatin) {
	font = &font_Liberation_SansItalic_44px;
	unicode_block = &unicode_block_Liberation_SansItalic_44px_BasicLatin;
	expectation_name = "Liberation_SansItalic_44px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_44px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_44px;
	unicode_block = &unicode_block_Liberation_SansItalic_44px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_44px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_44px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_44px;
	unicode_block = &unicode_block_Liberation_SansItalic_44px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_44px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_48px_BasicLatin) {
	font = &font_Liberation_SansItalic_48px;
	unicode_block = &unicode_block_Liberation_SansItalic_48px_BasicLatin;
	expectation_name = "Liberation_SansItalic_48px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_48px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_48px;
	unicode_block = &unicode_block_Liberation_SansItalic_48px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_48px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_48px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_48px;
	unicode_block = &unicode_block_Liberation_SansItalic_48px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_48px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_54px_BasicLatin) {
	font = &font_Liberation_SansItalic_54px;
	unicode_block = &unicode_block_Liberation_SansItalic_54px_BasicLatin;
	expectation_name = "Liberation_SansItalic_54px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_54px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_54px;
	unicode_block = &unicode_block_Liberation_SansItalic_54px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_54px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_54px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_54px;
	unicode_block = &unicode_block_Liberation_SansItalic_54px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_54px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_60px_BasicLatin) {
	font = &font_Liberation_SansItalic_60px;
	unicode_block = &unicode_block_Liberation_SansItalic_60px_BasicLatin;
	expectation_name = "Liberation_SansItalic_60px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_60px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_60px;
	unicode_block = &unicode_block_Liberation_SansItalic_60px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_60px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_60px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_60px;
	unicode_block = &unicode_block_Liberation_SansItalic_60px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_60px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_66px_BasicLatin) {
	font = &font_Liberation_SansItalic_66px;
	unicode_block = &unicode_block_Liberation_SansItalic_66px_BasicLatin;
	expectation_name = "Liberation_SansItalic_66px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_66px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_66px;
	unicode_block = &unicode_block_Liberation_SansItalic_66px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_66px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_66px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_66px;
	unicode_block = &unicode_block_Liberation_SansItalic_66px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_66px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_72px_BasicLatin) {
	font = &font_Liberation_SansItalic_72px;
	unicode_block = &unicode_block_Liberation_SansItalic_72px_BasicLatin;
	expectation_name = "Liberation_SansItalic_72px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_72px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_72px;
	unicode_block = &unicode_block_Liberation_SansItalic_72px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_72px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_72px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_72px;
	unicode_block = &unicode_block_Liberation_SansItalic_72px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_72px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_7px_BasicLatin) {
	font = &font_Liberation_SansItalic_7px;
	unicode_block = &unicode_block_Liberation_SansItalic_7px_BasicLatin;
	expectation_name = "Liberation_SansItalic_7px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_7px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_7px;
	unicode_block = &unicode_block_Liberation_SansItalic_7px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_7px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_7px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_7px;
	unicode_block = &unicode_block_Liberation_SansItalic_7px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_7px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_80px_BasicLatin) {
	font = &font_Liberation_SansItalic_80px;
	unicode_block = &unicode_block_Liberation_SansItalic_80px_BasicLatin;
	expectation_name = "Liberation_SansItalic_80px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_80px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_80px;
	unicode_block = &unicode_block_Liberation_SansItalic_80px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_80px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_80px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_80px;
	unicode_block = &unicode_block_Liberation_SansItalic_80px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_80px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_83px_BasicLatin) {
	font = &font_Liberation_SansItalic_83px;
	unicode_block = &unicode_block_Liberation_SansItalic_83px_BasicLatin;
	expectation_name = "Liberation_SansItalic_83px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_83px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_83px;
	unicode_block = &unicode_block_Liberation_SansItalic_83px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_83px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_83px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_83px;
	unicode_block = &unicode_block_Liberation_SansItalic_83px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_83px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_88px_BasicLatin) {
	font = &font_Liberation_SansItalic_88px;
	unicode_block = &unicode_block_Liberation_SansItalic_88px_BasicLatin;
	expectation_name = "Liberation_SansItalic_88px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_88px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_88px;
	unicode_block = &unicode_block_Liberation_SansItalic_88px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_88px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_88px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_88px;
	unicode_block = &unicode_block_Liberation_SansItalic_88px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_88px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_8px_BasicLatin) {
	font = &font_Liberation_SansItalic_8px;
	unicode_block = &unicode_block_Liberation_SansItalic_8px_BasicLatin;
	expectation_name = "Liberation_SansItalic_8px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_8px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_8px;
	unicode_block = &unicode_block_Liberation_SansItalic_8px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_8px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_8px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_8px;
	unicode_block = &unicode_block_Liberation_SansItalic_8px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_8px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_96px_BasicLatin) {
	font = &font_Liberation_SansItalic_96px;
	unicode_block = &unicode_block_Liberation_SansItalic_96px_BasicLatin;
	expectation_name = "Liberation_SansItalic_96px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_96px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_96px;
	unicode_block = &unicode_block_Liberation_SansItalic_96px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_96px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_96px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_96px;
	unicode_block = &unicode_block_Liberation_SansItalic_96px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_96px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansItalic_9px_BasicLatin) {
	font = &font_Liberation_SansItalic_9px;
	unicode_block = &unicode_block_Liberation_SansItalic_9px_BasicLatin;
	expectation_name = "Liberation_SansItalic_9px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansItalic_9px_Latin1Supplement) {
	font = &font_Liberation_SansItalic_9px;
	unicode_block = &unicode_block_Liberation_SansItalic_9px_Latin1Supplement;
	expectation_name = "Liberation_SansItalic_9px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansItalic_9px_LatinExtendedA) {
	font = &font_Liberation_SansItalic_9px;
	unicode_block = &unicode_block_Liberation_SansItalic_9px_LatinExtendedA;
	expectation_name = "Liberation_SansItalic_9px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_10px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_10px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_10px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_10px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_10px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_10px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_10px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_10px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_10px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_10px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_10px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_10px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_11px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_11px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_11px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_11px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_11px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_11px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_11px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_11px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_11px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_11px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_11px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_11px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_12px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_12px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_12px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_12px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_12px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_12px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_12px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_12px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_12px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_12px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_12px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_12px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_13px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_13px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_13px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_13px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_13px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_13px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_13px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_13px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_13px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_13px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_13px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_13px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_14px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_14px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_14px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_14px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_14px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_14px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_14px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_14px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_14px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_14px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_14px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_14px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_15px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_15px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_15px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_15px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_15px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_15px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_15px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_15px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_15px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_15px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_15px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_15px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_16px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_16px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_16px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_16px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_16px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_16px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_16px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_16px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_16px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_16px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_16px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_16px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_18px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_18px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_18px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_18px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_18px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_18px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_18px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_18px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_18px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_18px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_18px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_18px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_20px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_20px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_20px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_20px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_20px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_20px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_20px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_20px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_20px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_20px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_20px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_20px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_22px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_22px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_22px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_22px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_22px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_22px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_22px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_22px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_22px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_22px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_22px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_22px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_24px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_24px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_24px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_24px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_24px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_24px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_24px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_24px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_24px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_24px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_24px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_24px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_26px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_26px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_26px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_26px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_26px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_26px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_26px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_26px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_26px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_26px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_26px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_26px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_28px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_28px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_28px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_28px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_28px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_28px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_28px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_28px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_28px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_28px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_28px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_28px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_32px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_32px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_32px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_32px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_32px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_32px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_32px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_32px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_32px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_32px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_32px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_32px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_36px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_36px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_36px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_36px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_36px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_36px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_36px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_36px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_36px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_36px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_36px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_36px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_40px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_40px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_40px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_40px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_40px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_40px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_40px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_40px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_40px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_40px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_40px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_40px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_44px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_44px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_44px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_44px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_44px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_44px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_44px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_44px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_44px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_44px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_44px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_44px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_48px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_48px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_48px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_48px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_48px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_48px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_48px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_48px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_48px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_48px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_48px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_48px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_54px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_54px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_54px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_54px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_54px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_54px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_54px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_54px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_54px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_54px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_54px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_54px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_60px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_60px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_60px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_60px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_60px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_60px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_60px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_60px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_60px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_60px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_60px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_60px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_66px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_66px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_66px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_66px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_66px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_66px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_66px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_66px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_66px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_66px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_66px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_66px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_72px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_72px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_72px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_72px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_72px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_72px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_72px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_72px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_72px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_72px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_72px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_72px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_7px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_7px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_7px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_7px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_7px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_7px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_7px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_7px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_7px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_7px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_7px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_7px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_80px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_80px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_80px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_80px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_80px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_80px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_80px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_80px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_80px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_80px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_80px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_80px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_83px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_83px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_83px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_83px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_83px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_83px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_83px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_83px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_83px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_83px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_83px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_83px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_88px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_88px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_88px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_88px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_88px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_88px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_88px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_88px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_88px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_88px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_88px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_88px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_8px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_8px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_8px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_8px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_8px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_8px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_8px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_8px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_8px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_8px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_8px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_8px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_96px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_96px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_96px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_96px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_96px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_96px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_96px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_96px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_96px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_96px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_96px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_96px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBold_9px_BasicLatin) {
	font = &font_Liberation_SansNarrowBold_9px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_9px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBold_9px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBold_9px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBold_9px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_9px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBold_9px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBold_9px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBold_9px;
	unicode_block = &unicode_block_Liberation_SansNarrowBold_9px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBold_9px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_10px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_10px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_10px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_10px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_10px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_10px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_10px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_10px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_10px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_10px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_10px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_10px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_11px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_11px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_11px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_11px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_11px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_11px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_11px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_11px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_11px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_11px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_11px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_11px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_12px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_12px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_12px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_12px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_12px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_12px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_12px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_12px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_12px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_12px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_12px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_12px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_13px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_13px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_13px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_13px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_13px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_13px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_13px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_13px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_13px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_13px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_13px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_13px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_14px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_14px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_14px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_14px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_14px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_14px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_14px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_14px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_14px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_14px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_14px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_14px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_15px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_15px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_15px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_15px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_15px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_15px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_15px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_15px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_15px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_15px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_15px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_15px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_16px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_16px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_16px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_16px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_16px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_16px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_16px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_16px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_16px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_16px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_16px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_16px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_18px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_18px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_18px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_18px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_18px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_18px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_18px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_18px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_18px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_18px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_18px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_18px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_20px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_20px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_20px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_20px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_20px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_20px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_20px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_20px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_20px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_20px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_20px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_20px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_22px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_22px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_22px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_22px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_22px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_22px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_22px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_22px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_22px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_22px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_22px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_22px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_24px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_24px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_24px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_24px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_24px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_24px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_24px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_24px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_24px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_24px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_24px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_24px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_26px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_26px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_26px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_26px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_26px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_26px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_26px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_26px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_26px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_26px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_26px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_26px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_28px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_28px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_28px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_28px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_28px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_28px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_28px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_28px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_28px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_28px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_28px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_28px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_32px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_32px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_32px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_32px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_32px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_32px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_32px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_32px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_32px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_32px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_32px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_32px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_36px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_36px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_36px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_36px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_36px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_36px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_36px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_36px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_36px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_36px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_36px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_36px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_40px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_40px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_40px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_40px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_40px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_40px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_40px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_40px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_40px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_40px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_40px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_40px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_44px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_44px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_44px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_44px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_44px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_44px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_44px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_44px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_44px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_44px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_44px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_44px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_48px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_48px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_48px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_48px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_48px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_48px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_48px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_48px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_48px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_48px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_48px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_48px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_54px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_54px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_54px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_54px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_54px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_54px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_54px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_54px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_54px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_54px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_54px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_54px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_60px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_60px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_60px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_60px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_60px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_60px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_60px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_60px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_60px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_60px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_60px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_60px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_66px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_66px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_66px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_66px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_66px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_66px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_66px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_66px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_66px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_66px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_66px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_66px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_72px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_72px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_72px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_72px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_72px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_72px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_72px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_72px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_72px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_72px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_72px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_72px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_7px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_7px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_7px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_7px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_7px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_7px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_7px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_7px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_7px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_7px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_7px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_7px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_80px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_80px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_80px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_80px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_80px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_80px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_80px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_80px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_80px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_80px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_80px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_80px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_83px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_83px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_83px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_83px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_83px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_83px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_83px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_83px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_83px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_83px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_83px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_83px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_88px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_88px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_88px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_88px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_88px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_88px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_88px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_88px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_88px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_88px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_88px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_88px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_8px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_8px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_8px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_8px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_8px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_8px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_8px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_8px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_8px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_8px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_8px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_8px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_96px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_96px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_96px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_96px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_96px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_96px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_96px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_96px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_96px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_96px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_96px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_96px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_9px_BasicLatin) {
	font = &font_Liberation_SansNarrowBoldItalic_9px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_9px_BasicLatin;
	expectation_name = "Liberation_SansNarrowBoldItalic_9px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_9px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowBoldItalic_9px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_9px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowBoldItalic_9px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowBoldItalic_9px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowBoldItalic_9px;
	unicode_block = &unicode_block_Liberation_SansNarrowBoldItalic_9px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowBoldItalic_9px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_10px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_10px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_10px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_10px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_10px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_10px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_10px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_10px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_10px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_10px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_10px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_10px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_11px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_11px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_11px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_11px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_11px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_11px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_11px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_11px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_11px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_11px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_11px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_11px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_12px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_12px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_12px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_12px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_12px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_12px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_12px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_12px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_12px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_12px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_12px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_12px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_13px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_13px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_13px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_13px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_13px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_13px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_13px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_13px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_13px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_13px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_13px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_13px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_14px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_14px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_14px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_14px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_14px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_14px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_14px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_14px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_14px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_14px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_14px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_14px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_15px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_15px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_15px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_15px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_15px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_15px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_15px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_15px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_15px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_15px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_15px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_15px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_16px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_16px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_16px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_16px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_16px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_16px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_16px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_16px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_16px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_16px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_16px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_16px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_18px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_18px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_18px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_18px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_18px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_18px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_18px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_18px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_18px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_18px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_18px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_18px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_20px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_20px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_20px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_20px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_20px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_20px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_20px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_20px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_20px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_20px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_20px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_20px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_22px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_22px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_22px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_22px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_22px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_22px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_22px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_22px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_22px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_22px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_22px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_22px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_24px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_24px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_24px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_24px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_24px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_24px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_24px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_24px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_24px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_24px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_24px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_24px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_26px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_26px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_26px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_26px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_26px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_26px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_26px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_26px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_26px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_26px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_26px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_26px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_28px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_28px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_28px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_28px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_28px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_28px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_28px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_28px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_28px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_28px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_28px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_28px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_32px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_32px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_32px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_32px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_32px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_32px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_32px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_32px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_32px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_32px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_32px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_32px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_36px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_36px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_36px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_36px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_36px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_36px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_36px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_36px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_36px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_36px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_36px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_36px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_40px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_40px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_40px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_40px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_40px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_40px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_40px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_40px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_40px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_40px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_40px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_40px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_44px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_44px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_44px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_44px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_44px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_44px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_44px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_44px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_44px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_44px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_44px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_44px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_48px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_48px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_48px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_48px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_48px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_48px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_48px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_48px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_48px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_48px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_48px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_48px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_54px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_54px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_54px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_54px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_54px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_54px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_54px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_54px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_54px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_54px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_54px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_54px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_60px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_60px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_60px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_60px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_60px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_60px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_60px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_60px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_60px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_60px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_60px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_60px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_66px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_66px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_66px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_66px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_66px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_66px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_66px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_66px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_66px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_66px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_66px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_66px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_72px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_72px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_72px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_72px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_72px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_72px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_72px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_72px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_72px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_72px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_72px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_72px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_7px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_7px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_7px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_7px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_7px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_7px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_7px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_7px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_7px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_7px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_7px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_7px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_80px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_80px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_80px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_80px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_80px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_80px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_80px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_80px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_80px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_80px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_80px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_80px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_83px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_83px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_83px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_83px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_83px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_83px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_83px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_83px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_83px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_83px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_83px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_83px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_88px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_88px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_88px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_88px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_88px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_88px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_88px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_88px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_88px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_88px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_88px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_88px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_8px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_8px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_8px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_8px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_8px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_8px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_8px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_8px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_8px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_8px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_8px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_8px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_96px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_96px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_96px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_96px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_96px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_96px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_96px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_96px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_96px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_96px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_96px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_96px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_9px_BasicLatin) {
	font = &font_Liberation_SansNarrowItalic_9px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_9px_BasicLatin;
	expectation_name = "Liberation_SansNarrowItalic_9px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_9px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowItalic_9px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_9px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowItalic_9px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowItalic_9px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowItalic_9px;
	unicode_block = &unicode_block_Liberation_SansNarrowItalic_9px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowItalic_9px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_10px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_10px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_10px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_10px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_10px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_10px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_10px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_10px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_10px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_10px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_10px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_10px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_11px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_11px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_11px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_11px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_11px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_11px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_11px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_11px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_11px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_11px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_11px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_11px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_12px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_12px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_12px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_12px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_12px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_12px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_12px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_12px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_12px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_12px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_12px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_12px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_13px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_13px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_13px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_13px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_13px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_13px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_13px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_13px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_13px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_13px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_13px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_13px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_14px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_14px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_14px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_14px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_14px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_14px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_14px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_14px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_14px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_14px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_14px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_14px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_15px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_15px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_15px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_15px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_15px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_15px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_15px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_15px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_15px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_15px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_15px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_15px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_16px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_16px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_16px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_16px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_16px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_16px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_16px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_16px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_16px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_16px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_16px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_16px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_18px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_18px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_18px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_18px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_18px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_18px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_18px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_18px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_18px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_18px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_18px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_18px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_20px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_20px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_20px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_20px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_20px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_20px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_20px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_20px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_20px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_20px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_20px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_20px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_22px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_22px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_22px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_22px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_22px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_22px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_22px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_22px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_22px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_22px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_22px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_22px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_24px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_24px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_24px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_24px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_24px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_24px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_24px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_24px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_24px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_24px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_24px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_24px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_26px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_26px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_26px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_26px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_26px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_26px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_26px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_26px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_26px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_26px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_26px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_26px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_28px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_28px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_28px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_28px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_28px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_28px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_28px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_28px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_28px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_28px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_28px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_28px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_32px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_32px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_32px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_32px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_32px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_32px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_32px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_32px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_32px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_32px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_32px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_32px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_36px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_36px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_36px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_36px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_36px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_36px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_36px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_36px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_36px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_36px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_36px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_36px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_40px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_40px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_40px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_40px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_40px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_40px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_40px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_40px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_40px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_40px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_40px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_40px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_44px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_44px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_44px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_44px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_44px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_44px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_44px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_44px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_44px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_44px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_44px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_44px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_48px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_48px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_48px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_48px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_48px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_48px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_48px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_48px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_48px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_48px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_48px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_48px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_54px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_54px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_54px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_54px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_54px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_54px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_54px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_54px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_54px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_54px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_54px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_54px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_60px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_60px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_60px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_60px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_60px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_60px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_60px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_60px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_60px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_60px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_60px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_60px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_66px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_66px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_66px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_66px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_66px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_66px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_66px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_66px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_66px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_66px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_66px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_66px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_72px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_72px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_72px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_72px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_72px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_72px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_72px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_72px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_72px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_72px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_72px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_72px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_7px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_7px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_7px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_7px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_7px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_7px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_7px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_7px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_7px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_7px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_7px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_7px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_80px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_80px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_80px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_80px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_80px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_80px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_80px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_80px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_80px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_80px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_80px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_80px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_83px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_83px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_83px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_83px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_83px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_83px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_83px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_83px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_83px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_83px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_83px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_83px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_88px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_88px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_88px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_88px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_88px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_88px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_88px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_88px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_88px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_88px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_88px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_88px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_8px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_8px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_8px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_8px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_8px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_8px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_8px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_8px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_8px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_8px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_8px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_8px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_96px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_96px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_96px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_96px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_96px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_96px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_96px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_96px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_96px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_96px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_96px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_96px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_9px_BasicLatin) {
	font = &font_Liberation_SansNarrowRegular_9px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_9px_BasicLatin;
	expectation_name = "Liberation_SansNarrowRegular_9px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_9px_Latin1Supplement) {
	font = &font_Liberation_SansNarrowRegular_9px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_9px_Latin1Supplement;
	expectation_name = "Liberation_SansNarrowRegular_9px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansNarrowRegular_9px_LatinExtendedA) {
	font = &font_Liberation_SansNarrowRegular_9px;
	unicode_block = &unicode_block_Liberation_SansNarrowRegular_9px_LatinExtendedA;
	expectation_name = "Liberation_SansNarrowRegular_9px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_10px_BasicLatin) {
	font = &font_Liberation_SansRegular_10px;
	unicode_block = &unicode_block_Liberation_SansRegular_10px_BasicLatin;
	expectation_name = "Liberation_SansRegular_10px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_10px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_10px;
	unicode_block = &unicode_block_Liberation_SansRegular_10px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_10px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_10px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_10px;
	unicode_block = &unicode_block_Liberation_SansRegular_10px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_10px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_11px_BasicLatin) {
	font = &font_Liberation_SansRegular_11px;
	unicode_block = &unicode_block_Liberation_SansRegular_11px_BasicLatin;
	expectation_name = "Liberation_SansRegular_11px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_11px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_11px;
	unicode_block = &unicode_block_Liberation_SansRegular_11px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_11px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_11px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_11px;
	unicode_block = &unicode_block_Liberation_SansRegular_11px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_11px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_12px_BasicLatin) {
	font = &font_Liberation_SansRegular_12px;
	unicode_block = &unicode_block_Liberation_SansRegular_12px_BasicLatin;
	expectation_name = "Liberation_SansRegular_12px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_12px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_12px;
	unicode_block = &unicode_block_Liberation_SansRegular_12px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_12px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_12px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_12px;
	unicode_block = &unicode_block_Liberation_SansRegular_12px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_12px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_13px_BasicLatin) {
	font = &font_Liberation_SansRegular_13px;
	unicode_block = &unicode_block_Liberation_SansRegular_13px_BasicLatin;
	expectation_name = "Liberation_SansRegular_13px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_13px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_13px;
	unicode_block = &unicode_block_Liberation_SansRegular_13px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_13px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_13px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_13px;
	unicode_block = &unicode_block_Liberation_SansRegular_13px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_13px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_14px_BasicLatin) {
	font = &font_Liberation_SansRegular_14px;
	unicode_block = &unicode_block_Liberation_SansRegular_14px_BasicLatin;
	expectation_name = "Liberation_SansRegular_14px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_14px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_14px;
	unicode_block = &unicode_block_Liberation_SansRegular_14px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_14px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_14px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_14px;
	unicode_block = &unicode_block_Liberation_SansRegular_14px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_14px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_15px_BasicLatin) {
	font = &font_Liberation_SansRegular_15px;
	unicode_block = &unicode_block_Liberation_SansRegular_15px_BasicLatin;
	expectation_name = "Liberation_SansRegular_15px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_15px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_15px;
	unicode_block = &unicode_block_Liberation_SansRegular_15px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_15px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_15px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_15px;
	unicode_block = &unicode_block_Liberation_SansRegular_15px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_15px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_16px_BasicLatin) {
	font = &font_Liberation_SansRegular_16px;
	unicode_block = &unicode_block_Liberation_SansRegular_16px_BasicLatin;
	expectation_name = "Liberation_SansRegular_16px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_16px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_16px;
	unicode_block = &unicode_block_Liberation_SansRegular_16px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_16px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_16px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_16px;
	unicode_block = &unicode_block_Liberation_SansRegular_16px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_16px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_18px_BasicLatin) {
	font = &font_Liberation_SansRegular_18px;
	unicode_block = &unicode_block_Liberation_SansRegular_18px_BasicLatin;
	expectation_name = "Liberation_SansRegular_18px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_18px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_18px;
	unicode_block = &unicode_block_Liberation_SansRegular_18px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_18px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_18px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_18px;
	unicode_block = &unicode_block_Liberation_SansRegular_18px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_18px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_20px_BasicLatin) {
	font = &font_Liberation_SansRegular_20px;
	unicode_block = &unicode_block_Liberation_SansRegular_20px_BasicLatin;
	expectation_name = "Liberation_SansRegular_20px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_20px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_20px;
	unicode_block = &unicode_block_Liberation_SansRegular_20px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_20px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_20px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_20px;
	unicode_block = &unicode_block_Liberation_SansRegular_20px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_20px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_22px_BasicLatin) {
	font = &font_Liberation_SansRegular_22px;
	unicode_block = &unicode_block_Liberation_SansRegular_22px_BasicLatin;
	expectation_name = "Liberation_SansRegular_22px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_22px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_22px;
	unicode_block = &unicode_block_Liberation_SansRegular_22px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_22px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_22px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_22px;
	unicode_block = &unicode_block_Liberation_SansRegular_22px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_22px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_24px_BasicLatin) {
	font = &font_Liberation_SansRegular_24px;
	unicode_block = &unicode_block_Liberation_SansRegular_24px_BasicLatin;
	expectation_name = "Liberation_SansRegular_24px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_24px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_24px;
	unicode_block = &unicode_block_Liberation_SansRegular_24px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_24px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_24px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_24px;
	unicode_block = &unicode_block_Liberation_SansRegular_24px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_24px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_26px_BasicLatin) {
	font = &font_Liberation_SansRegular_26px;
	unicode_block = &unicode_block_Liberation_SansRegular_26px_BasicLatin;
	expectation_name = "Liberation_SansRegular_26px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_26px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_26px;
	unicode_block = &unicode_block_Liberation_SansRegular_26px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_26px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_26px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_26px;
	unicode_block = &unicode_block_Liberation_SansRegular_26px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_26px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_28px_BasicLatin) {
	font = &font_Liberation_SansRegular_28px;
	unicode_block = &unicode_block_Liberation_SansRegular_28px_BasicLatin;
	expectation_name = "Liberation_SansRegular_28px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_28px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_28px;
	unicode_block = &unicode_block_Liberation_SansRegular_28px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_28px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_28px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_28px;
	unicode_block = &unicode_block_Liberation_SansRegular_28px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_28px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_32px_BasicLatin) {
	font = &font_Liberation_SansRegular_32px;
	unicode_block = &unicode_block_Liberation_SansRegular_32px_BasicLatin;
	expectation_name = "Liberation_SansRegular_32px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_32px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_32px;
	unicode_block = &unicode_block_Liberation_SansRegular_32px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_32px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_32px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_32px;
	unicode_block = &unicode_block_Liberation_SansRegular_32px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_32px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_36px_BasicLatin) {
	font = &font_Liberation_SansRegular_36px;
	unicode_block = &unicode_block_Liberation_SansRegular_36px_BasicLatin;
	expectation_name = "Liberation_SansRegular_36px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_36px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_36px;
	unicode_block = &unicode_block_Liberation_SansRegular_36px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_36px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_36px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_36px;
	unicode_block = &unicode_block_Liberation_SansRegular_36px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_36px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_40px_BasicLatin) {
	font = &font_Liberation_SansRegular_40px;
	unicode_block = &unicode_block_Liberation_SansRegular_40px_BasicLatin;
	expectation_name = "Liberation_SansRegular_40px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_40px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_40px;
	unicode_block = &unicode_block_Liberation_SansRegular_40px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_40px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_40px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_40px;
	unicode_block = &unicode_block_Liberation_SansRegular_40px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_40px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_44px_BasicLatin) {
	font = &font_Liberation_SansRegular_44px;
	unicode_block = &unicode_block_Liberation_SansRegular_44px_BasicLatin;
	expectation_name = "Liberation_SansRegular_44px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_44px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_44px;
	unicode_block = &unicode_block_Liberation_SansRegular_44px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_44px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_44px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_44px;
	unicode_block = &unicode_block_Liberation_SansRegular_44px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_44px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_48px_BasicLatin) {
	font = &font_Liberation_SansRegular_48px;
	unicode_block = &unicode_block_Liberation_SansRegular_48px_BasicLatin;
	expectation_name = "Liberation_SansRegular_48px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_48px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_48px;
	unicode_block = &unicode_block_Liberation_SansRegular_48px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_48px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_48px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_48px;
	unicode_block = &unicode_block_Liberation_SansRegular_48px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_48px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_54px_BasicLatin) {
	font = &font_Liberation_SansRegular_54px;
	unicode_block = &unicode_block_Liberation_SansRegular_54px_BasicLatin;
	expectation_name = "Liberation_SansRegular_54px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_54px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_54px;
	unicode_block = &unicode_block_Liberation_SansRegular_54px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_54px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_54px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_54px;
	unicode_block = &unicode_block_Liberation_SansRegular_54px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_54px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_60px_BasicLatin) {
	font = &font_Liberation_SansRegular_60px;
	unicode_block = &unicode_block_Liberation_SansRegular_60px_BasicLatin;
	expectation_name = "Liberation_SansRegular_60px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_60px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_60px;
	unicode_block = &unicode_block_Liberation_SansRegular_60px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_60px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_60px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_60px;
	unicode_block = &unicode_block_Liberation_SansRegular_60px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_60px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_66px_BasicLatin) {
	font = &font_Liberation_SansRegular_66px;
	unicode_block = &unicode_block_Liberation_SansRegular_66px_BasicLatin;
	expectation_name = "Liberation_SansRegular_66px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_66px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_66px;
	unicode_block = &unicode_block_Liberation_SansRegular_66px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_66px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_66px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_66px;
	unicode_block = &unicode_block_Liberation_SansRegular_66px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_66px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_72px_BasicLatin) {
	font = &font_Liberation_SansRegular_72px;
	unicode_block = &unicode_block_Liberation_SansRegular_72px_BasicLatin;
	expectation_name = "Liberation_SansRegular_72px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_72px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_72px;
	unicode_block = &unicode_block_Liberation_SansRegular_72px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_72px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_72px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_72px;
	unicode_block = &unicode_block_Liberation_SansRegular_72px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_72px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_7px_BasicLatin) {
	font = &font_Liberation_SansRegular_7px;
	unicode_block = &unicode_block_Liberation_SansRegular_7px_BasicLatin;
	expectation_name = "Liberation_SansRegular_7px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_7px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_7px;
	unicode_block = &unicode_block_Liberation_SansRegular_7px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_7px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_7px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_7px;
	unicode_block = &unicode_block_Liberation_SansRegular_7px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_7px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_80px_BasicLatin) {
	font = &font_Liberation_SansRegular_80px;
	unicode_block = &unicode_block_Liberation_SansRegular_80px_BasicLatin;
	expectation_name = "Liberation_SansRegular_80px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_80px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_80px;
	unicode_block = &unicode_block_Liberation_SansRegular_80px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_80px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_80px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_80px;
	unicode_block = &unicode_block_Liberation_SansRegular_80px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_80px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_83px_BasicLatin) {
	font = &font_Liberation_SansRegular_83px;
	unicode_block = &unicode_block_Liberation_SansRegular_83px_BasicLatin;
	expectation_name = "Liberation_SansRegular_83px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_83px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_83px;
	unicode_block = &unicode_block_Liberation_SansRegular_83px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_83px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_83px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_83px;
	unicode_block = &unicode_block_Liberation_SansRegular_83px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_83px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_88px_BasicLatin) {
	font = &font_Liberation_SansRegular_88px;
	unicode_block = &unicode_block_Liberation_SansRegular_88px_BasicLatin;
	expectation_name = "Liberation_SansRegular_88px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_88px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_88px;
	unicode_block = &unicode_block_Liberation_SansRegular_88px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_88px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_88px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_88px;
	unicode_block = &unicode_block_Liberation_SansRegular_88px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_88px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_8px_BasicLatin) {
	font = &font_Liberation_SansRegular_8px;
	unicode_block = &unicode_block_Liberation_SansRegular_8px_BasicLatin;
	expectation_name = "Liberation_SansRegular_8px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_8px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_8px;
	unicode_block = &unicode_block_Liberation_SansRegular_8px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_8px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_8px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_8px;
	unicode_block = &unicode_block_Liberation_SansRegular_8px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_8px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_96px_BasicLatin) {
	font = &font_Liberation_SansRegular_96px;
	unicode_block = &unicode_block_Liberation_SansRegular_96px_BasicLatin;
	expectation_name = "Liberation_SansRegular_96px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_96px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_96px;
	unicode_block = &unicode_block_Liberation_SansRegular_96px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_96px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_96px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_96px;
	unicode_block = &unicode_block_Liberation_SansRegular_96px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_96px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SansRegular_9px_BasicLatin) {
	font = &font_Liberation_SansRegular_9px;
	unicode_block = &unicode_block_Liberation_SansRegular_9px_BasicLatin;
	expectation_name = "Liberation_SansRegular_9px_BasicLatin";
}END_TEST

START_TEST(Liberation_SansRegular_9px_Latin1Supplement) {
	font = &font_Liberation_SansRegular_9px;
	unicode_block = &unicode_block_Liberation_SansRegular_9px_Latin1Supplement;
	expectation_name = "Liberation_SansRegular_9px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SansRegular_9px_LatinExtendedA) {
	font = &font_Liberation_SansRegular_9px;
	unicode_block = &unicode_block_Liberation_SansRegular_9px_LatinExtendedA;
	expectation_name = "Liberation_SansRegular_9px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_10px_BasicLatin) {
	font = &font_Liberation_SerifBold_10px;
	unicode_block = &unicode_block_Liberation_SerifBold_10px_BasicLatin;
	expectation_name = "Liberation_SerifBold_10px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_10px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_10px;
	unicode_block = &unicode_block_Liberation_SerifBold_10px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_10px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_10px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_10px;
	unicode_block = &unicode_block_Liberation_SerifBold_10px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_10px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_11px_BasicLatin) {
	font = &font_Liberation_SerifBold_11px;
	unicode_block = &unicode_block_Liberation_SerifBold_11px_BasicLatin;
	expectation_name = "Liberation_SerifBold_11px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_11px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_11px;
	unicode_block = &unicode_block_Liberation_SerifBold_11px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_11px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_11px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_11px;
	unicode_block = &unicode_block_Liberation_SerifBold_11px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_11px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_12px_BasicLatin) {
	font = &font_Liberation_SerifBold_12px;
	unicode_block = &unicode_block_Liberation_SerifBold_12px_BasicLatin;
	expectation_name = "Liberation_SerifBold_12px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_12px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_12px;
	unicode_block = &unicode_block_Liberation_SerifBold_12px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_12px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_12px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_12px;
	unicode_block = &unicode_block_Liberation_SerifBold_12px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_12px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_13px_BasicLatin) {
	font = &font_Liberation_SerifBold_13px;
	unicode_block = &unicode_block_Liberation_SerifBold_13px_BasicLatin;
	expectation_name = "Liberation_SerifBold_13px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_13px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_13px;
	unicode_block = &unicode_block_Liberation_SerifBold_13px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_13px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_13px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_13px;
	unicode_block = &unicode_block_Liberation_SerifBold_13px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_13px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_14px_BasicLatin) {
	font = &font_Liberation_SerifBold_14px;
	unicode_block = &unicode_block_Liberation_SerifBold_14px_BasicLatin;
	expectation_name = "Liberation_SerifBold_14px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_14px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_14px;
	unicode_block = &unicode_block_Liberation_SerifBold_14px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_14px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_14px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_14px;
	unicode_block = &unicode_block_Liberation_SerifBold_14px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_14px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_15px_BasicLatin) {
	font = &font_Liberation_SerifBold_15px;
	unicode_block = &unicode_block_Liberation_SerifBold_15px_BasicLatin;
	expectation_name = "Liberation_SerifBold_15px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_15px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_15px;
	unicode_block = &unicode_block_Liberation_SerifBold_15px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_15px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_15px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_15px;
	unicode_block = &unicode_block_Liberation_SerifBold_15px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_15px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_16px_BasicLatin) {
	font = &font_Liberation_SerifBold_16px;
	unicode_block = &unicode_block_Liberation_SerifBold_16px_BasicLatin;
	expectation_name = "Liberation_SerifBold_16px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_16px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_16px;
	unicode_block = &unicode_block_Liberation_SerifBold_16px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_16px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_16px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_16px;
	unicode_block = &unicode_block_Liberation_SerifBold_16px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_16px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_18px_BasicLatin) {
	font = &font_Liberation_SerifBold_18px;
	unicode_block = &unicode_block_Liberation_SerifBold_18px_BasicLatin;
	expectation_name = "Liberation_SerifBold_18px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_18px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_18px;
	unicode_block = &unicode_block_Liberation_SerifBold_18px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_18px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_18px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_18px;
	unicode_block = &unicode_block_Liberation_SerifBold_18px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_18px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_20px_BasicLatin) {
	font = &font_Liberation_SerifBold_20px;
	unicode_block = &unicode_block_Liberation_SerifBold_20px_BasicLatin;
	expectation_name = "Liberation_SerifBold_20px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_20px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_20px;
	unicode_block = &unicode_block_Liberation_SerifBold_20px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_20px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_20px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_20px;
	unicode_block = &unicode_block_Liberation_SerifBold_20px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_20px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_22px_BasicLatin) {
	font = &font_Liberation_SerifBold_22px;
	unicode_block = &unicode_block_Liberation_SerifBold_22px_BasicLatin;
	expectation_name = "Liberation_SerifBold_22px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_22px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_22px;
	unicode_block = &unicode_block_Liberation_SerifBold_22px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_22px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_22px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_22px;
	unicode_block = &unicode_block_Liberation_SerifBold_22px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_22px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_24px_BasicLatin) {
	font = &font_Liberation_SerifBold_24px;
	unicode_block = &unicode_block_Liberation_SerifBold_24px_BasicLatin;
	expectation_name = "Liberation_SerifBold_24px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_24px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_24px;
	unicode_block = &unicode_block_Liberation_SerifBold_24px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_24px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_24px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_24px;
	unicode_block = &unicode_block_Liberation_SerifBold_24px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_24px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_26px_BasicLatin) {
	font = &font_Liberation_SerifBold_26px;
	unicode_block = &unicode_block_Liberation_SerifBold_26px_BasicLatin;
	expectation_name = "Liberation_SerifBold_26px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_26px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_26px;
	unicode_block = &unicode_block_Liberation_SerifBold_26px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_26px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_26px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_26px;
	unicode_block = &unicode_block_Liberation_SerifBold_26px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_26px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_28px_BasicLatin) {
	font = &font_Liberation_SerifBold_28px;
	unicode_block = &unicode_block_Liberation_SerifBold_28px_BasicLatin;
	expectation_name = "Liberation_SerifBold_28px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_28px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_28px;
	unicode_block = &unicode_block_Liberation_SerifBold_28px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_28px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_28px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_28px;
	unicode_block = &unicode_block_Liberation_SerifBold_28px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_28px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_32px_BasicLatin) {
	font = &font_Liberation_SerifBold_32px;
	unicode_block = &unicode_block_Liberation_SerifBold_32px_BasicLatin;
	expectation_name = "Liberation_SerifBold_32px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_32px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_32px;
	unicode_block = &unicode_block_Liberation_SerifBold_32px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_32px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_32px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_32px;
	unicode_block = &unicode_block_Liberation_SerifBold_32px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_32px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_36px_BasicLatin) {
	font = &font_Liberation_SerifBold_36px;
	unicode_block = &unicode_block_Liberation_SerifBold_36px_BasicLatin;
	expectation_name = "Liberation_SerifBold_36px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_36px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_36px;
	unicode_block = &unicode_block_Liberation_SerifBold_36px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_36px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_36px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_36px;
	unicode_block = &unicode_block_Liberation_SerifBold_36px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_36px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_40px_BasicLatin) {
	font = &font_Liberation_SerifBold_40px;
	unicode_block = &unicode_block_Liberation_SerifBold_40px_BasicLatin;
	expectation_name = "Liberation_SerifBold_40px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_40px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_40px;
	unicode_block = &unicode_block_Liberation_SerifBold_40px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_40px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_40px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_40px;
	unicode_block = &unicode_block_Liberation_SerifBold_40px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_40px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_44px_BasicLatin) {
	font = &font_Liberation_SerifBold_44px;
	unicode_block = &unicode_block_Liberation_SerifBold_44px_BasicLatin;
	expectation_name = "Liberation_SerifBold_44px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_44px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_44px;
	unicode_block = &unicode_block_Liberation_SerifBold_44px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_44px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_44px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_44px;
	unicode_block = &unicode_block_Liberation_SerifBold_44px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_44px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_48px_BasicLatin) {
	font = &font_Liberation_SerifBold_48px;
	unicode_block = &unicode_block_Liberation_SerifBold_48px_BasicLatin;
	expectation_name = "Liberation_SerifBold_48px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_48px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_48px;
	unicode_block = &unicode_block_Liberation_SerifBold_48px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_48px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_48px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_48px;
	unicode_block = &unicode_block_Liberation_SerifBold_48px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_48px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_54px_BasicLatin) {
	font = &font_Liberation_SerifBold_54px;
	unicode_block = &unicode_block_Liberation_SerifBold_54px_BasicLatin;
	expectation_name = "Liberation_SerifBold_54px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_54px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_54px;
	unicode_block = &unicode_block_Liberation_SerifBold_54px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_54px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_54px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_54px;
	unicode_block = &unicode_block_Liberation_SerifBold_54px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_54px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_60px_BasicLatin) {
	font = &font_Liberation_SerifBold_60px;
	unicode_block = &unicode_block_Liberation_SerifBold_60px_BasicLatin;
	expectation_name = "Liberation_SerifBold_60px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_60px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_60px;
	unicode_block = &unicode_block_Liberation_SerifBold_60px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_60px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_60px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_60px;
	unicode_block = &unicode_block_Liberation_SerifBold_60px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_60px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_66px_BasicLatin) {
	font = &font_Liberation_SerifBold_66px;
	unicode_block = &unicode_block_Liberation_SerifBold_66px_BasicLatin;
	expectation_name = "Liberation_SerifBold_66px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_66px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_66px;
	unicode_block = &unicode_block_Liberation_SerifBold_66px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_66px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_66px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_66px;
	unicode_block = &unicode_block_Liberation_SerifBold_66px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_66px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_72px_BasicLatin) {
	font = &font_Liberation_SerifBold_72px;
	unicode_block = &unicode_block_Liberation_SerifBold_72px_BasicLatin;
	expectation_name = "Liberation_SerifBold_72px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_72px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_72px;
	unicode_block = &unicode_block_Liberation_SerifBold_72px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_72px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_72px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_72px;
	unicode_block = &unicode_block_Liberation_SerifBold_72px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_72px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_7px_BasicLatin) {
	font = &font_Liberation_SerifBold_7px;
	unicode_block = &unicode_block_Liberation_SerifBold_7px_BasicLatin;
	expectation_name = "Liberation_SerifBold_7px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_7px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_7px;
	unicode_block = &unicode_block_Liberation_SerifBold_7px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_7px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_7px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_7px;
	unicode_block = &unicode_block_Liberation_SerifBold_7px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_7px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_80px_BasicLatin) {
	font = &font_Liberation_SerifBold_80px;
	unicode_block = &unicode_block_Liberation_SerifBold_80px_BasicLatin;
	expectation_name = "Liberation_SerifBold_80px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_80px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_80px;
	unicode_block = &unicode_block_Liberation_SerifBold_80px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_80px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_80px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_80px;
	unicode_block = &unicode_block_Liberation_SerifBold_80px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_80px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_83px_BasicLatin) {
	font = &font_Liberation_SerifBold_83px;
	unicode_block = &unicode_block_Liberation_SerifBold_83px_BasicLatin;
	expectation_name = "Liberation_SerifBold_83px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_83px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_83px;
	unicode_block = &unicode_block_Liberation_SerifBold_83px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_83px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_83px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_83px;
	unicode_block = &unicode_block_Liberation_SerifBold_83px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_83px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_88px_BasicLatin) {
	font = &font_Liberation_SerifBold_88px;
	unicode_block = &unicode_block_Liberation_SerifBold_88px_BasicLatin;
	expectation_name = "Liberation_SerifBold_88px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_88px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_88px;
	unicode_block = &unicode_block_Liberation_SerifBold_88px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_88px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_88px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_88px;
	unicode_block = &unicode_block_Liberation_SerifBold_88px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_88px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_8px_BasicLatin) {
	font = &font_Liberation_SerifBold_8px;
	unicode_block = &unicode_block_Liberation_SerifBold_8px_BasicLatin;
	expectation_name = "Liberation_SerifBold_8px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_8px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_8px;
	unicode_block = &unicode_block_Liberation_SerifBold_8px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_8px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_8px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_8px;
	unicode_block = &unicode_block_Liberation_SerifBold_8px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_8px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_96px_BasicLatin) {
	font = &font_Liberation_SerifBold_96px;
	unicode_block = &unicode_block_Liberation_SerifBold_96px_BasicLatin;
	expectation_name = "Liberation_SerifBold_96px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_96px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_96px;
	unicode_block = &unicode_block_Liberation_SerifBold_96px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_96px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_96px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_96px;
	unicode_block = &unicode_block_Liberation_SerifBold_96px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_96px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBold_9px_BasicLatin) {
	font = &font_Liberation_SerifBold_9px;
	unicode_block = &unicode_block_Liberation_SerifBold_9px_BasicLatin;
	expectation_name = "Liberation_SerifBold_9px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBold_9px_Latin1Supplement) {
	font = &font_Liberation_SerifBold_9px;
	unicode_block = &unicode_block_Liberation_SerifBold_9px_Latin1Supplement;
	expectation_name = "Liberation_SerifBold_9px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBold_9px_LatinExtendedA) {
	font = &font_Liberation_SerifBold_9px;
	unicode_block = &unicode_block_Liberation_SerifBold_9px_LatinExtendedA;
	expectation_name = "Liberation_SerifBold_9px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_10px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_10px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_10px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_10px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_10px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_10px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_10px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_10px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_10px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_10px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_10px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_10px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_11px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_11px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_11px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_11px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_11px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_11px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_11px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_11px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_11px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_11px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_11px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_11px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_12px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_12px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_12px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_12px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_12px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_12px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_12px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_12px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_12px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_12px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_12px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_12px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_13px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_13px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_13px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_13px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_13px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_13px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_13px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_13px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_13px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_13px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_13px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_13px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_14px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_14px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_14px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_14px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_14px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_14px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_14px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_14px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_14px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_14px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_14px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_14px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_15px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_15px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_15px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_15px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_15px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_15px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_15px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_15px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_15px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_15px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_15px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_15px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_16px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_16px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_16px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_16px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_16px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_16px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_16px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_16px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_16px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_16px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_16px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_16px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_18px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_18px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_18px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_18px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_18px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_18px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_18px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_18px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_18px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_18px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_18px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_18px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_20px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_20px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_20px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_20px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_20px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_20px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_20px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_20px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_20px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_20px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_20px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_20px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_22px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_22px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_22px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_22px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_22px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_22px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_22px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_22px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_22px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_22px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_22px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_22px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_24px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_24px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_24px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_24px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_24px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_24px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_24px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_24px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_24px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_24px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_24px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_24px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_26px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_26px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_26px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_26px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_26px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_26px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_26px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_26px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_26px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_26px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_26px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_26px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_28px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_28px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_28px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_28px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_28px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_28px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_28px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_28px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_28px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_28px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_28px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_28px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_32px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_32px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_32px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_32px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_32px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_32px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_32px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_32px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_32px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_32px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_32px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_32px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_36px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_36px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_36px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_36px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_36px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_36px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_36px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_36px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_36px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_36px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_36px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_36px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_40px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_40px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_40px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_40px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_40px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_40px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_40px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_40px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_40px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_40px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_40px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_40px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_44px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_44px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_44px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_44px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_44px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_44px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_44px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_44px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_44px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_44px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_44px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_44px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_48px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_48px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_48px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_48px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_48px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_48px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_48px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_48px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_48px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_48px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_48px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_48px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_54px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_54px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_54px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_54px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_54px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_54px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_54px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_54px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_54px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_54px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_54px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_54px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_60px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_60px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_60px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_60px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_60px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_60px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_60px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_60px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_60px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_60px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_60px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_60px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_66px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_66px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_66px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_66px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_66px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_66px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_66px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_66px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_66px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_66px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_66px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_66px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_72px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_72px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_72px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_72px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_72px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_72px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_72px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_72px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_72px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_72px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_72px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_72px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_7px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_7px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_7px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_7px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_7px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_7px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_7px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_7px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_7px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_7px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_7px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_7px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_80px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_80px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_80px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_80px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_80px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_80px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_80px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_80px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_80px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_80px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_80px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_80px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_83px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_83px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_83px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_83px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_83px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_83px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_83px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_83px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_83px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_83px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_83px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_83px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_88px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_88px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_88px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_88px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_88px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_88px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_88px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_88px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_88px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_88px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_88px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_88px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_8px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_8px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_8px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_8px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_8px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_8px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_8px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_8px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_8px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_8px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_8px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_8px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_96px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_96px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_96px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_96px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_96px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_96px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_96px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_96px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_96px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_96px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_96px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_96px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_9px_BasicLatin) {
	font = &font_Liberation_SerifBoldItalic_9px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_9px_BasicLatin;
	expectation_name = "Liberation_SerifBoldItalic_9px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_9px_Latin1Supplement) {
	font = &font_Liberation_SerifBoldItalic_9px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_9px_Latin1Supplement;
	expectation_name = "Liberation_SerifBoldItalic_9px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifBoldItalic_9px_LatinExtendedA) {
	font = &font_Liberation_SerifBoldItalic_9px;
	unicode_block = &unicode_block_Liberation_SerifBoldItalic_9px_LatinExtendedA;
	expectation_name = "Liberation_SerifBoldItalic_9px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_10px_BasicLatin) {
	font = &font_Liberation_SerifItalic_10px;
	unicode_block = &unicode_block_Liberation_SerifItalic_10px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_10px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_10px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_10px;
	unicode_block = &unicode_block_Liberation_SerifItalic_10px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_10px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_10px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_10px;
	unicode_block = &unicode_block_Liberation_SerifItalic_10px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_10px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_11px_BasicLatin) {
	font = &font_Liberation_SerifItalic_11px;
	unicode_block = &unicode_block_Liberation_SerifItalic_11px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_11px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_11px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_11px;
	unicode_block = &unicode_block_Liberation_SerifItalic_11px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_11px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_11px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_11px;
	unicode_block = &unicode_block_Liberation_SerifItalic_11px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_11px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_12px_BasicLatin) {
	font = &font_Liberation_SerifItalic_12px;
	unicode_block = &unicode_block_Liberation_SerifItalic_12px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_12px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_12px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_12px;
	unicode_block = &unicode_block_Liberation_SerifItalic_12px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_12px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_12px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_12px;
	unicode_block = &unicode_block_Liberation_SerifItalic_12px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_12px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_13px_BasicLatin) {
	font = &font_Liberation_SerifItalic_13px;
	unicode_block = &unicode_block_Liberation_SerifItalic_13px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_13px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_13px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_13px;
	unicode_block = &unicode_block_Liberation_SerifItalic_13px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_13px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_13px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_13px;
	unicode_block = &unicode_block_Liberation_SerifItalic_13px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_13px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_14px_BasicLatin) {
	font = &font_Liberation_SerifItalic_14px;
	unicode_block = &unicode_block_Liberation_SerifItalic_14px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_14px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_14px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_14px;
	unicode_block = &unicode_block_Liberation_SerifItalic_14px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_14px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_14px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_14px;
	unicode_block = &unicode_block_Liberation_SerifItalic_14px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_14px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_15px_BasicLatin) {
	font = &font_Liberation_SerifItalic_15px;
	unicode_block = &unicode_block_Liberation_SerifItalic_15px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_15px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_15px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_15px;
	unicode_block = &unicode_block_Liberation_SerifItalic_15px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_15px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_15px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_15px;
	unicode_block = &unicode_block_Liberation_SerifItalic_15px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_15px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_16px_BasicLatin) {
	font = &font_Liberation_SerifItalic_16px;
	unicode_block = &unicode_block_Liberation_SerifItalic_16px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_16px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_16px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_16px;
	unicode_block = &unicode_block_Liberation_SerifItalic_16px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_16px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_16px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_16px;
	unicode_block = &unicode_block_Liberation_SerifItalic_16px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_16px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_18px_BasicLatin) {
	font = &font_Liberation_SerifItalic_18px;
	unicode_block = &unicode_block_Liberation_SerifItalic_18px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_18px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_18px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_18px;
	unicode_block = &unicode_block_Liberation_SerifItalic_18px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_18px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_18px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_18px;
	unicode_block = &unicode_block_Liberation_SerifItalic_18px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_18px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_20px_BasicLatin) {
	font = &font_Liberation_SerifItalic_20px;
	unicode_block = &unicode_block_Liberation_SerifItalic_20px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_20px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_20px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_20px;
	unicode_block = &unicode_block_Liberation_SerifItalic_20px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_20px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_20px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_20px;
	unicode_block = &unicode_block_Liberation_SerifItalic_20px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_20px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_22px_BasicLatin) {
	font = &font_Liberation_SerifItalic_22px;
	unicode_block = &unicode_block_Liberation_SerifItalic_22px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_22px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_22px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_22px;
	unicode_block = &unicode_block_Liberation_SerifItalic_22px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_22px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_22px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_22px;
	unicode_block = &unicode_block_Liberation_SerifItalic_22px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_22px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_24px_BasicLatin) {
	font = &font_Liberation_SerifItalic_24px;
	unicode_block = &unicode_block_Liberation_SerifItalic_24px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_24px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_24px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_24px;
	unicode_block = &unicode_block_Liberation_SerifItalic_24px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_24px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_24px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_24px;
	unicode_block = &unicode_block_Liberation_SerifItalic_24px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_24px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_26px_BasicLatin) {
	font = &font_Liberation_SerifItalic_26px;
	unicode_block = &unicode_block_Liberation_SerifItalic_26px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_26px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_26px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_26px;
	unicode_block = &unicode_block_Liberation_SerifItalic_26px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_26px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_26px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_26px;
	unicode_block = &unicode_block_Liberation_SerifItalic_26px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_26px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_28px_BasicLatin) {
	font = &font_Liberation_SerifItalic_28px;
	unicode_block = &unicode_block_Liberation_SerifItalic_28px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_28px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_28px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_28px;
	unicode_block = &unicode_block_Liberation_SerifItalic_28px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_28px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_28px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_28px;
	unicode_block = &unicode_block_Liberation_SerifItalic_28px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_28px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_32px_BasicLatin) {
	font = &font_Liberation_SerifItalic_32px;
	unicode_block = &unicode_block_Liberation_SerifItalic_32px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_32px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_32px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_32px;
	unicode_block = &unicode_block_Liberation_SerifItalic_32px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_32px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_32px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_32px;
	unicode_block = &unicode_block_Liberation_SerifItalic_32px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_32px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_36px_BasicLatin) {
	font = &font_Liberation_SerifItalic_36px;
	unicode_block = &unicode_block_Liberation_SerifItalic_36px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_36px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_36px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_36px;
	unicode_block = &unicode_block_Liberation_SerifItalic_36px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_36px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_36px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_36px;
	unicode_block = &unicode_block_Liberation_SerifItalic_36px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_36px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_40px_BasicLatin) {
	font = &font_Liberation_SerifItalic_40px;
	unicode_block = &unicode_block_Liberation_SerifItalic_40px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_40px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_40px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_40px;
	unicode_block = &unicode_block_Liberation_SerifItalic_40px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_40px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_40px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_40px;
	unicode_block = &unicode_block_Liberation_SerifItalic_40px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_40px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_44px_BasicLatin) {
	font = &font_Liberation_SerifItalic_44px;
	unicode_block = &unicode_block_Liberation_SerifItalic_44px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_44px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_44px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_44px;
	unicode_block = &unicode_block_Liberation_SerifItalic_44px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_44px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_44px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_44px;
	unicode_block = &unicode_block_Liberation_SerifItalic_44px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_44px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_48px_BasicLatin) {
	font = &font_Liberation_SerifItalic_48px;
	unicode_block = &unicode_block_Liberation_SerifItalic_48px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_48px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_48px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_48px;
	unicode_block = &unicode_block_Liberation_SerifItalic_48px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_48px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_48px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_48px;
	unicode_block = &unicode_block_Liberation_SerifItalic_48px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_48px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_54px_BasicLatin) {
	font = &font_Liberation_SerifItalic_54px;
	unicode_block = &unicode_block_Liberation_SerifItalic_54px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_54px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_54px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_54px;
	unicode_block = &unicode_block_Liberation_SerifItalic_54px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_54px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_54px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_54px;
	unicode_block = &unicode_block_Liberation_SerifItalic_54px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_54px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_60px_BasicLatin) {
	font = &font_Liberation_SerifItalic_60px;
	unicode_block = &unicode_block_Liberation_SerifItalic_60px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_60px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_60px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_60px;
	unicode_block = &unicode_block_Liberation_SerifItalic_60px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_60px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_60px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_60px;
	unicode_block = &unicode_block_Liberation_SerifItalic_60px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_60px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_66px_BasicLatin) {
	font = &font_Liberation_SerifItalic_66px;
	unicode_block = &unicode_block_Liberation_SerifItalic_66px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_66px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_66px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_66px;
	unicode_block = &unicode_block_Liberation_SerifItalic_66px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_66px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_66px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_66px;
	unicode_block = &unicode_block_Liberation_SerifItalic_66px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_66px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_72px_BasicLatin) {
	font = &font_Liberation_SerifItalic_72px;
	unicode_block = &unicode_block_Liberation_SerifItalic_72px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_72px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_72px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_72px;
	unicode_block = &unicode_block_Liberation_SerifItalic_72px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_72px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_72px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_72px;
	unicode_block = &unicode_block_Liberation_SerifItalic_72px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_72px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_7px_BasicLatin) {
	font = &font_Liberation_SerifItalic_7px;
	unicode_block = &unicode_block_Liberation_SerifItalic_7px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_7px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_7px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_7px;
	unicode_block = &unicode_block_Liberation_SerifItalic_7px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_7px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_7px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_7px;
	unicode_block = &unicode_block_Liberation_SerifItalic_7px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_7px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_80px_BasicLatin) {
	font = &font_Liberation_SerifItalic_80px;
	unicode_block = &unicode_block_Liberation_SerifItalic_80px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_80px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_80px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_80px;
	unicode_block = &unicode_block_Liberation_SerifItalic_80px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_80px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_80px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_80px;
	unicode_block = &unicode_block_Liberation_SerifItalic_80px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_80px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_83px_BasicLatin) {
	font = &font_Liberation_SerifItalic_83px;
	unicode_block = &unicode_block_Liberation_SerifItalic_83px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_83px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_83px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_83px;
	unicode_block = &unicode_block_Liberation_SerifItalic_83px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_83px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_83px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_83px;
	unicode_block = &unicode_block_Liberation_SerifItalic_83px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_83px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_88px_BasicLatin) {
	font = &font_Liberation_SerifItalic_88px;
	unicode_block = &unicode_block_Liberation_SerifItalic_88px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_88px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_88px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_88px;
	unicode_block = &unicode_block_Liberation_SerifItalic_88px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_88px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_88px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_88px;
	unicode_block = &unicode_block_Liberation_SerifItalic_88px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_88px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_8px_BasicLatin) {
	font = &font_Liberation_SerifItalic_8px;
	unicode_block = &unicode_block_Liberation_SerifItalic_8px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_8px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_8px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_8px;
	unicode_block = &unicode_block_Liberation_SerifItalic_8px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_8px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_8px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_8px;
	unicode_block = &unicode_block_Liberation_SerifItalic_8px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_8px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_96px_BasicLatin) {
	font = &font_Liberation_SerifItalic_96px;
	unicode_block = &unicode_block_Liberation_SerifItalic_96px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_96px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_96px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_96px;
	unicode_block = &unicode_block_Liberation_SerifItalic_96px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_96px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_96px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_96px;
	unicode_block = &unicode_block_Liberation_SerifItalic_96px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_96px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifItalic_9px_BasicLatin) {
	font = &font_Liberation_SerifItalic_9px;
	unicode_block = &unicode_block_Liberation_SerifItalic_9px_BasicLatin;
	expectation_name = "Liberation_SerifItalic_9px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifItalic_9px_Latin1Supplement) {
	font = &font_Liberation_SerifItalic_9px;
	unicode_block = &unicode_block_Liberation_SerifItalic_9px_Latin1Supplement;
	expectation_name = "Liberation_SerifItalic_9px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifItalic_9px_LatinExtendedA) {
	font = &font_Liberation_SerifItalic_9px;
	unicode_block = &unicode_block_Liberation_SerifItalic_9px_LatinExtendedA;
	expectation_name = "Liberation_SerifItalic_9px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_10px_BasicLatin) {
	font = &font_Liberation_SerifRegular_10px;
	unicode_block = &unicode_block_Liberation_SerifRegular_10px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_10px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_10px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_10px;
	unicode_block = &unicode_block_Liberation_SerifRegular_10px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_10px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_10px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_10px;
	unicode_block = &unicode_block_Liberation_SerifRegular_10px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_10px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_11px_BasicLatin) {
	font = &font_Liberation_SerifRegular_11px;
	unicode_block = &unicode_block_Liberation_SerifRegular_11px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_11px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_11px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_11px;
	unicode_block = &unicode_block_Liberation_SerifRegular_11px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_11px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_11px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_11px;
	unicode_block = &unicode_block_Liberation_SerifRegular_11px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_11px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_12px_BasicLatin) {
	font = &font_Liberation_SerifRegular_12px;
	unicode_block = &unicode_block_Liberation_SerifRegular_12px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_12px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_12px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_12px;
	unicode_block = &unicode_block_Liberation_SerifRegular_12px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_12px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_12px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_12px;
	unicode_block = &unicode_block_Liberation_SerifRegular_12px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_12px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_13px_BasicLatin) {
	font = &font_Liberation_SerifRegular_13px;
	unicode_block = &unicode_block_Liberation_SerifRegular_13px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_13px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_13px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_13px;
	unicode_block = &unicode_block_Liberation_SerifRegular_13px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_13px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_13px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_13px;
	unicode_block = &unicode_block_Liberation_SerifRegular_13px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_13px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_14px_BasicLatin) {
	font = &font_Liberation_SerifRegular_14px;
	unicode_block = &unicode_block_Liberation_SerifRegular_14px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_14px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_14px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_14px;
	unicode_block = &unicode_block_Liberation_SerifRegular_14px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_14px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_14px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_14px;
	unicode_block = &unicode_block_Liberation_SerifRegular_14px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_14px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_15px_BasicLatin) {
	font = &font_Liberation_SerifRegular_15px;
	unicode_block = &unicode_block_Liberation_SerifRegular_15px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_15px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_15px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_15px;
	unicode_block = &unicode_block_Liberation_SerifRegular_15px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_15px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_15px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_15px;
	unicode_block = &unicode_block_Liberation_SerifRegular_15px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_15px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_16px_BasicLatin) {
	font = &font_Liberation_SerifRegular_16px;
	unicode_block = &unicode_block_Liberation_SerifRegular_16px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_16px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_16px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_16px;
	unicode_block = &unicode_block_Liberation_SerifRegular_16px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_16px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_16px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_16px;
	unicode_block = &unicode_block_Liberation_SerifRegular_16px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_16px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_18px_BasicLatin) {
	font = &font_Liberation_SerifRegular_18px;
	unicode_block = &unicode_block_Liberation_SerifRegular_18px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_18px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_18px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_18px;
	unicode_block = &unicode_block_Liberation_SerifRegular_18px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_18px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_18px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_18px;
	unicode_block = &unicode_block_Liberation_SerifRegular_18px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_18px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_20px_BasicLatin) {
	font = &font_Liberation_SerifRegular_20px;
	unicode_block = &unicode_block_Liberation_SerifRegular_20px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_20px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_20px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_20px;
	unicode_block = &unicode_block_Liberation_SerifRegular_20px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_20px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_20px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_20px;
	unicode_block = &unicode_block_Liberation_SerifRegular_20px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_20px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_22px_BasicLatin) {
	font = &font_Liberation_SerifRegular_22px;
	unicode_block = &unicode_block_Liberation_SerifRegular_22px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_22px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_22px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_22px;
	unicode_block = &unicode_block_Liberation_SerifRegular_22px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_22px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_22px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_22px;
	unicode_block = &unicode_block_Liberation_SerifRegular_22px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_22px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_24px_BasicLatin) {
	font = &font_Liberation_SerifRegular_24px;
	unicode_block = &unicode_block_Liberation_SerifRegular_24px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_24px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_24px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_24px;
	unicode_block = &unicode_block_Liberation_SerifRegular_24px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_24px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_24px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_24px;
	unicode_block = &unicode_block_Liberation_SerifRegular_24px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_24px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_26px_BasicLatin) {
	font = &font_Liberation_SerifRegular_26px;
	unicode_block = &unicode_block_Liberation_SerifRegular_26px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_26px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_26px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_26px;
	unicode_block = &unicode_block_Liberation_SerifRegular_26px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_26px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_26px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_26px;
	unicode_block = &unicode_block_Liberation_SerifRegular_26px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_26px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_28px_BasicLatin) {
	font = &font_Liberation_SerifRegular_28px;
	unicode_block = &unicode_block_Liberation_SerifRegular_28px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_28px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_28px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_28px;
	unicode_block = &unicode_block_Liberation_SerifRegular_28px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_28px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_28px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_28px;
	unicode_block = &unicode_block_Liberation_SerifRegular_28px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_28px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_32px_BasicLatin) {
	font = &font_Liberation_SerifRegular_32px;
	unicode_block = &unicode_block_Liberation_SerifRegular_32px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_32px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_32px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_32px;
	unicode_block = &unicode_block_Liberation_SerifRegular_32px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_32px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_32px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_32px;
	unicode_block = &unicode_block_Liberation_SerifRegular_32px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_32px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_36px_BasicLatin) {
	font = &font_Liberation_SerifRegular_36px;
	unicode_block = &unicode_block_Liberation_SerifRegular_36px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_36px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_36px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_36px;
	unicode_block = &unicode_block_Liberation_SerifRegular_36px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_36px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_36px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_36px;
	unicode_block = &unicode_block_Liberation_SerifRegular_36px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_36px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_40px_BasicLatin) {
	font = &font_Liberation_SerifRegular_40px;
	unicode_block = &unicode_block_Liberation_SerifRegular_40px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_40px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_40px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_40px;
	unicode_block = &unicode_block_Liberation_SerifRegular_40px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_40px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_40px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_40px;
	unicode_block = &unicode_block_Liberation_SerifRegular_40px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_40px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_44px_BasicLatin) {
	font = &font_Liberation_SerifRegular_44px;
	unicode_block = &unicode_block_Liberation_SerifRegular_44px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_44px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_44px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_44px;
	unicode_block = &unicode_block_Liberation_SerifRegular_44px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_44px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_44px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_44px;
	unicode_block = &unicode_block_Liberation_SerifRegular_44px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_44px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_48px_BasicLatin) {
	font = &font_Liberation_SerifRegular_48px;
	unicode_block = &unicode_block_Liberation_SerifRegular_48px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_48px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_48px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_48px;
	unicode_block = &unicode_block_Liberation_SerifRegular_48px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_48px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_48px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_48px;
	unicode_block = &unicode_block_Liberation_SerifRegular_48px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_48px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_54px_BasicLatin) {
	font = &font_Liberation_SerifRegular_54px;
	unicode_block = &unicode_block_Liberation_SerifRegular_54px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_54px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_54px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_54px;
	unicode_block = &unicode_block_Liberation_SerifRegular_54px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_54px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_54px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_54px;
	unicode_block = &unicode_block_Liberation_SerifRegular_54px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_54px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_60px_BasicLatin) {
	font = &font_Liberation_SerifRegular_60px;
	unicode_block = &unicode_block_Liberation_SerifRegular_60px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_60px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_60px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_60px;
	unicode_block = &unicode_block_Liberation_SerifRegular_60px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_60px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_60px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_60px;
	unicode_block = &unicode_block_Liberation_SerifRegular_60px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_60px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_66px_BasicLatin) {
	font = &font_Liberation_SerifRegular_66px;
	unicode_block = &unicode_block_Liberation_SerifRegular_66px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_66px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_66px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_66px;
	unicode_block = &unicode_block_Liberation_SerifRegular_66px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_66px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_66px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_66px;
	unicode_block = &unicode_block_Liberation_SerifRegular_66px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_66px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_72px_BasicLatin) {
	font = &font_Liberation_SerifRegular_72px;
	unicode_block = &unicode_block_Liberation_SerifRegular_72px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_72px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_72px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_72px;
	unicode_block = &unicode_block_Liberation_SerifRegular_72px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_72px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_72px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_72px;
	unicode_block = &unicode_block_Liberation_SerifRegular_72px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_72px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_7px_BasicLatin) {
	font = &font_Liberation_SerifRegular_7px;
	unicode_block = &unicode_block_Liberation_SerifRegular_7px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_7px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_7px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_7px;
	unicode_block = &unicode_block_Liberation_SerifRegular_7px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_7px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_7px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_7px;
	unicode_block = &unicode_block_Liberation_SerifRegular_7px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_7px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_80px_BasicLatin) {
	font = &font_Liberation_SerifRegular_80px;
	unicode_block = &unicode_block_Liberation_SerifRegular_80px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_80px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_80px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_80px;
	unicode_block = &unicode_block_Liberation_SerifRegular_80px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_80px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_80px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_80px;
	unicode_block = &unicode_block_Liberation_SerifRegular_80px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_80px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_83px_BasicLatin) {
	font = &font_Liberation_SerifRegular_83px;
	unicode_block = &unicode_block_Liberation_SerifRegular_83px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_83px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_83px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_83px;
	unicode_block = &unicode_block_Liberation_SerifRegular_83px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_83px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_83px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_83px;
	unicode_block = &unicode_block_Liberation_SerifRegular_83px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_83px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_88px_BasicLatin) {
	font = &font_Liberation_SerifRegular_88px;
	unicode_block = &unicode_block_Liberation_SerifRegular_88px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_88px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_88px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_88px;
	unicode_block = &unicode_block_Liberation_SerifRegular_88px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_88px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_88px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_88px;
	unicode_block = &unicode_block_Liberation_SerifRegular_88px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_88px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_8px_BasicLatin) {
	font = &font_Liberation_SerifRegular_8px;
	unicode_block = &unicode_block_Liberation_SerifRegular_8px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_8px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_8px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_8px;
	unicode_block = &unicode_block_Liberation_SerifRegular_8px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_8px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_8px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_8px;
	unicode_block = &unicode_block_Liberation_SerifRegular_8px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_8px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_96px_BasicLatin) {
	font = &font_Liberation_SerifRegular_96px;
	unicode_block = &unicode_block_Liberation_SerifRegular_96px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_96px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_96px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_96px;
	unicode_block = &unicode_block_Liberation_SerifRegular_96px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_96px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_96px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_96px;
	unicode_block = &unicode_block_Liberation_SerifRegular_96px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_96px_LatinExtendedA";
}END_TEST

START_TEST(Liberation_SerifRegular_9px_BasicLatin) {
	font = &font_Liberation_SerifRegular_9px;
	unicode_block = &unicode_block_Liberation_SerifRegular_9px_BasicLatin;
	expectation_name = "Liberation_SerifRegular_9px_BasicLatin";
}END_TEST

START_TEST(Liberation_SerifRegular_9px_Latin1Supplement) {
	font = &font_Liberation_SerifRegular_9px;
	unicode_block = &unicode_block_Liberation_SerifRegular_9px_Latin1Supplement;
	expectation_name = "Liberation_SerifRegular_9px_Latin1Supplement";
}END_TEST

START_TEST(Liberation_SerifRegular_9px_LatinExtendedA) {
	font = &font_Liberation_SerifRegular_9px;
	unicode_block = &unicode_block_Liberation_SerifRegular_9px_LatinExtendedA;
	expectation_name = "Liberation_SerifRegular_9px_LatinExtendedA";
}END_TEST

Suite * build_suite(void) {
	Suite *suite;
	TCase *tcase;

	suite = suite_create("liberation");

	tcase = tcase_create("liberation");
	tcase_add_checked_fixture(tcase, setup, teardown);
	tcase_add_test(tcase, Liberation_MonoBold_10px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_10px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_10px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBold_11px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_11px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_11px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBold_12px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_12px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_12px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBold_13px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_13px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_13px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBold_14px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_14px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_14px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBold_15px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_15px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_15px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBold_16px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_16px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_16px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBold_18px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_18px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_18px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBold_20px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_20px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_20px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBold_22px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_22px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_22px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBold_24px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_24px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_24px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBold_26px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_26px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_26px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBold_28px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_28px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_28px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBold_32px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_32px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_32px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBold_36px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_36px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_36px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBold_40px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_40px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_40px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBold_44px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_44px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_44px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBold_48px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_48px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_48px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBold_54px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_54px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_54px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBold_60px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_60px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_60px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBold_66px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_66px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_66px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBold_72px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_72px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_72px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBold_7px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_7px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_7px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBold_80px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_80px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_80px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBold_83px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_83px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_83px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBold_88px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_88px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_88px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBold_8px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_8px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_8px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBold_96px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_96px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_96px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBold_9px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBold_9px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBold_9px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_10px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_10px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_10px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_11px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_11px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_11px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_12px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_12px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_12px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_13px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_13px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_13px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_14px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_14px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_14px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_15px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_15px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_15px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_16px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_16px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_16px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_18px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_18px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_18px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_20px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_20px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_20px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_22px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_22px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_22px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_24px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_24px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_24px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_26px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_26px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_26px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_28px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_28px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_28px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_32px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_32px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_32px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_36px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_36px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_36px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_40px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_40px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_40px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_44px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_44px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_44px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_48px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_48px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_48px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_54px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_54px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_54px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_60px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_60px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_60px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_66px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_66px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_66px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_72px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_72px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_72px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_7px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_7px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_7px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_80px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_80px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_80px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_83px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_83px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_83px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_88px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_88px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_88px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_8px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_8px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_8px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_96px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_96px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_96px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_9px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_9px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoBoldItalic_9px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_10px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_10px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_10px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_11px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_11px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_11px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_12px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_12px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_12px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_13px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_13px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_13px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_14px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_14px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_14px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_15px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_15px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_15px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_16px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_16px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_16px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_18px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_18px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_18px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_20px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_20px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_20px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_22px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_22px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_22px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_24px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_24px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_24px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_26px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_26px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_26px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_28px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_28px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_28px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_32px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_32px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_32px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_36px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_36px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_36px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_40px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_40px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_40px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_44px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_44px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_44px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_48px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_48px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_48px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_54px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_54px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_54px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_60px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_60px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_60px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_66px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_66px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_66px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_72px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_72px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_72px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_7px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_7px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_7px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_80px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_80px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_80px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_83px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_83px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_83px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_88px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_88px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_88px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_8px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_8px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_8px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_96px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_96px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_96px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoItalic_9px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoItalic_9px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoItalic_9px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_10px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_10px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_10px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_11px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_11px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_11px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_12px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_12px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_12px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_13px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_13px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_13px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_14px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_14px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_14px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_15px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_15px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_15px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_16px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_16px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_16px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_18px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_18px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_18px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_20px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_20px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_20px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_22px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_22px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_22px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_24px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_24px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_24px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_26px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_26px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_26px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_28px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_28px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_28px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_32px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_32px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_32px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_36px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_36px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_36px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_40px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_40px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_40px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_44px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_44px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_44px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_48px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_48px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_48px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_54px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_54px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_54px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_60px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_60px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_60px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_66px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_66px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_66px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_72px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_72px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_72px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_7px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_7px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_7px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_80px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_80px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_80px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_83px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_83px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_83px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_88px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_88px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_88px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_8px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_8px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_8px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_96px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_96px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_96px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_MonoRegular_9px_BasicLatin);
	tcase_add_test(tcase, Liberation_MonoRegular_9px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_MonoRegular_9px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_10px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_10px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_10px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_11px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_11px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_11px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_12px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_12px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_12px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_13px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_13px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_13px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_14px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_14px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_14px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_15px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_15px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_15px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_16px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_16px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_16px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_18px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_18px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_18px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_20px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_20px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_20px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_22px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_22px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_22px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_24px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_24px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_24px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_26px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_26px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_26px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_28px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_28px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_28px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_32px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_32px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_32px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_36px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_36px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_36px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_40px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_40px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_40px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_44px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_44px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_44px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_48px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_48px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_48px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_54px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_54px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_54px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_60px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_60px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_60px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_66px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_66px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_66px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_72px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_72px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_72px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_7px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_7px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_7px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_80px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_80px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_80px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_83px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_83px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_83px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_88px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_88px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_88px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_8px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_8px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_8px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_96px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_96px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_96px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBold_9px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBold_9px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBold_9px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_10px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_10px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_10px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_11px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_11px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_11px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_12px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_12px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_12px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_13px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_13px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_13px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_14px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_14px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_14px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_15px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_15px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_15px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_16px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_16px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_16px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_18px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_18px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_18px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_20px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_20px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_20px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_22px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_22px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_22px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_24px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_24px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_24px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_26px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_26px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_26px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_28px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_28px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_28px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_32px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_32px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_32px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_36px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_36px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_36px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_40px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_40px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_40px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_44px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_44px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_44px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_48px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_48px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_48px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_54px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_54px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_54px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_60px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_60px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_60px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_66px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_66px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_66px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_72px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_72px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_72px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_7px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_7px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_7px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_80px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_80px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_80px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_83px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_83px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_83px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_88px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_88px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_88px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_8px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_8px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_8px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_96px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_96px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_96px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansBoldItalic_9px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansBoldItalic_9px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansBoldItalic_9px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_10px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_10px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_10px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_11px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_11px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_11px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_12px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_12px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_12px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_13px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_13px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_13px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_14px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_14px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_14px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_15px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_15px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_15px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_16px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_16px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_16px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_18px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_18px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_18px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_20px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_20px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_20px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_22px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_22px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_22px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_24px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_24px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_24px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_26px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_26px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_26px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_28px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_28px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_28px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_32px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_32px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_32px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_36px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_36px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_36px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_40px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_40px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_40px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_44px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_44px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_44px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_48px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_48px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_48px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_54px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_54px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_54px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_60px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_60px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_60px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_66px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_66px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_66px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_72px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_72px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_72px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_7px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_7px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_7px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_80px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_80px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_80px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_83px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_83px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_83px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_88px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_88px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_88px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_8px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_8px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_8px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_96px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_96px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_96px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansItalic_9px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansItalic_9px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansItalic_9px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_10px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_10px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_10px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_11px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_11px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_11px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_12px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_12px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_12px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_13px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_13px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_13px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_14px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_14px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_14px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_15px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_15px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_15px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_16px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_16px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_16px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_18px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_18px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_18px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_20px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_20px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_20px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_22px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_22px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_22px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_24px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_24px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_24px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_26px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_26px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_26px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_28px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_28px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_28px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_32px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_32px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_32px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_36px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_36px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_36px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_40px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_40px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_40px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_44px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_44px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_44px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_48px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_48px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_48px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_54px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_54px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_54px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_60px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_60px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_60px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_66px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_66px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_66px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_72px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_72px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_72px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_7px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_7px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_7px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_80px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_80px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_80px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_83px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_83px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_83px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_88px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_88px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_88px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_8px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_8px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_8px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_96px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_96px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_96px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBold_9px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBold_9px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBold_9px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_10px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_10px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_10px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_11px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_11px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_11px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_12px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_12px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_12px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_13px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_13px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_13px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_14px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_14px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_14px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_15px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_15px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_15px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_16px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_16px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_16px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_18px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_18px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_18px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_20px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_20px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_20px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_22px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_22px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_22px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_24px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_24px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_24px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_26px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_26px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_26px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_28px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_28px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_28px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_32px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_32px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_32px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_36px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_36px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_36px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_40px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_40px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_40px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_44px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_44px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_44px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_48px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_48px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_48px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_54px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_54px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_54px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_60px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_60px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_60px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_66px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_66px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_66px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_72px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_72px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_72px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_7px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_7px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_7px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_80px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_80px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_80px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_83px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_83px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_83px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_88px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_88px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_88px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_8px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_8px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_8px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_96px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_96px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_96px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_9px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_9px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowBoldItalic_9px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_10px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_10px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_10px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_11px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_11px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_11px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_12px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_12px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_12px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_13px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_13px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_13px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_14px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_14px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_14px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_15px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_15px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_15px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_16px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_16px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_16px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_18px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_18px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_18px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_20px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_20px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_20px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_22px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_22px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_22px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_24px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_24px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_24px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_26px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_26px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_26px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_28px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_28px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_28px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_32px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_32px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_32px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_36px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_36px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_36px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_40px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_40px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_40px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_44px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_44px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_44px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_48px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_48px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_48px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_54px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_54px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_54px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_60px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_60px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_60px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_66px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_66px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_66px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_72px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_72px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_72px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_7px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_7px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_7px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_80px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_80px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_80px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_83px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_83px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_83px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_88px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_88px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_88px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_8px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_8px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_8px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_96px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_96px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_96px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_9px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_9px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowItalic_9px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_10px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_10px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_10px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_11px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_11px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_11px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_12px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_12px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_12px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_13px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_13px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_13px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_14px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_14px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_14px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_15px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_15px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_15px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_16px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_16px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_16px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_18px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_18px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_18px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_20px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_20px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_20px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_22px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_22px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_22px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_24px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_24px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_24px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_26px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_26px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_26px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_28px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_28px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_28px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_32px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_32px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_32px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_36px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_36px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_36px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_40px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_40px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_40px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_44px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_44px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_44px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_48px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_48px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_48px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_54px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_54px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_54px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_60px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_60px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_60px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_66px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_66px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_66px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_72px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_72px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_72px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_7px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_7px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_7px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_80px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_80px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_80px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_83px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_83px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_83px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_88px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_88px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_88px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_8px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_8px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_8px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_96px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_96px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_96px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_9px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_9px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansNarrowRegular_9px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_10px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_10px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_10px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_11px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_11px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_11px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_12px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_12px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_12px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_13px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_13px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_13px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_14px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_14px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_14px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_15px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_15px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_15px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_16px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_16px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_16px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_18px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_18px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_18px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_20px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_20px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_20px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_22px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_22px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_22px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_24px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_24px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_24px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_26px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_26px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_26px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_28px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_28px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_28px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_32px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_32px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_32px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_36px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_36px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_36px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_40px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_40px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_40px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_44px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_44px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_44px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_48px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_48px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_48px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_54px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_54px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_54px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_60px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_60px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_60px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_66px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_66px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_66px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_72px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_72px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_72px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_7px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_7px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_7px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_80px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_80px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_80px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_83px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_83px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_83px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_88px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_88px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_88px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_8px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_8px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_8px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_96px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_96px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_96px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SansRegular_9px_BasicLatin);
	tcase_add_test(tcase, Liberation_SansRegular_9px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SansRegular_9px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_10px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_10px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_10px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_11px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_11px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_11px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_12px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_12px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_12px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_13px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_13px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_13px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_14px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_14px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_14px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_15px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_15px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_15px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_16px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_16px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_16px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_18px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_18px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_18px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_20px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_20px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_20px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_22px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_22px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_22px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_24px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_24px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_24px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_26px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_26px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_26px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_28px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_28px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_28px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_32px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_32px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_32px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_36px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_36px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_36px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_40px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_40px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_40px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_44px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_44px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_44px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_48px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_48px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_48px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_54px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_54px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_54px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_60px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_60px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_60px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_66px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_66px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_66px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_72px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_72px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_72px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_7px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_7px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_7px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_80px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_80px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_80px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_83px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_83px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_83px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_88px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_88px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_88px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_8px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_8px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_8px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_96px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_96px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_96px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBold_9px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBold_9px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBold_9px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_10px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_10px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_10px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_11px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_11px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_11px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_12px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_12px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_12px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_13px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_13px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_13px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_14px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_14px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_14px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_15px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_15px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_15px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_16px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_16px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_16px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_18px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_18px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_18px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_20px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_20px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_20px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_22px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_22px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_22px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_24px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_24px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_24px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_26px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_26px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_26px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_28px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_28px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_28px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_32px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_32px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_32px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_36px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_36px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_36px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_40px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_40px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_40px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_44px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_44px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_44px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_48px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_48px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_48px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_54px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_54px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_54px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_60px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_60px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_60px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_66px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_66px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_66px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_72px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_72px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_72px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_7px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_7px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_7px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_80px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_80px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_80px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_83px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_83px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_83px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_88px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_88px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_88px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_8px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_8px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_8px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_96px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_96px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_96px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_9px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_9px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifBoldItalic_9px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_10px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_10px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_10px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_11px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_11px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_11px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_12px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_12px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_12px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_13px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_13px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_13px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_14px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_14px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_14px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_15px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_15px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_15px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_16px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_16px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_16px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_18px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_18px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_18px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_20px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_20px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_20px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_22px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_22px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_22px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_24px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_24px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_24px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_26px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_26px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_26px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_28px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_28px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_28px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_32px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_32px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_32px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_36px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_36px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_36px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_40px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_40px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_40px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_44px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_44px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_44px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_48px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_48px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_48px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_54px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_54px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_54px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_60px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_60px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_60px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_66px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_66px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_66px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_72px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_72px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_72px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_7px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_7px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_7px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_80px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_80px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_80px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_83px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_83px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_83px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_88px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_88px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_88px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_8px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_8px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_8px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_96px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_96px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_96px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifItalic_9px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifItalic_9px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifItalic_9px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_10px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_10px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_10px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_11px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_11px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_11px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_12px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_12px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_12px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_13px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_13px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_13px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_14px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_14px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_14px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_15px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_15px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_15px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_16px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_16px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_16px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_18px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_18px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_18px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_20px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_20px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_20px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_22px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_22px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_22px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_24px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_24px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_24px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_26px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_26px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_26px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_28px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_28px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_28px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_32px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_32px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_32px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_36px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_36px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_36px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_40px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_40px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_40px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_44px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_44px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_44px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_48px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_48px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_48px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_54px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_54px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_54px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_60px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_60px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_60px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_66px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_66px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_66px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_72px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_72px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_72px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_7px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_7px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_7px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_80px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_80px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_80px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_83px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_83px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_83px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_88px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_88px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_88px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_8px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_8px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_8px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_96px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_96px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_96px_LatinExtendedA);
	tcase_add_test(tcase, Liberation_SerifRegular_9px_BasicLatin);
	tcase_add_test(tcase, Liberation_SerifRegular_9px_Latin1Supplement);
	tcase_add_test(tcase, Liberation_SerifRegular_9px_LatinExtendedA);

	suite_add_tcase(suite, tcase);

	return suite;
}