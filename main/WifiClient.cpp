#include <stdio.h>
#include<string.h>    //strlen
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
// #include "esp_event_loop.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "logdef.h"
#include "RingBufCPP.h"
#include "Protocols.h"
#include "WifiClient.h"
#include "sensor.h"



EventGroupHandle_t WifiClient::wifi_event_group;
bool WifiClient::cl_connected=false;
esp_netif_t *WifiClient::sta_netif = 0;
std::string WifiClient::SSID;
int WifiClient::sock = -1;


void WifiClient::wifi_connect(){
	ESP_LOGI(FNAME,"wifi_connect()");
	wifi_config_t cfg;
	memset( &cfg, 0, sizeof(cfg) );
	strcpy( (char *)cfg.sta.ssid, SSID.c_str() );
	strcpy( (char *)cfg.sta.password, PASSPHARSE );

    ESP_ERROR_CHECK( esp_wifi_disconnect() );
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &cfg) );
    ESP_ERROR_CHECK( esp_wifi_connect() );
}

void WifiClient::event_handler(void* arg, esp_event_base_t event_base,
		int32_t event_id, void* event_data)
{
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
		ESP_LOGI(FNAME,"SYSTEM_EVENT_STA_START");
		WifiClient::wifi_connect();
	} else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
		ESP_LOGI(FNAME,"SYSTEM_EVENT_STA_DISCONNECTED");
		esp_wifi_connect();
		xEventGroupClearBits( WifiClient::wifi_event_group, CONNECTED_BIT);
		if( sock > 0 ){
			close( sock );
			sock = -1;
			WifiClient::cl_connected = false;
		}

	} else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
		ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
		ESP_LOGI(FNAME, "SYSTEM_EVENT_STA_GOT_IP ip:" IPSTR, IP2STR(&event->ip_info.ip));
		xEventGroupSetBits( WifiClient::wifi_event_group, CONNECTED_BIT);
	}

}


void WifiClient::initialise_wifi(void)
{
    esp_log_level_set("wifi", ESP_LOG_DEBUG); // disable wifi driver logging
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());
    tcpip_adapter_init();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));
    // Initialize default station as network interface instance (esp-netif)
    if( !sta_netif )
    	sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}

#define DEFAULT_SCAN_LIST_SIZE 20

std::string WifiClient::scan( void ){
	ESP_LOGI(FNAME,"wifi scan");
	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	sta_netif = esp_netif_create_default_wifi_sta();
	assert(sta_netif);
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK( esp_wifi_init(&cfg));

	uint16_t number = DEFAULT_SCAN_LIST_SIZE;
	wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
	uint16_t ap_count = 0;
	memset(ap_info, 0, sizeof(ap_info));

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_start());
	bool found = false;
	while( !found ) {
		ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, true));
		ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
		ESP_LOGI(FNAME, "Total APs scanned = %u", ap_count);
		if( Rotary.readSwitch() )
			break;
		ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
		for (int i = 0; (i < DEFAULT_SCAN_LIST_SIZE) && (i < ap_count); i++) {
			if( strncmp( (char*)ap_info[i].ssid, "XCVARIO-", 8 ) == 0 )
			ESP_LOGI(FNAME, "SSID \t%s", ap_info[i].ssid);
			ESP_LOGI(FNAME, "RSSI \t%d", ap_info[i].rssi);
			if( strncmp( (char*)ap_info[i].ssid, "XCVario-", 8 ) == 0 ) {
				found = true;
				SSID = std::string( (char*)ap_info[i].ssid );
				ESP_LOGI(FNAME,"SCAN found master XCVario: %s", SSID.c_str() );
			}
		}
	}
	ESP_ERROR_CHECK( esp_wifi_disconnect() );
	ESP_ERROR_CHECK( esp_wifi_stop() );
	ESP_ERROR_CHECK( esp_wifi_restore() );
	return SSID;
}


void WifiClient::tcp_client(void *pvParam){
    ESP_LOGI(FNAME,"tcp_client task started");
    struct sockaddr_in tcpServerAddr;
    tcpServerAddr.sin_addr.s_addr = inet_addr(TCPServerIP);
    tcpServerAddr.sin_family = AF_INET;
    tcpServerAddr.sin_port = htons( 8880 );
	int num = 0;
    SString send;
    int timeout=0;
    while(1){
        xEventGroupWaitBits(wifi_event_group,CONNECTED_BIT,false,true,portMAX_DELAY);
        timeout++;
        // ESP_LOGI(FNAME,"tcp_client task timeout=%d", timeout);
        if( timeout > 60 )
           cl_connected = false;
        if( sock < 0 ){
        	sock = socket(AF_INET, SOCK_STREAM, 0);
        	if(sock < 0) {
        		ESP_LOGE(FNAME, "Failed to allocate socket, %s", strerror(errno) );
        		vTaskDelay(2000 / portTICK_PERIOD_MS);
        		cl_connected = false;
        		continue;
        	}
        	// fcntl(sock, F_SETFL, O_NONBLOCK);
        	ESP_LOGI(FNAME, "allocated socket %d", sock);
        	if(connect(sock, (struct sockaddr *)&tcpServerAddr, sizeof(tcpServerAddr)) != 0) {
        		ESP_LOGE(FNAME, "socket connect failed, %s", strerror(errno ));
        		close(sock);
        		sock = -1;
        		cl_connected = false;
        		vTaskDelay(4000 / portTICK_PERIOD_MS);
        		continue;
        	}
        	ESP_LOGI(FNAME, "socket %d connected", sock);
        }

        if( send.length() ){
        	ESP_LOGI(FNAME, "have data to send");
        	if( write(sock , send.c_str() , send.length() ) < 0)
        	{
        		ESP_LOGE(FNAME, "Send failed, socket %d", sock );
        		close(sock);
        		sock = -1;
        		cl_connected = false;
        		vTaskDelay(4000 / portTICK_PERIOD_MS);
        		continue;
        	}
        	ESP_LOGI(FNAME, "socket send success");
        }
        SString rec;
        num = recv(sock, rec.c_str(), SSTRLEN-1, MSG_DONTWAIT );
        if(num > 0){
        	rec.setLen( num );
        	ESP_LOGV(FNAME, "socket read %d bytes: %s", num, rec.c_str() );
        	Protocols::parseNMEA( rec.c_str() );
        	timeout = 0;
        	cl_connected = true;
        }
        vTaskDelay(30 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(FNAME, "tcp_client task closed\n");
}

void WifiClient::start()
{	
	ESP_LOGI(FNAME, "start wifi_client"  );
    initialise_wifi();
    xTaskCreate(&tcp_client,"tcp_client",4048,NULL,5,NULL);
}
