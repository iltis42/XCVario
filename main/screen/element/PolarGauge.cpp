/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "PolarGauge.h"

#include "math/Trigenometry.h"
#include "Colors.h"
#include "AdaptUGC.h"
#include "logdef.h"

#include <cmath>

extern AdaptUGC *MYUCG;
const static ucg_color_t needlecolor[3] = {{COLOR_WHITE}, {COLOR_ORANGE}, {COLOR_RED}};

////////////////////////////
// Function mapping the gauges value range to rad, or diced 0,5° steps

class LinGaugeFunc : public GaugeFunc
{
public:
    LinGaugeFunc(float scale, float zero) : GaugeFunc(scale, zero) {}
    float operator()(float a) const override { return (a - _zero_at) * _scale_k; }
    float invers(float rad) const override { return (rad / _scale_k) + _zero_at; }
};
class LogGaugeFunc : public GaugeFunc
{
public:
    LogGaugeFunc(float scale, float zero) : GaugeFunc(scale, zero) {}
    float operator()(float a) const override { return fast_log2f(std::abs((a-_zero_at)) + 1.f) * _scale_k * (std::signbit((a-_zero_at)) ? -1. : 1.); }
    float invers(float rad) const override { return ((pow(2., std::abs(rad)) - 1.f) / _scale_k * (std::signbit(rad) ? -1.f : 1.f)) + _zero_at; }
};


////////////////////////////
// PolarIndicator

typedef struct Triangle
{
    int16_t x_0 = 0, y_0 = 0, x_1 = 0, y_1 = 1, x_2 = 1, y_2 = 0;
} Triangle_t;

class PolarIndicator
{
public:
    PolarIndicator() = delete;
    PolarIndicator(PolarGauge &g);
    // API
    void setColor(ucg_color_t c) { color = c; }
    bool draw(int16_t val);
    int16_t getSize() { return _tip - _base; }

    // attributes
private:
    PolarGauge &_gauge;             // ref to gauge the pointer belonges to
    const int16_t _base;            // distance from base center to shoulder of the arrow
    const int16_t _tip;             // distance from base center to arrow tip
    //  int16_t h_width = 8; // half width of the arrow base
    const int16_t _base_val_offset = 8; // .5deg steps from base point to shoulder point of arrow
    int16_t _needle_pos;
    int16_t prev_needle_pos = 0;    // -_range .. _range diced up
    ucg_color_t color;
    Triangle_t prev;
};

PolarIndicator::PolarIndicator(PolarGauge &g) : 
    _gauge(g),
   	_base(DISPLAY_H/2-24-26),
	_tip(DISPLAY_H/2-24),
    _base_val_offset(atan(8.f/_base)*g._idx_scale)

{
    color = needlecolor[1];
    ESP_LOGI(FNAME,"PolarIndicator base off: %d %f", _base_val_offset, atan(static_cast<float>(8.)/_base));
    prev.x_0 = _gauge.CosCenteredDeg2(_base_val_offset, _base); // top shoulder
    prev.y_0 = _gauge.SinCenteredDeg2(_base_val_offset, _base);
    prev.x_1 = _gauge.CosCenteredDeg2(-_base_val_offset, _base); // lower shoulder
    prev.y_1 = _gauge.SinCenteredDeg2(-_base_val_offset, _base);
    prev.x_2 = _gauge.CosCenteredDeg2(0, _tip); // tip
    prev.y_2 = _gauge.SinCenteredDeg2(0, _tip);
}


