/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "DataLink.h"

#include "protocol/CANMasterReg.h"
#include "protocol/JumboCmdHost.h"
#include "Messages.h"

#include "DeviceMgr.h"

#include <logdef.h>

DataLink::~DataLink()
{
    if ( _protocol ) {
        delete _protocol;
    }
}

// protocol factory
ProtocolItf* DataLink::setProtocol(ProtocolType ptyp, int send_port)
{
    if (_protocol && _protocol->getProtocolId() == ptyp) {
        return _protocol;
    }

    // Remove the old one -> todo enable 1.n relation to data link and a list of protocols
    if (_protocol)
    {
        ProtocolItf *tmp = _protocol;
        _protocol = nullptr;
        delete tmp;
    }

    // Create a new one
    switch (ptyp)
    {
    case REGISTRATION:
        ESP_LOGI(FNAME, "New MasterReg");
        _protocol = new CANMasterReg(send_port);
        break;
    case JUMBO_CMD:
        ESP_LOGI(FNAME, "New JumboCmdHost sp%d", send_port);
        _protocol = new JumboCmdHost(send_port);
        break;
    default:
        break;
    }
    return _protocol;
}

ProtocolItf* DataLink::getProtocol(ProtocolType ptyp) const
{
    if ( ptyp == NO_ONE ) {
        return _protocol;
    }
    else if ( _protocol->getProtocolId() == ptyp ) {
        return _protocol;
    }
    return nullptr;
}

bool DataLink::hasProtocol(ProtocolType ptyp) const
{
    return _protocol->getProtocolId() == ptyp;
}

ProtocolItf* DataLink::removeProtocol()
{
    ProtocolItf *tmp = _protocol;
    _protocol = 0;
    return tmp;
}

void DataLink::process(const char *packet, int len)
{
    if (_protocol == nullptr)
        return;

    gen_state_t state;

    // Special use, "no data" timeout, might be expected and normal
    // We just reset the protocol state machine then
    if (packet == nullptr)
    {
        _protocol->timeOut();
        _binary_mode = false;
        return;
    }
    else
    {
        // process every frame byte through state machine
        // ESP_LOGI(FNAME,"Port %d: RX len: %d bytes", port, len );
        // ESP_LOG_BUFFER_HEXDUMP(FNAME,packet, len, ESP_LOG_INFO);
        for (int i = 0; i < len; i++)
        {
            state = _protocol->nextByte(packet[i]);
            if ( state == CHECK_OK ) {
                ; // route further
            } else if (state == GO_BINARY) {
                _binary_mode = true;
            }
        }
    }
}


