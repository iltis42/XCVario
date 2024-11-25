/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "Routing.h"

#include "Messages.h"

#include <logdef.h>

// Routing relevant things
// - Buffer pool
// - routing table
// - Send queue


// QueueHandle_t CanSendQueue = 0;
MessagePool MP;

namespace DEV {

Message* acqMessage(DeviceId target_id, int port)
{
    Message* m = MP.getOne();
    if ( m ) {
        m->target_id = target_id;
        m->port = port;
        return m;
    }
    ESP_LOGW(FNAME, "Buffer pool empty (%d)", target_id);
    return nullptr;
}

void relMessage(Message *msg)
{
    MP.recycleMsg(msg);
}

bool Send(Message* msg)
{
    if ( CanSendQueue ) {
        if ( pdTRUE != xQueueSend( CanSendQueue, (void * ) &msg, (TickType_t)0 ) ) {
            // drop it
            ESP_LOGW(FNAME, "Dropped message to %d", msg->target_id);
            MP.recycleMsg(msg);
        }
        return true;
    }
    return false;
}



} // namespace