// > 0,5° step counter
// return status on updated a changed pointer position
bool PolarIndicator::draw(int16_t val)
{
    Triangle_t n;
    bool change = val != prev_needle_pos || _gauge._dirty;
    if (!change)
    {
        return false; // nothing painted
    }
    _gauge._dirty = false;

    n.x_0 = _gauge.CosCenteredDeg2(val + _base_val_offset, _base); // top shoulder
    n.y_0 = _gauge.SinCenteredDeg2(val + _base_val_offset, _base);
    n.x_1 = _gauge.CosCenteredDeg2(val - _base_val_offset, _base); // lower shoulder
    n.y_1 = _gauge.SinCenteredDeg2(val - _base_val_offset, _base);
    n.x_2 = _gauge.CosCenteredDeg2(val, _tip); // tip
    n.y_2 = _gauge.SinCenteredDeg2(val, _tip);
    // ESP_LOGI(FNAME,"draw Triangle  x0:%d y0:%d x1:%d y1:%d x2:%d y2:%d", n.x_0, n.y_0, n.x_1 ,n.y_1, n.x_2, n.y_2 );

    if (std::abs(val - prev_needle_pos) < _gauge.dice_rad(deg2rad(9.f)))
    { // 6deg:=atan(7/70)

        // Clear just a smal triangle
        int16_t x_2 = _gauge.CosCenteredDeg2(prev_needle_pos, _base + 7);
        int16_t y_2 = _gauge.SinCenteredDeg2(prev_needle_pos, _base + 7);
        // if( change ){
        MYUCG->setColor(DARK_DGREY);
        MYUCG->drawTriangle(prev.x_0, prev.y_0, prev.x_1, prev.y_1, x_2, y_2);
        // }

        // draw pointer
        MYUCG->setColor(color.color[0], color.color[1], color.color[2]);
        MYUCG->drawTriangle(n.x_0, n.y_0, n.x_1, n.y_1, n.x_2, n.y_2);

        // cleanup respecting overlap
        // if( change ){  // we need to cleanup only if position has changed, otherwise a redraw at same position is enough
        MYUCG->setColor(DARK_DGREY);
        // clear area to the side
        if (val > prev_needle_pos)
        {
            // up
            MYUCG->drawTetragon(prev.x_2, prev.y_2, prev.x_1, prev.y_1, n.x_1, n.y_1, n.x_2, n.y_2);
        }
        else
        {
            MYUCG->drawTetragon(prev.x_2, prev.y_2, n.x_2, n.y_2, n.x_0, n.y_0, prev.x_0, prev.y_0);
        }
        // }
    }
    else
    {
        // if( change ){
        // cleanup previous incarnation
        MYUCG->setColor(DARK_DGREY);
        MYUCG->drawTriangle(prev.x_0, prev.y_0, prev.x_1, prev.y_1, prev.x_2, prev.y_2);
        // draw pointer
        MYUCG->setColor(color.color[0], color.color[1], color.color[2]);
        MYUCG->drawTriangle(n.x_0, n.y_0, n.x_1, n.y_1, n.x_2, n.y_2);
        // }
    }
    // ESP_LOGI(FNAME,"change=%d  prev=%d  now=%d", change, prev_needle_pos, val  );
    prev = n;
    prev_needle_pos = val;
    return change;
}

////////////////////////////
// PolarGauge
const ucg_color_t PolarGauge::bow_color[4] = {{COLOR_GREEN}, {COLOR_BBLUE}, {COLOR_ORANGE}, {COLOR_RED}};

// pixel, pixel, degree, pixel
PolarGauge::PolarGauge(int16_t refx, int16_t refy, int16_t scale_end, int16_t radius) :
    ScreenElement(refx, refy),
    _scale_max(deg2rad((float)scale_end)),
    _radius(radius),
    _idx_scale(360.f/My_PIf) // scale 0.5° [in rad] to 2 (0.5deg resolution discrete scale)
{
    _indicator = new PolarIndicator(*this);
    func = new GaugeFunc(1.,0.);
    ESP_LOGI(FNAME, "_idx_scale: %f", _idx_scale);
}
PolarGauge::~PolarGauge()
{
    delete _indicator;
    if ( func ) {
        delete func;
    }
}

