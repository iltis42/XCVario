#include "tga.h"
#include "frame_buffer.h"

#include <stdlib.h>
#include <stdio.h>

static void init(eglib_t *eglib) {
	tga_config_t *config;

	config = eglib_GetDisplayConfig(eglib);

	config->tga_data = (uint8_t *)calloc(
		config->width * config->height,
		3
	);
	if ( config->tga_data == NULL )
		exit(1);
}

static void sleep_in(eglib_t *eglib) {
	(void)eglib;
}

static void sleep_out(eglib_t *eglib) {
	(void)eglib;
}

static void get_dimension(
	eglib_t *eglib,
	coordinate_t *width,
	coordinate_t *height
) {
	tga_config_t *config;

	config = eglib_GetDisplayConfig(eglib);

	*width = config->width;
	*height = config->height;
}

static void get_pixel_format(eglib_t *eglib, enum pixel_format_t *pixel_format) {
	(void)eglib;

	*pixel_format = PIXEL_FORMAT_24BIT_RGB;
}

static void draw_pixel_color(
	eglib_t *eglib,
	coordinate_t x,
	coordinate_t y,
	color_t color
) {
	tga_config_t *config;
	uint8_t *p;

	config = eglib_GetDisplayConfig(eglib);

	if(x >= config->width || y >= config->height || x < 0 || y < 0)
		return;

	y = config->height -1 - y;

	p = config->tga_data + (y * config->width * 3) + (x * 3);
	*p++ = color.b;
	*p++ = color.g;
	*p++ = color.r;
}

static void send_buffer(
       eglib_t *eglib,
       void *buffer_ptr,
       coordinate_t x, coordinate_t y,
       coordinate_t width, coordinate_t height
) {
	tga_config_t *config;
	uint8_t *buffer = (uint8_t *)buffer_ptr;
	color_t color;
	coordinate_t y_start, y_end, x_start, x_end;

	config = eglib_GetDisplayConfig(eglib);

	y_start = x;
	y_end = y + height;
	x_start = x;
	x_end = x + width;

	for(y = y_start; y < y_end ; y++) {
		for(x = x_start; x < x_end ; x++) {
			uint8_t *pixel;

			pixel = buffer + (config->width * y + x) * 3;;

			color.r = *pixel;
			pixel++;
			color.g = *pixel;
			pixel++;
			color.b = *pixel;
			eglib_DrawPixelColor(
				eglib,
				x, y,
				color
			);
		}
	}
}

static bool refresh(eglib_t *eglib) {
	(void)eglib;
	return false;
}

static hal_four_wire_spi_config_t four_wire_spi_config = {
		.mode = 0,
		.bit_numbering = HAL_MSB_FIRST,
		.cs_setup_ns = 0,
		.cs_hold_ns = 0,
		.cs_disable_ns = 0,
		.dc_setup_ns = 0,
		.dc_hold_ns = 0,
		.sck_cycle_ns = 0,
};

const display_t tga = {
	.comm.four_wire_spi = &four_wire_spi_config,
	.init = init,
	.sleep_in = sleep_in,
	.sleep_out = sleep_out,
	.get_dimension = get_dimension,
	.get_pixel_format = get_pixel_format,
	.draw_pixel_color = draw_pixel_color,
	.draw_line = display_default_draw_line,
	.send_buffer = send_buffer,
	.refresh = refresh,
};

static void tga_write_byte(FILE *fp, uint8_t byte) {
	fputc(byte, fp);
}

static void tga_write_word(FILE *fp, uint16_t word) {
	tga_write_byte(fp, word&255);
	tga_write_byte(fp, word>>8);
}

void tga_Save(eglib_t *eglib, char *path) {
	tga_config_t *config;
	FILE *fp;

	config = eglib_GetDisplayConfig(eglib);

	fp = fopen(path, "wb");
	if ( fp != NULL ) {
		tga_write_byte(fp, 0);		/* no ID */
		tga_write_byte(fp, 0);		/* no color map */
		tga_write_byte(fp, 2);		/* uncompressed true color */
		tga_write_word(fp, 0);
		tga_write_word(fp, 0);
		tga_write_byte(fp, 0);
		tga_write_word(fp, 0);		/* x origin */
		tga_write_word(fp, 0);		/* y origin */
		tga_write_word(fp, config->width);		/* width */
		tga_write_word(fp, config->height);		/* height */
		tga_write_byte(fp, 24);		/* color depth */
		tga_write_byte(fp, 0);
		fwrite(config->tga_data, 3, config->width * config->height, fp);
		tga_write_word(fp, 0);
		tga_write_word(fp, 0);
		tga_write_word(fp, 0);
		tga_write_word(fp, 0);
		fwrite("TRUEVISION-XFILE.", 18, 1, fp);
		fclose(fp);
	}
}

void tga_Free(eglib_t *eglib) {
	tga_config_t *config;

	config = eglib_GetDisplayConfig(eglib);

	free(config->tga_data);
}