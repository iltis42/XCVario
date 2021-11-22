#ifndef EGLIB_DRAWING_H
#define EGLIB_DRAWING_H

#include "../eglib.h"
#include <wchar.h>

/* eglib Circle and Disc options */
#define EGLIB_DRAW_UPPER_RIGHT 0x01
#define EGLIB_DRAW_UPPER_LEFT  0x02
#define EGLIB_DRAW_LOWER_LEFT 0x04
#define EGLIB_DRAW_LOWER_RIGHT  0x08
#define EGLIB_DRAW_ALL (EGLIB_DRAW_UPPER_RIGHT|EGLIB_DRAW_UPPER_LEFT|EGLIB_DRAW_LOWER_RIGHT|EGLIB_DRAW_LOWER_LEFT)

/**
 * These are generic drawing functions.
 *
 * :See also: :c:func:`eglib_Init` or :c:func:`eglib_Init_FrameBuffer`.
 */

/**
 * Color
 * =====
 *
 * Various drawing functions do not take a color argument and instead use the
 * color from a previously defined index.
 */

/** Set color for given index, which other drawing functions may use. */
void eglib_SetIndexColor(
	eglib_t *eglib,
	size_t idx,
	color_channel_t r,
	color_channel_t g,
	color_channel_t b
);

/**
 * Pixel
 * =====
 */

/**
 * Draw given pixel coordinates with given color.
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawPixelColor.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawPixelColor.png
 *   :width: 200
 */
void eglib_DrawPixelColor(eglib_t *eglib, coordinate_t x, coordinate_t y, color_t color);

/**
 * Draw given pixel coordinates using color from index 0.
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawPixel.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawPixel.png
 *   :width: 200
 *
 * :See also: :c:func:`eglib_SetIndexColor`.
 */
void eglib_DrawPixel(eglib_t *eglib, coordinate_t x, coordinate_t y);

/**
 * Lines
 * =====
 */

/**
 * Draw line from coordinates (`x1`, `y1`) to (`x2`, `y2`) using color from
 * index 0.
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawLine.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawLine.png
 *   :width: 200
 *
 * :See also: :c:func:`eglib_SetIndexColor`.
 */
void eglib_DrawLine(eglib_t *eglib, coordinate_t x1, coordinate_t y1, coordinate_t x2, coordinate_t y2);

/**
 * Draw horizontal line starting at (`x`, `y`) with length `len` using color from
 * index 0.
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawHLine.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawHLine.png
 *   :width: 200
 *
 * :See also: :c:func:`eglib_SetIndexColor`.
 */
#define eglib_DrawHLine(eglib, x, y, len) eglib_DrawLine(eglib, x, y, x + len - 1, y);

/**
 * Draw vertical line starting at (`x`, `y`) with length `len` using color from
 * index 0.
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawVLine.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawVLine.png
 *   :width: 200
 *
 * :See also: :c:func:`eglib_SetIndexColor`.
 */
#define eglib_DrawVLine(eglib, x, y, len) eglib_DrawLine(eglib, x, y, x, y + len - 1);

/**
 * Draw line from coordinates (`x1`, `y1`) to (`x2`, `y2`).
 *
 * Line color will be a gradient from index 0 at (`x1`, `y1`) to color from index
 * 1 at (`x2`, `y2`).
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawGradientLine.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawGradientLine.png
 *   :width: 200
 *
 * :See also: :c:func:`eglib_SetIndexColor`.
 */
void eglib_DrawGradientLine(
	eglib_t *eglib,
	coordinate_t x1, coordinate_t y1,
	coordinate_t x2, coordinate_t y2
);

/**
 * Draw horizontal gradient line from coordinates (`x`, `y`) to (`x + len`, `y`).
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawGradientHLine.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawGradientHLine.png
 *   :width: 200
 *
 * :See also: :c:func:`eglib_DrawGradientLine`.
 */
#define eglib_DrawGradientHLine(eglib, x, y, len) eglib_DrawGradientLine( \
	eglib, \
	x, y, \
	x + len - 1, y \
)

/**
 * Draw vertical gradient line from coordinates (`x`, `y`) to (`x`, `y + len`).
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawGradientVLine.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawGradientVLine.png
 *   :width: 200
 *
 * :See also: :c:func:`eglib_DrawGradientLine`.
 */
#define eglib_DrawGradientVLine(eglib, x, y, len) eglib_DrawGradientLine( \
	eglib, \
	x, y, \
	x, y + len - 1 \
)