void PolarGauge::setRange(float pos_range, float zero_at, bool log)
{
    _range = pos_range;
    _mrange = 2 * zero_at - pos_range;
    if ( func ) delete func;
    if ( log ) {
        func = new LogGaugeFunc(_scale_max / fast_log2f((_range - zero_at) + 1.), zero_at);
    }
    else {
        func = new LinGaugeFunc(_scale_max / (_range - zero_at), zero_at);
    }
}
void PolarGauge::setColor(int color_idx)
{
    _indicator->setColor(needlecolor[color_idx & 3]);
}

float PolarGauge::clipValue(float a) const
{
    if (a > _range)
    {
        a = _range;
    }
    else if (a < _mrange)
    {
        a = _mrange;
    }
    return a;
}

bool PolarGauge::draw(float a)
{
    int16_t val = dice_up(a);
    // ESP_LOGI(FNAME,"draw  a:%f - %d", a, val);
    if (_indicator->draw(val))
    {
        // Draw colored bow
        int16_t bar_val = (val > 0) ? val : 0;
        // draw green vario bar
        drawBow(bar_val, _old_bow_idx, 3, GREEN);
        return true;
    }
    return false;
}

bool PolarGauge::drawIndicator(float a)
{
    return _indicator->draw(dice_up(a));
}

// sink speed in m/s
void PolarGauge::drawPolarSink(float a)
{
    int16_t val = dice_up(a);
    // ESP_LOGI(FNAME,"sink  a:%f - %d", a, val);
    // Draw blue bow for polar sink
    drawBow(val, _old_polar_sink, 3, BLUE);
}

// idx [rad], end radius, width
void PolarGauge::drawOneScaleLine(float a, int16_t l2, int16_t w) const
{
    float si = fast_sin_rad(a);
    float co = fast_cos_rad(a);
    int16_t l1 = _radius + 1;
    int16_t w0 = w / 2;
    int16_t w1 = w - w0; // total width := w1 + w0
    int16_t xn_0 = _ref_x - static_cast<int16_t>(std::roundf(co * l1 - si * w0));
    int16_t yn_0 = _ref_y - static_cast<int16_t>(std::roundf(si * l1 + co * w0));
    int16_t xn_1 = _ref_x - static_cast<int16_t>(std::roundf(co * l1 + si * w1));
    if (xn_1 < 0)
        xn_1 = 0;
    int16_t yn_1 = _ref_y - static_cast<int16_t>(std::roundf(si * l1 - co * w1));
    int16_t xn_2 = _ref_x - static_cast<int16_t>(std::roundf(co * l2 + si * w1));
    if (xn_2 < 0)
        xn_2 = 0;
    int16_t yn_2 = _ref_y - static_cast<int16_t>(std::roundf(si * l2 - co * w1));
    int16_t xn_3 = _ref_x - static_cast<int16_t>(std::roundf(co * l2 - si * w0));
    int16_t yn_3 = _ref_y - static_cast<int16_t>(std::roundf(si * l2 + co * w0));
    // ESP_LOGI(FNAME,"drawTetragon  x0:%d y0:%d x1:%d y1:%d x2:%d y2:%d x3:%d y3:%d", xn_0, yn_0, xn_1, yn_1, xn_2, yn_2, xn_3, yn_3 );
    if (w == 1)
    {
        MYUCG->setColor(COLOR_MGREY);
    }
    else if (w == 2)
    {
        MYUCG->setColor(COLOR_MGREY);
    }
    else
    {
        MYUCG->setColor(COLOR_WHITE);
    }
    MYUCG->drawTetragon(xn_0, yn_0, xn_1, yn_1, xn_2, yn_2, xn_3, yn_3);
}

