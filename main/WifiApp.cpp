/*  WiFi softAP Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
 */

#include "WifiApp.h"
#include "Router.h"
#include "sensor.h"
#include "DataMonitor.h"
#include "comm/DataLink.h"
#include "WifiClient.h"
#include "logdefnone.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <lwip/sockets.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_mac.h>
#include <esp_event.h>

extern bool netif_initialized;

WifiApp *Wifi = nullptr;

#define WIFI_BUFFER_SIZE 513
#define MAX_CLIENTS   4


bool WifiApp::full[NUM_TCP_PORTS] = { false, false, false, false };
bool WifiApp::task_created=false;
sock_server_t *WifiApp::_socks[NUM_TCP_PORTS] = { nullptr, nullptr, nullptr, nullptr };
TaskHandle_t WifiApp::pid = nullptr;

class WIFI_EVENT_HANDLER
{
public:

	static int create_socket( int port ){
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
	static void on_client_connect( int port, int msg ){
		if( port == 8884 ){ // have a client to XCVario protocol connected
			// ESP_LOGV(FNAME, "on_client_connect: Send MC, Ballast, Bugs, etc");
			SetupCommon::syncEntry(msg);
		}
	}
    // WiFi Task
	static void socket_server(void *setup)
	{
		while (1) {
			for (int n = 0; n < NUM_TCP_PORTS; n++) {
				if (WifiApp::_socks[n] == nullptr)
					continue;

				sock_server_t *config = (sock_server_t *)WifiApp::_socks[n];
				std::list<client_record_t> &clients = config->clients;
				ESP_LOGI(FNAME, "sock server, port: %d, clients: %d", 8880+n, clients.size() );
				fd_set read_fds;
				struct timeval timeout;
				int max_fd = config->socket;

				FD_ZERO(&read_fds);
				FD_SET(config->socket, &read_fds);

				timeout.tv_sec = 0;
				timeout.tv_usec = 200 * 1000; // Timeout of 200ms

				for (auto &client_rec : clients) {
					if (client_rec.client >= 0) {
						FD_SET(client_rec.client, &read_fds);
						max_fd = std::max(max_fd, client_rec.client);
					}
				}
				int select_result = select(max_fd + 1, &read_fds, nullptr, nullptr, &timeout);
				if (select_result < 0) {
					ESP_LOGE(FNAME, "select error: %d", errno);
					continue;
				}
				if (FD_ISSET(config->socket, &read_fds)) {
					ESP_LOGI(FNAME, "FD_ISSET socket: %d num clients %d, port %d", config->socket, clients.size(), config->port );
					struct sockaddr_in clientAddress;
					socklen_t clientAddressLength = sizeof(clientAddress);
					int new_client = accept(config->socket, (struct sockaddr *)&clientAddress, &clientAddressLength);
					if (new_client >= 0 && clients.size() < MAX_CLIENTS) {
						client_record_t new_rec;
						new_rec.client = new_client;
						new_rec.retries = 0;
						new_rec.clientAddress = clientAddress;
						clients.push_back(new_rec);
						ESP_LOGI(FNAME, "New client: %d, number of clients: %d", new_client, clients.size());
					}
				}
				for (auto it = clients.begin(); it != clients.end(); ) {
					client_record_t &client_rec = *it;
					if (FD_ISSET(client_rec.client, &read_fds)) {
						ESP_LOGI(FNAME, "FD_ISSET socket: client %d, port %d", client_rec.client, config->port );
						char r[SSTRLEN + 1];
						ssize_t sizeRead = recv(client_rec.client, r, SSTRLEN - 1, MSG_DONTWAIT);
						if (sizeRead > 0) {
							ESP_LOGI(FNAME, "FD socket recv: client %d, port %d, read:%d bytes", client_rec.client, config->port, sizeRead );
							WifiApp *wifi = static_cast<WifiApp *>(config->mywifi);
							auto dl = wifi->_dlink.find(config->port);
							if (dl != wifi->_dlink.end()) {
								dl->second->process(r, sizeRead);
							}
						}
					}
					if (client_rec.retries > 100) {
						ESP_LOGW(FNAME, "tcp client %d (port %d) permanent send error: %s, removing!", client_rec.client, config->port, strerror(errno));
						close(client_rec.client);
						it = clients.erase(it);
					} else {
						++it;
					}
				}
				// Router::routeWLAN();  // to be removed later
				// Router::routeCAN();   // dito
				if (uxTaskGetStackHighWaterMark(WifiApp::pid) < 128)
					ESP_LOGW(FNAME, "Warning wifi task stack low: %d bytes, port %d", uxTaskGetStackHighWaterMark(WifiApp::pid), config->port);

				vTaskDelay(200 / portTICK_PERIOD_MS);
			}
		}
		vTaskDelete(NULL);
	}

	static void wifi_event_handler(void* arg, esp_event_base_t event_base,	int32_t event_id, void* event_data)
	{
		if (event_id == WIFI_EVENT_AP_STACONNECTED) {
			wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
			ESP_LOGI(FNAME,"station %x:%x:%x:%x:%x:%x joined, AID=%d", MAC2STR(event->mac), event->aid);
		} else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
			wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
			ESP_LOGI(FNAME,"station %x:%x:%x:%x:%x:%x left, AID=%d", MAC2STR(event->mac), event->aid);
		}
	}

}; // WIFI_EVENT_HANDLER


