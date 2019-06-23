/*
 * S2F.cpp
 *
 *  Created on: Dec 26, 2018
 *      Author: iltis
 */

#include "S2F.h"
#include "math.h"
#include "Polars.h"


S2F::S2F( Setup* setup ) {
    _setup = setup;
    a0=a1=a2=0;
    w0=w1=w2=0;
    MC = 0;
    _minsink = 0;
}

S2F::~S2F() {
}

void S2F::change_polar()
{
	printf("S2F::change_polar()\n");
	t_polar p = _setup->get()->_polar;
    double v1=p.speed1 / 3.6;
    double v2=p.speed2 / 3.6;
    double v3=p.speed3 / 3.6;
    double w1=p.sink1;
    double w2=p.sink2;
    double w3=p.sink3;
    // w= a0 + a1*v + a2*v^2   from ilec
    // w=  c +  b*v +  a*v^2   from wiki
	a2= ((v2-v3)*(w1-w3)+(v3-v1)*(w2-w3)) / (pow(v1,2)*(v2-v3)+pow(v2,2)*(v3-v1)+ pow(v3,2)*(v1-v2));
	a1= (w2-w3-a2*(pow(v2,2)-pow(v3,2))) / (v2-v3);
	a0= w3 -a2*pow(v3,2) - a1*v3;
    a2 = a2/sqrt( ( _setup->get()->_ballast +100.0)/100.0 );   // wingload  e.g. 100l @ 500 kg = 1.2
    minsink();
}

void S2F::select_polar()
{
	printf("S2F::select_polar()\n");
	int n = _setup->get()->_glider_type;
	printf("Selected Polar N %d\n", n );
	_setup->get()->_polar = Polars::getPolar(n);
	printf("now change\n");
    change_polar();
    printf("now test\n");
    test();
}

void S2F::change_mc_bal()
{
	printf("S2F::change_mc_bal()\n");
	MC = _setup->get()->_MC;
	change_polar();
	test();
}

double S2F::sink( double v_in ) {
	double v=v_in/3.6;
	double s = a0+a1*v+a2*pow(v,2);
	return s;
}

double S2F::speed( double st )
{
   double stf = 3.6*sqrt( (a0-MC+st) / a2 );
   // printf("speed()  %f\n", stf );
   if( (stf < _minsink) or isnan(stf) )
	   return _minsink;
   if( stf > 450.0 or isinf( stf) )
	   return 450.0;
   else
	   return stf;
}

double S2F::minsink()
{
	printf("minsink()\n");
   // 2*a2*v + a1 = 0
	printf("a1=%f  a2=%f \n", a1, a2);
   _minsink = (3.6*-a1)/(2*a2);
   return _minsink;
}


void S2F::test( void )
{
	printf( "Minimal Sink %f km/h\n", minsink());
	printf( "Sink %f @ %s km/h \n", sink( 0.0 ), "0");
	printf( "Sink %f @ %s km/h \n", sink( 20.0 ), "20");
	printf( "Sink %f @ %s km/h \n", sink( 40.0 ), "40");
	printf( "Sink %f @ %s km/h \n", sink( 80.0 ), "80");
	printf( "Sink %f @ %s km/h \n", sink( 100.0 ), "100");
	printf( "Sink %f @ %s km/h \n", sink( 120.0 ), "120");
	printf( "Sink %f @ %s km/h \n", sink( 150.0 ), "150");
	printf( "Sink %f @ %s km/h \n", sink( 180.0 ), "180");
	printf( "Sink %f @ %s km/h \n", sink( 220.0 ), "220");
    printf("MC %f  Ballast %f\n", MC, _setup->get()->_ballast );
	for( int st=20; st >= -20; st-=5 )
	{
		printf( "S2F %g km/h vario %g m/s\n", speed( (double)st/10 ), (double)st/10 );
	}
}

