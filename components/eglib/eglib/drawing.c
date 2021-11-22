#include "drawing.h"
#include "display.h"
#include <math.h>
#include <stdlib.h>
#include <esp_log.h>


#define degrees_to_radians(degrees) ((degrees) * M_PI / 180.0)


//
// Color
//

void eglib_SetIndexColor(
  eglib_t *eglib,
  size_t idx,
  color_channel_t r,
  color_channel_t g,
  color_channel_t b
) {
  eglib->drawing.color_index[idx].r = r;
  eglib->drawing.color_index[idx].g = g;
  eglib->drawing.color_index[idx].b = b;
}

// Pixel
//

void eglib_DrawPixelColor(eglib_t *eglib, coordinate_t x, coordinate_t y, color_t color) {
  if(x < 0 || x >= eglib_GetWidth(eglib))
    return;
  if(y < 0 || y >= eglib_GetHeight(eglib))
    return;

  eglib->display.driver->draw_pixel_color(eglib, x, y, color);
}

void eglib_DrawPixel(eglib_t *eglib, coordinate_t x, coordinate_t y) {
	eglib_DrawPixelColor(eglib, x, y, eglib->drawing.color_index[0]);
}

//
// Gradient
//

static void gradient_channel_begin(
  struct _gradient_channel_t *gradient_channel,
  color_channel_t color_channel_start,
  color_channel_t color_channel_end,
  coordinate_t steps
) {
  gradient_channel->color_channel = color_channel_start;
  gradient_channel->count = 0;
  gradient_channel->step = ((float)color_channel_end - (float)color_channel_start) / (float)steps;
}

static void gradient_begin(
  struct _gradient_t *gradient,
  color_t color_start,
  color_t color_end,
  coordinate_t steps
) {
  gradient_channel_begin(&gradient->r, color_start.r, color_end.r, steps);
  gradient_channel_begin(&gradient->g, color_start.g, color_end.g, steps);
  gradient_channel_begin(&gradient->b, color_start.b, color_end.b, steps);
}

static color_channel_t get_gradient_channel_color(
  struct _gradient_channel_t *gradient_channel
) {
  return gradient_channel->color_channel + gradient_channel->step * ++gradient_channel->count;
}

static color_t get_next_gradient_color(struct _gradient_t *gradient) {
  color_t color;

  color.r = get_gradient_channel_color(&gradient->r);
  color.g = get_gradient_channel_color(&gradient->g);
  color.b = get_gradient_channel_color(&gradient->b);

  return color;
}

static color_t get_color_index_0(eglib_t *eglib) {
  return eglib->drawing.color_index[0];
}

static color_t get_next_gradient_color_eglib(eglib_t *eglib) {
  color_t color;

  color.r = get_gradient_channel_color(&eglib->drawing.gradient.r);
  color.g = get_gradient_channel_color(&eglib->drawing.gradient.g);
  color.b = get_gradient_channel_color(&eglib->drawing.gradient.b);

  return color;
}

//
// Line
//

static void draw_fast_90_line(
  eglib_t *eglib,
  coordinate_t x1, coordinate_t y1,
  coordinate_t x2, coordinate_t y2,
  color_t (*get_next_color)(eglib_t *eglib)
) {
    enum display_line_direction_t direction;
    coordinate_t length;

    if(x1==x2) {  // vertical
      length = y2 > y1 ? y2 - y1 : y1 - y2;
      if(y2 > y1)
        direction = DISPLAY_LINE_DIRECTION_DOWN;
      else
        direction = DISPLAY_LINE_DIRECTION_UP;
    } else if(y1==y2) {  // horizontal
      length = x2 > x1 ? x2 - x1 : x1 - x2;
      if(x2 > x1)
        direction = DISPLAY_LINE_DIRECTION_RIGHT;
      else
        direction = DISPLAY_LINE_DIRECTION_LEFT;
    } else
      while(true);

    if(x1 >= eglib_GetWidth(eglib))
      return;
    if(y1 >= eglib_GetHeight(eglib))
      return;
    switch(direction) {
      case DISPLAY_LINE_DIRECTION_RIGHT:
        if(x1 + length > eglib_GetWidth(eglib))
          length = eglib_GetWidth(eglib) - x1;
        break;
      case DISPLAY_LINE_DIRECTION_LEFT:
        if(x1 - length < -1)
          length = x1 + 1;
        break;
      case DISPLAY_LINE_DIRECTION_DOWN:
        if(y1 + length > eglib_GetHeight(eglib))
          length = eglib_GetHeight(eglib) - y1;
        break;
      case DISPLAY_LINE_DIRECTION_UP:
        if(y1 - length < -1)
          length = y1 + 1;
        break;
    }

    if(length < 1)
      return;

    eglib->display.driver->draw_line(
      eglib,
      x1, y1,
      direction,
      length,
      get_next_color
    );
}

