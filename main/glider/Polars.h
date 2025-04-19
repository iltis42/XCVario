/*
 * Polars.h
 *
 *  Created on: Mar 1, 2019
 *      Author: iltis
 */

#ifndef MAIN_POLARS_H_
#define MAIN_POLARS_H_

typedef struct s_polars {
	int 	 index;
	const char	 *type;
	float    wingload;
	float    speed1;
	float    sink1;
	float    speed2;
	float    sink2;
	float    speed3;
	float    sink3;
	float    max_ballast;   // in liters or kg
	float    wingarea;
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
