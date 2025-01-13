#pragma once

#include "logdef.h"
#include <cstring>

#define SSTRLEN 256  // max size of NMEA, 80 bytes are no more hardlimit in modern receivers such as Beitian BN-80, so extend

// wrapper class for a size limited string, pre-allocation to be done resize() method

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
    SString(const char *s, int alen)
    {
        str = std::string( s, alen );
    }
    inline void set(const char *s)
    {
        // add pure strings
    	str = std::string( s );
    }
    inline void set(const char *s, int alen)
    {
      str = std::string( s, alen );
    }
    inline SString& operator=(const SString& right)
    {
       str = right.str;
       return *this;
    }
    inline void append(char c)
    {
    	str += c;
    }
    inline void append(const char *s, int alen)
    {
       str.append( s, alen );
    }
    inline SString& operator+=(const SString& right)
    {
      str += right.str;
      return *this;
    }
    inline void clear()
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
