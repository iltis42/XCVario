/*
 * Setup.h
 *
 *  Created on: Dec 23, 2017
 *      Author: iltis
 */

#ifndef MAIN_SETUP_H_
#define MAIN_SETUP_H_
#include <string>
#include "esp_system.h"
#include "stdio.h"
#include <esp_log.h>
#include <string>
#include "string.h"
#include "BTSender.h"


typedef struct {
	char  _bt_name[32];
	uint8_t _blue_enable;
	uint8_t _factory_reset;   // 0 = no,  1= yes
	float   _offset;
	uint32_t _checksum;
} setup_t;


class Setup {
public:
	Setup() {
		memset( _ID, 0, sizeof( _ID ) );
	}
	// Setup( bool &operationMode, float &speedcal );
	virtual ~Setup() {};
	void factorySetting();
	inline setup_t *get() { return &_setup; };
	void begin();
	void commit();
	void  help();
	bool checkSum( bool set=false );
	inline char  *getBtName() { return getID(); }
	char* getID();

private:
	setup_t _setup;
	static char _ID[14];
};


#endif /* MAIN_SETUP_H_ */
