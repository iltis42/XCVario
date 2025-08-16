/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "ArrowIndicator.h"

#include "PolarGauge.h"
#include "math/Trigenometry.h"
#include "Colors.h"
#include "logdef.h"

#include <cmath>

extern AdaptUGC *MYUCG;


////////////////////////////
// ArrowIndicator

ArrowIndicator::ArrowIndicator(PolarGauge &g, int16_t tipref, int16_t length) :
    _gauge(g),
    _tip(g._radius-tipref),
    _base(_tip-length),
    _base_val_offset(atan(8.f/_base)*g.IDX_SCALE)
{
    color = { COLOR_WHITE };
    prev.x_0 = _gauge.CosCenteredDeg2(0, _base); // base center
    prev.y_0 = _gauge.SinCenteredDeg2(0, _base);
    prev.x_1 = prev.x_0 - 0; // lower shoulder
    prev.y_1 = prev.y_0 + _half_width;
    prev.x_0 += 0; // base shoulder
    prev.y_0 -= _half_width;
    prev.x_2 = _gauge.CosCenteredDeg2(0, _tip); // tip
    prev.y_2 = _gauge.SinCenteredDeg2(0, _tip);
}


// > 0,5° step counter
// return status on changed pointer position
bool ArrowIndicator::draw(int16_t val)
{
    Triangle_t n;
    bool change = val != _needle_pos;
    if (!change && !_gauge._dirty)
    {
        return false; // nothing painted
    }
    if ( _gauge._dirty ) {
        _gauge._old_bow_idx = 0; // redraw bows
        _gauge._old_polar_sink = 0;
        _gauge._dirty = false;
    }

    // same about ~5000 cycles effort, but with tiny artifacts remaining
    // n.x_0 = _gauge.CosCenteredDeg2(val, _base); // base center
    // n.y_0 = _gauge.SinCenteredDeg2(val, _base);
    // int16_t bc = fast_cos_idx(val) * _half_width;
    // int16_t bs = fast_sin_idx(val) * _half_width;
    // n.x_1 = n.x_0 - bs; // lower shoulder
    // n.y_1 = n.y_0 + bc;
    // n.x_0 += bs; // top shoulder
    // n.y_0 -= bc;
    // n.x_2 = _gauge.CosCenteredDeg2(val, _tip); // tip
    // n.y_2 = _gauge.SinCenteredDeg2(val, _tip);
    n.x_0 = _gauge.CosCenteredDeg2(val + _base_val_offset, _base); // top shoulder
    n.y_0 = _gauge.SinCenteredDeg2(val + _base_val_offset, _base);
    n.x_1 = _gauge.CosCenteredDeg2(val - _base_val_offset, _base); // lower shoulder
    n.y_1 = _gauge.SinCenteredDeg2(val - _base_val_offset, _base);
    n.x_2 = _gauge.CosCenteredDeg2(val, _tip); // tip
    n.y_2 = _gauge.SinCenteredDeg2(val, _tip);
    // ESP_LOGI(FNAME,"draw Triangle  x0:%d y0:%d x1:%d y1:%d x2:%d y2:%d", n.x_0, n.y_0, n.x_1 ,n.y_1, n.x_2, n.y_2 );

    if (std::abs(val - _needle_pos) < _gauge.dice_rad(deg2rad(9.f)))
    {
        // new indicator position overlaps
        // Clear just a smal triangle
        int16_t x_2 = _gauge.CosCenteredDeg2(_needle_pos, _base + 7);
        int16_t y_2 = _gauge.SinCenteredDeg2(_needle_pos, _base + 7);
        if( change ){
            MYUCG->setColor(DARK_DGREY);
            MYUCG->drawTriangle(prev.x_0, prev.y_0, prev.x_1, prev.y_1, x_2, y_2);
        }

        // draw pointer
        MYUCG->setColor(color.color[0], color.color[1], color.color[2]);
        MYUCG->drawTriangle(n.x_0, n.y_0, n.x_1, n.y_1, n.x_2, n.y_2);

        // cleanup respecting overlap
        if( change ){  // we need to cleanup only if position has changed, otherwise a redraw at same position is enough
            MYUCG->setColor(DARK_DGREY);
            // clear area to the side
            if (val > _needle_pos)
            {
                // up
                MYUCG->drawTetragon(prev.x_2, prev.y_2, prev.x_1, prev.y_1, n.x_1, n.y_1, n.x_2, n.y_2);
            }
            else
            {
                MYUCG->drawTetragon(prev.x_2, prev.y_2, n.x_2, n.y_2, n.x_0, n.y_0, prev.x_0, prev.y_0);
            }
        }
    }
    else
    {
        if( change ){
            // cleanup previous incarnation
            MYUCG->setColor(DARK_DGREY);
            MYUCG->drawTriangle(prev.x_0, prev.y_0, prev.x_1, prev.y_1, prev.x_2, prev.y_2);
            // draw pointer
            MYUCG->setColor(color.color[0], color.color[1], color.color[2]);
            MYUCG->drawTriangle(n.x_0, n.y_0, n.x_1, n.y_1, n.x_2, n.y_2);
        }
    }
    // ESP_LOGI(FNAME,"change=%d  prev=%d  now=%d", change, _needle_pos, val  );
    prev = n;
    _needle_pos = val;
    return change;
}
