/*
 * S2F.h
 *
 *  Created on: Dec 26, 2018
 *      Author: iltis
 */

#ifndef MAIN_S2F_H_
#define MAIN_S2F_H_
#include "Setup.h"

class S2F {
public:
	S2F( Setup* setup );
	virtual ~S2F();
	void change_polar();
	void change_mc_bal();
	float speed( float st );
	float sink( float v );
	float minsink();
	void test( void );
private:
	setup_t *s;
	Setup * _setup;
	double a0,a1,a2;
	double w0,w1,w2;
	float MC;
	float _minsink;
};

#endif /* MAIN_S2F_H_ */
