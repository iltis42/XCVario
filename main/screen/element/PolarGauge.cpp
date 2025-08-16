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
#include "math/Trigenometry.h"
#include "Units.h"
#include "Colors.h"
#include "AdaptUGC.h"
#include "logdefnone.h"

#include <cmath>

extern AdaptUGC *MYUCG;
static const ucg_color_t ndl_color[3] = {{COLOR_WHITE}, {COLOR_ORANGE}, {COLOR_RED}};

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
class RoseGaugeFunc : public GaugeFunc
{
public:
    RoseGaugeFunc() : GaugeFunc(My_PIf/180.f, 90.f) {}
    float operator()(float a) const override { return (a + _zero_at) * _scale_k; }
    float invers(float rad) const override { return (rad / _scale_k) - _zero_at; }
};

////////////////////////////
// PolarFigure

void PolarFigure::draw(float val)
{
	int16_t ival = std::rint(val*10);  // integer value in steps of 10th
	if (_figure != ival || _dirty) {
        // only print if there is a change in rounded numeric string
		char s[32];
		MYUCG->setFont(ucg_font_fub35_hn, false );
		MYUCG->setFontPosCenter();
		
		sprintf(s, _format[std::abs(ival)>100], float(abs(ival)/10.) );
		int16_t tmp = MYUCG->getStrWidth(s);
		if( tmp < _fig_len ) {
            // do we have a shorter string
			MYUCG->setColor( COLOR_BLACK );
			int16_t fh = MYUCG->getFontAscent();
            // -> so blank exact prepending area
			MYUCG->drawBox( _ref_x-_fig_len, _ref_y-fh/2, _fig_len-tmp, fh );
		}
		if (val<0) {
			MYUCG->setColor( COLOR_BBLUE );
		} else {
			MYUCG->setColor( COLOR_WHITE );
		}
		MYUCG->setPrintPos(_ref_x-tmp, _ref_y+8);
		MYUCG->print(s);
		_figure = ival;
		_fig_len = tmp;
		MYUCG->setFontPosBottom();
	}
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
}
PolarGauge::~PolarGauge()
{
    delete _indicator;
    if ( func ) {
        delete func;
    }
}

void PolarGauge::forceAllRedraw()
{
    _dirty = true;
    _figure.forceRedraw();
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
    _indicator->setColor(needlecolor[color_idx & 3]);
}

void PolarGauge::setFigOffset(int16_t ox, int16_t oy)
{
    _figure.setPos(_ref_x+ox, _ref_y+oy);
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
    if (_indicator->draw(val))
    {
        // Draw colored bow
        int16_t bar_val = (val > 0) ? val : 0;
        // draw green vario bar
        drawBow(bar_val, _old_bow_idx, 3, GREEN);
    }
}

void PolarGauge::drawIndicator(float a)
{
    _indicator->draw(dice_up(clipValue(a)));
}

// sink speed in [m/s]
void PolarGauge::drawPolarSink(float a)
{
    a *= _unit_fac;
    int16_t val = dice_up(clipValue(a));
    // ESP_LOGI(FNAME,"sink  a:%f - %d", a, val);
    // Draw blue bow for polar sink
    drawBow(val, _old_polar_sink, 3, BLUE);
}

void PolarGauge::drawFigure(float a)
{
    a *= _unit_fac;
    _figure.draw(a);
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
    if (idx == old) {
        return;
    }
    // ESP_LOGI(FNAME,"drawBbow af= level=%d old_level=%d", idx, old );

    // potentially clean first
    if (std::abs(idx) < std::abs(old) || idx * old < 0) {
        MYUCG->setColor(DARK_DGREY);
    }
    else {
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
    ESP_LOGI(FNAME, "range %f/%f lines go m%d %d %d", _range, _mrange, modulo, dist, mid_lpos_upper);

    // increment in 1/10 scale steps
    int16_t start = std::roundf(_range)*10, stop = std::roundf(_mrange)*10;
    int16_t l_start = start, l_stop = stop;
    if (at != -1000)
    {
        // partial scale repainting
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

            drawOneScaleLine(val, end, width);
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
        drawBow(to, prev, _indicator->getSize() + 5);
    }
    else
    {
        // draw bow towards zero to get the dark grey (background color)
        drawBow(prev, to, _indicator->getSize() + 5);
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