// keep in sync with draw_generic_line()
static coordinate_t get_line_pixel_count(
  coordinate_t x1, coordinate_t y1,
  coordinate_t x2, coordinate_t y2
) {
  coordinate_t tmp;
  coordinate_t dx, dy;

  dx = abs(x2 - x1);
  dy = abs(y2 - y1);

  if (dy > dx) {
    tmp = dx; dx = dy; dy = tmp;
    tmp = x1; x1 = y1; y1 = tmp;
    tmp = x2; x2 = y2; y2 = tmp;
  }
  if (x1 > x2) {
    tmp = x1; x1 =x2; x2 = tmp;
    tmp = y1; y1 =y2; y2 = tmp;
  }

  return x2 - x1 + 1;
}

// keep in sync with get_line_pixel_count()
static void draw_generic_line(
  eglib_t *eglib,
  coordinate_t x1, coordinate_t y1,
  coordinate_t x2, coordinate_t y2,
  color_t (*get_next_color)(eglib_t *eglib)
) {
  // ESP_LOGI("draw_generic_line", "x1:%d y1:%d x2:%d y2:%d", x1,x2,y1,y2 );
  coordinate_t tmp;
  coordinate_t x,y;
  coordinate_t dx, dy;
  coordinate_t err;
  coordinate_t ystep;
  bool swapxy = false;
  bool reversed = false;
  
  dx = abs(x2 - x1);
  dy = abs(y2 - y1);

  if(dy > dx) {
    swapxy = 1;
    tmp = dx; dx =dy; dy = tmp;
    tmp = x1; x1 =y1; y1 = tmp;
    tmp = x2; x2 =y2; y2 = tmp;
  }

  if(x1 > x2) {
    reversed = true;
    tmp = x1; x1 =x2; x2 = tmp;
    tmp = y1; y1 =y2; y2 = tmp;
  }

  err = dx >> 1;

  ystep = y2 > y1 ? 1 : -1;
  y = y1;

  if(reversed) {
    for(x = x2; x >= x1; x--) {
      if(swapxy == 0)
        eglib_DrawPixelColor(eglib, x, y2 + y1 - y, get_next_color(eglib));
      else
        eglib_DrawPixelColor(eglib, y2 + y1 - y, x, get_next_color(eglib));
      err -= dy;
      if(err < 0) {
        y += ystep;
        err += dx;
      }
    }
  } else {
    for(x = x1; x <= x2; x++) {
      if(swapxy == 0)
        eglib_DrawPixelColor(eglib, x, y, get_next_color(eglib));
      else
        eglib_DrawPixelColor(eglib, y, x, get_next_color(eglib));
      err -= dy;
      if(err < 0) {
        y += ystep;
        err += dx;
      }
    }
  }
}

static void draw_line(
  eglib_t *eglib,
  coordinate_t x1, coordinate_t y1,
  coordinate_t x2, coordinate_t y2,
  color_t (*get_next_color)(eglib_t *eglib)
) {
  // ESP_LOGI("draw_line", "x1:%d y1:%d x2:%d y2:%d", x1,x2,y1,y2 );
  if((x1 == x2) || (y1 == y2))
    draw_fast_90_line(eglib, x1, y1, x2, y2, get_next_color);
  else
    draw_generic_line(eglib, x1, y1, x2, y2, get_next_color);
}

void eglib_DrawLine(
  eglib_t *eglib,
  coordinate_t x1, coordinate_t y1,
  coordinate_t x2, coordinate_t y2
) {
  // ESP_LOGI("eglib_DrawLine", "x1:%d y1:%d x2:%d y2:%d", x1,x2,y1,y2 );
  draw_line(eglib, x1, y1, x2, y2, get_color_index_0);
}

void eglib_DrawGradientLine(
  eglib_t *eglib,
  coordinate_t x1, coordinate_t y1,
  coordinate_t x2, coordinate_t y2
) {
  gradient_begin(
    &eglib->drawing.gradient,
    eglib->drawing.color_index[0], eglib->drawing.color_index[1],
    get_line_pixel_count(x1, y1, x2, y2)
  );
  draw_line(eglib, x1, y1, x2, y2, get_next_gradient_color_eglib);
}

