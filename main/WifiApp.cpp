/*  WiFi softAP Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
 */


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "logdef.h"

#include <lwip/sockets.h>
#include <string.h>
#include <errno.h>
#include "sdkconfig.h"
#include "Setup.h"
#include "RingBufCPP.h"
#include "BTSender.h"
#include "Router.h"
#include <string.h>
#include "esp_wifi.h"
#include <list>
#include "WifiClient.h"


typedef struct xcv_sock_server {
	RingBufCPP<SString, QUEUE_SIZE>* txbuf;
	RingBufCPP<SString, QUEUE_SIZE>* rxbuf;
	int port;
}sock_server_t;

static sock_server_t XCVario = { .txbuf = &wl_vario_tx_q, .rxbuf = &wl_vario_rx_q, .port=8880 };
static sock_server_t FLARM   = { .txbuf = &wl_flarm_tx_q, .rxbuf = &wl_flarm_rx_q, .port=8881 };
static sock_server_t AUX     = { .txbuf = &wl_aux_tx_q,   .rxbuf = &wl_aux_rx_q,   .port=8882 };


int create_socket( int port ){
	struct sockaddr_in serverAddress;
	// Create a socket that we will listen upon.
	int mysock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mysock < 0) {
		ESP_LOGE(FNAME, "socket: %d %s", mysock, strerror(errno));
		return -1;
	}
	// Bind our server socket to a port.
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(port);
	int rc  = bind(mysock, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
	if (rc < 0) {
		ESP_LOGE(FNAME, "bind: %d %s", rc, strerror(errno));
		return -1;
	}
	int flag = 1;
	// this is really realtime data, so sent TCP_NODELAY for XCVario data exchange
	setsockopt(mysock, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
	ESP_LOGI(FNAME, "bind port: %d", port  );
	// Flag the socket as listening for new connections.
	rc = listen(mysock, 5);
	if (rc < 0) {
		ESP_LOGE(FNAME, "listen: %d %s", rc, strerror(errno));
		return -1;
	}
	return mysock;
}

// Multi client TCP server with dynamic updated list of clients connected

void socket_server(void *setup) {
	sock_server_t *config = (sock_server_t *)setup;
	struct sockaddr_in clientAddress[10];  // we support max 10 clients try to connect same time
	socklen_t clientAddressLength = sizeof(struct sockaddr_in);
	std::list<int>  clients;
	int sock = create_socket( config->port );
	if( sock < 0 ) {
		ESP_LOGE(FNAME, "Socket creation for %d port FAILED: Abort task", config->port );
		vTaskDelete(NULL);
	}
	// we have a socket
	fcntl(sock, F_SETFL, O_NONBLOCK); // socket should not block, so we can server several clients
	while(1)
	{
		while (1) {
			int new_client = accept(sock, (struct sockaddr *)&clientAddress[clients.size()], &clientAddressLength);
			if( new_client >= 0 && clients.size() < 10 ){
				clients.push_back( new_client );
				ESP_LOGV(FNAME, "New sock client: %d, number of clients: %d", new_client, clients.size()  );
			}
			if( clients.size() ) {
				SString s;
				Router::pullMsg( *(config->txbuf), s);
				if( s.length() )
					ESP_LOGV(FNAME, "port %d to sent %d: bytes, %s", config->port, s.length(), s.c_str() );
				std::list<int>::iterator it;
				int client_dead = 0;
				int client;
				for(it = clients.begin(); it != clients.end(); ++it)
				{
					client=*it;
					// ESP_LOGD(FNAME, "loop tcp client %d, port %d", client , config->port );
					if ( s.length() ){
						ESP_LOGV(FNAME, "sent to tcp client %d: %d bytes", client, s.length() );
						ESP_LOG_BUFFER_HEXDUMP(FNAME,s.c_str(),s.length(), ESP_LOG_VERBOSE);
						if( client >= 0 ){
							int num = send(client, s.c_str(), s.length(), 0);
							// ESP_LOGI(FNAME, "client %d, num send %d", client, num );
							if( num < 0 ) {
								ESP_LOGW(FNAME, "tcp client %d (port %d) send err: %s, remove!", client,  config->port, strerror(errno) );
								close(client);
								client_dead = client;
								// check on sending and remove from list if client has died
							}
							else{
								// ESP_LOGI(FNAME, "tcp send to client %d (port: %d), bytes %d success", client, config->port, num );
							}
						}
					}
					if( !client_dead ){
						// ESP_LOGI(FNAME, "read from client %d", client);
						SString tcprx;
						ssize_t sizeRead = recv(client, tcprx.c_str(), SSTRLEN-1, MSG_DONTWAIT);
						if (sizeRead > 0) {
							tcprx.setLen( sizeRead );
							Router::forwardMsg( tcprx, *(config->rxbuf) );
							ESP_LOGV(FNAME, "tcp read from port %d size: %d data: %s", config->port, sizeRead, tcprx.c_str() );
						}
					}
				}
				if( client_dead )
					clients.remove( client_dead );
			}
			vTaskDelay(20/portTICK_PERIOD_MS);
		}
	}
	vTaskDelete(NULL);
}



/* WiFi configuration and startup

 */

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
		int32_t event_id, void* event_data)
{
	if (event_id == WIFI_EVENT_AP_STACONNECTED) {
		wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
		ESP_LOGI(FNAME,"station %x:%x:%x:%x:%x:%x joined, AID=%d", MAC2STR(event->mac), event->aid);
	} else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
		wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
		ESP_LOGI(FNAME,"station %x:%x:%x:%x:%x:%x left, AID=%d", MAC2STR(event->mac), event->aid);
	}
}


