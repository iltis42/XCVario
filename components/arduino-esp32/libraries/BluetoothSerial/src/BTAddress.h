/*
 * BTAddress.h
 *
 *  Created on: Jul 2, 2017
 *      Author: kolban
 *  Ported  on: Feb 5, 2021
 *      Author: Thomas M. (ArcticSnowSky)
 */

#ifndef COMPONENTS_CPP_UTILS_BTADDRESS_H_
#define COMPONENTS_CPP_UTILS_BTADDRESS_H_
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)
#include <esp_gap_bt_api.h> // ESP32 BT
#include <string>


/**
 * @brief A %BT device address.
 *
 * Every %BT device has a unique address which can be used to identify it and form connections.
 */
class BTAddress {
public:
	BTAddress(esp_bd_addr_t address);
	BTAddress(std::string stringAddress);
	bool           equals(BTAddress otherAddress);
	esp_bd_addr_t* getNative();
	std::string    toString();

private:
	esp_bd_addr_t m_address;
};

#endif /* CONFIG_BT_ENABLED */
#endif /* COMPONENTS_CPP_UTILS_BTADDRESS_H_ */