WifiApp::WifiApp(): InterfaceCtrl()
{
	for(int i=0; i<NUM_TCP_PORTS; i++) {
		_socks[i] = nullptr;
	}
}

WifiApp::~WifiApp()
{
	vTaskDelete(pid);
}

int  WifiApp::queueFull(){
	// ESP_LOGI(FNAME, "WQF: 0:%d 1:%d 2:%d 3:%d", full[0], full[0],full[0],full[0] );
	if( full[0] || full[1] || full[2] || full[3] )
		return 1;
	else
		return 0;
}

// it's mostly static IP a cfg dependig of port so give here port minus 8880
void WifiApp::ConfigureIntf(int port){
	if (port < 8880 || port > 8883) {
		ESP_LOGE(FNAME, "Invalid cfg: %d, should be between 8880 and 8883", port);
		return;
	}
	wifi_init_softap();
	int pidx = port-8880;
	sock_server_t *sock = _socks[pidx];  // particular pointer to socket server record for this port
	if ( sock == nullptr ) {  // its a one-way train, we can only create those ATM
		_socks[pidx] = sock = new sock_server_t(port, this);  // WiFiApp only once, all point to here
		int socket = WIFI_EVENT_HANDLER::create_socket( port );  // Create already the socket for this port structure as interface to the server task
		if( socket < 0 ) {
			ESP_LOGE(FNAME, "Socket creation port %d FAILED with (%d): Abort ConfigureIntf", port, socket );
			return;
		}
		fcntl(socket, F_SETFL, O_NONBLOCK); // socket should not block, so we can server several clients
		sock->socket = socket;   // store socket number in socket list
		ESP_LOGI(FNAME, "Socket creation successful socket: %d", socket );
		if( !task_created ){  // create the one an only task if not yet done
			ESP_LOGI(FNAME, "Task creation successful");
			xTaskCreatePinnedToCore(&WIFI_EVENT_HANDLER::socket_server, "wifitask", 4096, _socks, 8, &pid, 0);
			task_created = true;
		}
	}
}

int WifiApp::Send(const char *msg, int &len, int port)
{
	if (port < 8880 || port > 8884) {
		ESP_LOGE(FNAME, "Invalid port: %d, should be between 8880 and 8884", port);
		return -1;
	}
	// ESP_LOGI(FNAME, "port %d to sent %d: bytes, %s", port, len, msg );
	int socket_num=port-8880;
	sock_server_t *socks = _socks[socket_num];
	if( socks == nullptr ) {
		return -1;   // erratic port, socket unavail -> return, do not try ever again
	}
	// here we go
	full[socket_num]=true;  // init state means busy
	bool sendOK=false;
	for(auto &rec : socks->clients)  // iterate through all clients
	{
		if( rec.client >= 0 ){
			int num = send(rec.client, msg, len, MSG_DONTWAIT);
			// ESP_LOGI(FNAME, "client %d, num send %d", rec.client, num );
			if( num == len ){  // at least once we needed to sent the rest in one step for okay status
				sendOK = true;
				rec.retries = 0;
				full[socket_num]=false; // if at leat one client works, we say wifi is okay -> blue symbol
				// ESP_LOGI(FNAME, "tcp send to client %d (port: %d), bytes %d success", rec.client, config->port, num );
			}
			else {
				ESP_LOGI(FNAME, "tcp send to  %d (port: %d), bytes %d fail", rec.client, config->port, num );
			}
		}
	}
	if( sendOK ) {
		return 0;
	}
	len = 0;
	return 50;  // this port -> socket number is currently unavailable please try again 50 ms later
}

void WifiApp::wifi_init_softap()
{
	if ( ! netif_initialized ) {
		netif_initialized = true;
		ESP_ERROR_CHECK(esp_netif_init()); // can only be called once
		ESP_ERROR_CHECK(esp_event_loop_create_default());
		ESP_LOGV(FNAME,"now esp_netif_create_default_wifi_ap");
		esp_netif_create_default_wifi_ap();
		ESP_LOGV(FNAME,"now esp_wifi_init");
		wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
		ESP_ERROR_CHECK(esp_wifi_init(&cfg));

		ESP_LOGV(FNAME,"now esp_event_handler_instance_register");
		ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &WIFI_EVENT_HANDLER::wifi_event_handler, NULL, NULL));

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

		ESP_LOGV(FNAME,"now esp_wifi_set_mode");
		ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
		ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wc));

		ESP_LOGV(FNAME,"now esp_wifi_set_storage");
		ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
		// For further testing, may improve wifi range
		// ESP_ERROR_CHECK(esp_wifi_set_protocol(ESP_IF_WIFI_STA, WIFI_PROTOCOL_LR ));

		sleep(1);
		ESP_LOGI(FNAME,"now start WSP wifi access point");
		ESP_ERROR_CHECK(esp_wifi_start());
		ESP_ERROR_CHECK(esp_wifi_set_max_tx_power( int(wifi_max_power.get()*80.0/100.0) ));
		ESP_LOGV(FNAME, "wifi_init_softap finished SUCCESS. SSID:%s password:%s channel:%d", (char *)wc.ap.ssid, (char *)wc.ap.password, wc.ap.channel );
	}
}
