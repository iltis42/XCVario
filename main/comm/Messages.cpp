
#include "Messages.h"
#include "logdef.h"

const int BUFFER_COUNT = 20;


Message spare_msg; // To be able to grant a message, even this one will not be an exclusive buffer worst case.

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
