/*
 * S2F.cpp
 *
 *  Created on: Dec 26, 2018
 *      Author: iltis
 */

#include "S2F.h"
#include "math.h"
#include "Polars.h"
#include "logdef.h"
#include "sensor.h"
#include "Units.h"
#include "Blackboard.h"

S2F::S2F() {
    a0=a1=a2=0;
    w0=w1=w2=0;
    _MC = 0;
    _speedMinSink = 0;
    _circling_speed = 0;
	_circling_sink = 0;
	_minimumSink = 0;
	_stall_speed_ms = 0;
}

S2F::~S2F() {
}

float S2F::getN() {
	if( accelG[0] < 0.3 )        // Polars and airfoils physics behave different negative or even low g forces, we stop here impacting from g force at 0.3 g
		return 0.3;
	return accelG[0];
}

void S2F::begin(){
	if( empty_weight.get() == 0 )
		empty_weight.set( (polar_wingload.get() * polar_wingarea.get())- 80.0 );
	calculateOverweight();
	recalculatePolar();
	change_ballast();
}

void S2F::modifyPolar(){
	recalculatePolar();
	recalcSinkNSpeeds();
}

void S2F::recalculatePolar()
{
	ESP_LOGI(FNAME, "S2F::recalculatePolar() bugs: %f ", bugs.get());
	double v1 = polar_speed1.get() / 3.6;
	double v2 = polar_speed2.get() / 3.6;
	double v3 = polar_speed3.get() / 3.6;
	double w1 = polar_sink1.get();
	double w2 = polar_sink2.get();
	double w3 = polar_sink3.get();
	ESP_LOGI(FNAME, "v1/s1 %.1f/%.1f", v1 * 3.6, w1);
	ESP_LOGI(FNAME, "v2/s2 %.1f/%.1f", v2 * 3.6, w2);
	ESP_LOGI(FNAME, "v3/s3 %.1f/%.1f", v3 * 3.6, w3);
	// w= a0 + a1*v + a2*v^2   from ilec
	// w=  c +  b*v +  a*v^2   from wiki
	a2 = ((v2 - v3) * (w1 - w3) + (v3 - v1) * (w2 - w3)) / (pow(v1, 2) * (v2 - v3) + pow(v2, 2) * (v3 - v1) + pow(v3, 2) * (v1 - v2));
	a1 = (w2 - w3 - a2 * (pow(v2, 2) - pow(v3, 2))) / (v2 - v3);
	a0 = w3 - a2 * pow(v3, 2) - a1 * v3;
	const double loading_factor = sqrt((myballast + 100.0) / 100.0);
	a0 = a0 * loading_factor;
	a2 = a2 / loading_factor; // wingload  e.g. 100l @ 500 kg = 1.2 and G-Force
	a0 = a0 * ((bugs.get() + 100.0) / 100.0);
	a1 = a1 * ((bugs.get() + 100.0) / 100.0);
	a2 = a2 * ((bugs.get() + 100.0) / 100.0);
	ESP_LOGI(FNAME, "bugs:%d balo:%.1f%% a0=%f a1=%f  a2=%f s(80)=%f, s(160)=%f", (int)bugs.get(), myballast, a0, a1, a2, sink(80), sink(160));
	_stall_speed_ms = stall_speed.get() / 3.6;
}

void S2F::setPolar()
{
	int n = glider_type.get();
	ESP_LOGI(FNAME,"S2F::setPolar()");
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
	polar_wingarea.set( p.wingarea, true, false );
	empty_weight.set( (p.wingload * p.wingarea) - 80.0, true, false ); // Calculate default for emtpy mass
	ESP_LOGI(FNAME,"Referelce weight:%.1f, new empty_weight: %.1f", (p.wingload * p.wingarea), empty_weight.get() );
	modifyPolar();
}

float S2F::bal_percent = 0;

float S2F::getBallastPercent(){ return bal_percent; }

void S2F::calculateOverweight()
{
	ESP_LOGI(FNAME,"S2F::calculateOverweight()" );
	gross_weight.set( empty_weight.get() + crew_weight.get() + ballast_kg.get() );
	myballast = ( ((100*gross_weight.get()) / (polar_wingload.get()*polar_wingarea.get())) - 100.0 );
	ESP_LOGI(FNAME,"New ballast overweight: %.2f %%", myballast );
	ballast.set( myballast );
}

void S2F::change_ballast()
{
	ESP_LOGI(FNAME,"S2F::change_ballast()" );
	float refw = polar_wingload.get() * polar_wingarea.get();
	ESP_LOGI(FNAME,"Reference weight: %.1f kg", refw);
	ESP_LOGI(FNAME,"Empty weight    : %.1f kg", empty_weight.get());
	ESP_LOGI(FNAME,"Crew weight     : %.1f kg", crew_weight.get());
	ESP_LOGI(FNAME,"Water Ballast   : %.1f kg", ballast_kg.get());
	ESP_LOGI(FNAME,"Gross weight    : %.1f kg", gross_weight.get());
	float max_bal = polar_max_ballast.get();
	if( (int)(polar_max_ballast.get()) == 0 ) { // We use 100 liters as default once its not with the polar
		max_bal = 100;
	}
	ESP_LOGI(FNAME,"Max ballast %.1f", max_bal );
	calculateOverweight();
	recalculatePolar();
	recalcSinkNSpeeds();
}

