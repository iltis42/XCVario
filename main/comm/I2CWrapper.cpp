/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "I2CWrapper.h"

#include "setup/SetupNG.h"

#include "I2Cbus.hpp"


I2CWrapper *I2Cext = nullptr;

I2CWrapper::I2CWrapper()
{
    _i2cbus = &i2c0;
    I2Cext = this;
}

I2CWrapper::~I2CWrapper()
{
    I2Cext = nullptr;
    _i2cbus->close();
}

void I2CWrapper::ConfigureIntf(int cfg)
{
    _i2cbus->begin(GPIO_NUM_4, GPIO_NUM_18, GPIO_PULLUP_DISABLE, GPIO_PULLUP_DISABLE, (int)(compass_i2c_cl.get()*1000) );
}