void wifi_init_softap()
{
	if( blue_enable.get() == WL_WLAN ){
		tcpip_adapter_init();
		ESP_LOGI(FNAME,"now esp_netif_init");
		ESP_ERROR_CHECK(esp_netif_init());
		ESP_ERROR_CHECK(esp_event_loop_create_default());
		ESP_LOGI(FNAME,"now esp_event_handler_instance_register");
		ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
				ESP_EVENT_ANY_ID,
				&wifi_event_handler,
				NULL,
				NULL));

		// ESP_LOGI(FNAME,"now esp_netif_create_default_wifi_ap");
		// esp_netif_create_default_wifi_ap();
		ESP_LOGI(FNAME,"now esp_wifi_init");
		wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
		ESP_ERROR_CHECK(esp_wifi_init(&cfg));

		esp_event_loop_init( (system_event_cb_t)wifi_event_handler, 0 );

		ESP_LOGI(FNAME,"now esp_wifi_set_mode");
		ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

		ESP_LOGI(FNAME,"now esp_wifi_set_config ssid:%s", SetupCommon::getID() );
		wifi_config_t wc;
		strcpy( (char *)wc.ap.ssid, SetupCommon::getID() );
		wc.ap.ssid_len = strlen( (char *)wc.ap.ssid );
		strcpy( (char *)wc.ap.password, PASSPHARSE );
		wc.ap.channel = 1;
		wc.ap.max_connection = 4;
		wc.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
		wc.ap.ssid_hidden = 0;
		wc.ap.beacon_interval = 50;

		ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wc));

		ESP_LOGI(FNAME,"now esp_wifi_set_storage");
		ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

		sleep(1);
		ESP_LOGI(FNAME,"now esp_wifi_start");
		ESP_ERROR_CHECK(esp_wifi_start());
		ESP_ERROR_CHECK(esp_wifi_set_max_tx_power(8));

		xTaskCreatePinnedToCore(&socket_server, "socket_srv_0", 2500, &XCVario, 17, 0, 0);  // 10
		xTaskCreatePinnedToCore(&socket_server, "socket_ser_1", 2500, &FLARM, 18, 0, 0);  // 10
		xTaskCreatePinnedToCore(&socket_server, "socket_ser_2", 2500, &AUX, 16, 0, 0);  // 10

		ESP_LOGI(FNAME, "wifi_init_softap finished SUCCESS. SSID:%s password:%s channel:%d", (char *)wc.ap.ssid, (char *)wc.ap.password, wc.ap.channel );
	}
}

