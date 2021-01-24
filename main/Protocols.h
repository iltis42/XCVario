/*
 * Protocols.h
 *
 *  Created on: Dec 20, 2017
 *      Author: iltis
 */
#include <string>
#include "Setup.h"
#include "S2F.h"

#ifndef MAIN_OPENVARIO_H_
#define MAIN_OPENVARIO_H_

// Supported Protocols
typedef enum protocol_t  { P_OPENVARIO, P_BORGELT, P_CAMBRIDGE, P_EYE_PEYA, P_EYE_PEYI, P_AHRS_RPYL, P_AHRS_APENV1, P_GENERIC, P_XCVARIO } proto_t;


class Protocols {
public:
	Protocols( S2F * as2f );
	virtual ~Protocols( );
	void sendMcChange( float mc ) {};
	void sendWkChange( float wk );
	void sendMeanClimb( float climb );
	void sendBallastChange( float ballast );
	void sendBugsChange( float bugs );
	void sendQNHChange( float qnh );
	void sendNMEA( proto_t proto, char* str, float baro, float dp, float te, float temp, float ias, float tas,
			       float mc, int bugs, float ballast, bool cruise, float alt,
				   bool validTemp=false, float ax=0, float ay=0, float az=0, float gx=0, float gy=0, float gz=0 );

	static void parseNMEA( char *str );
	static int getCheckSum(char * s);
private:
	static S2F *   _s2f;
};

#endif /* MAIN_OPENVARIO_H_ */
