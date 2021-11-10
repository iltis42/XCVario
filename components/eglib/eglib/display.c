#include "display.h"
#include "hal.h"

void display_default_draw_line(
	eglib_t *eglib,
	coordinate_t x,
	coordinate_t y,
	enum display_line_direction_t direction,
	coordinate_t length,
	color_t (*get_next_color)(eglib_t *eglib)
) {
	coordinate_t dx=0, dy=0;

	switch(direction) {
		case DISPLAY_LINE_DIRECTION_RIGHT:
			dx = 1;
			break;
		case DISPLAY_LINE_DIRECTION_LEFT:
			dx = -1;
			break;
		case DISPLAY_LINE_DIRECTION_DOWN:
			dy = 1;
			break;
		case DISPLAY_LINE_DIRECTION_UP:
			dy = -1;
			break;
	}

	for(length++ ; length-- ; ) {
		eglib->display.driver->draw_pixel_color(eglib, x, y, get_next_color(eglib));
		x += dx;
		y += dy;
	}
}

coordinate_t eglib_GetWidth(eglib_t *eglib) {
	coordinate_t width, heigh;

	eglib->display.driver->get_dimension(eglib, &width, &heigh);

	return width;
}

coordinate_t eglib_GetHeight(eglib_t *eglib) {
	coordinate_t width, heigh;

	eglib->display.driver->get_dimension(eglib, &width, &heigh);

	return heigh;
}

bool eglib_Refresh(eglib_t *eglib) {
	bool is_currently_refreshing;

	is_currently_refreshing = eglib->display.driver->refresh(eglib);

	eglib->display.refreshing = is_currently_refreshing;

	return is_currently_refreshing;
}