//pg_Polygon Functions

/*===========================================*/
/* procedures, which should not be inlined (save as much flash ROM as possible) */

static uint8_t pge_Next(struct pg_edge_struct *pge) PG_NOINLINE;
static uint8_t pg_inc(pg_struct *pg, uint8_t i) PG_NOINLINE;
static uint8_t pg_dec(pg_struct *pg, uint8_t i) PG_NOINLINE;
static void pg_expand_min_y(pg_struct *pg, pg_word_t min_y, uint8_t pge_idx) PG_NOINLINE;
static void pg_line_init(pg_struct * const pg, uint8_t pge_index) PG_NOINLINE;

/*===========================================*/
/* line draw algorithm */

static uint8_t pge_Next(struct pg_edge_struct *pge)
{
  if ( pge->current_y >= pge->max_y )
    return 0;
  
  pge->current_x += pge->current_x_offset;
  pge->error += pge->error_offset;
  if ( pge->error > 0 )
  {
    pge->current_x += pge->x_direction;
    pge->error -= pge->height;
  }  
  
  pge->current_y++;
  return 1;
}

/* assumes y2 > y1 */
static void pge_Init(struct pg_edge_struct *pge, pg_word_t x1, pg_word_t y1, pg_word_t x2, pg_word_t y2)
{
  pg_word_t dx = x2 - x1;
  pg_word_t width;

  pge->height = y2 - y1;
  pge->max_y = y2;
  pge->current_y = y1;
  pge->current_x = x1;

  if ( dx >= 0 )
  {
    pge->x_direction = 1;
    width = dx;
    pge->error = 0;
  }
  else
  {
    pge->x_direction = -1;
    width = -dx;
    pge->error = 1 - pge->height;
  }
  
  pge->current_x_offset = dx / pge->height;
  pge->error_offset = width % pge->height;
}

/*===========================================*/
/* convex polygon algorithm */

static uint8_t pg_inc(pg_struct *pg, uint8_t i)
{
    i++;
    if ( i >= pg->cnt )
      i = 0;
    return i;
}

static uint8_t pg_dec(pg_struct *pg, uint8_t i)
{
    i--;
    if ( i >= pg->cnt )
      i = pg->cnt-1;
    return i;
}

static void pg_expand_min_y(pg_struct *pg, pg_word_t min_y, uint8_t pge_idx)
{
  uint8_t i = pg->pge[pge_idx].curr_idx;
  for(;;)
  {
    i = pg->pge[pge_idx].next_idx_fn(pg, i);
    if ( pg->list[i].y != min_y )
      break;	
    pg->pge[pge_idx].curr_idx = i;
  }
}

static uint8_t pg_prepare(pg_struct *pg)
{
  pg_word_t max_y;
  pg_word_t min_y;
  uint8_t i;

  /* setup the next index procedures */
  pg->pge[PG_RIGHT].next_idx_fn = pg_inc;
  pg->pge[PG_LEFT].next_idx_fn = pg_dec;
  
  /* search for highest and lowest point */
  max_y = pg->list[0].y;
  min_y = pg->list[0].y;
  pg->pge[PG_LEFT].curr_idx = 0;
  for( i = 1; i < pg->cnt; i++ )
  {
    if ( max_y < pg->list[i].y )
    {
      max_y = pg->list[i].y;
    }
    if ( min_y > pg->list[i].y )
    {
      pg->pge[PG_LEFT].curr_idx = i;
      min_y = pg->list[i].y;
    }
  }

  /* calculate total number of scan lines */
  pg->total_scan_line_cnt = max_y;
  pg->total_scan_line_cnt -= min_y;
  
  /* exit if polygon height is zero */
  if ( pg->total_scan_line_cnt == 0 )
    return 0;
  
  /* if the minimum y side is flat, try to find the lowest and highest x points */
  pg->pge[PG_RIGHT].curr_idx = pg->pge[PG_LEFT].curr_idx;  
  pg_expand_min_y(pg, min_y, PG_RIGHT);
  pg_expand_min_y(pg, min_y, PG_LEFT);
  
  /* check if the min side is really flat (depends on the x values) */
  pg->is_min_y_not_flat = 1;
  if ( pg->list[pg->pge[PG_LEFT].curr_idx].x != pg->list[pg->pge[PG_RIGHT].curr_idx].x )
  {
    pg->is_min_y_not_flat = 0;
  }
  else
  {
    pg->total_scan_line_cnt--;
    if ( pg->total_scan_line_cnt == 0 )
      return 0;
  }

  return 1;
}

