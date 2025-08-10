/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "Altimeter.h"

#include "Colors.h"
#include "setup/SetupNG.h"
#include "Units.h"
#include "AdaptUGC.h"
#include "sensor.h"

#include <cstdio>

extern AdaptUGC *MYUCG;

Altimeter::Altimeter(int16_t cx, int16_t cy) :
    ScreenElement(cx, cy)
{
    MYUCG->setFont(ucg_font_fub25_hr, true);
    _char_width = MYUCG->getStrWidth("2");
    _char_height = MYUCG->getFontAscent() - MYUCG->getFontDescent() - 4;
    _unit = alt_unit.get();
}

void Altimeter::drawUnit()
{
    int16_t tmp = alt_unit.get();
    if (gflags.standard_setting == true) {
        tmp = ALT_UNIT_FL;
    }
    _unit = tmp;

    switch (alt_quantization.get())
    {
    case ALT_QUANT_DISABLE:
        _quant = 0;
        break;
    case ALT_QUANT_5:
        _quant = 5;
        break;
    case ALT_QUANT_10:
        _quant = 10;
        break;
    case ALT_QUANT_20:
        _quant = 20;
        break;
    default:
        _quant = 2;
    }

    MYUCG->setFont(ucg_font_fub11_hr, true);
    MYUCG->setColor( COLOR_HEADER );
    MYUCG->setPrintPos(_ref_x+5, _ref_y-3);
    const char *dmode;
    if( alt_display_mode.get() == MODE_QFE )
        dmode = "QFE";
    else if( alt_display_mode.get() == MODE_QNH )
        dmode = "QNH";
    else
        dmode = "";
    MYUCG->print(dmode);// e.g. 'QNH'
    MYUCG->setPrintPos(_ref_x+5, _ref_y-3+16);
    MYUCG->print(Units::AltitudeUnit(_unit)); // e.g. 'm'
    // QNH
    float qnh = QNH.get();
    if( gflags.standard_setting == true ){
        qnh = 1013;
    }
    char s[24];
    if( qnh_unit.get() == QNH_INHG )
        sprintf(s, "%.2f ", Units::Qnh(qnh));
    else
        sprintf(s, "%d ", Units::QnhRounded(qnh));
    MYUCG->setPrintPos(_ref_x+5, _ref_y-3-16);
    MYUCG->setColor( COLOR_WHITE );
    MYUCG->print(s);
}


