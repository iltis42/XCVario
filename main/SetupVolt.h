/*
 * SetupVolt.h
 *
 *  Created on: Dec 23, 2017
 *      Author: iltis
 */

#ifndef MAIN_SetupVolt_H_
#define MAIN_SetupVolt_H_
#include <string>
#include "esp_system.h"
#include "stdio.h"
#include <esp_log.h>
#include <string>
#include "string.h"
#include "BTSender.h"
#include "Polars.h"


typedef struct {
	float _factory_volt_adjust;
	uint32_t _checksum;
} setup_volt_t;


class SetupVolt {
public:
	SetupVolt() {   }
	// SetupVolt( bool &operationMode, float &speedcal );
	virtual ~SetupVolt() {};
	void factorySetting();
	inline setup_volt_t *get() { return &_setup; };
	void begin();
	void commit();
	void  help();
	bool checkSum( bool set=false );

private:
	 setup_volt_t _setup;  // factory adjust voltmeter
};


#endif /* MAIN_SetupVolt_H_ */
