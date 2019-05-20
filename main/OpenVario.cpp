/*
 * OpenVario.cpp
 *
 *  Created on: Dec 20, 2017
 *      Author: iltis
 */

#include "OpenVario.h"
#include <stdio.h>
#include <string.h>
#include "sensor.h"

OpenVario::OpenVario() {

}

OpenVario::~OpenVario() {

}


void OpenVario::makeNMEA(char* str, float baro, float dp, float te, float temp ){
#ifdef VARIO
		sprintf(str,"$POV,E,%0.1f",te);
#else
		sprintf(str,"$POV,P,%0.1f,Q,%0.1f,E,%0.1f,T,%0.1f",baro,dp,te,temp);
#endif
	int cs = getCheckSum(&str[1]);
	int i = strlen(str);
	sprintf( &str[i], "*%02X\n", cs );
}

// Checksum berechnen und als int ausgeben
// wird als HEX benötigt im NMEA Datensatz
// zwischen $ oder ! und * rechnen
int OpenVario::getCheckSum(char * s) {
  int i, XOR, c;
  for (XOR = 0, i = 0; i < strlen(s); i++) {
    c = (unsigned char)s[i];
    if (c == '*') break;
    if ((c != '$') && (c != '!')) XOR ^= c;
  }
  return XOR;
}
