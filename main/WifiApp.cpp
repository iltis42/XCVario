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
#include "sensor.h"
#include "Flarm.h"

typedef struct xcv_sock_server {
	RingBufCPP<SString, QUEUE_SIZE>* txbuf;
	RingBufCPP<SString, QUEUE_SIZE>* rxbuf;
	int port;
	TaskHandle_t *pid;
}sock_server_t;

static sock_server_t XCVario = { .txbuf = &wl_vario_tx_q, .rxbuf = &wl_vario_rx_q, .port=8880, .pid = 0 };
static sock_server_t FLARM   = { .txbuf = &wl_flarm_tx_q, .rxbuf = &wl_flarm_rx_q, .port=8881, .pid = 0  };
static sock_server_t AUX     = { .txbuf = &wl_aux_tx_q,   .rxbuf = &wl_aux_rx_q,   .port=8882, .pid = 0  };


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
	// setsockopt(mysock, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
	ESP_LOGV(FNAME, "bind port: %d", port  );
	// Flag the socket as listening for new connections.
	rc = listen(mysock, 5);
	if (rc < 0) {
		ESP_LOGE(FNAME, "listen: %d %s", rc, strerror(errno));
		return -1;
	}
	return mysock;
}

// Multi client TCP server with dynamic updated list of clients connected

void on_client_connect( int port, int msg ){
	if( port == 8880 && !Flarm::bincom ){ // have a client to XCVario protocol connected
		// ESP_LOGV(FNAME, "on_client_connect: Send MC, Ballast, Bugs, etc");
		if( msg == 1 )
			OV.sendQNHChange( QNH.get() );
		if( msg == 2 ) {
			if( wireless == WL_WLAN_CLIENT )
				OV.sendBallastChange( ballast.get(), false );
			else
				OV.sendBallastChange( ballast.get(), true );
		}
		if( msg == 3 )
			OV.sendBugsChange( bugs.get() );
		if( msg == 4 )
			OV.sendClientMcChange( MC.get() );
		if( msg == 5 )
			OV.sendTemperatureChange( temperature );
		SetupCommon::syncEntry(msg);
	}
}


void socket_server(void *setup) {
	sock_server_t *config = (sock_server_t *)setup;
	struct sockaddr_in clientAddress[10];  // we support max 10 clients try to connect same time
	socklen_t clientAddressLength = sizeof(struct sockaddr_in);
	std::list<int>  clients;
	int num_send = 0;
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
				num_send = 0;
				ESP_LOGI(FNAME, "New sock client: %d, number of clients: %d", new_client, clients.size()  );
			}
			// ESP_LOGV(FNAME, "Number of clients %d, port %d", clients.size(), config->port );
			if( clients.size() ) {
				char block[512];
				int len = Router::pullBlock( *(config->txbuf), block );
				if( len )
					ESP_LOGV(FNAME, "port %d to sent %d: bytes, %s", config->port, len, block );
				std::list<int>::iterator it;
				int client;
				for(it = clients.begin(); it != clients.end(); ++it)
				{
					client=*it;
					int client_dead = 0;
					// ESP_LOGD(FNAME, "loop tcp client %d, port %d", client , config->port );
					if ( len ){
						// ESP_LOGI(FNAME, "sent to tcp client %d, bytes %d, port %d", client, len, config->port );
						// ESP_LOG_BUFFER_HEXDUMP(FNAME,block,len, ESP_LOG_INFO);
						if( client >= 0 ){
							int num = send(client, block, len, MSG_DONTWAIT);
							// ESP_LOGV(FNAME, "client %d, num send %d", client, num );
							if( num < 0 ) {
								ESP_LOGW(FNAME, "tcp client %d (port %d) send err: %s, remove!", client,  config->port, strerror(errno) );
								close(client);
								it = clients.erase( it );
								client_dead = client;
								num_send = 0;
							}
							// ESP_LOGV(FNAME, "tcp send to client %d (port: %d), bytes %d success", client, config->port, num );
						}
					}
					if( Flarm::bincom )
						vTaskDelay(10/portTICK_PERIOD_MS); // maximize realtime throuput for flight download
					if( !client_dead ){
						// ESP_LOGI(FNAME, "read from client %d", client);
						SString tcprx;
						ssize_t sizeRead = recv(client, tcprx.c_str(), SSTRLEN-1, MSG_DONTWAIT);
						if (sizeRead > 0) {
							tcprx.setLen( sizeRead );
							Router::forwardMsg( tcprx, *(config->rxbuf) );
							// ESP_LOGI(FNAME, "tcp read from port %d size: %d", config->port, sizeRead );
							// ESP_LOG_BUFFER_HEXDUMP(FNAME,tcprx.c_str(),sizeRead, ESP_LOG_INFO);
						}
						if( config->port == 8880 ){
							if( num_send < SetupCommon::numEntries() ){
								on_client_connect( config->port, num_send );
								num_send ++;
							}
						}
					}
				}
			}
			if( uxTaskGetStackHighWaterMark( config->pid ) < 256 )
				ESP_LOGW(FNAME,"Warning task stack low: %d bytes, port %d", uxTaskGetStackHighWaterMark( config->pid ), config->port );
			if( Flarm::bincom )
				vTaskDelay(10/portTICK_PERIOD_MS);  // maximize realtime throuput for flight download
			else
				vTaskDelay(200/portTICK_PERIOD_MS);
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
		ESP_LOGV(FNAME,"station %x:%x:%x:%x:%x:%x joined, AID=%d", MAC2STR(event->mac), event->aid);
	} else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
		wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
		ESP_LOGV(FNAME,"station %x:%x:%x:%x:%x:%x left, AID=%d", MAC2STR(event->mac), event->aid);
	}
}


