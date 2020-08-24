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
	S2F( );
	virtual ~S2F();
	void change_polar();
	void select_polar();
	void change_mc_bal();
	double speed( double st );
	double sink( double v );
	double minsink();
	void test( void );
private:
	double a0,a1,a2;
	double w0,w1,w2;
	double _MC;
	double _minsink;
};

#endif /* MAIN_S2F_H_ */