/*================================================*/
/* eglib_polygon */

typedef int16_t pg_word_t;

//#define PG_NOINLINE UCG_NOINLINE
#define PG_NOINLINE __attribute__((noinline))

struct pg_point_struct
{
  pg_word_t x;
  pg_word_t y;
};

typedef struct _pg_struct pg_struct;	/* forward declaration */

struct pg_edge_struct
{
  pg_word_t x_direction;	/* 1, if x2 is greater than x1, -1 otherwise */
  pg_word_t height;
  pg_word_t current_x_offset;
  pg_word_t error_offset;
  
  /* --- line loop --- */
  pg_word_t current_y;
  pg_word_t max_y;
  pg_word_t current_x;
  pg_word_t error;

  /* --- outer loop --- */
  uint8_t (*next_idx_fn)(pg_struct *pg, uint8_t i);
  uint8_t curr_idx;
};

/* maximum number of points in the polygon */
/* can be redefined, but highest possible value is 254 */
#define PG_MAX_POINTS 4

/* index numbers for the pge structures below */
#define PG_LEFT 0
#define PG_RIGHT 1


struct _pg_struct
{
  struct pg_point_struct list[PG_MAX_POINTS];
  uint8_t cnt;
  uint8_t is_min_y_not_flat;
  pg_word_t total_scan_line_cnt;
  struct pg_edge_struct pge[2];	/* left and right line draw structures */
};

void pg_ClearPolygonXY(pg_struct *pg);
void pg_AddPolygonXY(pg_struct *pg, eglib_t *eglib, int16_t x, int16_t y);
void pg_DrawPolygon(pg_struct *pg, eglib_t *eglib);



/**
 * Triangles
 * =========
 */

/**
 * Draw triangle for coordinates (`x1`, `y1`), (`x2`, `y2`) and (`x3`, `y3`)
 * using color from index 0.
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawTriangle.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawTriangle.png
 *   :width: 200
 *
 * :See also: :c:func:`eglib_SetIndexColor`.
 */
void eglib_DrawTriangle(
	eglib_t *eglib,
	coordinate_t x1, coordinate_t y1,
	coordinate_t x2, coordinate_t y2,
	coordinate_t x3, coordinate_t y3
);

/**
 * Draw filled Triangle for coordinates (`x1`, `y1`), (`x2`, `y2`) and (`x3`, `y3`)
 * using color from index 0.
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawFilledTriangle.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawFilledTriangle.png
 *   :width: 200
 *
 * :See also: :c:func:`eglib_SetIndexColor`.
 */
void eglib_DrawFilledTriangle(
	eglib_t *eglib,
	coordinate_t x1, coordinate_t y1,
	coordinate_t x2, coordinate_t y2,
	coordinate_t x3, coordinate_t y3
);

/**
 * Draw (filled) Tetragon for coordinates (`x0`, `y0`), (`x1`, `y1`), (`x2`, `y2`) and (`x3`, `y3`)
 * using color from index 0.
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawTetragon.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawTetragon.png
 *   :width: 200
 *
 * :See also: :c:func:`eglib_SetIndexColor`.
 */
void eglib_DrawTetragon(
	eglib_t *eglib,
	coordinate_t x0, coordinate_t y0,
	coordinate_t x1, coordinate_t y1,
	coordinate_t x2, coordinate_t y2,
	coordinate_t x3, coordinate_t y3
);

/**
 * Frames
 * ======
 */

/**
 * Draw frame starting at ``(x, y)`` with ``width`` and ``height`` using color
 * from index 0.
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawFrame.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawFrame.png
 *   :width: 200
 *
 * :See also: :c:func:`eglib_SetIndexColor`.
 */
void eglib_DrawFrame(
	eglib_t *eglib,
	coordinate_t x, coordinate_t y,
	coordinate_t width, coordinate_t height
);

/**
 * Draw frame starting at ``(x, y)`` with ``width`` and ``height``.
 *
 * Its colors will be a gradient:
 *
 * - Top left: color index 0.
 * - Top right: color index 1.
 * - Bottom left: color index 2.
 * - Bottom right: color index 3.
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawGradientFrame.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawGradientFrame.png
 *   :width: 200
 *
 * :See also: :c:func:`eglib_SetIndexColor`.
 */
void eglib_DrawGradientFrame(
	eglib_t *eglib,
	coordinate_t x, coordinate_t y,
	coordinate_t width, coordinate_t height
);

