#ifndef AVERAGE_VARIO_H
#define AVERAGE_VARIO_H

class AverageVario{
	AverageVario() {};
	~AverageVario() {};
public:
	static void newSample( float te );
	static void begin();
	static void recalcAvgClimb();
	static inline float readAvgClimb() { return averageClimb; };

private:
	static float averageClimbSec;
	static float averageClimb;
	static float avClimb100MSec[10];
	static float avClimbSec[60];
	static float avClimbMin[300];
	static int avindex100MSec;
	static int avindexSec;
	static int avindexMin;
	static int samples;
};


#endif