static void pg_hline(pg_struct *pg, eglib_t *eglib)
{
  pg_word_t x1, x2, y;
  x1 = pg->pge[PG_LEFT].current_x;
  x2 = pg->pge[PG_RIGHT].current_x;
  y = pg->pge[PG_RIGHT].current_y;
  
  if ( y < 0 )
    return;
  if ( y >= eglib_GetHeight(eglib) )
    return;
  if ( x1 < x2 )
  {
    if ( x2 < 0 )
      return;
    if ( x1 >= eglib_GetWidth(eglib) )
      return;
    if ( x1 < 0 )
      x1 = 0;
    if ( x2 >= eglib_GetWidth(eglib) )
      x2 = eglib_GetWidth(eglib);
    eglib_DrawHLine(eglib, x1, y, x2 - x1);
  }
  else
  {
    if ( x1 < 0 )
      return;
    if ( x2 >= eglib_GetWidth(eglib) )
      return;
    if ( x2 < 0 )
      x1 = 0;
    if ( x1 >= eglib_GetWidth(eglib) )
      x1 = eglib_GetWidth(eglib);
    eglib_DrawHLine(eglib, x2, y, x1 - x2);
  }
}

static void pg_line_init(pg_struct * pg, uint8_t pge_index)
{
  struct pg_edge_struct  *pge = pg->pge+pge_index;
  uint8_t idx;  
  pg_word_t x1;
  pg_word_t y1;
  pg_word_t x2;
  pg_word_t y2;

  idx = pge->curr_idx;  
  y1 = pg->list[idx].y;
  x1 = pg->list[idx].x;
  idx = pge->next_idx_fn(pg, idx);
  y2 = pg->list[idx].y;
  x2 = pg->list[idx].x; 
  pge->curr_idx = idx;
  
  pge_Init(pge, x1, y1, x2, y2);
}

static void pg_exec(pg_struct *pg, eglib_t *eglib)
{
  pg_word_t i = pg->total_scan_line_cnt;

  /* first line is skipped if the min y line is not flat */
  pg_line_init(pg, PG_LEFT);		
  pg_line_init(pg, PG_RIGHT);
  
  if ( pg->is_min_y_not_flat != 0 )
  {
    pge_Next(&(pg->pge[PG_LEFT])); 
    pge_Next(&(pg->pge[PG_RIGHT]));
  }

  do
  {
    pg_hline(pg, eglib);
    while ( pge_Next(&(pg->pge[PG_LEFT])) == 0 )
    {
      pg_line_init(pg, PG_LEFT);
    }
    while ( pge_Next(&(pg->pge[PG_RIGHT])) == 0 )
    {
      pg_line_init(pg, PG_RIGHT);
    }
    i--;
  } while( i > 0 );
}

/*===========================================*/
/* API procedures */

void pg_ClearPolygonXY(pg_struct *pg)
{
  pg->cnt = 0;
}

void pg_AddPolygonXY(pg_struct *pg, eglib_t *eglib, int16_t x, int16_t y)
{
  if ( pg->cnt < PG_MAX_POINTS )
  {
    pg->list[pg->cnt].x = x;
    pg->list[pg->cnt].y = y;
    pg->cnt++;
  }
}

void pg_DrawPolygon(pg_struct *pg, eglib_t *eglib)
{
  if ( pg_prepare(pg) == 0 )
    return;
  pg_exec(pg, eglib);
}


//
// Triangle
//

void eglib_DrawTriangle(
  eglib_t *eglib,
  coordinate_t x1, coordinate_t y1,
  coordinate_t x2, coordinate_t y2,
  coordinate_t x3, coordinate_t y3
) {
  eglib_DrawLine(eglib, x1, y1, x2, y2);
  eglib_DrawLine(eglib, x2, y2, x3, y3);
  eglib_DrawLine(eglib, x3, y3, x1, y1);
}

//
// Filled Triangle
//

void eglib_DrawFilledTriangle(
  eglib_t *eglib,
  coordinate_t x1, coordinate_t y1,
  coordinate_t x2, coordinate_t y2,
  coordinate_t x3, coordinate_t y3
) {
  pg_struct eglib_pg;

  pg_ClearPolygonXY(&eglib_pg);
  pg_AddPolygonXY(&eglib_pg, eglib, x1, y1);
  pg_AddPolygonXY(&eglib_pg, eglib, x2, y2);
  pg_AddPolygonXY(&eglib_pg, eglib, x3, y3);
  
  pg_DrawPolygon(&eglib_pg, eglib);
}

