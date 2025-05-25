# Compressed glider polars

# run-time floting point representation:
	float    wingload;		// kg/mxm
	float    speed1;		// km/h
	float    sink1;			// m/s
	float    speed2;		// km/h
	float    sink2;			// m/s
	float    speed3;		// km/h
	float    sink3;			// m/s
	float    max_ballast;	// in liters or kg
	float    wingarea;		// mxm

# flash store fix point representation:
   	uint16_t wingload;		// 1/100x kg/mxm
	uint8_t  speed1;		// km/h
	int16_t  sink1;			// 1/100x m/s
	uint8_t  speed2;		// km/h
	int16_t  sink2;			// 1/100x m/s
	uint8_t  speed3;		// km/h
	int16_t  sink3;			// 1/100x m/s
	uint8_t  max_ballast;	// 2x kg
	uint16_t wingarea;		// 1/100x mxm

# Rational
Accepted and negligible reduction is the linear reposition of polar points to the nearest integer km/h speed. As well as the reduction of the max. water ballast down to a 2 liters resolution.

The python compressor script has the option to print fix point errors, that should all be zero.
