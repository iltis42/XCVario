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

// use it like this:
// #include <mutex>
// ...
// SemaphoreMutex mutex;
// ...
// {
//     std::lock_guard<SemaphoreMutex> lock(mutex);
//     // critical section
// }

class SemaphoreMutex {
public:
    SemaphoreMutex() { handle = xSemaphoreCreateMutex(); }

    void lock() {
        // Wait indefinitely until the mutex can be taken
        xSemaphoreTake(handle, portMAX_DELAY);
    }

    void unlock() {
        xSemaphoreGive(handle);
    }

    // Optional: try_lock support if you need std::unique_lock
    bool try_lock() {
        return xSemaphoreTake(handle, 0) == pdTRUE;
    }

private:
    SemaphoreHandle_t handle;
};

