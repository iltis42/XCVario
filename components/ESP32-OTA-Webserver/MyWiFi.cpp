
#include "MyWiFi.h"

#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <freertos/event_groups.h>

#include <esp_netif_types.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>




/* FreeRTOS event group to signal when we are connected*/
EventGroupHandle_t wifi_event_group;

const int AP_CLIENT_CONNECTED_BIT = BIT0;
const int AP_CLIENT_DISCONNECTED_BIT = BIT1;
const int WIFI_STA_CONNECTED_BIT = BIT0;
const int WIFI_STA_DISCONNECTED_BIT = BIT1;


static esp_err_t event_handler(void *ctx, system_event_t *event) 
{
	switch (event->event_id) 
	{
		case SYSTEM_EVENT_WIFI_READY:			printf("SYSTEM_EVENT_WIFI_READY\r\n");	break;
		case SYSTEM_EVENT_SCAN_DONE:			printf("SYSTEM_EVENT_SCAN_DONE\r\n");	break;                /**< ESP32 finish scanning AP */
		case SYSTEM_EVENT_STA_START:			/**< ESP32 station start */
												printf("SYSTEM_EVENT_STA_START\r\n");
												esp_wifi_connect();
												printf("Connectiing To SSID:%s : Pass:%s\r\n", CONFIG_STATION_SSID, CONFIG_STATION_PASSPHRASE);
												break;
		
		case SYSTEM_EVENT_STA_STOP:				printf("SYSTEM_EVENT_STA_STOP\r\n");	break;                 /**< ESP32 station stop */
		case SYSTEM_EVENT_STA_CONNECTED:		printf("SYSTEM_EVENT_STA_CONNECTED\r\n");	break;            /**< ESP32 station connected to AP */
		case SYSTEM_EVENT_STA_DISCONNECTED:		/**< ESP32 station disconnected from AP */
												printf("SYSTEM_EVENT_STA_DISCONNECTED\r\n");
												esp_wifi_connect();
												xEventGroupClearBits(wifi_event_group, WIFI_STA_DISCONNECTED_BIT);
												ESP_LOGI("WiFI", "retry to connect to the AP\r\n");
												break;
		
		case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:	printf("SYSTEM_EVENT_STA_AUTHMODE_CHANGE\r\n");	break;      /**< the auth mode of AP connected by ESP32 station changed */
		case SYSTEM_EVENT_STA_GOT_IP:			/**< ESP32 station got IP from connected AP */
												printf("SYSTEM_EVENT_STA_GOT_IP\r\n");
												ESP_LOGI("WiFI", "got ip:%s",	ip4addr_ntoa((const struct ip4_addr *)&event->event_info.got_ip.ip_info.ip));
												xEventGroupSetBits(wifi_event_group, WIFI_STA_CONNECTED_BIT);
												break;
		
		case SYSTEM_EVENT_STA_LOST_IP:  		printf("SYSTEM_EVENT_STA_LOST_IP\r\n");	break;           /**< ESP32 station lost IP and the IP is reset to 0 */
		case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:  	printf("SYSTEM_EVENT_STA_WPS_ER_SUCCESS\r\n");	break;     /**< ESP32 station wps succeeds in enrollee mode */
		case SYSTEM_EVENT_STA_WPS_ER_FAILED:	printf("SYSTEM_EVENT_STA_WPS_ER_FAILED\r\n");	break;       /**< ESP32 station wps fails in enrollee mode */
		case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:  	printf("SYSTEM_EVENT_STA_WPS_ER_TIMEOUT\r\n");	break;     /**< ESP32 station wps timeout in enrollee mode */
		case SYSTEM_EVENT_STA_WPS_ER_PIN:  		printf("SYSTEM_EVENT_STA_WPS_ER_PIN\r\n");	break;         /**< ESP32 station wps pin code in enrollee mode */
		case SYSTEM_EVENT_AP_START:   			printf("SYSTEM_EVENT_AP_START\r\n");	break;              /**< ESP32 soft-AP start */
		case SYSTEM_EVENT_AP_STOP:   			printf("SYSTEM_EVENT_AP_STOP\r\n");	break;               /**< ESP32 soft-AP stop */
		case SYSTEM_EVENT_AP_STACONNECTED:  	/**< a station connected to ESP32 soft-AP */
												printf("SYSTEM_EVENT_AP_STACONNECTED\r\n");
		
												ESP_LOGI("WiFI-AP",	"station: %02x:%02x:%02x:%02x:%02x:%02x join, AID=%d", MAC2STR(event->event_info.sta_connected.mac),	event->event_info.sta_connected.aid);
												xEventGroupSetBits(wifi_event_group, AP_CLIENT_CONNECTED_BIT);
												break;
		
	    case SYSTEM_EVENT_AP_STADISCONNECTED:	/**< a station disconnected from ESP32 soft-AP */
												printf("SYSTEM_EVENT_AP_STADISCONNECTED\r\n");
												ESP_LOGI("WiFI-AP",	"station: %02x:%02x:%02x:%02x:%02x:%02x leave, AID=%d", MAC2STR(event->event_info.sta_disconnected.mac),	event->event_info.sta_disconnected.aid);
												xEventGroupSetBits(wifi_event_group, AP_CLIENT_DISCONNECTED_BIT);
												break;
		
	    case SYSTEM_EVENT_AP_PROBEREQRECVED:	printf("SYSTEM_EVENT_AP_PROBEREQRECVED\r\n");	break;      /**< Receive probe request packet in soft-AP interface */
	    case SYSTEM_EVENT_GOT_IP6:     			printf("SYSTEM_EVENT_GOT_IP6\r\n");	break;					/**< ESP32 station or ap or ethernet interface v6IP addr is preferred */
	    case SYSTEM_EVENT_ETH_START:   			printf("SYSTEM_EVENT_ETH_START\r\n");	break;				/**< ESP32 ethernet start */
	    case SYSTEM_EVENT_ETH_STOP:   			printf("SYSTEM_EVENT_ETH_STOP\r\n");	break;              /**< ESP32 ethernet stop */
	    case SYSTEM_EVENT_ETH_CONNECTED:    	printf("SYSTEM_EVENT_ETH_CONNECTED\r\n");	break;			/**< ESP32 ethernet phy link up */
	    case SYSTEM_EVENT_ETH_DISCONNECTED:		printf("SYSTEM_EVENT_ETH_DISCONNECTED\r\n");	break;      /**< ESP32 ethernet phy link down */
		case SYSTEM_EVENT_ETH_GOT_IP: 			printf("SYSTEM_EVENT_ETH_GOT_IP\r\n");	break;				/**< ESP32 ethernet got IP from connected AP */
		case SYSTEM_EVENT_MAX:					printf("SYSTEM_EVENT_MAX\r\n");	break;
		default:								printf("SYSTEM_EVENT_ OTHER\r\n");	break;
	}
	
	return ESP_OK;
}
/* -----------------------------------------------------------------------------
  start_dhcp_server(void)

  Notes:  
  
 -----------------------------------------------------------------------------*/
