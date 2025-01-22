
#include "Messages.h"
#include "logdef.h"

#include <iostream>
#include <sstream>
#include <iomanip>

const int BUFFER_COUNT = 20;

// Fixme, not yet thought t the end, it must not get mixed up with others in the regular queue
Message spare_msg; // To be able to grant a message, even this one will not be an exclusive buffer worst case.

std::string Message::hexDump(int upto) const
{
    if ( upto == 0 ) upto = buffer.size();
    std::ostringstream oss;
    for (unsigned char c : buffer.substr(0,upto)) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
    }
    return oss.str();
}

MessagePool::MessagePool()
{
    // Preallocate the desired number of messages
    _buffers.reserve(BUFFER_COUNT);
    for (int i = 0; i < BUFFER_COUNT; ++i) {
        _buffers.emplace_back(new Message);
        _freeList.push(_buffers.back());
    }
    _mutex = xSemaphoreCreateMutex();
}
MessagePool::~MessagePool()
{
    for (int i = 0; i < BUFFER_COUNT; ++i) {
        delete _buffers[i];
    }
    vSemaphoreDelete(_mutex);
}

// granted none nullptr return value
Message* MessagePool::getOne()
{
    Message* msg = nullptr;
    xSemaphoreTake(_mutex, portMAX_DELAY);
    if ( ! _freeList.empty() ) {
        msg = _freeList.front();
        msg->busy = true;
        _freeList.pop();
        _nr_acquisition++;
    }
    else {
        msg = &spare_msg;
        _nr_acqfails++;
        ESP_LOGW(FNAME, "Buffer pool empty.");

    }
    xSemaphoreGive(_mutex);
    return msg;
}

void MessagePool::recycleMsg(Message* msg) {
    msg->busy = false;
    xSemaphoreTake(_mutex, portMAX_DELAY);
    _freeList.push(msg);
    xSemaphoreGive(_mutex);
}

int MessagePool::nrFree() const
{
    return 0; // todo
}
int MessagePool::nrUsed() const
{
    return 0; // todo
}