/**
 * Draw frame starting at ``(x, y)``, with ``width`` and ``height`` and rounded
 * edges with ``radius``, using color from index 0.
 *
 * Both ``width`` and ``height`` must be greater than ``2 * radius``.
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawRoundFrame.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawRoundFrame.png
 *   :width: 200
 *
 * :See also: :c:func:`eglib_SetIndexColor`.
 */
void eglib_DrawRoundFrame(
	eglib_t *eglib,
	coordinate_t x, coordinate_t y,
	coordinate_t width, coordinate_t height,
	coordinate_t radius
);

/**
 * Boxes
 * =====
 */

/**
 * Draw box starting at ``(x, y)`` with ``width`` and ``height`` using color
 * from index 0.
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawBox.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawBox.png
 *   :width: 200
 *
 * :See also: :c:func:`eglib_SetIndexColor`.
 */
void eglib_DrawBox(
	eglib_t *eglib,
	coordinate_t x, coordinate_t y,
	coordinate_t width, coordinate_t height
);

/**
 * Draw box starting at ``(x, y)`` with ``width`` and ``height``.
 *
 * Its colors will be a gradient:
 *
 * - Top left: color index 0.
 * - Top right: color index 1.
 * - Bottom left: color index 2.
 * - Bottom right: color index 3.
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawGradientBox.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawGradientBox.png
 *   :width: 200
 *
 * :See also: :c:func:`eglib_SetIndexColor`.
 */
void eglib_DrawGradientBox(
	eglib_t *eglib,
	coordinate_t x, coordinate_t y,
	coordinate_t width, coordinate_t height
);

/**
 * Draw frame starting at ``(x, y)``, with ``width`` and ``height`` and rounded
 * edges with ``radius``, using color from index 0.
 *
 * Both ``width`` and ``height`` must be greater than ``2 * radius``.
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawRoundBox.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawRoundBox.png
 *   :width: 200
 *
 * :See also: :c:func:`eglib_SetIndexColor`.
 */
void eglib_DrawRoundBox(
	eglib_t *eglib,
	coordinate_t x, coordinate_t y,
	coordinate_t width, coordinate_t height,
	coordinate_t radius
);

/**
 * Set all pixels to the color from index 0.
 *
 * Example:
 *
 * .. literalinclude:: eglib_ClearScreen.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_ClearScreen.png
 *   :width: 200
 *
 * :See also: :c:func:`eglib_SetIndexColor`.
 */
void eglib_ClearScreen(eglib_t *eglib);

/**
 * Round things
 * ============
 */

/**
 * Draw an arc with color from index 0
 *
 * :param x: Center x.
 * :param y: Center y.
 * :param radius: Radius.
 * :param start_angle: Start angle from 0 to 360, where 0 is "up" and 90 is "right".
 * :param end_angle: End angle, bigger than ``start_angle`` and less or equal to 360.
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawArc.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawArc.png
 *   :width: 200
 */
void eglib_DrawArc(
	eglib_t *eglib,
	coordinate_t x, coordinate_t y,
	coordinate_t radius,
	float start_angle,
	float end_angle
);

/**
 * Draw an arc with color gradient from index 0 to index 1.
 *
 * :param x: Center x.
 * :param y: Center y.
 * :param radius: Radius.
 * :param start_angle: Start angle from 0 to 360, where 0 is "up" and 90 is "right".
 * :param end_angle: End angle, bigger than ``start_angle`` and less or equal to 360.
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawGradientArc.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawGradientArc.png
 *   :width: 200
 */
void eglib_DrawGradientArc(
	eglib_t *eglib,
	coordinate_t x, coordinate_t y,
	coordinate_t radius,
	float start_angle,
	float end_angle
);

/**
 * Draw an arc with color from index 0
 *
 * :param x: Center x.
 * :param y: Center y.
 * :param radius: Radius.
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawCircle.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawCircle.png
 *   :width: 200
 */
//#define eglib_DrawCircle(eglib, x, y, radius) eglib_DrawArc(eglib, x, y, radius, 0, 360);

/**
 * Draw a circle with color from index 0
 *
 * :param x: Center x.
 * :param y: Center y.
 * :param radius: Radius.
 * :param option being binary coding of the 4 quadrants 
 * eglib Circle and Disc options *
 * EGLIB_DRAW_UPPER_RIGHT 
 * EGLIB_DRAW_UPPER_LEFT  
 * EGLIB_DRAW_LOWER_LEFT 
 * EGLIB_DRAW_LOWER_RIGHT  
 * EGLIB_DRAW_ALL 
 */
