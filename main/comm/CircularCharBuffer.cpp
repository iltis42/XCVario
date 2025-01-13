/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "CircularCharBuffer.h"
#include <algorithm>

// Serial message of max length < capacity to UART fifo buffer.
// Designed for multi core usage w/o semaphore and the exclusive 
// access from two always same peer context. A provider and a consumer context.
// The only shared attribute is the load counter, wich is an atomic variable.
// 
// The circular buffer minimizes the memeory footprint and does not limit the number of 
// messages in the buffer, as long as the sum of their length fit into its capacity.
//
// Usage
// The provider pushes messages with "push", or "push_block". Optionally check ahead of the
// amount of free space, otherwise check the push result is ok. Partial pushing of messages is not
// supported.
//
// The consumer most interresting use case is to retreive contiguos buffer chucks, despite the buffers
// circular design. Instead if the simple access with "pop_block" use two steps:
// - First retrieve the next contigous block with "next_block"
// - Process the block by provided pointer and length
// - Discard the processed block with "pop" from the buffer
// - Check if a second contigous block is availableand process it the same way.


CircularCharBuffer::CircularCharBuffer(int capacity)
    : capacity(capacity), head(0), tail(0), count(0)
{
    buffer = new char[capacity];
}

CircularCharBuffer::~CircularCharBuffer()
{
    delete[] buffer;
}

bool CircularCharBuffer::push(char c)
{
    if (count.load(std::memory_order_acquire) == capacity) {
        return false; // Buffer is full
    }

    buffer[head] = c;
    head = (head + 1) % capacity;

    count.fetch_add(1, std::memory_order_release);
    return true;
}

bool CircularCharBuffer::push_block(const char* data, int len)
{
    if (len > capacity - count.load(std::memory_order_acquire)) {
        return false; // Not enough space
    }

    int space_at_end = capacity - head;
    int to_copy = std::min(len, space_at_end);

    // Copy first part
    std::copy(data, data + to_copy, buffer + head);
    head = (head + to_copy) % capacity;

    // Copy second part (if needed)
    if (len > to_copy) {
        int remaining = len - to_copy;
        std::copy(data + to_copy, data + len, buffer);
        head = remaining;
    }

    count.fetch_add(len, std::memory_order_release);
    return true;
}

int CircularCharBuffer::pop_block(char* dest, int len)
{
    int available = count.load(std::memory_order_acquire);
    int to_read = std::min(len, available);

    int space_at_end = capacity - tail;
    int first_part = std::min(to_read, space_at_end);

    // Copy first part
    std::copy(buffer + tail, buffer + tail + first_part, dest);
    tail = (tail + first_part) % capacity;

    // Copy second part (if needed)
    if (to_read > first_part) {
        int second_part = to_read - first_part;
        std::copy(buffer, buffer + second_part, dest + first_part);
        tail = second_part;
    }

    count.fetch_sub(to_read, std::memory_order_release);
    return to_read;
}

char *CircularCharBuffer::next_block(int &len)
{
    int available = count.load(std::memory_order_acquire);
    int space_at_end = capacity - tail;
    len = std::min(space_at_end, available); // contiguous part

    return buffer + tail;
}

void CircularCharBuffer::pop(int len)
{
    len = std::min(len, count.load(std::memory_order_acquire));
    tail = (tail + len) % capacity;
    count.fetch_sub(len, std::memory_order_release);
}

int CircularCharBuffer::size() const {
    return count.load(std::memory_order_acquire);
}

int CircularCharBuffer::available_space() const
{
    return capacity - count.load(std::memory_order_acquire);
}

bool CircularCharBuffer::empty() const
{
    return count.load(std::memory_order_acquire) == 0;
}

bool CircularCharBuffer::full() const
{
    return count.load(std::memory_order_acquire) == capacity;
}
