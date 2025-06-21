/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "WindCalcTask.h"
#include "CircleWind.h"
#include "StraightWind.h"
#include "setup/SetupCommon.h"
#include "setup/SetupNG.h"
#include "logdef.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


QueueHandle_t BackgroundTaskQueue = nullptr;
WindCalcTask *CalcTask = nullptr;

constexpr int CALC_MQ_SIZE = 5;

// background task to calculate wind with lowest prio
static void wind_calc_task(void *arg)
{
    QueueHandle_t queue = (QueueHandle_t)arg;
    CalkTaskJob job(0);

    while (true)
    {
        // sleep until the queue gives us something to do, or we have to do a retry
        TickType_t timeout = pdMS_TO_TICKS(1100);
        bool new_job = xQueueReceive(queue, &job, timeout) == pdTRUE;

        if ( new_job ) {
            if (job.raw == 0) {
                break;
            } // termination signal
            ESP_LOGI(FNAME, "job type %x", (unsigned)job.getJobTyp() );

            switch(job.getJobTyp()) {
            case CalkTaskJob::CALK_TASK_EVENT_NEW_GPSPOSE:
                if ( circleWind ) {
                    circleWind->setGpsStatus(true);
                    circleWind->newSample();
                }

                if ( straightWind ) {
                    straightWind->calculateWind();
                }
                break;
            case CalkTaskJob::CALK_TASK_EVENT_NUMSAT:
                if ( circleWind ) {
                    circleWind->newConstellation(job.getDetail());
                }
                break;
            default:
                ESP_LOGE(FNAME, "Unknown job type %d", job.getJobTyp() );
                break;
            }
        }
        else {
            // time-out, no valid gps fix any more
            if ( circleWind ) {
                circleWind->setGpsStatus(false);
                circleWind->tick();
            }
            if ( straightWind ) { straightWind->tick(); }
        }
    }
    vQueueDelete(queue);
    vTaskDelete(NULL);
}


// more or less just a guard to the background task
WindCalcTask::WindCalcTask()
{
    _queue = xQueueCreate( CALC_MQ_SIZE, sizeof(CalkTaskJob) );
    xTaskCreate(wind_calc_task, "background", 4000, _queue, 1, nullptr); // least priority
}


WindCalcTask::~WindCalcTask()
{
    xQueueSend( _queue, nullptr, 0 ); // stop task
}

void WindCalcTask::createWindResources()
{
    if ( SetupCommon::isMaster() ) {
        // straight wind
        bool needCircleWind = wind_enable.get() & WA_CIRCLING;
        if ( wind_enable.get() & WA_STRAIGHT) {
            needCircleWind = true;
            if ( ! straightWind ) {
                straightWind = new StraightWind();
                straightWind->begin();
            }
        }
        else if ( ! (wind_enable.get() & WA_STRAIGHT) && straightWind ) {
            StraightWind *tmp = straightWind;
            straightWind = nullptr;
            delete tmp;
        }

        // circle wind
        if ( needCircleWind && ! circleWind ) {
            circleWind = new CircleWind();
        }
        else if ( ! needCircleWind && circleWind ) {
            CircleWind *tmp = circleWind;
            circleWind = nullptr;
            delete tmp;
        }

        // wind calculation
        if ( ! CalcTask && (circleWind || straightWind) ) {
            CalcTask = new WindCalcTask();
            BackgroundTaskQueue = CalcTask->getQueue();
        }
        else if ( CalcTask && ! (circleWind || straightWind) ) {
            BackgroundTaskQueue = nullptr;
            // WindCalcTask *tmp = CalcTask;
            // CalcTask = nullptr;
            // delete tmp;
        }

    }
}
