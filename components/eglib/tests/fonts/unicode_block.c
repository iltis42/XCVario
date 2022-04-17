#include "../common.h"
#include "unicode_block.h"
#include <eglib.h>
#include <eglib/display/tga.h>
#include <eglib/drawing.h>
#include <eglib/hal/four_wire_spi/none.h>

#define CHARS_PER_ROW 32

extern struct font_t *font;
extern struct glyph_unicode_block_t *unicode_block;
extern char *expectation_name;

void setup(void) {

}

static void get_dimensions(coordinate_t *width, int16_t *rows) {
	eglib_t eglib_probe;
	tga_config_t tga_config_probe = {
		.width = 100,
		.height = 100,
	};
	wchar_t charcode;

	eglib_Init(&eglib_probe, &four_wire_spi_none, NULL, &tga, &tga_config_probe);


	eglib_SetFont(&eglib_probe, font);
	eglib_AddUnicodeBlockToFont(font, unicode_block);

	*width = 0;
	*rows = 0;

	for(charcode = unicode_block->charcode_start ; charcode <= unicode_block->charcode_end ; ) {
		wchar_t charcode_end;
		coordinate_t new_width;
		const struct glyph_t *glyph;

		*rows += 1;

		charcode_end = charcode + CHARS_PER_ROW;
		new_width = 0;
		for(;charcode < charcode_end && charcode <= unicode_block->charcode_end; charcode++) {
			glyph = eglib_GetGlyph(&eglib_probe, charcode);
			if(glyph == NULL)
				new_width += font->pixel_size;
			else
				new_width += glyph->advance;
		}
		if(new_width > *width)
			*width = new_width;
	}

	tga_Free(&eglib_probe);
}

void teardown(void) {
	eglib_t eglib;
	tga_config_t tga_config;
	coordinate_t width;
	int16_t rows;
	wchar_t charcode;

	get_dimensions(&width, &rows);

	tga_config.width = width;
	tga_config.height = font->line_space * rows;

	eglib_Init(&eglib, &four_wire_spi_none, NULL, &tga, &tga_config);

	eglib_SetIndexColor(&eglib, 0, 255, 255, 255);
	eglib_ClearScreen(&eglib);

	eglib_SetFont(&eglib, font);
	eglib_SetIndexColor(&eglib, 0, 0, 0, 0);
	charcode = unicode_block->charcode_start;
	for(int row=0 ; row < rows ; row++) {
		const struct glyph_t *glyph;
		coordinate_t x;

		x=0;
		for(int i=0 ; i < CHARS_PER_ROW && charcode <= unicode_block->charcode_end; i++) {
			glyph = eglib_GetGlyph(&eglib, charcode);
			eglib_DrawGlyph(&eglib, x, row * font->line_space + font->ascent, glyph);
			if(glyph == NULL)
				x += font->pixel_size;
			else
				x += glyph->advance;
			charcode++;
		}
	}

	compare_expectation(expectation_name, &eglib);
}