// draw incremental bow up to indicator given in diced 0.5° steps, pos
void PolarGauge::drawBow(int16_t idx, int16_t &old, int16_t w, int16_t color) const
{
    if (idx == old)
    {
        return;
    }
    // ESP_LOGI(FNAME,"drawBbow af= level=%d old_level=%d", idx, old );

    // potentially clean first
    if (std::abs(idx) < std::abs(old) || idx * old < 0)
    {
        MYUCG->setColor(DARK_DGREY);
    }
    else
    {
        MYUCG->setColor(bow_color[color].color[0], bow_color[color].color[1], bow_color[color].color[2]);
    }
    // ESP_LOGI(FNAME,"bow lev %d", idx);
    int16_t l1 = _radius;
    if (  w < 0 ) {
        w = -w;
        l1 += w;
    }
    int inc = (idx - old > 0) ? 1 : -1;
    if ( std::abs(w) > 4 && std::abs(old - idx) > 4)
    {
        inc *= 4;
    }
    int16_t step = std::abs(inc) * ((old<0||idx<0)?-1:1);
    for (int i = old; i != idx; i += inc)
    {
        int16_t x0 = CosCenteredDeg2(i, l1);
        int16_t y0 = SinCenteredDeg2(i, l1);
        int16_t x1 = CosCenteredDeg2(i + step, l1);
        int16_t y1 = SinCenteredDeg2(i + step, l1);
        int16_t xe = CosDeg2(i, w);
        int16_t ye = SinDeg2(i, w);

        MYUCG->drawTetragon(x0, y0, x1, y1, x1 + xe, y1 + ye, x0 + xe, y0 + ye);
        if (std::abs(inc) > 1 && std::abs(i - idx) < 4)
        {
            inc /= std::abs(inc);;
            step /= std::abs(step);
        }
    }
    old = idx;
}

// Draw scale label numbers for -_range .. _range w/o sign
void PolarGauge::drawOneLabel(float val, int16_t labl) const
{
    // ESP_LOGI( FNAME,"drawOneLabel val %.2f, label %d", val, labl );
    float to_side = 0.05;
    float incr = (_scale_max - std::abs(val)) * 2; // increase pos towards 0
    int16_t pos = _radius + 12 + (int)incr - 3;
    if (val > 0)
    {
        to_side += incr / ((My_PIf / 2.f) * 80);
    }
    else
    {
        to_side = -to_side;
        to_side -= incr / ((My_PIf / 2.f) * 80);
    }
    // ESP_LOGI( FNAME,"drawOneLabel val:%.2f label:%d  toside:%.2f inc:%.2f", val, labl, to_side, incr );
    int x = CosCentered(val + to_side, pos + (2 * incr));
    int y = SinCentered(val + to_side, pos + (2 * incr)) + 6;

    MYUCG->setColor(COLOR_LBBLUE);
    MYUCG->setPrintPos(x, y);
    MYUCG->print(abs(labl));
}

void PolarGauge::colorRange(float from, float to, int16_t color)
{
    int16_t ol = dice_up(from);
    drawBow(dice_up(to), ol, -10, color);
}

