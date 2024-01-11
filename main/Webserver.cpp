#include <esp_ota_ops.h>

// #include "SetupCommon.h"
#include "Webserver.h"
#include "logdef.h"
#include "coredump_to_server.h"

cWebserver* cWebserver::m_instance = nullptr;
extern char * program_version;
extern bool do_factory_reset();
extern void send_config( httpd_req *req );
extern int restore_config( int len, char *data );
extern float get_speed2Fly_bound();
extern void set_speed2Fly_bound(float speed2FlyBound);
extern float get_gLoad_upper_bound();
extern void set_gLoad_upper_bound(float gLoadUpperBound);
extern float get_gLoad_lower_bound();
extern void set_gLoad_lower_bound(float gLoadLowerBound);
extern void do_excess_purge();
extern uint16_t get_row_count();
extern std::vector<float> get_row(uint16_t index);

// file assets
extern const uint8_t index_html_start[]             asm("_binary_index_html_start");
extern const uint8_t index_html_end[]               asm("_binary_index_html_end");
extern const uint8_t index_html_gz_start[]          asm("_binary_index_html_gz_start");
extern const uint8_t index_html_gz_end[]            asm("_binary_index_html_gz_end");
extern const uint8_t milligram_min_css_start[]      asm("_binary_milligram_min_css_start");
extern const uint8_t milligram_min_css_end[]        asm("_binary_milligram_min_css_end");
extern const uint8_t milligram_min_css_gz_start[]   asm("_binary_milligram_min_css_gz_start");
extern const uint8_t milligram_min_css_gz_end[]     asm("_binary_milligram_min_css_gz_end");

// http handlers
static esp_err_t GET_index_html_handler(httpd_req_t *req);
static esp_err_t GET_milligram_min_css_handler(httpd_req_t *req);
static esp_err_t GET_status_json_handler(httpd_req_t *req);
static esp_err_t POST_update_handler(httpd_req_t *req);
static esp_err_t GET_backup_handler(httpd_req_t *req);
static esp_err_t POST_restore_handler(httpd_req_t *req);
static esp_err_t GET_load_static_handler(httpd_req_t *req);
static esp_err_t PUT_save_static_handler(httpd_req_t *req);
static esp_err_t GET_download_handler(httpd_req_t *req);
static esp_err_t DELETE_clear_handler(httpd_req_t *req);
static esp_err_t DELETE_reset_handler(httpd_req_t *req);
static esp_err_t GET_coredump_handler(httpd_req_t *req);

httpd_uri_t GET_index_html = {
	.uri = "/",
	.method = HTTP_GET,
	.handler = GET_index_html_handler,
	.user_ctx = NULL
};

httpd_uri_t GET_milligram_min_css = {
	.uri = "/milligram.min.css",
	.method = HTTP_GET,
	.handler = GET_milligram_min_css_handler,
	.user_ctx = NULL
};

httpd_uri_t GET_stats_json = {
	.uri = "/status.json",
	.method = HTTP_GET,
	.handler = GET_status_json_handler,
	.user_ctx = NULL
};

httpd_uri_t POST_update = {
	.uri = "/update",
	.method = HTTP_POST,
	.handler = POST_update_handler,
	.user_ctx = NULL
};

httpd_uri_t GET_backup = {
	.uri = "/backup",
	.method = HTTP_GET,
	.handler = GET_backup_handler,
	.user_ctx = NULL
};

httpd_uri_t POST_restore = {
	.uri = "/restore",
	.method = HTTP_POST,
	.handler = POST_restore_handler,
	.user_ctx = NULL
};

httpd_uri_t GET_load_static = {
	.uri = "/loadStatic",
	.method = HTTP_GET,
	.handler = GET_load_static_handler,
	.user_ctx = NULL
};

httpd_uri_t PUT_save_static = {
	.uri = "/saveStatic",
	.method = HTTP_PUT,
	.handler = PUT_save_static_handler,
	.user_ctx = NULL
};

httpd_uri_t GET_download = {
	.uri = "/download",
	.method = HTTP_GET,
	.handler = GET_download_handler,
	.user_ctx = NULL
};

httpd_uri_t DELETE_clear = {
	.uri = "/clear",
	.method = HTTP_DELETE,
	.handler = DELETE_clear_handler,
	.user_ctx = NULL
};

