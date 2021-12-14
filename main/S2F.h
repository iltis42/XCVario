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
	void change_polar();
	void select_polar( int num );
	void change_mc_bal();
	double speed( double st, bool circling=false );
	double sink( double v );
	inline double minsink() { return _speedMinSink; };
	void recalc();
	static float getBallastPercent();
	inline double circlingSink(double v) {
		if( v > Units::Airspeed2Kmh( stall_speed.get())*0.6 )
			return _circling_sink;
		else
			return 0;
	};
	float cw( float v );
	void test( void );
	float getN();
	float getVn( float v );

private:
	static float bal_percent;
	double a0,a1,a2;
	double w0,w1,w2;
	double _MC;
	double _minimumSink;
	double _speedMinSink;
	double _circling_speed;
	double _circling_sink;
	double _stall_speed_ms;
};

#endif /* MAIN_S2F_H_ */
