/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// when set the wind task wants to be feed with new data.
extern QueueHandle_t BackgroundTaskQueue;
// Calculation is then triggered by events on the queue
struct CalkTaskJob
{
    enum { CALK_TASK_EVENT_NEW_GPSPOSE = 0x0100,
           CALK_TASK_EVENT_NUMSAT = 0x0200
    };

    uint16_t raw;
    CalkTaskJob() = delete;
    constexpr CalkTaskJob(const uint16_t v) : raw(v) {}
    constexpr int getDetail() const { return (raw & 0xff); } // get the job argument
    void setDetail(int8_t v) { raw |= (v & 0xff); }
    constexpr int getJobTyp() const { return (raw & 0xff00); }

};


class WindCalcTask
{
public:
    WindCalcTask();
    ~WindCalcTask();

    // API
    static void createWindResources();
    QueueHandle_t getQueue() { return _queue; }

private:
    QueueHandle_t _queue = 0;
};

extern WindCalcTask *CalcTask;
