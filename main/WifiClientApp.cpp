#include <stdio.h>
#include<string.h>    //strlen
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
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

#define SSID "XCVario-9491"
#define PASSPHARSE "xcvario-21"
#define MESSAGE "HelloTCPServer"
#define TCPServerIP "192.168.4.1"

static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;
bool cl_connected=false;

bool client_connected() {
	return cl_connected;
}

void wifi_connect(){
	ESP_LOGI(FNAME,"wifi_connect()");
	wifi_config_t cfg;
	memset( &cfg, 0, sizeof(cfg) );
	strcpy( (char *)cfg.sta.ssid, SSID );
	strcpy( (char *)cfg.sta.password, PASSPHARSE );

    ESP_ERROR_CHECK( esp_wifi_disconnect() );
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &cfg) );
    ESP_ERROR_CHECK( esp_wifi_connect() );
}

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
    	ESP_LOGI(FNAME,"SYSTEM_EVENT_STA_START");
        wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
    	ESP_LOGI(FNAME,"SYSTEM_EVENT_STA_GOT_IP");
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
    	ESP_LOGI(FNAME,"SYSTEM_EVENT_STA_DISCONNECTED");
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

static void initialise_wifi(void)
{
    esp_log_level_set("wifi", ESP_LOG_DEBUG); // disable wifi driver logging
    tcpip_adapter_init();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    // Initialize default station as network interface instance (esp-netif)
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}

void tcp_client(void *pvParam){
    ESP_LOGI(FNAME,"tcp_client task started");
    struct sockaddr_in tcpServerAddr;
    tcpServerAddr.sin_addr.s_addr = inet_addr(TCPServerIP);
    tcpServerAddr.sin_family = AF_INET;
    tcpServerAddr.sin_port = htons( 8880 );
    int sock = -1;
	int num = 0;
    SString send;
    while(1){
        xEventGroupWaitBits(wifi_event_group,CONNECTED_BIT,false,true,portMAX_DELAY);
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
        	cl_connected = true;
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
        SString recv;
        num = read(sock, recv.c_str(), SSTRLEN-1 );
        if(num > 0){
        	recv.setLen( num );
        	ESP_LOGI(FNAME, "socket read %d bytes: %s", num, recv.c_str() );
        	Protocols::parseNMEA( recv.c_str() );
        }
        vTaskDelay(30 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(FNAME, "tcp_client task closed\n");
}

void start_wifi_client()
{	
	ESP_LOGI(FNAME, "start_wifi_client()"  );
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_event_group = xEventGroupCreate();
    initialise_wifi();
    xTaskCreate(&tcp_client,"tcp_client",4048,NULL,5,NULL);
}
