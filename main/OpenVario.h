/*
 * OpenVario.h
 *
 *  Created on: Dec 20, 2017
 *      Author: iltis
 */
#include <string>
#include "Setup.h"
#include "S2F.h"

#ifndef MAIN_OPENVARIO_H_
#define MAIN_OPENVARIO_H_

class OpenVario {
public:
	OpenVario( Setup * setup, S2F * s2f );
	virtual ~OpenVario( );
	void makeNMEA( char* str, float baro, float dp, float te, float temp, float ias, float tas, float mc, int bugs, float ballast, bool cruise );
	static void parseNMEA( char *str );
	static int getCheckSum(char * s);
private:
	static Setup * _setup;
	static S2F *   _s2f;
};

#endif /* MAIN_OPENVARIO_H_ */
