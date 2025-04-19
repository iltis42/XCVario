/*
 * Polars.h
 *
 *  Created on: Mar 1, 2019
 *      Author: iltis
 */

#ifndef MAIN_POLARS_H_
#define MAIN_POLARS_H_

typedef struct s_polars {
	int      index;
	const char *type;
	float    wingload;		// kg/mxm
	float    speed1;		// km/h
	float    sink1;			// m/s
	float    speed2;		// km/h
	float    sink2;			// m/s
	float    speed3;		// km/h
	float    sink3;			// m/s
	float    max_ballast;	// in liters or kg
	float    wingarea;		// mxm
} t_polar;


class Polars {
public:
	Polars();
	static const t_polar getPolar( int x );
	static int numPolars();
	static void begin();
	virtual ~Polars();
};



#endif /* MAIN_POLARS_H_ */