void eglib_DrawCircle(eglib_t *eglib, int16_t x0, int16_t y0, int16_t rad, uint8_t option);

//#define eglib_DrawDisc(eglib, x, y, radius) eglib_DrawArc(eglib, x, y, radius, 0, 360);

/**
 * Draw a filled disc with color from index 0
 *
 * :param x: Center x.
 * :param y: Center y.
 * :param radius: Radius.
 * :param option being binary coding of the 4 quadrants 
 * eglib Circle and Disc options *
 * EGLIB_DRAW_UPPER_RIGHT 
 * EGLIB_DRAW_UPPER_LEFT  
 * EGLIB_DRAW_LOWER_LEFT 
 * EGLIB_DRAW_LOWER_RIGHT  
 * EGLIB_DRAW_ALL 
 */
void eglib_DrawDisc(eglib_t *eglib, int16_t x0, int16_t y0, int16_t rad, uint8_t option);

/**
 * Draw a filled arc with color from index 0
 *
 * :param x: Center x.
 * :param y: Center y.
 * :param radius: Radius.
 * :param start_angle: Start angle from 0 to 360, where 0 is "up" and 90 is "right".
 * :param end_angle: End angle, bigger than ``start_angle`` and less or equal to 360.
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawFilledArc.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawFilledArc.png
 *   :width: 200
 */
void eglib_DrawFilledArc(
	eglib_t *eglib,
	coordinate_t x, coordinate_t y,
	coordinate_t radius,
	float start_angle,
	float end_angle
);

/**
 * Draw a filled arc with color gradient from index 0 at the center and index 1
 * at the radius.
 *
 * :param x: Center x.
 * :param y: Center y.
 * :param radius: Radius.
 * :param start_angle: Start angle from 0 to 360, where 0 is "up" and 90 is "right".
 * :param end_angle: End angle, bigger than ``start_angle`` and less or equal to 360.
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawGradientFilledArc.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawGradientFilledArc.png
 *   :width: 200
 */
void eglib_DrawGradientFilledArc(
	eglib_t *eglib,
	coordinate_t x, coordinate_t y,
	coordinate_t radius,
	float start_angle,
	float end_angle
);

/**
 * Draw a disc with color from index 0
 *
 * :param x: Center x.
 * :param y: Center y.
 * :param radius: Radius.
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawDisc.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawDisc.png
 *   :width: 200
 */
//#define eglib_DrawDisc(eglib, x, y, radius) eglib_DrawFilledArc(eglib, x, y, radius, 0, 360)

/**
 * Draw a disc with color gradient from index 0 at the center and index 1 at the
 * radius.
 *
 * :param x: Center x.
 * :param y: Center y.
 * :param radius: Radius.
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawGradientDisc.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawGradientDisc.png
 *   :width: 200
 */
#define eglib_DrawGradientDisc(eglib, x, y, radius) eglib_DrawGradientFilledArc(eglib, x, y, radius, 0, 360)

/**
 * Bitmaps
 * =======
 */

/**
 * Format of bitmap data.
 */
enum bitmap_format_t {
	/**
	 * 1 bit per pixel black and white.
	 *
	 * :Tip: this is the same data format as `XBM <https://en.wikipedia.org/wiki/X_BitMap>`_.
	 */
	BITMAP_BW,
	/** 8bit per channel RGB */
	BITMAP_RGB24,
};

/**
 * A bitmap definition.
 *
 * :See also: :c:func:`eglib_DrawBitmap`.
 */
struct bitmap_t {
	/** Width */
	coordinate_t width;
	/** Height */
	coordinate_t height;
	/** Data format */
	enum bitmap_format_t format;
	/** Pointer to bitmap data */
	uint8_t *data;
};

/**
 * Draw given bitmap at ``(x, y)``.
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawBitmap.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawBitmap.png
 *   :width: 200
 */
void eglib_DrawBitmap(
	eglib_t *eglib,
	coordinate_t x,
	coordinate_t y,
	const struct bitmap_t *bitmap
);

#include <eglib/drawing/fonts.h>

/**
 * Text
 * ====
 */

/**
 * A glyph.
 *
 * :See also: :c:type:`glyph_block_t`.
 */
struct glyph_t {
	/** Bitmap width. */
	uint8_t width : 7;
	/** Bitmap height. */
	uint8_t height : 7;
	/** Left padding before bitmap. */
	int8_t left : 6;
	/** Distance to increment the pen position after rendering this glyph. */
	uint8_t advance : 7;
	/** Distance from baseline to glyph's highest pixel. */
	int8_t top;
	/** Bitmap data. */
	const uint8_t *data;
};

