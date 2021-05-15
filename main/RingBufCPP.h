#ifndef EM_RINGBUF_CPP_H
#define EM_RINGBUF_CPP_H

#include "RingBufHelpers.h"
#include <cstring>
#include "logdef.h"


#define SSTRLEN 250

class SString {
public:
	inline SString() { clear();
	}
	inline SString( SString& os) {
		len = SSTRLEN-1;
		if( os.length() < len)
			len = os.length();
		memcpy(str,os.c_str(),len);
		str[len] = 0;
	}
	inline SString( const char * s ) {
		len = SSTRLEN-1;
		if( strlen(s) < len)
			len = strlen(s);
		memcpy(str,s,len);
		str[len] = 0;
	}
	inline void add( char c ) {
		if( len < SSTRLEN-1 )
			str[len++] = c;
	}
	inline void add( char *s ) {   // add pure strings
		len = SSTRLEN-1;
		if( strlen(s) < len)
			len = strlen(s);
		memcpy(str,s,len);
		str[len] = 0;
	}
	inline void addl( char *s, int alen ) {
			len = SSTRLEN-1;
			if( alen < len)
				len = alen;
			memcpy(str,s,len);
			str[len] = 0;
	}
	inline void append( char *s, int alen ) {
		if( alen+len < SSTRLEN-1 ){
			memcpy(str+len,s,alen);
			str[alen+len] = 0;
			len += alen;
		}
	}
	inline void clear() {
		memset(str,0,SSTRLEN);
		len = 0;
	}
	inline void setLen( int alen ) {
		len = alen;
	}
	inline char *c_str() { return str; };
	inline size_t length() { return len; }
private:
	char str[SSTRLEN];
	int  len;
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
inline bool add(const Type &obj, bool overwrite=false)
{
    bool full = false;
    RB_ATOMIC_START
    {
        full = isFull();
        if (!full || overwrite) {
            _buf[_head] = obj;
            _head = (_head + 1)%MaxElements;
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
inline bool pull(Type *dest)
{
    bool ret = false;
    RB_ATOMIC_START
    {
        if (!isEmpty()) {
            *dest = _buf[_tail];
            _numElements--;
            _tail = (_tail +1)%MaxElements;
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
inline Type* peek(size_t num)
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
inline bool isFull() const
{
    return( _numElements >= MaxElements );
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
inline size_t getTail() const
{
    return _tail;
}


// underlying array
Type _buf[MaxElements];

size_t _head;
size_t _tail;
size_t _numElements;
private:

};

#endif
