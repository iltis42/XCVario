#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include "Devices.h"

#include <string>
#include <queue>
#include <vector>
#include <memory>


constexpr int MSG_POOL_SIZE = 20;
constexpr int PREALLOC_BUF_SIZE = 128;

// One Message
class Message
{
public:
    Message() { buffer.reserve(PREALLOC_BUF_SIZE); }
    std::string hexDump(int upto=0) const;
    bool busy = false;
    DeviceId target_id = DeviceId::NO_DEVICE;
    int port = 0;
    std::string buffer;
};

// Preallocated pool of messages
class MessagePool {
public:
    MessagePool();
    ~MessagePool();

    Message* getOne();
    void recycleMsg(Message* msg);

    // Some debug and statistics
    int nrFree() const;
    int nrUsed() const;
    int nrAcq() const { return _nr_acquisition; }

private:
    std::vector<Message *> _buffers;
    std::queue<Message *> _freeList;
    long _nr_acquisition = 0;
    long _nr_acqfails = 0;
    SemaphoreHandle_t _mutex = nullptr;
};