httpd_uri_t DELETE_reset = {
	.uri = "/reset",
	.method = HTTP_DELETE,
	.handler = DELETE_reset_handler,
	.user_ctx = NULL
};

httpd_uri_t GET_coredump = {
	.uri = "/coredump",
	.method = HTTP_GET,
	.handler = GET_coredump_handler,
	.user_ctx = NULL
};

cWebserver& cWebserver::getInstance()
{
    if(m_instance == nullptr)
    {
        m_instance = new cWebserver();
    }
    return *m_instance;
}

void cWebserver::destroy()
{
    if(m_instance != nullptr)
    {
        delete m_instance;
        m_instance = nullptr;
    }
}

void cWebserver::start()
{
    if(m_httpHandle != nullptr)
    {
        // Webserver already active - return
        return;
    }

    ESP_LOGW(FNAME, "heap: %d", esp_get_free_heap_size());

	httpd_config_t config = HTTPD_DEFAULT_CONFIG();
		
	// Lets bump up the stack size (default was 4096)
	config.stack_size = 8192;
	config.max_uri_handlers = 10;
	
	// Start the httpd server
	ESP_LOGI(FNAME, "Starting http server on port: '%d'", config.server_port);

	if (httpd_start(&m_httpHandle, &config) == ESP_OK) 
	{
		// Set URI handlers
		ESP_LOGI(FNAME, "Registering URI handlers");
		httpd_register_uri_handler(m_httpHandle, &GET_index_html);
		httpd_register_uri_handler(m_httpHandle, &GET_milligram_min_css);
		httpd_register_uri_handler(m_httpHandle, &GET_stats_json);
		httpd_register_uri_handler(m_httpHandle, &POST_update);
		httpd_register_uri_handler(m_httpHandle, &GET_backup);
		httpd_register_uri_handler(m_httpHandle, &POST_restore);
		httpd_register_uri_handler(m_httpHandle, &GET_load_static);
		httpd_register_uri_handler(m_httpHandle, &PUT_save_static);
		httpd_register_uri_handler(m_httpHandle, &GET_download);
		httpd_register_uri_handler(m_httpHandle, &DELETE_clear);
		httpd_register_uri_handler(m_httpHandle, &DELETE_reset);
	    httpd_register_uri_handler(m_httpHandle, &GET_coredump);
	}
    else
    {
	    ESP_LOGE(FNAME, "Error starting http server!");
    }
}

void cWebserver::stop()
{
    httpd_stop(m_httpHandle);
    m_httpHandle = nullptr;
}

/**
 * @brief HTTP Handlers
 * 
 */

// GET /index.html
static esp_err_t GET_index_html_handler(httpd_req_t *req)
{
	ESP_LOGI(FNAME, "index.html Requested");

	httpd_resp_set_type(req, "text/html");

#if CONFIG_WEBSERVER_USE_GZIP
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
	httpd_resp_send(req, (const char *)index_html_gz_start, index_html_gz_end - index_html_gz_start);
#else
	httpd_resp_send(req, (const char *)index_html_start, index_html_end - index_html_start);
#endif

	return ESP_OK;
}

// GET /milligram.min.css
static esp_err_t GET_milligram_min_css_handler(httpd_req_t *req)
{
	ESP_LOGI(FNAME, "milligram.min.css Requested");

	httpd_resp_set_type(req, "text/css");

#if CONFIG_WEBSERVER_USE_GZIP 
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
	httpd_resp_send(req, (const char *)milligram_min_css_gz_start, milligram_min_css_gz_end - milligram_min_css_gz_start);
#else
	httpd_resp_send(req, (const char *)milligram_min_css_start, milligram_min_css_end - milligram_min_css_start);
#endif

	return ESP_OK;
}

// GET /status.json
static esp_err_t GET_status_json_handler(httpd_req_t *req)
{
  	ESP_LOGI(FNAME, "status.json Requested");

	char jsonBuffer[200];
    const char json[] = R"({"compile_time":"%s","compile_date":"%s","program_version":"%s","ota_status":"%d","coredump_available":"%d"})";

	snprintf(jsonBuffer, 199, json, __TIME__, __DATE__, program_version, 0, coredump_available());

	httpd_resp_set_type(req, "application/json ");
	httpd_resp_send(req, jsonBuffer, strlen(jsonBuffer));

	return ESP_OK;
}

