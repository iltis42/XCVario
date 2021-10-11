
#pragma once

#include "RingBufHelpers.h"
#include "logdef.h"
#include <cstring>

#define SSTRLEN 256  // max size of NMEA, 80 bytes are no more hardlimit in modern receivers such as Beitian BN-80, so extend


// wrapper class for a limited string, pre-allocation to be done resize() method

class SString
{
public:
    SString()
    {
    }
    SString(const SString& s) { str = s.str; }
    ~SString(){ str.erase( 0, length() ); }

    SString(const char *s)
    {
        str = std::string( s );
    }
    void set(const char *s)
    {
        // add pure strings
    	str = std::string( s );
    }
    void set(const char *s, int alen)
    {
      str = std::string( s, alen );
    }
    SString& operator=(const SString& right)
    {
       str = right.str;
       return *this;
    }
    void append(char c)
    {
    	str += c;
    }
    void append(const char *s, int alen)
    {
       str.append( s, alen );
    }
    SString& operator+=(const SString& right)
    {
      str += right.str;
      return *this;
    }
    void clear()
    {
        str.erase( 0, length() );
    }
    inline void setLen(int alen)
    {
    	str.resize( alen );
    }
    inline void addLen(int alen)
    {
       str.append( ' ', alen );
    }
    inline const char *c_str() const { return str.c_str(); };
    inline int length() const { return str.length(); }
    inline int max_length() const { return SSTRLEN; }
    inline int rem_length() const { return SSTRLEN - 1 - length(); }
    inline int full() const { return str.length() >= SSTRLEN - 1; }
    inline const char *a_str() { return str.c_str(); }
    inline void resize(int len) { str.resize(len); };

private:
    std::string str;
};

template <typename Type, size_t MaxElements>
class RingBufCPP
{
public:
    inline RingBufCPP()
    {
        _numElements = 0;
        _head = 0;
        _tail = 0;
    }

    /**
     * Add element obj to the buffer.
     *
     * If there is already MaxElements in the buffer,
     * the oldest element will either be overwritten (when overwrite is true) or
     * this add will have no effect (when overwrite is false).
     *
     * Return: true if there was room in the buffer to add this element
     */
    bool add(const Type &obj, bool overwrite = false)
    {
        bool full = false;
        RB_ATOMIC_START
        {
            full = isFull();
            if (!full || overwrite)
            {
                _buf[_head] = obj;
                _head = (_head + 1) % MaxElements;
                _numElements++;
            }
        }
        RB_ATOMIC_END
        return !full;
    }

    /**
     * Remove last element from buffer, and copy it to dest
     * Return: true on success
     */
    bool pull(Type &dest)
    {
        bool ret = false;
        RB_ATOMIC_START
        {
            if (!isEmpty())
            {
                dest = _buf[_tail];
                _numElements--;
                _tail = (_tail + 1) % MaxElements;
                ret = true;
            }
        }
        RB_ATOMIC_END

        return ret;
    }

    /**
     * Peek at num'th element in the buffer
     * Return: a pointer to the num'th element
     */
    Type *peek(size_t num)
    {
        Type *ret = NULL;
        RB_ATOMIC_START
        {
            if (num < MaxElements) //make sure not out of bounds
                ret = &_buf[num];
        }
        RB_ATOMIC_END
        return ret;
    }

    /**
     * Return: true if buffer is full
     */
    bool isFull() const
    {
        return (_numElements >= MaxElements);
    }

    /**
     * Return: number of elements in buffer
     */
    size_t numElements() const
    {
        return _numElements;
    }

    /**
     * Return: true if buffer is empty
     */
    bool isEmpty() const
    {
        return !_numElements;
    }

protected:
    /**
     * Calculates the index in the array of the oldest element
     * Return: index in array of element
     */
    size_t getTail() const
    {
        return _tail;
    }

    // underlying array
    Type _buf[MaxElements];

    size_t _head;
    size_t _tail;
    size_t _numElements;
};