void eglib_DrawTetragon(eglib_t *eglib, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3)
{
  pg_struct eglib_pg;
  pg_ClearPolygonXY(&eglib_pg);
  pg_AddPolygonXY(&eglib_pg, eglib, x0, y0);
  pg_AddPolygonXY(&eglib_pg, eglib, x1, y1);
  pg_AddPolygonXY(&eglib_pg, eglib, x2, y3);
  pg_AddPolygonXY(&eglib_pg, eglib, x3, y3);
  pg_DrawPolygon(&eglib_pg, eglib);
}
//
// Frames
//

void eglib_DrawFrame(
  eglib_t *eglib,
  coordinate_t x, coordinate_t y,
  coordinate_t width, coordinate_t height
) {
  eglib_DrawHLine(eglib, x, y, width);
  eglib_DrawHLine(eglib, x, y + height, width);
  eglib_DrawVLine(eglib, x, y, height);
  eglib_DrawVLine(eglib, x + width, y, height + 1);
}

void eglib_DrawGradientFrame(
  eglib_t *eglib,
  coordinate_t x, coordinate_t y,
  coordinate_t width, coordinate_t height
) {
  color_t previous_color_index_0;
  color_t previous_color_index_1;

  previous_color_index_0 = eglib->drawing.color_index[0];
  previous_color_index_1 = eglib->drawing.color_index[1];

  eglib_DrawGradientHLine(eglib, x, y, width);
  eglib->drawing.color_index[0] = eglib->drawing.color_index[2];
  eglib->drawing.color_index[1] = eglib->drawing.color_index[3];
  eglib_DrawGradientHLine(eglib, x, y + height - 1, width);
  eglib->drawing.color_index[0] = previous_color_index_0;
  eglib->drawing.color_index[1] = eglib->drawing.color_index[2];
  eglib_DrawGradientVLine(eglib, x, y, height);
  eglib->drawing.color_index[0] = previous_color_index_1;
  eglib->drawing.color_index[1] = eglib->drawing.color_index[3];
  eglib_DrawGradientVLine(eglib, x + width - 1, y, height);

  eglib->drawing.color_index[0] = previous_color_index_0;
  eglib->drawing.color_index[1] = previous_color_index_1;
}

void eglib_DrawRoundFrame(
  eglib_t *eglib,
  coordinate_t x, coordinate_t y,
  coordinate_t width, coordinate_t height,
  coordinate_t radius
) {
  coordinate_t diameter;

  diameter = 2 * radius;

  eglib_DrawArc(eglib, x + radius, y + radius, radius, 270, 360);
  eglib_DrawHLine(eglib, x + radius, y, width - diameter);
  eglib_DrawArc(eglib, x + width - radius, y + radius, radius, 0, 90);
  eglib_DrawVLine(eglib, x + width, y + radius, height - diameter);
  eglib_DrawArc(eglib, x + width - radius, y + height - radius, radius, 90, 180);
  eglib_DrawHLine(eglib, x + radius, y + height, width - diameter);
  eglib_DrawArc(eglib, x + radius, y + height - radius, radius, 180, 270);
  eglib_DrawVLine(eglib, x, y + radius, height - diameter);
}

//
// Boxes
//

void eglib_DrawBox(
  eglib_t *eglib,
  coordinate_t x, coordinate_t y,
  coordinate_t width, coordinate_t height
) {
  for( ; height ; height--, y++)
    eglib_DrawHLine(eglib, x, y, width);
}

void eglib_DrawGradientBox(
  eglib_t *eglib,
  coordinate_t x, coordinate_t y,
  coordinate_t width, coordinate_t height
) {
  color_t previous_color_index_0;
  color_t previous_color_index_1;
  struct _gradient_t gradient_left;
  struct _gradient_t gradient_right;

  previous_color_index_0 = eglib->drawing.color_index[0];
  previous_color_index_1 = eglib->drawing.color_index[1];

  gradient_begin(
    &gradient_left,
    eglib->drawing.color_index[0], eglib->drawing.color_index[2],
    height
  );
  gradient_begin(
    &gradient_right,
    eglib->drawing.color_index[1], eglib->drawing.color_index[3],
    height
  );

  for( ; height ; height--, y++) {
    eglib->drawing.color_index[0] = get_next_gradient_color(&gradient_left);
    eglib->drawing.color_index[1] = get_next_gradient_color(&gradient_right);
    eglib_DrawGradientHLine(eglib, x, y, width);
  }

  eglib->drawing.color_index[0] = previous_color_index_0;
  eglib->drawing.color_index[1] = previous_color_index_1;
}