void start_dhcp_server(void) 
{
    
	// stop DHCP server
	ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP));

	ip_addr_t dnsserver;
	dhcps_offer_t dhcps_dns_value = OFFER_DNS;
	dhcps_set_option_info(ESP_NETIF_DOMAIN_NAME_SERVER, &dhcps_dns_value, sizeof(dhcps_dns_value));

	// Set custom dns server address for dhcp server
	dnsserver.u_addr.ip4.addr = esp_ip4addr_aton("0.0.0.0");
	dnsserver.type = IPADDR_TYPE_V4;
	dhcps_dns_setserver(&dnsserver);
	
	ESP_ERROR_CHECK(tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP));
	printf("DHCP server started \n");
}
/* -----------------------------------------------------------------------------
  printStationList(void)

 print the list of connected stations  
  
 -----------------------------------------------------------------------------*/
void printStationList(void) 
{
	wifi_sta_list_t wifi_sta_list;
	tcpip_adapter_sta_list_t adapter_sta_list;
   
	memset(&wifi_sta_list, 0, sizeof(wifi_sta_list));
	memset(&adapter_sta_list, 0, sizeof(adapter_sta_list));

	// Give some time to gather intel of whats connected
	vTaskDelay(500 / portTICK_PERIOD_MS);
	
	ESP_ERROR_CHECK(esp_wifi_ap_get_sta_list(&wifi_sta_list));	
	ESP_ERROR_CHECK(tcpip_adapter_get_sta_list(&wifi_sta_list, &adapter_sta_list));

	printf(" Connected Station List:\n");
	printf("--------------------------------------------------\n");
	
	if (adapter_sta_list.num >= 1)
	{
		for (int i = 0; i < adapter_sta_list.num; i++) 
		{
		
			tcpip_adapter_sta_info_t station = adapter_sta_list.sta[i];
			printf("%d - mac: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x - IP: %s\n",
				i + 1,
				station.mac[0],
				station.mac[1],
				station.mac[2],
				station.mac[3],
				station.mac[4],
				station.mac[5],
				ip4addr_ntoa( (const struct ip4_addr *)&(station.ip) ) );
		}

		printf("\r\n");
	}
	else
	{
		printf("No Sations Connected\r\n");
	}

}
/* -----------------------------------------------------------------------------
  init_wifi_softap(void)

  Notes:  
  
  // https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/wifi/esp_wifi.html
  // https://github.com/sankarcheppali/esp_idf_esp32_posts/blob/master/esp_softap/main/esp_softap.c
  
 -----------------------------------------------------------------------------*/
