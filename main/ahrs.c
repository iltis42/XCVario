/*
 * ahrs.c
 *
 *  Created on: 22 mai 2021
 *      Author: GFM
 */
#include "ahrs.h"
#include "../components/MPUdriver/include/MPU.hpp"
#include <stdlib.h> /* for abs() */

#ifndef AHRS_ALIGNER_SAMPLES_NB
#define AHRS_ALIGNER_SAMPLES_NB 100
#endif

static struct Int32Rates gyro_sum;
static struct Int32Vect3 accel_sum;
static struct Int32Vect3 mag_sum;
static int32_t ref_sensor_samples[AHRS_ALIGNER_SAMPLES_NB];
static uint32_t samples_idx;


MPU_t MPU;         // create an object

void ahrs_aligner_init(void)
{

  ahrs_aligner.status = AHRS_ALIGNER_RUNNING;
  INT_RATES_ZERO(gyro_sum);
  INT_VECT3_ZERO(accel_sum);
  INT_VECT3_ZERO(mag_sum);
  samples_idx = 0;
  ahrs_aligner.noise = 0;
  ahrs_aligner.low_noise_cnt = 0;
}

