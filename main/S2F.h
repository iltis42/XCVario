/*
 * S2F.h
 *
 *  Created on: Dec 26, 2018
 *      Author: iltis
 */

#ifndef MAIN_S2F_H_
#define MAIN_S2F_H_

#include "Setup.h"
#include "Units.h"

class S2F {
public:
	S2F( );
	virtual ~S2F();
	void begin();
	void calculateOverweight();
	void modifyPolar();
	bool IsValid() const;
	void recalculatePolar();
	void change_ballast();
	void change_mc();
	void setPolar();
	double speed( double st, bool circling=false );
	double sink( double v );
	inline double minsink() { return _min_speed; };
	void recalcSinkNSpeeds();
	static float getBallastPercent();
	inline double circlingSink(double v) {
		if( v > stall_speed.get()*0.6 )
			return _circling_sink;
		else
			return 0;
	};
	float cw( float v );
	void test( void );
	float getN();
	float getVn( float v );

private:
	double myballast;
	static float bal_percent;
	double a0,a1,a2;
	double w0,w1,w2;
	double _min_speed;
	double _min_sink;
	double _circling_speed;
	double _circling_sink;
	double _stall_speed_ms;
};

#endif /* MAIN_S2F_H_ */
