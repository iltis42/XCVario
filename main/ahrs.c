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

void ahrs_aligner_run(void)
{
		mpud::raw_axes_t accelRaw;     // holds x, y, z axes as int16
		mpud::raw_axes_t gyroRaw;      // holds x, y, z axes as int16
		esp_err_t err = MPU.acceleration(&accelRaw);  // fetch raw data from the registers
		esp_err_t err = MPU.gyro(&gyroRaw);  // fetch raw data from the registers

  RATES_ADD(gyro_sum, gyroRaw);
  VECT3_ADD(accel_sum, accelRaw);
/*  VECT3_ADD(mag_sum,   imu.mag);*/

  ref_sensor_samples[samples_idx] = accelRaw.z;
  samples_idx++;

#ifdef AHRS_ALIGNER_LED
  RunOnceEvery(50, {LED_TOGGLE(AHRS_ALIGNER_LED);});
#endif

  if (samples_idx >= AHRS_ALIGNER_SAMPLES_NB) {
    int32_t avg_ref_sensor = accel_sum.z;
    if (avg_ref_sensor >= 0) {
      avg_ref_sensor += AHRS_ALIGNER_SAMPLES_NB / 2;
    } else {
      avg_ref_sensor -= AHRS_ALIGNER_SAMPLES_NB / 2;
    }
    avg_ref_sensor /= AHRS_ALIGNER_SAMPLES_NB;

    ahrs_aligner.noise = 0;
    int i;
    for (i = 0; i < AHRS_ALIGNER_SAMPLES_NB; i++) {
      int32_t diff = ref_sensor_samples[i] - avg_ref_sensor;
      ahrs_aligner.noise += abs(diff);
    }

    RATES_SDIV(ahrs_aligner.lp_gyro,  gyro_sum,  AHRS_ALIGNER_SAMPLES_NB);
    VECT3_SDIV(ahrs_aligner.lp_accel, accel_sum, AHRS_ALIGNER_SAMPLES_NB);
    VECT3_SDIV(ahrs_aligner.lp_mag,   mag_sum,   AHRS_ALIGNER_SAMPLES_NB);

    INT_RATES_ZERO(gyro_sum);
    INT_VECT3_ZERO(accel_sum);
    INT_VECT3_ZERO(mag_sum);
    samples_idx = 0;

    if (ahrs_aligner.noise < LOW_NOISE_THRESHOLD) {
      ahrs_aligner.low_noise_cnt++;
    } else if (ahrs_aligner.low_noise_cnt > 0) {
      ahrs_aligner.low_noise_cnt--;
    }

    if (ahrs_aligner.low_noise_cnt > LOW_NOISE_TIME) {
      ahrs_aligner.status = AHRS_ALIGNER_LOCKED;
#ifdef AHRS_ALIGNER_LED
      LED_ON(AHRS_ALIGNER_LED);
#endif
      uint32_t now_ts = get_sys_time_usec();
      AbiSendMsgIMU_LOWPASSED(ABI_BROADCAST, now_ts, &ahrs_aligner.lp_gyro,
                              &ahrs_aligner.lp_accel, &ahrs_aligner.lp_mag);
    }
  }

}

