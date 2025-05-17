/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "InterfaceCtrl.h"

class DataLink;
namespace i2cbus {
    class I2C;
}

class I2CWrapper final : public InterfaceCtrl
{
public:
    I2CWrapper();
    ~I2CWrapper();

    // Ctrl
    InterfaceId getId() const override { return I2C; }
    const char *getStringId() const override { return "I2C"; }
    void ConfigureIntf(int cfg) override;
    int Send(const char*, int&, int) override { return 0; }
    // I2C specific
    i2cbus::I2C *getI2C() const { return _i2cbus; }

private:
    i2cbus::I2C *_i2cbus = nullptr;
};

extern I2CWrapper *I2Cext;
