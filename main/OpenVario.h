/*
 * OpenVario.h
 *
 *  Created on: Dec 20, 2017
 *      Author: iltis
 */
#include <string>

#ifndef MAIN_OPENVARIO_H_
#define MAIN_OPENVARIO_H_

class OpenVario {
public:
	OpenVario();
	virtual ~OpenVario();
	void makeNMEA(char *str, float baro, float dp, float te, float temp );

private:
	int getCheckSum(char * s);
};

#endif /* MAIN_OPENVARIO_H_ */