void eglib_DrawRoundBox(
  eglib_t *eglib,
  coordinate_t x, coordinate_t y,
  coordinate_t width, coordinate_t height,
  coordinate_t radius
) {
  coordinate_t diameter;
  coordinate_t v;

  diameter = 2 * radius;

  eglib_DrawFilledArc(eglib, x + radius, y + radius, radius, 270, 360);
  for(v=y ; v <= y + radius ; v++)
    eglib_DrawHLine(eglib, x + radius, v, width - diameter);
  eglib_DrawFilledArc(eglib, x + width - radius, y + radius, radius, 0, 90);
  for(v=y + radius ; v <= y + height - radius ; v++)
    eglib_DrawHLine(eglib, x, v, width);
  eglib_DrawFilledArc(eglib, x + radius, y + height - radius, radius, 180, 270);
  for(v=y + height - radius ; v <= y + width ; v++)
    eglib_DrawHLine(eglib, x + radius, v, width - diameter);
  eglib_DrawFilledArc(eglib, x + width - radius, y + height - radius, radius, 90, 180);
}

void eglib_ClearScreen(eglib_t *eglib) {
  eglib_DrawBox(eglib, 0, 0, eglib_GetWidth(eglib), eglib_GetHeight(eglib));
}

//
// Arc
//

// keep in sync with get_arc_pixel_count()
static void draw_arc(
  eglib_t *eglib,
  coordinate_t x, coordinate_t y,
  coordinate_t radius,
  float start_angle,
  float end_angle,
  color_t (*get_next_color)(eglib_t *eglib)
) {
  float angle_step;
  coordinate_t last_x=-1, last_y=-1;

  radius--;

  if(radius < 1)
    return;

  angle_step = degrees_to_radians((2.0 * M_PI * radius) / 360.0);
  start_angle = degrees_to_radians(start_angle - 90);
  end_angle = degrees_to_radians(end_angle - 90);

  for(float angle=start_angle ; angle <= end_angle ; angle+= angle_step) {
    coordinate_t curr_x, curr_y;

    curr_x = x + round(cos(angle) * radius);
    curr_y = y + round(sin(angle) * radius);

    if(curr_x == last_x && curr_y == last_y)
      continue;

    eglib_DrawPixelColor(eglib, curr_x, curr_y, get_next_color(eglib));

    last_x = curr_x;
    last_y = curr_y;
  }
}

// Keep in sync with draw_arc()
static coordinate_t get_arc_pixel_count(
  coordinate_t x,
  coordinate_t y,
  coordinate_t radius,
  float start_angle,
  float end_angle
) {
  float angle_step;
  coordinate_t last_x=-1, last_y=-1;
  coordinate_t steps=0;

  radius--;

  if(radius < 1)
    return 0;

  angle_step = degrees_to_radians((2.0 * M_PI * radius) / 360.0);
  start_angle = degrees_to_radians(start_angle - 90);
  end_angle = degrees_to_radians(end_angle - 90);

  for(float angle=start_angle ; angle <= end_angle ; angle+= angle_step) {
    coordinate_t curr_x, curr_y;

    curr_x = x + round(cos(angle) * radius);
    curr_y = y + round(sin(angle) * radius);

    if(curr_x == last_x && curr_y == last_y)
      continue;

    steps++;

    last_x = curr_x;
    last_y = curr_y;
  }

  return steps;
}

void eglib_DrawArc(
  eglib_t *eglib,
  coordinate_t x, coordinate_t y,
  coordinate_t radius,
  float start_angle,
  float end_angle
) {
  draw_arc(eglib, x, y, radius, start_angle, end_angle, get_color_index_0);
}

void eglib_DrawGradientArc(
  eglib_t *eglib,
  coordinate_t x, coordinate_t y,
  coordinate_t radius,
  float start_angle,
  float end_angle
) {
  gradient_begin(
    &eglib->drawing.gradient,
    eglib->drawing.color_index[0], eglib->drawing.color_index[1],
    get_arc_pixel_count(x, y, radius, start_angle, end_angle)
  );

  draw_arc(eglib, x, y, radius, start_angle, end_angle, get_next_gradient_color_eglib);
}

