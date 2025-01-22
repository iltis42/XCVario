#pragma once

#include <cstring>
#include <queue>


template <typename Type>
class RingBufCPP : public std::queue<Type>
{
public:
	size_t MaxElements = 8;
    /*
     * Set max number of elements in the BufferQ
     *
     */
	void setSize( size_t size ){
		MaxElements = size;
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
	bool add(const Type& value ) {
		if (this->size() == MaxElements) {
			this->pop();
		}
		this->push(value);
		return( !isFull() );
	}

	/**
	 * Remove last element from buffer, and copy it to dest
	 * Return: true on success
	 */
	bool pull(Type &dest)
	{
		if(hasEntries())
		{
			dest = this->front();
			this->pop();
			return true;
		}
		return false;
	}

	int pull(char *dest)
	{
		if(hasEntries())
		{
			Type elem = this->front();
			int len = elem.length();
			memcpy( dest, elem.c_str(), len );
			this->pop();
			return len;
		}
		return 0;
	}

	/**
	 * Removes all items from the queue.
	 */
	inline void clear()
	{
		this->clear();
	}

	/**
	 * Return: true if buffer is full
	 */
	inline bool isFull() const
	{
		return( this->size() == MaxElements );
	}

	/**
	 * Return: number of elements in buffer
	 */
	inline size_t numElements() const
	{
		return this->size();
	}

	/**
	 * Return: true if buffer is empty
	 */
	inline bool isEmpty() const
	{
		return( this->size() == 0 );
	}

	/**
	 * Return: true if there is at least one entry
	 */
	inline bool hasEntries() const
	{
		return( this->size() );
	}

};
