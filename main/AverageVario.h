#ifndef AVERAGE_VARIO_H
#define AVERAGE_VARIO_H

#include <list>
#include <vector>
#include <array>

class AverageVario{
	AverageVario() {};
	~AverageVario() {};
public:
	static void newSample( float te );
	static void begin();
	static void recalcAvgClimb();
	static float readAvgClimb();

private:
	static float averageClimbSec;
	static float averageClimb;
	static std::array<float, 10> avClimb100MSec;
	static std::array<float, 60> avClimbSec;
	static std::list<float> avClimbMin;
	static int avindex100MSec;
	static int avindexSec;
	static int samples;
};


#endif
