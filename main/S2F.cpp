/*
 * S2F.cpp
 *
 *  Created on: Dec 26, 2018
 *      Author: iltis
 */

#include "S2F.h"
#include "math.h"

S2F::S2F( SetupCMD* setup ) {
    _setup = setup;
    a0=a1=a2=0;
    w0=w1=w2=0;
    s = 0;
    MC = 0;
}

S2F::~S2F() {
}

void S2F::change_polar()
{
	s = _setup->get();
    float v1=s->_v1 / 3.6;
    float v2=s->_v2 / 3.6;
    float v3=s->_v3 / 3.6;
    float w1=s->_w1;
    float w2=s->_w2;
    float w3=s->_w3;
    // w= a0 + a1*v + a2*v^2   from ilec
    // w=  c +  b*v +  a*v^2   from wiki
	a2= ((v2-v3)*(w1-w3)+(v3-v1)*(w2-w3)) / (pow(v1,2)*(v2-v3)+pow(v2,2)*(v3-v1)+ pow(v3,2)*(v1-v2));
	a1= (w2-w3-a2*(pow(v2,2)-pow(v3,2))) / (v2-v3);
	a0= w3 -a2*pow(v3,2) - a1*v3;
    a2 = a2/sqrt( (s->_ballast+100.0)/100.0 );   // wingload  e.g. 100l @ 500 kg = 1.2
    minsink();
}

void S2F::change_mc_bal()
{
	MC = s->_MC;
	change_polar();
	test();
}

float S2F::sink( float v_in ) {
	float v=v_in/3.6;
	float s = a0+a1*v+a2*pow(v,2);
	return s;
}

float S2F::speed( float st )
{
   float stf = 3.6*sqrt( (a0-MC+st) / a2 );
   if( (stf < _minsink) or isnan(stf) )
	   return _minsink;
   if( stf > 450.0 or isinf( stf) )
	   return 450.0;
   else
	   return stf;
}

float S2F::minsink()
{
   // 2*a2*v + a1 = 0
   _minsink = (3.6*-a1)/(2*a2);
   return _minsink;
}


void S2F::test( void )
{
	printf( "Minimal Sink %f km/h\n", minsink());
	printf( "Sink %f @ %s km/h \n", sink( 80.0 ), "80");
	printf( "Sink %f @ %s km/h \n", sink( 100.0 ), "100");
	printf( "Sink %f @ %s km/h \n", sink( 120.0 ), "120");
	printf( "Sink %f @ %s km/h \n", sink( 150.0 ), "150");
	printf( "Sink %f @ %s km/h \n", sink( 180.0 ), "180");
	printf( "Sink %f @ %s km/h \n", sink( 220.0 ), "220");
    printf("MC %f  Ballast %f\n", MC, s->_ballast );
	for( int st=30; st > -30; st-=5 )
	{
		printf( "S2F %g km/h vario %g m/s\n", speed( (float)st/12 ), (float)st/10 );
	}
}

