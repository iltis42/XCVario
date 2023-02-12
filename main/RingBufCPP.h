#pragma once

#include "RingBufHelpers.h"
#include "logdef.h"
#include <cstring>
#include <SString.h>

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
		{
			full = isFull();
			if (!full || overwrite)
			{
				_buf[_head] = obj;
				_head = (_head + 1) % MaxElements;
				_numElements++;
			}
		}
		return !full;
	}

	/**
	 * Remove last element from buffer, and copy it to dest
	 * Return: true on success
	 */
	bool pull(Type &dest)
	{
		bool ret = false;
		{
			if (!isEmpty())
			{
				dest = _buf[_tail];
				_numElements--;
				_tail = (_tail + 1) % MaxElements;
				ret = true;
			}
		}
		return ret;
	}

	int pull(char *dest)
	{
		int ret = 0;
		{
			if (!isEmpty())
			{
				memcpy( dest, _buf[_tail].c_str(), _buf[_tail].length() );
				ret = _buf[_tail].length();
				_numElements--;
				_tail = (_tail + 1) % MaxElements;
			}
		}
		return ret;
	}

	/**
	 * Removes all items from the queue.
	 */
	inline void clear()
	{
		_numElements = 0;
		_head = 0;
		_tail = 0;
	}

	/**
	 * Return: true if buffer is full
	 */
	inline bool isFull() const
	{
		return (_numElements >= MaxElements);
	}

	/**
	 * Return: number of elements in buffer
	 */
	inline size_t numElements() const
	{
		return _numElements;
	}

	/**
	 * Return: true if buffer is empty
	 */
	inline bool isEmpty() const
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