bool otaStarted = false;
char *otaBuffer = nullptr;
const esp_partition_t *otaUpdatePartition;
esp_ota_handle_t otaHandle = {0};
size_t otaSize = 0;
size_t otaReceived = 0;

// Receive .Bin file
static esp_err_t POST_update_handler(httpd_req_t *req)
{
    size_t ota_buff_size = 16 * 1024;

    size_t content_length = req->content_len;
    int content_received = 0;
    int recv_len;

    if (content_length < 1)
    {
        return ESP_OK;
    }

    // Received first chunk - start OTA procedure
    if (!otaStarted)
    {
        otaStarted = true;
        otaBuffer = (char*)malloc(ota_buff_size);

        // Get total OTA file size
        size_t buf_len = httpd_req_get_hdr_value_len(req, "X-OTA-SIZE") + 1;
        if (buf_len > 1)
        {
            char otaSizeBuffer[32];
            if (httpd_req_get_hdr_value_str(req, "X-OTA-SIZE", otaSizeBuffer, sizeof(otaSizeBuffer)) == ESP_OK)
            {
                otaSize = atoi(otaSizeBuffer);
                ESP_LOGI(FNAME, "Found header => X-OTA-SIZE: %s", otaSizeBuffer);
            }
        }

        otaUpdatePartition = esp_ota_get_next_update_partition(NULL);

        esp_err_t err = esp_ota_begin(otaUpdatePartition, otaSize, &otaHandle);
        if (err != ESP_OK)
        {
            ESP_LOGE(FNAME, "Error With OTA Begin, Cancelling OTA");
            return ESP_FAIL;
        }
        else
        {
            ESP_LOGI(FNAME, "Writing to partition subtype %d at offset 0x%x", otaUpdatePartition->subtype, otaUpdatePartition->address);
        }
    }

    do
    {
        // Read the ota data
        if ((recv_len = httpd_req_recv(req, otaBuffer, std::min(content_length, ota_buff_size))) < 0)
        {
            if (recv_len == HTTPD_SOCK_ERR_TIMEOUT)
            {
                ESP_LOGW(FNAME, "Socket Timeout");
                /* Retry receiving if timeout occurred */
                continue;
            }
            ESP_LOGE(FNAME, "OTA Other Error %d", recv_len);
            return ESP_FAIL;
        }

        // Write OTA data
        esp_ota_write(otaHandle, otaBuffer, recv_len);
        content_received += recv_len;
        otaReceived += recv_len;

    } while (recv_len > 0 && content_received < content_length);

    Webserver.setOtaProgress((otaReceived * 100.0f) / otaSize); 
    ESP_LOGI(FNAME, "Received %d / %d (%.02f)", otaReceived, otaSize, (otaReceived * 100.0) / otaSize);

    if (otaReceived >= otaSize)
    {
        free(otaBuffer);
        otaStarted = false;
        // End response
        httpd_resp_set_type(req, "application/json");
        httpd_resp_send(req, "OK", strlen("OK"));

        if (esp_ota_end(otaHandle) == ESP_OK)
        {
            // Lets update the partition
            if (esp_ota_set_boot_partition(otaUpdatePartition) == ESP_OK)
            {
                const esp_partition_t *boot_partition = esp_ota_get_boot_partition();

                ESP_LOGI(FNAME, "Next boot partition subtype %d at offset 0x%x", boot_partition->subtype, boot_partition->address);
                ESP_LOGI(FNAME, "Rebooting in 3 seconds...");
                
                Webserver.setOtaStatus(otaStatus::DONE);
            }
            else
            {
                ESP_LOGE(FNAME, "\r\n\r\n !!! Flashed Error !!!\r\n");
            }
        }
        else
        {
            ESP_LOGE(FNAME, "\r\n\r\n !!! OTA End Error !!!\r\n");
        }
    }
    else
    {
        httpd_resp_send(req, NULL, 0);
    }

    return ESP_OK;
}

static esp_err_t GET_backup_handler(httpd_req_t *req)
{
	ESP_LOGI(FNAME, "Backup Requested");

	httpd_resp_set_type(req, "text/html");
    // SetupCommon::giveConfigChanges(req);
    send_config(req);
	return ESP_OK;
}

