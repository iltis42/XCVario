/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include <string>

namespace NMEA {

// Calculate a nmea checksum
std::string CheckSum(const char *nmea);
// Extract comma seperated NMEA word/value
std::string extractWord(const std::string &str, int pos);
void incrCRC(int &crc, const char c);
void ensureTermination(std::string& str);
std::string hexDump(const char *buffer, int len);

}