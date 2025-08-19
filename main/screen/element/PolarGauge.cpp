/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "PolarGauge.h"

#include "ArrowIndicator.h"
#include "WindIndicator.h"
#include "LargeFigure.h"
#include "math/Trigenometry.h"
#include "Units.h"
#include "Colors.h"
#include "AdaptUGC.h"
#include "logdefnone.h"

#include <cmath>

float getHeading(); // fixme

extern AdaptUGC *MYUCG;
static const ucg_color_t ndl_color[3] = {{COLOR_WHITE}, {COLOR_ORANGE}, {COLOR_RED}};
static constexpr const ucg_color_t bow_color[4] = {{COLOR_GREEN}, {COLOR_BBLUE}, {COLOR_ORANGE}, {COLOR_RED}};
static const ucg_color_t lne_color[3] = {{COLOR_LGREY}, {COLOR_MGREY}, {COLOR_WHITE}};


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
// class RoseGaugeFunc : public GaugeFunc
// {
// public:
//     RoseGaugeFunc() : GaugeFunc(My_PIf/180.f, 90.f) {}
//     float operator()(float a) const override { return (a + _zero_at) * _scale_k; }
//     float invers(float rad) const override { return (rad / _scale_k) - _zero_at; }
// };

////////////////////////////
// PolarGauge

// pixel, pixel, degree, pixel
PolarGauge::PolarGauge(int16_t refx, int16_t refy, int16_t scale_end, int16_t radius, int16_t flavor) :
    ScreenElement(refx, refy),
    _scale_max(deg2rad((float)scale_end)),
    _radius(radius)
{
    func = new GaugeFunc(1.,0.);
    if ( flavor != COMPASS ) {
        _arrow = new ArrowIndicator(*this, 4, 20);
    }
    else {
        _wind_avg = new WindIndicator(*this, false);
        _wind_live = new WindIndicator(*this, true);

        _unit_fac = 1.;
        _range = 360.;
        _mrange = -360.;
        // func = new RoseGaugeFunc();
    }
}
PolarGauge::~PolarGauge()
{
    if ( _arrow ) {
        delete _arrow;
    }
    if ( _wind_avg ) {
        delete _wind_avg;
    }
    if ( _wind_live ) {
        delete _wind_live;
    }
    if ( _figure ) {
        delete _figure;
    }
    if ( func ) {
        delete func;
    }
}

void PolarGauge::forceAllRedraw()
{
    _dirty = true;
    if ( _figure ) {
        _figure->forceRedraw();
    }
    _old_bow_idx = 0;
    _old_polar_sink = 0;
}

void PolarGauge::setRange(float pos_range, float zero_at, bool log)
{
    _range = pos_range * _unit_fac;
    _mrange = 2 * zero_at - _range;
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
    if ( _arrow ) {
        _arrow->setColor(ndl_color[color_idx & 3]);
    }
    if ( _wind_live ) {
        _wind_live->setColor(ndl_color[color_idx & 3]);
    }
}

void PolarGauge::setFigOffset(int16_t ox, int16_t oy)
{
    if ( ! _figure ) {
        _figure = new LargeFigure(_ref_x+ox, _ref_y+oy);
    }
    else {
        _figure->setPos(_ref_x+ox, _ref_y+oy);
    }
}

float PolarGauge::clipValue(float a) const
{
    if (a > _range) {
        a = _range;
    }
    else if (a < _mrange) {
        a = _mrange;
    }
    return a;
}

// > in [m/s]
void PolarGauge::draw(float a)
{
    a *= _unit_fac;
    int16_t val = dice_up(clipValue(a));
    // ESP_LOGI(FNAME,"draw  a:%f - %d", a, val);
    if (_arrow->draw(val))
    {
        // Draw colored bow
        int16_t bar_val = (val > 0) ? val : 0;
        // draw green vario bar
        drawBow(bar_val, _old_bow_idx, 3, 0, GREEN);
    }
}

void PolarGauge::drawIndicator(float a)
{
    _arrow->draw(dice_up(clipValue(a)));
}

// sink speed in [m/s]
void PolarGauge::drawPolarSink(float a)
{
    a *= _unit_fac;
    int16_t val = dice_up(clipValue(a));
    // ESP_LOGI(FNAME,"sink  a:%f - %d", a, val);
    drawBow(val, _old_polar_sink, 3, 0, BLUE);
}

void PolarGauge::drawFigure(float a)
{
    if ( _figure ) {
        a *= _unit_fac;
        _figure->draw(a);
    }
}

void PolarGauge::drawWind(int16_t wdir, int16_t wval, int16_t idir, int16_t ival)
{
    int16_t heading = 0;

    if ( ! _north_up ) {
        heading = static_cast<int16_t>(std::roundf(getHeading()));
    }

    wval *= _unit_fac;
    wdir -= heading;
    // ESP_LOGI(FNAME,"PW  d:%d - %d", wdir%360, wval);
    bool redraw_w = _wind_avg->changed(wdir, wval) || _dirty;

    ival *= _unit_fac;
    idir -= heading;
    bool redraw_i = _wind_live->changed(idir, ival) || _dirty;

    if (redraw_w || redraw_i) {
        if (redraw_w) {
            _wind_avg->drawWind();
        }
        if (redraw_i || (angleDiffDeg(idir, wdir) % 180) < 20) {
            _wind_live->drawWind();
        }
    }
}


