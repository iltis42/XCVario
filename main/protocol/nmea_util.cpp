/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "nmea_util.h"

#include <iostream>
#include <sstream>
#include <iomanip>
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
// pos - points to the position of the current word delimiter
// returns:
// srting - the extracted word
// pos - points to the next commy seperator
std::string extractWord(const std::string &str, int &pos)
{
    int len = str.size();
    const char *cptr = str.c_str() + pos;
    while ( pos < len ) {
        if ( *cptr != ' ' && *cptr != ',' ) {
            break;
        }
        pos++;
        cptr++;
    }
    int wstart = pos;
    while ( pos < len ) {
        if ( *cptr == ',' || *cptr == ' '  || *cptr == '*'  || *cptr == '\r'  || *cptr == '\n' ) {
            break;
        }
        pos++;
        cptr++;
    }
    return str.substr(wstart, pos-wstart);
}

void incrCRC(int &crc, const char c)
{
    crc ^= c;
}

void ensureTermination(std::string& str)
{
    // Check if the string already ends with "\r\n"
    size_t len = str.length();
    if (len >= 2 && str[len - 2] == '\r' && str[len - 1] == '\n') {
        // String is already properly terminated
        return;
    }

    // If the string ends with '\r', append '\n'
    if (len >= 1 && str[len - 1] == '\r') {
        str += '\n';
    } 
    // If the string ends with '\n', replace it with "\r\n"
    else if (len >= 1 && str[len - 1] == '\n') {
        str[len - 1] = '\r';
        str += '\n';
    } 
    // Otherwise, append "\r\n" to the string
    else {
        str += "\r\n";
    }
}


std::string hexDump(const char *buffer, int len)
{
    std::ostringstream oss;
    for (const char *c=buffer; len>0; len--, c++) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(*c);
    }
    return oss.str();
}


} // namespace