// right-aligned to value, unit optional behind
// altidude >0 and <0 are displayed correctly with last two digits rolling accoring to their fraction to the right
// > [m] QNH
void Altimeter::draw(float alt_m)
{
    // ESP_LOGI(FNAME,"draw alt %f", altitude);
    // check on the rendered value for change

    float altitude = _altflt += (alt_m - _altflt) * 0.1; // a bit lowpass make sense, any jitter would mess up tape display readability
    if (alt_display_mode.get() == MODE_QFE)
    {
        altitude -= elevation.get(); // fixme, what is elevation is not set?
    }
    altitude = Units::Altitude(altitude, _unit);

    int alt = (int)(altitude);
    int16_t unitalt = alt_unit.get();
    if (gflags.standard_setting)
    { // respect autotransition switch
        unitalt = ALT_UNIT_FL;
    }
    if (_unit != unitalt)
    {
        drawUnit();
    }
    int used_quant = _quant;
    if (unitalt == ALT_UNIT_FL)
    {                   // may change dynamically in case of autotransition enabled
        used_quant = 1; // we use 1 for FL, this rolls smooth as slowly
    }
    if (used_quant)
    {
        alt = (int)(altitude * (20.0 / used_quant)); // respect difference according to choosen quantisation
    }
    // The dirty criterion
    if (alt == _alt_prev && !_dirty)
        return;
    _alt_prev = alt;
    _dirty = false;
    // alt = fast_roundf_to_int(altitude);

    char s[32]; // plain altimeter as a string
    MYUCG->setFont(ucg_font_fub25_hr, true);
    MYUCG->setColor(COLOR_WHITE);
    sprintf(s, "  %03d", alt); // need the string with at least three digits !!
    // FL also now also displayed fancy with low quant looks good to allow switching with no artefacts
    if (used_quant != _last_quant)
    { // cleanup artefacts from higher digits
        _last_quant = used_quant;
        MYUCG->setColor(COLOR_BLACK);
        MYUCG->drawBox(_ref_x - 2 * _char_width, _ref_y - _char_height * 1.5, 2 * _char_width, _char_height * 2);
    }

    if (!used_quant)
    {
        // Plain plot of altitude for m and ft
        sprintf(s, "  %d", alt);
        MYUCG->setPrintPos(_ref_x - MYUCG->getStrWidth(s), _ref_y);
        MYUCG->print(s);
    }
    else
    {
        // Modifications on altitude string when quantization is set
        // for meter and feet
        int len = std::strlen(s);
        int nr_rolling_digits = (used_quant > 9) ? 2 : 1; // maximum two rolling last digits

        // Quantized altitude, strip and save sign
        float alt_f = std::abs(altitude);           // float altitude w/o sign
        int sign = std::signbit(altitude) ? -1 : 1; // interger sign of the altitude
        alt = (int)(alt_f);                         // to integer truncated altitude
        alt = ((alt + used_quant / 2) / used_quant) * used_quant;
        float fraction = (alt_f + used_quant / 2 - alt) / used_quant;
        int mod = (nr_rolling_digits == 2) ? 100 : 10; // mod = pow10(nr_rolling_digits);
        int alt_leadpart = alt / (mod * 10);           // left remaining part of altitude
        s[len - nr_rolling_digits] = '\0';
        len -= nr_rolling_digits; // chop nr_rolling_digits digits
        static float fraction_prev = 1.;
        if (std::abs(fraction - fraction_prev) > 0.01)
        {
            // move last used_quant digit(s)
            int base = mod / 10;
            int lastdigit = alt % mod;
            int16_t m = sign * ((1.f - fraction) * _char_height - _char_height / 2); // to pixel offest
            // ESP_LOGI(FNAME,"Last %f/%d: %f m%d .%d", altitude, alt, fraction, m, lastdigit);
            int16_t xp = _ref_x - nr_rolling_digits * _char_width;
            // MYUCG->drawFrame(xp-1, _ref_y - _char_height* 1.35 -1, _char_width*nr_rolling_digits, _char_height*1.8 +1); // checker box
            MYUCG->setClipRange(xp, _ref_y - _char_height * 1.35, _char_width * nr_rolling_digits - 1, _char_height * 1.8); // space to get 2 digits displayed uncut
            MYUCG->setPrintPos(xp, _ref_y - m - _char_height);
            char tmp[32];
            sprintf(tmp, "%0*u", nr_rolling_digits, abs((lastdigit + (sign * used_quant)) % mod));
            // ESP_LOGI(FNAME,"tmp0 %s ld: %d", tmp, (lastdigit+(sign*used_quant))%mod );
            MYUCG->print(tmp); // one above
            MYUCG->setPrintPos(xp, _ref_y - m);
            sprintf(tmp, "%0*u", nr_rolling_digits, lastdigit);
            // ESP_LOGI(FNAME,"tmp1 %s ld: %d", tmp, lastdigit );
            MYUCG->print(tmp);
            MYUCG->setPrintPos(xp, _ref_y - m + _char_height);
            // ESP_LOGI(FNAME,"Last %f/%d: %f m%d .%d ldc:%d mod:%d", altitude, alt, fraction, m, lastdigit, ((lastdigit+mod-(sign*used_quant))%mod), mod );
            sprintf(tmp, "%0*u", nr_rolling_digits, abs((lastdigit + mod - (sign * used_quant)) % mod));
            // ESP_LOGI(FNAME,"tmp2 %s ld: %d rd:%d s:%d aq:%d las:%d ", tmp, (lastdigit-(sign*used_quant))%mod, nr_rolling_digits, sign, used_quant, lastdigit );
            MYUCG->print(tmp); // one below
            fraction_prev = fraction;
            MYUCG->undoClipRange();

            // Roll leading digit independant of quant setting in 2 * (mod/10) range
            int lead_quant = 2 * base; // eg. 2 for Q=1 and Q=5
            int rollover = ((int)(alt_f) % mod) / base;
            if ((rollover < 1 && alt_leadpart != 0) || (rollover > 8))
            { // [9.1,..,0.9]: roll-over needs clarification on leading digit
                // Re-Quantized leading altitude part
                fraction = (float)((int)((alt_f + base) * 10) % (mod * 10)) / (lead_quant * 10);
                int16_t m = sign * fraction * _char_height; // to pixel offest
                int lead_digit = ((alt + lead_quant) / mod) % 10;
                // ESP_LOGI(FNAME,"Lead %f/%d: %f - %f m%d %d.", altitude, alt_leadpart, fraction, m, lead_digit);
                nr_rolling_digits++; // one less digit remains to print
                xp -= _char_width;   // one to the left
                // MYUCG->drawFrame(xp-1, _ref_y - _char_height-1, _char_width+1, _char_height+1);
                MYUCG->setClipRange(xp, _ref_y - _char_height, _char_width - 1, _char_height - 1);
                MYUCG->setPrintPos(xp, _ref_y + m - _char_height);
                MYUCG->print(lead_digit); // one above
                MYUCG->setPrintPos(xp, _ref_y + m);
                MYUCG->print((lead_digit + 9) % 10);
                MYUCG->undoClipRange();
                // ESP_LOGI(FNAME,"ld4: %d", (lead_digit+9)%10 );
                s[len - 1] = '\0';
                len--; // chop another digits
            }
        }
        MYUCG->setPrintPos(_ref_x - MYUCG->getStrWidth(s) - nr_rolling_digits * _char_width, _ref_y);
        static char altpart_prev_s[32] = "";
        if (strcmp(altpart_prev_s, s) != 0)
        {
            MYUCG->print(s);
            // ESP_LOGI(FNAME,"s5: %s", s );
            strcpy(altpart_prev_s, s);
        }
    }
}
