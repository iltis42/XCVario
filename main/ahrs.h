/*
 * ahrs.h
 *
 *  Created on: 22 mai 2021
 *      Author: GFM
 */

#ifndef MAIN_AHRS_H_
#define MAIN_AHRS_H_

#include "../components/ahrs/include/std.h"
#include "../components/ahrs/include/pprz_algebra_int.h"

#define AHRS_ALIGNER_UNINIT  0
#define AHRS_ALIGNER_RUNNING 1
#define AHRS_ALIGNER_LOCKED  2

struct AhrsAligner {
  struct Int32Rates lp_gyro;
  struct Int32Vect3 lp_accel;
  struct Int32Vect3 lp_mag;
  int32_t           noise;
  int32_t           low_noise_cnt;
  uint8_t           status;
};

struct AhrsAligner ahrs_aligner;


void ahrs_aligner_init(void);
void ahrs_aligner_run(void);




#endif /* MAIN_AHRS_H_ */