void init_wifi_softap(void *arg)
{
	esp_log_level_set("wifi", ESP_LOG_INFO);    // disable wifi driver logging

	start_dhcp_server();
	
	wifi_event_group = xEventGroupCreate();
	
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, arg));
	
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();


	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
	
	
	// configure the wifi connection and start the interface
	wifi_config_t ap_config;
	strcpy( (char *)(ap_config.ap.ssid), "ESP32 OTA" );
	strcpy( (char*)ap_config.ap.password, "xcvario-21" );
	ap_config.ap.ssid_len = 0;
	uint32_t channel = esp_random()%11;
	ap_config.ap.channel = channel;
	ap_config.ap.authmode = AP_AUTHMODE;
	ap_config.ap.ssid_hidden = AP_SSID_HIDDEN;
	ap_config.ap.max_connection = AP_MAX_CONNECTIONS;
	ap_config.ap.beacon_interval = AP_BEACON_INTERVAL;
	
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
	
	vTaskDelay(100 / portTICK_PERIOD_MS);

	ESP_ERROR_CHECK(esp_wifi_start());
	printf("ESP WiFi started in AP mode, channel: %d \n", channel );
	
	/*
	 *
	 *  Spin up a task to show who connected or disconected
	 *
	 **/
	//xTaskCreate(&print_sta_info, "print_sta_info", 4096, NULL, 1, NULL);

	// https://demo-dijiudu.readthedocs.io/en/latest/api-reference/wifi/esp_wifi.html#_CPPv225esp_wifi_set_max_tx_power6int8_t
	// This can only be placed after esp_wifi_start();
	ESP_ERROR_CHECK(esp_wifi_set_max_tx_power(8));
	
}
/* -----------------------------------------------------------------------------
  init_wifi_station(void)

  Notes:  
  
  // https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/wifi/esp_wifi.html
  
 -----------------------------------------------------------------------------*/
void init_wifi_station(void *arg)
{
	esp_log_level_set("wifi", ESP_LOG_NONE);     // disable wifi driver logging
	
	wifi_event_group = xEventGroupCreate();

	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, arg));
	
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	
	wifi_config_t wifi_config;
	strcpy( (char *)wifi_config.ap.ssid, CONFIG_STATION_SSID );
	strcpy( (char *)wifi_config.ap.password, CONFIG_STATION_PASSPHRASE );
	
	ESP_ERROR_CHECK(esp_wifi_set_config((wifi_interface_t)ESP_IF_WIFI_STA, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());

	ESP_LOGI("WiFi", "Station Set to SSID:%s Pass:%s\r\n", CONFIG_STATION_SSID, CONFIG_STATION_PASSPHRASE);

}
/* -----------------------------------------------------------------------------
  print_sta_info(void)

  Notes:  
 
 -----------------------------------------------------------------------------*/
void print_sta_info(void *pvParam) 
{
	printf("print_sta_info task started \n");
	
	while (1) 
	{	
		EventBits_t staBits = xEventGroupWaitBits(wifi_event_group, AP_CLIENT_CONNECTED_BIT | AP_CLIENT_DISCONNECTED_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
		
		if ((staBits & AP_CLIENT_CONNECTED_BIT) != 0)
		{
			printf("New station connected\n\n");
		}
		else
		{
			printf("A station disconnected\n\n");
		}
		
		printStationList();
	}
}
