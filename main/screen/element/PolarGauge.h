/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "ScreenElement.h"
#include "AdaptUGC.h"

class ArrowIndicator;
class LargeFigure;

class GaugeFunc
{
public:
    GaugeFunc(float scale, float zero) : _scale_k(scale), _zero_at(zero) {}
    virtual ~GaugeFunc() = default;
    // calculate a gauge indicator position in rad (-_scale_max .. _scale_max) for a value
    virtual float operator()(float a) const { return 0.; }
    // inverse to operator. Get the value for an indicator position in radian
    virtual float invers(float rad) const { return 0.; }
    float getZero() const { return _zero_at; }

protected:
    float _scale_k; // scale factor
    float _zero_at; // value at the middle of the scale
};

class PolarGauge : public ScreenElement
{
    friend class PolarIndicator;
    friend class LargeFigure;
public:
    PolarGauge(int16_t refx, int16_t refy, int16_t scale_end, int16_t radius);
    ~PolarGauge();
    void forceAllRedraw();
    void setRange(float pos_range, float zero_at, bool log);
    float getMRange() const { return _mrange; }
    void setUnit(float uf) { _unit_fac = uf; }
    void setColor(int color_idx);
    void setFigOffset(int16_t ox, int16_t oy);
    float clipValue(float a) const;

    void draw(float a);
    void drawIndicator(float a);
    void drawPolarSink(float a);
    void drawFigure(float a);
    using BowColorIdx = enum { GREEN, BLUE, ORANGE, RED };
    void colorRange(float from, float to, int16_t color);
    void drawScale(int16_t at = -1000);
    // drawAVG();

private:
    // indicator and attributes
    ArrowIndicator *_arrow = nullptr;
    float _scale_max = 1.57f; // half scale extend in rad
    int16_t _radius = 50; // pixel
    float _range = 5.; // max positive value of the scale
    float _mrange = -5.; // resulting from range and zero_at, assuming an always symetric scale
    const float _idx_scale; // cut the scale range into discrete 0.5deg steps.
    float _unit_fac = 1.f; // scale  from SI units to the guage unit
    int16_t _old_idx = 360; // discretized previous index value
    int16_t _old_bow_idx = 0;
    int16_t _old_polar_sink = 0;
    static const ucg_color_t bow_color[4];

    // gauge value as average or figures
    LargeFigure *_figure = nullptr;

    // gauge function
    GaugeFunc *func = nullptr; // map value range to scale range [rad]
    // dice up into (arbitrary) discrete steps, map scale range into a 0,5° step counter
    int16_t dice_up(float a) const { return (int16_t)(_idx_scale*(*func)(a)); }
    int16_t dice_rad(float rad) const { return (int16_t)(rad*_idx_scale); }

    // trigenometric macros
    int16_t SinCentered(float val, int16_t len) const; // output in pixel
    int16_t CosCentered(float val, int16_t len) const;
    int16_t SinCenteredDeg2(int16_t val, int16_t len) const;
    int16_t CosCenteredDeg2(int16_t val, int16_t len) const;
    int16_t Sin(float val, int16_t len) const;
    int16_t Cos(float val, int16_t len) const;
    int16_t SinDeg2(int16_t val, int16_t len) const;
    int16_t CosDeg2(int16_t val, int16_t len) const;

    // gauge helpers
    void drawOneScaleLine(float a, int16_t l2, int16_t w) const;
    void drawBow(int16_t idx, int16_t &old, int16_t w, int16_t color=0) const;
    void drawSeg(float a0, float a1) const;
    void drawOneLabel(float val, int16_t labl) const;
};