void S2F::change_mc()
{
	_MC = MC.get();
	ESP_LOGI(FNAME,"S2F::change_mc_bal(), MC: %.1f", _MC );
	recalcSinkNSpeeds();
}

float S2F::getVn( float v ){
	float Vn = v*pow(getN(),0.5);
	if( Vn > _stall_speed_ms )
		return Vn;
	else
		return _stall_speed_ms;
}

double S2F::sink( double v_in ) {
	double v = v_in;
	double v_stall = Units::Airspeed2Kmh( stall_speed.get() * 0.9 );
	if ( v_in < v_stall ){
		// ESP_LOGI(FNAME,"S2F::sink, warning, airspeed %.1f below minimum speed", v_in );
		return 0.0;
	}
	v = v/3.6; // airspeed in meters per second
	double n=getN();
	double sqn = sqrt(n);
	double s = a0*pow(sqn,3) + a1*v*n + a2*pow(v,2)*sqn;
	// ESP_LOGI(FNAME,"S2F::sink() V:%0.1f sink:%2.2f G-Load:%1.2f", v_in, s, n );
	return s;
}

float S2F::cw( float v ){  // in m/s
	float cw = 0;
	if( v > 14.0 ) {
		double cur_sink = sink(v*3.6);
		// ESP_LOGI(FNAME,"S2F::cw( %0.1f ) sink: %2.1f cw. %2.2f  G: %1.1f", v, sink, cw, getN() );
		cw = cur_sink / v;
	}
	return cw;
}


double S2F::speed( double netto_vario, bool circling )
{
   float n = accelG[0];
   if( n < 0.3 )
		n = 0.3;
   double stf = 0;
   if( circling ){  // Optimum speed for a load factor of 1.4 g what corresponds 45° angle of bank and factor 1.2 speed increase; 3.6*1.2 = 4.32
	   stf = _circling_speed;
   }else{
	   if( s2f_blockspeed.get() )
		   stf = 3.6*sqrt( ((a0-_MC)) / a2 );  // no netto vario, no G impact
	   else
		   stf = 3.6*sqrt( ((a0-_MC+netto_vario)*getN()) / a2 );
   }
   // ESP_LOGI(FNAME,"speed()  %f", stf );
   if( (stf < _speedMinSink) or isnan(stf) )
	   return _speedMinSink;
   if( stf > Units::Airspeed2Kmh(v_max.get()) or isinf( stf) )
	   return Units::Airspeed2Kmh(v_max.get());
   else
	   return stf;
}

// minimum sink, circling sink, best circling speed
void S2F::recalcSinkNSpeeds()
{
   // 2*a2*v + a1 = 0
   _speedMinSink = (3.6*-a1)/(2*a2);
   if ( _speedMinSink < Units::Airspeed2Kmh( stall_speed.get() ) )
		_speedMinSink = Units::Airspeed2Kmh( stall_speed.get() );
   _minimumSink = sink( _speedMinSink );
	_circling_speed = 1.2*_speedMinSink;
	_circling_sink = sink( _circling_speed );
   	ESP_LOGI(FNAME,"Airspeed @ min Sink =%3.1f kmh", _speedMinSink );
   	ESP_LOGI(FNAME,"          min Sink  =%2.3f m/s", _minimumSink );
   	ESP_LOGI(FNAME,"Circling Speed      =%3.1f kmh", _circling_speed );
	ESP_LOGI(FNAME,"Circling Sink       =%2.3f",     _circling_sink );
}

void S2F::test( void )
{
	ESP_LOGI(FNAME, "Minimal Sink @ %f km/h", minsink());
	ESP_LOGI(FNAME, "Sink %f @ %s km/h ", sink( 0.0 ), "0");
	ESP_LOGI(FNAME, "Sink %f @ %s km/h ", sink( 20.0 ), "20");
	ESP_LOGI(FNAME, "Sink %f @ %s km/h ", sink( 40.0 ), "40");
	ESP_LOGI(FNAME, "Sink %f @ %s km/h ", sink( 80.0 ), "80");
	ESP_LOGI(FNAME, "Sink %f @ %s km/h ", sink( 100.0 ), "100");
	ESP_LOGI(FNAME, "Sink %f @ %s km/h ", sink( 120.0 ), "120");
	ESP_LOGI(FNAME, "Sink %f @ %s km/h ", sink( 150.0 ), "150");
	ESP_LOGI(FNAME, "Sink %f @ %s km/h ", sink( 180.0 ), "180");
	ESP_LOGI(FNAME, "Sink %f @ %s km/h ", sink( 220.0 ), "220");
    ESP_LOGI(FNAME,"MC %f  Ballast %f", _MC, myballast );
	for( int st=20; st >= -20; st-=5 )
	{
		ESP_LOGI(FNAME, "S2F %g km/h vario %g m/s", speed( (double)st/10 ), (double)st/10 );
	}
}