// idx [rad], end radius, width
void PolarGauge::drawOneScaleLine(float a, int16_t l2, int16_t w, int16_t cidx) const
{
    float si = fast_sin_rad(a);
    float co = fast_cos_rad(a);
    int16_t l1 = _radius;
    int16_t w0 = w / 2;
    int16_t w1 = w - w0; // total width := w1 + w0
    int16_t xn_0 = _ref_x - static_cast<int16_t>(std::roundf(co * l1 - si * w0));
    int16_t yn_0 = _ref_y - static_cast<int16_t>(std::roundf(si * l1 + co * w0));
    int16_t xn_1 = _ref_x - static_cast<int16_t>(std::roundf(co * l1 + si * w1));
    int16_t yn_1 = _ref_y - static_cast<int16_t>(std::roundf(si * l1 - co * w1));
    int16_t xn_2 = _ref_x - static_cast<int16_t>(std::roundf(co * l2 + si * w1));
    int16_t yn_2 = _ref_y - static_cast<int16_t>(std::roundf(si * l2 - co * w1));
    int16_t xn_3 = _ref_x - static_cast<int16_t>(std::roundf(co * l2 - si * w0));
    int16_t yn_3 = _ref_y - static_cast<int16_t>(std::roundf(si * l2 + co * w0));
    // ESP_LOGI(FNAME,"drawTetragon  x0:%d y0:%d x1:%d y1:%d x2:%d y2:%d x3:%d y3:%d", xn_0, yn_0, xn_1, yn_1, xn_2, yn_2, xn_3, yn_3 );
    MYUCG->setColor(lne_color[cidx].color[0], lne_color[cidx].color[1], lne_color[cidx].color[2]);
    MYUCG->drawTetragon(xn_0, yn_0, xn_1, yn_1, xn_2, yn_2, xn_3, yn_3);
}

// draw incremental bow up to indicator given in diced 0.5° steps, pos
void PolarGauge::drawBow(int16_t idx, int16_t &old, int16_t w, int16_t off, int16_t cidx) const
{
    if (idx == old) {
        return;
    }
    // ESP_LOGI(FNAME,"drawBbow af= level=%d old_level=%d", idx, old );

    // potentially clean first
    if (std::abs(idx) < std::abs(old) || idx * old < 0) {
        MYUCG->setColor(DARK_DGREY);
    }
    else {
        if (cidx>=0) {
            MYUCG->setColor(bow_color[cidx].color[0], bow_color[cidx].color[1], bow_color[cidx].color[2]);

        } else {
            MYUCG->setColor(COLOR_BLACK);
        }
    }
    // ESP_LOGI(FNAME,"bow lev %d", idx);
    int16_t l1 = _radius + off;
    if (  w < 0 ) {
        w = -w;
        l1 += w;
    }
    int inc = (idx - old > 0) ? 1 : -1;
    if ( std::abs(w) > 4 && std::abs(old - idx) > 4)
    {
        inc *= 4;
        if ( std::abs(idx-old)>160 ) {
            inc *= 2;
        }
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
        if (std::abs(inc) > 1 && std::abs(i - idx) < 8)
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
    drawBow(dice_up(to), ol, -10, 0, color);
}

// draw a scale from _range down to _mrange
// with radius _radius
// to _ref_xy center [pixel]
// and zero label offset according to (_mrange + _range) / 2
// opt. small area refresh at [scale]
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
    ESP_LOGI(FNAME, "range %f/%f lines go m%d %d %d", _range, _mrange, modulo, dist, mid_lpos_upper);

    // increment in 1/10 scale steps
    int16_t start = std::roundf(_range)*10, stop = std::roundf(_mrange)*10;
    int16_t l_start = start, l_stop = stop; // for labels
    if (at != -1000)
    {
        // partial scale repainting
        if ( at > 0 ) { // Redraw the AVG area
        int16_t tmp = 10 * at + 5; // alias .5
            if (tmp < start) {
                start = tmp;
            }
            tmp = 10 * at - 5;
            if (tmp > stop) {
                stop = tmp;
            }
            if (std::abs(start) <= 10) {
                modulo = (dist > 24) ? 1 : (dist > 16) ? 2 : (dist > 8) ? 5 : 10;
            }
        }
        else {
            start = 10 * at;
        }
    }
    ESP_LOGI(FNAME, "scale from %d to %d", start, stop);
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
                if (modulo < 11 || a == l_start || a == l_stop || a == mid_lpos_upper || a == mid_lpos_lower || a == zeroat)
                {
                    width = 3;
                    end = _radius + 15;
                }
                draw_label = a != zeroat && (draw_label || _range < 5. || a == mid_lpos_upper || a == mid_lpos_lower || a == l_start || a == l_stop);
            }
            ESP_LOGI(FNAME, "lines a:%d end:%d label: %d  width: %d", a, end, draw_label, width );

            drawOneScaleLine(val, end, width, width-1);
            if (draw_label)
            {
                drawOneLabel(val, a / 10);
            }
            draw_label = false;
        }
    }
    int16_t prev = dice_up(start/10.) +6; // overdraw a bit
    int16_t to = dice_up(stop/10.) -6;
    if (start > 0)
    {
        drawBow(to, prev, _radius - _arrow->getBase(), 0);
    }
    else
    {
        // draw bow towards zero to get the dark grey (background color)
        drawBow(prev, to, _radius - _arrow->getBase(), 0);
    }
    MYUCG->setFontPosBottom();
}