void eglib_DrawFilledArc(
  eglib_t *eglib,
  coordinate_t x, coordinate_t y,
  coordinate_t radius,
  float start_angle,
  float end_angle
) {
  eglib_DrawPixel(eglib, x, y);

  for(coordinate_t r=1 ; r <= radius ; r++)
    eglib_DrawArc(eglib, x, y, r, start_angle, end_angle);
}

void eglib_DrawGradientFilledArc(
  eglib_t *eglib,
  coordinate_t x, coordinate_t y,
  coordinate_t radius,
  float start_angle,
  float end_angle
) {
  color_t previous_color_index_0;

  previous_color_index_0 = eglib->drawing.color_index[0];

  gradient_begin(
    &eglib->drawing.gradient,
    eglib->drawing.color_index[0], eglib->drawing.color_index[1],
    radius + 1
  );

  eglib_DrawPixelColor(eglib, x, y, get_next_gradient_color_eglib(eglib));

  for(coordinate_t r=1 ; r <= radius ; r++) {
    eglib->drawing.color_index[0] = get_next_gradient_color_eglib(eglib);
    draw_arc(eglib, x, y, r, start_angle, end_angle, get_color_index_0);
  }

  eglib->drawing.color_index[0] = previous_color_index_0;
}

//
// Bitmap
//

static inline bool get_bit(const uint8_t *data, uint16_t bit) {
  return (*(data + bit / 8)) & (1<<(7-(bit % 8)));
}

void eglib_DrawBitmap(
  eglib_t *eglib,
  coordinate_t x,
  coordinate_t y,
  const struct bitmap_t *bitmap
) {
  coordinate_t u, v;
  color_t black = {0x00, 0x00, 0x00};
  color_t white = {0xff, 0xff, 0xff};
  uint8_t *data_ptr;

  switch(bitmap->format) {
    case BITMAP_BW:
      for(v=0 ; v < bitmap->height ; v++)
        for(u=0 ; u < bitmap->width ; u++) {
          eglib_DrawPixelColor(
            eglib,
            x + u, y + v,
            get_bit(bitmap->data, (v * bitmap->width) + u) ? white : black
          );
        }
      break;
    case BITMAP_RGB24:
      data_ptr = bitmap->data;
      for(v=0 ; v < bitmap->height ; v++)
        for(u=0 ; u < bitmap->width ; u++) {
          color_t color;

          color.r = *data_ptr;
          data_ptr++;
          color.g = *data_ptr;
          data_ptr++;
          color.b = *data_ptr;
          data_ptr++;

          eglib_DrawPixelColor(eglib, x + u, y + v, color);
        }
      break;
  }
}

//
// Font
//

void eglib_SetFont(eglib_t *eglib, const struct font_t *font) {
  eglib->drawing.font = font;
}

bool eglib_AddUnicodeBlockToFont(
  struct font_t *font,
  const struct glyph_unicode_block_t *unicode_block
) {
  if(font->unicode_blocks_count == FONT_MAX_UNICODE_BLOCKS)
    return true;

  font->unicode_blocks[font->unicode_blocks_count] = unicode_block;
  font->unicode_blocks_count++;

  return false;
}

const struct glyph_t *eglib_GetGlyph(eglib_t *eglib, wchar_t unicode_char) {
  const struct font_t *font;

  font = eglib->drawing.font;

  for(int i = 0 ; i < font->unicode_blocks_count ; i++) {
    if(
      unicode_char >= (wchar_t)font->unicode_blocks[i]->charcode_start
      && unicode_char <= (wchar_t)font->unicode_blocks[i]->charcode_end
    ) {
      return font->unicode_blocks[i]->glyphs[unicode_char - font->unicode_blocks[i]->charcode_start];
    }
  }
  return NULL;
}

