
#include "Messages.h"
#include "logdef.h"

#include <iostream>
#include <sstream>
#include <iomanip>

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
    _buffers.reserve(MSG_POOL_SIZE);
    for (int i = 0; i < MSG_POOL_SIZE; ++i) {
        _buffers.emplace_back(new Message);
        _freeList.push(_buffers.back());
    }
    _mutex = xSemaphoreCreateMutex();
}
MessagePool::~MessagePool()
{
    xSemaphoreHandle tmp = _mutex;
    _mutex = nullptr;
    for (int i = 0; i < MSG_POOL_SIZE; ++i) {
        delete _buffers[i];
    }
    vSemaphoreDelete(tmp);
}

// granted none nullptr return value
Message* MessagePool::getOne()
{
    Message* msg = nullptr;
    xSemaphoreTake(_mutex, portMAX_DELAY);
    while ( _freeList.empty() )
    {
        xSemaphoreGive(_mutex);
        vTaskDelay(10 / portTICK_PERIOD_MS);
        _nr_acqfails++;
        xSemaphoreTake(_mutex, portMAX_DELAY);
    }
    msg = _freeList.front();
    msg->busy = true;
    _freeList.pop();
    _nr_acquisition++;
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
    return _freeList.size();
}
int MessagePool::nrUsed() const
{
    return MSG_POOL_SIZE - _freeList.size();
}

