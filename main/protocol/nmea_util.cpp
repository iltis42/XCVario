/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "nmea_util.h"

#include <cstdlib>

namespace NMEA {

// Calculate the checksum and output it as an int
// is required as HEX in the NMEA data set
// between $ or ! and * character
std::string CheckSum(const char *nmea)
{
    char crc = 0;
    for (const char *cptr = nmea; *cptr != '\0'; cptr++)
    {
        char c = *cptr;
        if (c == '*') {
            break;
        }
        if ((c != '$') && (c != '!')) {
            crc ^= c;
        }
    }

    char buf[4];
    sprintf(buf, "%02x", crc);
    return std::string(buf);
}


// Extract the next word
// str - the frame buffer
// pos - points to the position after the last word delimiter
std::string extractWord(const std::string &str, int pos) {
    int i = pos;
    int len = str.size();
    const char *cptr = str.c_str() + pos;
    while ( i < len ) {
        if ( *cptr != ' ' ) {
            break;
        }
        i++;
        cptr++;
    }
    int wstart = i;
    while ( i < len ) {
        if ( *cptr == ',' || *cptr == ' '  || *cptr == '*'  || *cptr == '\r'  || *cptr == '\n' ) {
            break;
        }
        i++;
        cptr++;
    }
    return str.substr(wstart, i-wstart);
}

void incrCRC(int &crc, const char c)
{
    crc ^= c;
}

} // namespace