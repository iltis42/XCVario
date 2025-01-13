/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include <atomic>

class CircularCharBuffer {
public:
    explicit CircularCharBuffer(int capacity);
    ~CircularCharBuffer();

    bool push(char c);
    bool push_block(const char* data, int len);
    int pop_block(char* dest, int len);
    char* next_block(int &len);
    void pop(int len);

    int size() const;       // Current fill level
    int available_space() const;
    bool empty() const;     // Is the buffer empty?
    bool full() const;      // Is the buffer full?

private:
    char* buffer;           // Buffer storage
    const int capacity;     // Total buffer capacity
    int head;               // Write index
    int tail;               // Read index
    std::atomic<int> count; // Fill level
};