// +/- range, radius to AMID [pixel], zero label offset, opt. small area refresh at [scale*10]
void PolarGauge::drawScale(int16_t at)
{
    // line density on outer scale area
    int16_t modulo = (_range > 10) ? 20 : (_range < 6) ? 5 : 10;

    // for larger ranges put at least on extra label in the middle of each half scale
    int16_t mid_lpos_upper = static_cast<int16_t>(std::roundf(func->invers(0.5 * (*func)(_range)))) * 10;
    mid_lpos_upper = (mid_lpos_upper / modulo) * modulo; // round down to the next modulo hit
    int16_t mid_lpos_lower = static_cast<int16_t>(std::roundf(func->invers(0.5 * (*func)(_mrange)))) * 10;
    mid_lpos_lower = (mid_lpos_lower / modulo) * modulo;
    MYUCG->setFontPosCenter();
    MYUCG->setFont(ucg_font_fub14_hn);

    // calc pixel dist for interval 0.5-1 on scale bow
    int16_t dist = std::roundf(((*func)(1.) - (*func)(0.5)) * _radius); // in pixel
    // ESP_LOGI(FNAME, "lines go m%d %d %d", modulo, dist, mid_lpos_upper);

    // increment in 1/10 scale steps
    int16_t start = _range * 10, stop = _mrange * 10;
    if (at != -1000)
    {
        int16_t tmp = 10 * at + 40;
        if (tmp < start)
        {
            start = tmp;
        }
        tmp = 10 * at - 40;
        if (tmp > stop)
        {
            stop = tmp;
        }
        if (std::abs(start) <= 10)
        {
            modulo = (dist > 24) ? 1 : (dist > 16) ? 2 : (dist > 8) ? 5 : 10;
        }
    }
    // ESP_LOGI(FNAME, "scale from %d to %d", start, stop);
    bool draw_label = false;
    int zeroat = 10 * func->getZero();
    for (int16_t a = start; a >= stop; a--)
    {

        if (a == zeroat + 10 || a == zeroat - 10)
        {
            draw_label = true;
            if (a > 0)
            {
                modulo = (dist > 24) ? 1 : (dist > 16) ? 2 : (dist > 8) ? 5 : 10;
            }
            else
            {
                modulo = (_range > 10) ? 20 : (_range < 6) ? 5 : 10;
            }
        }

        if (!(a % modulo))
        {
            float val = (*func)(static_cast<float>(a) / 10.);
            // a line to draw
            int16_t width = 1; // .1 little/short lines
            int16_t end = _radius + 7;

            if (!(a % 5))
            {
                // .5 small line
                width = 2;
                end = _radius + 12;
            }

            if (!(a % 10))
            {
                // every integer big line
                if (modulo < 11 || a == _range * 10 || a == _range * 10 || a == mid_lpos_upper || a == mid_lpos_lower || a == zeroat)
                {
                    width = 3;
                    end = _radius + 15;
                }
                draw_label = a != zeroat && (draw_label || _range < 5 || a == mid_lpos_upper || a == mid_lpos_lower || a == _range * 10 || a == _mrange * 10);
            }
            // ESP_LOGI(FNAME, "lines a:%d end:%d label: %d  width: %d", a, end, draw_label, width );

            drawOneScaleLine(val, end, width);
            if (draw_label)
            {
                drawOneLabel(val, a / 10);
            }
            draw_label = false;
        }
    }
    int16_t prev = dice_up((start+5.f)/10.);
    int16_t end = 0;
    if (start > 0)
    {
        drawBow(-1, prev, _indicator->getSize() + 5);
    }
    else
    {
        end = (start-5.f) / 10;
    }
    if (stop < 0)
    {
        prev = dice_up((stop-5.f)/10.);
        drawBow(end, prev, _indicator->getSize() + 5);
    }
    MYUCG->setFontPosBottom();
}


////////////////////////////
// trigenometric helpers for gauge painters

// get sin/cos position from gauge index in radian with gauge centered mapping
int16_t PolarGauge::SinCentered(float val, int16_t len) const
{
    return _ref_y - static_cast<int16_t>(fast_roundf_to_int(fast_sin_rad(val) * len));
}
int16_t PolarGauge::CosCentered(float val, int16_t len) const
{
    return _ref_x - static_cast<int16_t>(fast_roundf_to_int(fast_cos_rad(val) * len));
}
// based on discrete integral values with 0.5deg resolution
int16_t PolarGauge::SinCenteredDeg2(int16_t val, int16_t len) const
{
    return _ref_y - fast_roundf_to_int(fast_sin_idx(val) * len);
}
int16_t PolarGauge::CosCenteredDeg2(int16_t val, int16_t len) const
{
    return _ref_x - fast_roundf_to_int(fast_cos_idx(val) * len);
}
int16_t PolarGauge::Sin(float val, int16_t len) const
{
    return static_cast<int16_t>(fast_roundf_to_int(fast_sin_rad(val) * len));
}
int16_t PolarGauge::Cos(float val, int16_t len) const
{
    return static_cast<int16_t>(fast_roundf_to_int(fast_cos_rad(val) * len));
}
int16_t PolarGauge::SinDeg2(int16_t val, int16_t len) const
{
    return fast_roundf_to_int(fast_sin_idx(val) * len);
}
int16_t PolarGauge::CosDeg2(int16_t val, int16_t len) const
{
    return fast_roundf_to_int(fast_cos_idx(val) * len);
}
