/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


// Everything is pumping events into this queue to update the screens
extern QueueHandle_t uiEventQueue;
void drawDisplay(void *arg);