/**
 * Glyphs for a `Unicode block <https://en.wikipedia.org/wiki/Unicode_block>`_.
 *
 * :See also: :c:type:`font_t`.
 */
struct glyph_unicode_block_t {
	/** First unicode character code this font supports. */
	wchar_t charcode_start;
	/** Last unicode character code this font supports. */
	wchar_t charcode_end;
	/** Array of glyphs for each supported unicode character. */
	const struct glyph_t **glyphs;
};

#define FONT_MAX_UNICODE_BLOCKS 5

/**
 * A Font definition.
 */
struct font_t {
	/** Font pixel size. */
	uint8_t pixel_size;
	/** The distance from the baseline to the highest or upper grid coordinate used to place an outline point. */
	int16_t ascent;
	/** The distance from the baseline to the lowest grid coordinate used to place an outline point. */
	int16_t descent;
	/** The distance that must be placed between two lines of text. */
	uint16_t line_space;
	/** Array of glyph unicode blocks. */
	const struct glyph_unicode_block_t *unicode_blocks[FONT_MAX_UNICODE_BLOCKS];
	/** Number of ``unicode_blocks``. */
	uint8_t unicode_blocks_count;
};

/**
 * Set font to be used by other font functions.
 *
 * :See also: :doc:`fonts`.
 * :See also: :c:func:`eglib_AddUnicodeBlockToFont`.
 */
void eglib_SetFont(eglib_t *eglib, const struct font_t *font);

/**
 * Add given unicode block to font.
 *
 * Returns `true` in case of error, `false` on success.
 *
 * Example:
 *
 * .. literalinclude:: eglib_AddUnicodeBlockToFont.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_AddUnicodeBlockToFont.png
 *   :width: 200
 *
 * :See also: :doc:`fonts`
 * :See also: :c:func:`eglib_SetFont`.
 */
bool eglib_AddUnicodeBlockToFont(
	struct font_t *font,
	const struct glyph_unicode_block_t *unicode_block
);

/**
 * Return given unicode character's :c:type:`glyph_t` or `NULL` if unsupported
 * by font.
 */
const struct glyph_t *eglib_GetGlyph(eglib_t *eglib, wchar_t unicode_char);

/**
 * Draw given :c:type:`glyph_t` at ``(x, y)``.
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawGlyph.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawGlyph.png
 *   :width: 200
 */
void eglib_DrawGlyph(eglib_t *eglib, coordinate_t x, coordinate_t y, const struct glyph_t *glyph);

/**
 * Draw given unicode character glyph at ``(x, y)``.
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawWChar.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawWChar.png
 *   :width: 200
 */
void eglib_DrawWChar(eglib_t *eglib, coordinate_t x, coordinate_t y, wchar_t unicode_char);

/**
 * Similar to :c:func:`eglib_DrawWChar`, but fills the background using color from index 1.
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawFilledWChar.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawFilledWChar.png
 *   :width: 200
 */

/**
 * Similar to :c:func:`eglib_DrawWChar`, but fills the background using color from index 1.
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawFilledWChar.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawFilledWChar.png
 *   :width: 200
 */
size_t eglib_DrawFilledWChar(eglib_t *eglib, coordinate_t x, coordinate_t y, wchar_t unicode_char);

/**
 * Draw given UTF-8 text starting at ``(x, y)``.
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawText.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawText.png
 *   :width: 200
 */
void eglib_DrawText(eglib_t *eglib, coordinate_t x, coordinate_t y, const char *utf8_text);

/**
 * Similar to :c:func:`eglib_DrawText`, but centers text horizontally at given
 * coordinates
 *
 * Example:
 *
 * .. literalinclude:: eglib_DrawTextCentered.c
 *   :language: C
 *
 * Output:
 *
 * .. image:: eglib_DrawTextCentered.png
 *   :width: 200
 */
#define eglib_DrawTextCentered(eglib, x, y, utf8_text) eglib_DrawText(eglib, x - eglib_GetTextWidth(eglib, utf8_text) / 2, y, utf8_text)

/**
 * Return the width in pixels of the given UTF-8 text.
 *
 * :See also: :c:func:`eglib_SetFont`.
 */
coordinate_t eglib_GetTextWidth(eglib_t *eglib, const char *utf8_text);

#endif