void eglib_DrawGlyph(eglib_t *eglib, coordinate_t x, coordinate_t y, const struct glyph_t *glyph) {
  if(glyph == NULL) {
    uint8_t pixel_size;

    pixel_size = eglib->drawing.font->pixel_size;
    eglib_DrawFrame(eglib, x, y - pixel_size, pixel_size, pixel_size);
    eglib_DrawLine(eglib, x, y, x + pixel_size, y - pixel_size);
    eglib_DrawLine(eglib, x, y - pixel_size, x + pixel_size, y);
    return;
  }

  for(coordinate_t v=0 ; v < glyph->height ; v++)
    for(coordinate_t u=0 ; u < glyph->width ; u++) {
      bool pixel;

      pixel = get_bit(glyph->data, (v * glyph->width) + u);

      if(pixel)
        eglib_DrawPixel(
          eglib,
          x + glyph->left + u, y - glyph->top + v
        );
    }
}
void eglib_DrawFilledGlyph(eglib_t *eglib, coordinate_t x, coordinate_t y, const struct glyph_t *glyph) {
  if(glyph == NULL) {
    uint8_t pixel_size;

    pixel_size = eglib->drawing.font->pixel_size;
    eglib_DrawFrame(eglib, x, y - pixel_size, pixel_size, pixel_size);
    eglib_DrawLine(eglib, x, y, x + pixel_size, y - pixel_size);
    eglib_DrawLine(eglib, x, y - pixel_size, x + pixel_size, y);
    return;
  }

  for(coordinate_t v=0 ; v < glyph->height ; v++)
    for(coordinate_t u=0 ; u < glyph->width ; u++) {
      bool pixel;

      pixel = get_bit(glyph->data, (v * glyph->width) + u);

      if(pixel){
        eglib_DrawPixel( eglib, x + glyph->left + u, y - glyph->top + v );
      }
      else{
        eglib_DrawPixelColor( eglib, x + glyph->left + u, y - glyph->top + v, (color_t){
		.r = eglib->drawing.color_index[1].r,
		.g = eglib->drawing.color_index[1].g,
		.b = eglib->drawing.color_index[1].b,
	} );
      }
    }
}
void eglib_DrawWChar(eglib_t *eglib, coordinate_t x, coordinate_t y, wchar_t unicode_char) {
  eglib_DrawGlyph(eglib, x, y, eglib_GetGlyph(eglib, unicode_char));
}

#define isutf(c) (((c)&0xC0)!=0x80)

static wchar_t utf8_nextchar(const char *utf8_text, uint16_t *index) {
    wchar_t c;
    int length;
    static const uint32_t offsets_from_utf8[] = {
        0x00000000UL, 0x00003080UL, 0x000E2080UL,
        0x03C82080UL, 0xFA082080UL, 0x82082080UL
    };

    c = 0;
    length = 0;

    do {
        c <<= 6;
        c += (uint8_t)utf8_text[(*index)++];
        length++;
    } while (utf8_text[*index] && !isutf(utf8_text[*index]));
    c -= offsets_from_utf8[length-1];

    return c;
}
size_t eglib_DrawFilledWChar(eglib_t *eglib, coordinate_t x, coordinate_t y, wchar_t unicode_char) {
  color_channel_t old_r0, old_g0, old_b0;
  const struct font_t *font;
  const struct glyph_t *glyph;
  coordinate_t box_x, box_width;

  old_r0 = eglib->drawing.color_index[0].r;
  old_g0 = eglib->drawing.color_index[0].g;
  old_b0 = eglib->drawing.color_index[0].b;

  eglib_SetIndexColor(
    eglib, 0,
    eglib->drawing.color_index[1].r,
    eglib->drawing.color_index[1].g,
    eglib->drawing.color_index[1].b
  );

  font = eglib->drawing.font;
  glyph = eglib_GetGlyph(eglib, unicode_char);

  if(glyph == NULL)
    return;

  eglib_DrawBox(
    eglib,
    x - glyph->left,
    y - font->ascent,
    glyph->left + glyph->width + glyph->advance,
    font->ascent - font->descent
  );

  eglib_SetIndexColor(eglib, 0, old_r0, old_g0, old_b0);

  eglib_DrawGlyph(eglib, x, y, glyph);
  return glyph->advance;
}


void eglib_DrawText(eglib_t *eglib, coordinate_t x, coordinate_t y, const char *utf8_text) {
  const struct glyph_t *glyph;

  for(uint16_t index=0 ; utf8_text[index] ; ) {
    glyph = eglib_GetGlyph(eglib, utf8_nextchar(utf8_text, &index));
    eglib_DrawGlyph(eglib, x, y, glyph);
    if(glyph == NULL)
      x += eglib->drawing.font->pixel_size;
    else
      x += glyph->advance;
  }
}

coordinate_t eglib_GetTextWidth(eglib_t *eglib, const char *utf8_text) {
  coordinate_t width;

  width = 0;

  for(uint16_t index=0 ; utf8_text[index] ; ) {
    const struct glyph_t *glyph;

    glyph = eglib_GetGlyph(eglib, utf8_nextchar(utf8_text, &index));
    if(glyph == NULL)
      width += eglib->drawing.font->pixel_size;
    else
      width += glyph->advance;
  }

  return width;
}
