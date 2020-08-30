/*
 * S2F.cpp
 *
 *  Created on: Dec 26, 2018
 *      Author: iltis
 */

#include "S2F.h"
#include "math.h"
#include "Polars.h"
#include <logdef.h>


S2F::S2F() {
    a0=a1=a2=0;
    w0=w1=w2=0;
    _MC = 0;
    _minsink = 0;
}

S2F::~S2F() {
}

void S2F::change_polar()
{
	ESP_LOGI(FNAME,"S2F::change_polar() bugs: %f ", bugs.get() );
    double v1=polar_speed1.get() / 3.6;
    double v2=polar_speed2.get() / 3.6;
    double v3=polar_speed3.get() / 3.6;
    double w1=polar_sink1.get();
    double w2=polar_sink2.get();
    double w3=polar_sink3.get();
    // w= a0 + a1*v + a2*v^2   from ilec
    // w=  c +  b*v +  a*v^2   from wiki
	a2= ((v2-v3)*(w1-w3)+(v3-v1)*(w2-w3)) / (pow(v1,2)*(v2-v3)+pow(v2,2)*(v3-v1)+ pow(v3,2)*(v1-v2));
	a1= (w2-w3-a2*(pow(v2,2)-pow(v3,2))) / (v2-v3);
	a0= w3 -a2*pow(v3,2) - a1*v3;
    a2 = a2/sqrt( ( ballast.get() +100.0)/100.0 );   // wingload  e.g. 100l @ 500 kg = 1.2
    a0 = a0 * ((bugs.get() + 100.0) / 100.0);
    a1 = a1 * ((bugs.get() + 100.0) / 100.0);
    a2 = a2 * ((bugs.get() + 100.0) / 100.0);
    ESP_LOGI(FNAME,"a0=%f a1=%f  a2=%f s(80)=%f, s(160)=%f", a0, a1, a2, sink(80), sink(160) );
    minsink();
}

void S2F::select_polar()
{
	ESP_LOGI(FNAME,"S2F::select_polar()");
	int n = glider_type.get();
	ESP_LOGI(FNAME,"Selected Polar N %d", n );
	t_polar p = Polars::getPolar(n);
	polar_speed1.set( p.speed1 );
	polar_speed2.set( p.speed2 );
	polar_speed3.set( p.speed3 );
	polar_sink1.set( p.sink1 );
	polar_sink2.set( p.sink2 );
	polar_sink3.set( p.sink3 );
	polar_wingload.set( p.wingload );
	polar_max_ballast.set( p.max_ballast );
	polar_wingarea.set( p.wingarea );
	ESP_LOGI(FNAME,"now change polar");
    change_polar();
    // ESP_LOGI(FNAME,"now test");
    // test();
}

void S2F::change_mc_bal()
{
	ESP_LOGI(FNAME,"S2F::change_mc_bal()");
	_MC = MC.get();
	change_polar();
	// test();
}

double S2F::sink( double v_in ) {
	double s=0;
	if ( v_in > 50 ){
		double v=v_in/3.6;
		s = a0+a1*v+a2*pow(v,2);
	}
	return s;
}

double S2F::speed( double st )
{
   double stf = 3.6*sqrt( (a0-_MC+st) / a2 );
   // ESP_LOGI(FNAME,"speed()  %f", stf );
   if( (stf < _minsink) or isnan(stf) )
	   return _minsink;
   if( stf > 450.0 or isinf( stf) )
	   return 450.0;
   else
	   return stf;
}

double S2F::minsink()
{
   // 2*a2*v + a1 = 0
   _minsink = (3.6*-a1)/(2*a2);
   ESP_LOGI(FNAME,"Airspeed @ minsink=%f", _minsink );
   return _minsink;
}


void S2F::test( void )
{
	ESP_LOGI(FNAME, "Minimal Sink %f km/h", minsink());
	ESP_LOGI(FNAME, "Sink %f @ %s km/h ", sink( 0.0 ), "0");
	ESP_LOGI(FNAME, "Sink %f @ %s km/h ", sink( 20.0 ), "20");
	ESP_LOGI(FNAME, "Sink %f @ %s km/h ", sink( 40.0 ), "40");
	ESP_LOGI(FNAME, "Sink %f @ %s km/h ", sink( 80.0 ), "80");
	ESP_LOGI(FNAME, "Sink %f @ %s km/h ", sink( 100.0 ), "100");
	ESP_LOGI(FNAME, "Sink %f @ %s km/h ", sink( 120.0 ), "120");
	ESP_LOGI(FNAME, "Sink %f @ %s km/h ", sink( 150.0 ), "150");
	ESP_LOGI(FNAME, "Sink %f @ %s km/h ", sink( 180.0 ), "180");
	ESP_LOGI(FNAME, "Sink %f @ %s km/h ", sink( 220.0 ), "220");
    ESP_LOGI(FNAME,"MC %f  Ballast %f", _MC, ballast.get() );
	for( int st=20; st >= -20; st-=5 )
	{
		ESP_LOGI(FNAME, "S2F %g km/h vario %g m/s", speed( (double)st/10 ), (double)st/10 );
	}
}