void PolarGauge::drawScaleBottom()
{
    // Area of -60° to -90° is concerned
    drawScale( static_cast<int16_t>(std::roundf(func->invers(deg2rad(-60.)))) );
}

// a [deg]; 0° ref on top
void PolarGauge::drawTwoDots(int16_t a, int16_t size, int16_t cidx) const
{
    float si = -fast_sin_idx(a*2);
    float co = fast_cos_idx(a*2);
    int16_t l1 = _radius + 2;
    int16_t bx = static_cast<int16_t>(std::roundf(si * l1));
    int16_t by = static_cast<int16_t>(std::roundf(co * l1));
    MYUCG->setColor(lne_color[cidx].color[0], lne_color[cidx].color[1], lne_color[cidx].color[2]);
    if ( (a%360) != 0 ) { // skip the "north" position
        MYUCG->drawDisc(_ref_x-bx,_ref_y-by, size, UCG_DRAW_ALL );
    }
    if ( ((a+180)%360) != 0 ) { // skip the "north" position (vis-a-vi)
        MYUCG->drawDisc(_ref_x+bx,_ref_y+by, size, UCG_DRAW_ALL ); // 180° mirrored
    }
}

void PolarGauge::drawRose(int16_t at) const
{
    int16_t start = _range/2 - 10;
    int16_t stop = 0;
    if (at != -1000)
    {
        // partial scale repainting
        start = at + 15;
        stop = at - 15;
    }
    start = (start/10)*10;
    stop = (stop/10)*10;
    for (int16_t a = start; a >= stop; a-=10)
    {
        // ESP_LOGI(FNAME, "dot a:%d", a%360);
        if (!(a % 30)) {
            drawTwoDots( a, 2, 2);
        }
        else if (!(a % 10)) {
            drawTwoDots( a, 1, 0);
        }
        if (!(a%180) ) {
            // Draw ^ for heading-up, or N for north-up
            MYUCG->setFont(ucg_font_fub11_hr);
            char s[3] = "^";
            if ( _north_up ) {
                s[0] = 'N';
            }
            int16_t w2 = MYUCG->getStrWidth(s)/2;
            MYUCG->setColor(COLOR_LBBLUE);
            MYUCG->setPrintPos(_ref_x-w2+1, _ref_y-_radius+9);
            MYUCG->print(s);
        }
    }
}

void PolarGauge::clearGauge()
{
    int16_t tmp = 0;
    drawBow(720, tmp, 14, 10, -1 );
    // remove last indicator print
    if (_wind_live) {
        _wind_live->drawWind(true);
    }
    if (_wind_avg) {
        _wind_avg->drawWind(true);
    }
}

////////////////////////////
// trigenometric helpers for gauge painters

// get sin/cos position from gauge index in radian with gauge centered mapping
int16_t PolarGauge::SinCentered(float val, int16_t len) const {
    return _ref_y - static_cast<int16_t>(fast_roundf_to_int(fast_sin_rad(val) * len));
}
int16_t PolarGauge::CosCentered(float val, int16_t len) const { return _ref_x - static_cast<int16_t>(fast_roundf_to_int(fast_cos_rad(val) * len)); }
// based on discrete integral values with 0.5deg resolution
int16_t PolarGauge::SinCenteredDeg2(int16_t val, int16_t len) const { return _ref_y - fast_roundf_to_int(fast_sin_idx(val) * len); }
int16_t PolarGauge::CosCenteredDeg2(int16_t val, int16_t len) const { return _ref_x - fast_roundf_to_int(fast_cos_idx(val) * len); }
int16_t PolarGauge::Sin(float val, int16_t len) const { return static_cast<int16_t>(fast_roundf_to_int(fast_sin_rad(val) * len)); }
int16_t PolarGauge::Cos(float val, int16_t len) const { return static_cast<int16_t>(fast_roundf_to_int(fast_cos_rad(val) * len)); }
int16_t PolarGauge::SinDeg2(int16_t val, int16_t len) const { return fast_roundf_to_int(fast_sin_idx(val) * len); }
int16_t PolarGauge::CosDeg2(int16_t val, int16_t len) const { return fast_roundf_to_int(fast_cos_idx(val) * len); }