void wifi_init_softap()
{
	if( wireless == WL_WLAN ){
		tcpip_adapter_init();
		ESP_LOGV(FNAME,"now esp_netif_init");
		ESP_ERROR_CHECK(esp_netif_init());
		ESP_ERROR_CHECK(esp_event_loop_create_default());
		ESP_LOGV(FNAME,"now esp_event_handler_instance_register");
		ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
				ESP_EVENT_ANY_ID,
				&wifi_event_handler,
				NULL,
				NULL));

		// ESP_LOGV(FNAME,"now esp_netif_create_default_wifi_ap");
		// esp_netif_create_default_wifi_ap();
		ESP_LOGV(FNAME,"now esp_wifi_init");
		wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
		ESP_ERROR_CHECK(esp_wifi_init(&cfg));

		esp_event_loop_init( (system_event_cb_t)wifi_event_handler, 0 );

		ESP_LOGV(FNAME,"now esp_wifi_set_mode");
		ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

		ESP_LOGV(FNAME,"now esp_wifi_set_config ssid:%s", SetupCommon::getID() );
		wifi_config_t wc;
		strcpy( (char *)wc.ap.ssid, SetupCommon::getID() );
		wc.ap.ssid_len = strlen( (char *)wc.ap.ssid );
		strcpy( (char *)wc.ap.password, PASSPHARSE );
		wc.ap.channel = 1;
		wc.ap.max_connection = 4;
		wc.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
		wc.ap.ssid_hidden = 0;
		wc.ap.beacon_interval = 50;

		ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wc));

		ESP_LOGV(FNAME,"now esp_wifi_set_storage");
		ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

		sleep(1);
		ESP_LOGV(FNAME,"now esp_wifi_start");
		ESP_ERROR_CHECK(esp_wifi_start());
		ESP_ERROR_CHECK(esp_wifi_set_max_tx_power(8));

		xTaskCreatePinnedToCore(&socket_server, "socket_ser_2", 3072, &AUX, 12, AUX.pid, 0);  // 10
		xTaskCreatePinnedToCore(&socket_server, "socket_srv_0", 3072, &XCVario, 13, XCVario.pid, 0);  // 10
		xTaskCreatePinnedToCore(&socket_server, "socket_ser_1", 3072, &FLARM, 14, FLARM.pid, 0);  // 10

		ESP_LOGV(FNAME, "wifi_init_softap finished SUCCESS. SSID:%s password:%s channel:%d", (char *)wc.ap.ssid, (char *)wc.ap.password, wc.ap.channel );
	}
}