static esp_err_t POST_restore_handler(httpd_req_t *req)
{
	const size_t max_len = 8192;
	ESP_LOGI(FNAME, "Restore Requested %d", req->content_len );
	char *buff = (char*)malloc(max_len);
	int recv_len;
	if ((recv_len = httpd_req_recv(req, buff, std::min(req->content_len, max_len ))) < 0){
		return ESP_FAIL;
	}
    ESP_LOGI(FNAME, "Len %d", recv_len );
	httpd_resp_set_type(req, "text/html");
	buff[recv_len] = '\0';
	int items = restore_config( recv_len, buff );
	if( items ){
		char res[80];
		sprintf(res,"%d config items restored successfully", items );
		ESP_LOGI(FNAME, "Config Items %d, strlen: %d", items, strlen(res) );
		httpd_resp_send(req, res, strlen(res) );
	}
	else{
		char res[40] = "Config file format error!";
		ESP_LOGI(FNAME, "%s strlen: %d", res, strlen(res) );
		httpd_resp_send(req,res, strlen(res) );
	}

    free(buff);
	return ESP_OK;
}

static esp_err_t GET_load_static_handler(httpd_req_t *req) 
{
	const int size = 200; // Format str len 80 
						  // 3x %X.Xf: -15
						  // Speed: 4 + 2 + 1 = 7
						  // G-Load: 2 + 4 + 1 = 7
						  // Duration: 3 + 4 + 1 = 8
	char jsonBuffer[size]; 
    const char json[] = R"({"speedUpperBound":"%4.2f","gLoadLowerBound":"%2.4f","gLoadUpperBound":"%3.4f"})";

	snprintf(
		jsonBuffer, 
		size - 1, 
		json, 
		get_speed2Fly_bound(), 
		get_gLoad_lower_bound(), 
		get_gLoad_upper_bound()
	);

	httpd_resp_set_type(req, "application/json ");
	httpd_resp_send(req, jsonBuffer, strlen(jsonBuffer));

	return ESP_OK;
}

static esp_err_t PUT_save_static_handler(httpd_req_t *req) 
{
	const size_t buffSize = req->content_len + 1;
	if (buffSize <= 1 || buffSize > 254) {
		ESP_LOGE(FNAME, "The request was to huge or zero %d", buffSize);
		return ESP_FAIL;
	}


	char *buffer = (char *)malloc(sizeof(char) * buffSize);
	
	bool wasSuccessfull = false;
	int try_counter = 0;
	do
    {
		int recv_len = httpd_req_recv(req, buffer, buffSize);
		// Read the excess configuration
		if (recv_len < 0)
		{
			if (recv_len == HTTPD_SOCK_ERR_TIMEOUT)
			{
				ESP_LOGW(FNAME, "Socket Timeout");
				/* Retry receiving if timeout occurred */
			} else {
				ESP_LOGE(FNAME, "Excess Config Other Error %d", recv_len);
				free(buffer);
				return ESP_FAIL;
			}
		} else {
			wasSuccessfull = true;
		}

		try_counter++;
		if (try_counter > 10) {
			ESP_LOGE(FNAME, "Unable to request Config %d", recv_len);
			free(buffer);
			return ESP_FAIL;
		}
    } while (!wasSuccessfull);


	cJSON *root = cJSON_Parse(buffer);
	if (root == NULL) {
		free(buffer);
		ESP_LOGE(FNAME, "Unable to parse JSON buffer: %s", buffer);
		return ESP_FAIL;
	}
	cJSON *speedUpperBound = cJSON_GetObjectItem(root, "speedUpperBound");
	if (speedUpperBound == NULL) {
		free(buffer);
		ESP_LOGE(FNAME, "Json Buffer has no speedUpperBound property");
		return ESP_FAIL;
	}
	cJSON *gLoadUpperBound = cJSON_GetObjectItem(root, "gLoadUpperBound");
	if (gLoadUpperBound == NULL) {
		free(buffer);
		ESP_LOGE(FNAME, "Json Buffer has no gLoadUpperBound property");
		return ESP_FAIL;
	}
	cJSON *gLoadLowerBound = cJSON_GetObjectItem(root, "gLoadLowerBound");
	if (gLoadLowerBound == NULL) {
		free(buffer);
		ESP_LOGE(FNAME, "Json Buffer has no gLoadLowerBound property");
		return ESP_FAIL;
	}

	set_speed2Fly_bound((float) speedUpperBound->valuedouble);
	set_gLoad_upper_bound((float) gLoadUpperBound->valuedouble);
	set_gLoad_upper_bound((float) gLoadLowerBound->valuedouble);

	free(buffer);
	return ESP_OK;
}

static esp_err_t GET_download_handler(httpd_req_t *req)
{
	ESP_LOGI(FNAME, "Exceedance Download Requested");

	// The result will be interpreted as CSV
	httpd_resp_set_type(req, "text/html");

	// Transmit column names
	const char *columnNames = "Speed, G-Load, Duration_S\n";
	httpd_resp_send_chunk(req, columnNames, strlen(columnNames));
	
	// Transmit all rows
	const int row_str_len = 26; // Speed: 4 + 2 + 1 = 7
								// G-Load: 2 + 4 + 1 = 7
								// Duration: 3 + 4 + 1 = 8
								// rest (", ", newLine, zero terminator): 4
    char row_str[row_str_len];
	uint16_t row_count = get_row_count();
	for (uint16_t i = 0; i < row_count; i++) {

		std::vector<float> row = get_row(i);

		if (row[0] != 0.0f && row[1] != 0.0f) {
			snprintf( // avoid buffer overflow errors
				row_str, 
				row_str_len, 
				"%4.2f, %2.4f, %3.4f\n", 
				row[0], 
				row[1],
				row[2]);
			httpd_resp_send_chunk(req, row_str, strlen(row_str));
		}
	}

	// Terminate transmission
	httpd_resp_send_chunk(req, row_str, 0);
	
	return ESP_OK;
}

static esp_err_t DELETE_clear_handler(httpd_req_t *req)
{
	ESP_LOGI(FNAME, "Exceedance Delete Requested");
	do_excess_purge();	
	return ESP_OK;
}

static esp_err_t DELETE_reset_handler(httpd_req_t *req)
{
    ESP_LOGI(FNAME, "Clear Settings Requested");
	// bool success = SetupCommon::factoryReset();
    bool success = do_factory_reset();

	httpd_resp_set_type(req, "text/html");
	if( success )
		httpd_resp_send(req, "Okay", 5 );
	else
		httpd_resp_send(req, "Error", 6 );

	vTaskDelay(5000 / portTICK_PERIOD_MS);
	esp_restart();

	return ESP_OK;
}

static esp_err_t _coredump_to_server_begin_cb_OTA(void * priv)
{
	char ver[128];
	sprintf( ver, "Software Version: %s\r\n", program_version );
	httpd_resp_send_chunk((httpd_req*)priv, ver, strlen( ver ) );
	const char *head="================= CORE DUMP START =================\r\n";
	httpd_resp_send_chunk((httpd_req*)priv, head, strlen( head ) );
    return ESP_OK;
}

static esp_err_t _coredump_to_server_end_cb_OTA(void * priv)
{
	const char *end="================= CORE DUMP END =================\r\n";
	httpd_resp_send_chunk((httpd_req*)priv, end, strlen( end ) );
	httpd_resp_send_chunk((httpd_req*)priv, end, 0 );
	return ESP_OK;
}

static esp_err_t _coredump_to_server_write_cb_OTA(void * priv, char const * const str)
{
	ESP_LOGI("OTA", "coredump_to_server_write_cb, size %d bytes; %s", strlen( str ), str );
	httpd_resp_send_chunk((httpd_req*)priv, str, strlen( str ) );
	httpd_resp_send_chunk((httpd_req*)priv, "\r\n", 2 );
	vTaskDelay(50 / portTICK_PERIOD_MS);
    return ESP_OK;
}

static void send_coredump( httpd_req *req ) {
	 coredump_to_server_config_t coredump_cfg = {
	        .start = _coredump_to_server_begin_cb_OTA,
	        .end = _coredump_to_server_end_cb_OTA,
	        .write = _coredump_to_server_write_cb_OTA,
	        .priv = req,
	    };
	 ESP_LOGI("OTA", "priv %08x", (unsigned int)req );
	 if( coredump_to_server(&coredump_cfg) != ESP_OK ){ // Dump to Webserver
		 httpd_resp_send(req, "", 0 );
	 }
}

static esp_err_t GET_coredump_handler(httpd_req_t *req)
{
    ESP_LOGI(FNAME, "Get Core File Requested");

	httpd_resp_set_type(req, "text/html");
	send_coredump( req );

	clear_coredump();
	return ESP_OK;
}
