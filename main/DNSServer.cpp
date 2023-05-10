/**
 * @file DNSServer.cpp
 * 
 * @author DeeFuse
 * @brief DNS-Server implementation for CaptivePortal functionality
 * @date 2023-05-08
 * 
 */

#include <sys/param.h>

#include <esp_log.h>
#include <esp_system.h>
#include <esp_netif.h>
#include <lwip/err.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/netdb.h>

#include "DNSServer.h"
#include "logdef.h"

#define DNS_PORT (53)
#define DNS_MAX_LEN (256)

#define OPCODE_MASK (0x7800)
#define QR_FLAG (1 << 7)
#define QD_TYPE_A (0x0001)
#define ANS_TTL_SEC (300)

// DNS Header Packet
typedef struct __attribute__((__packed__))
{
    uint16_t id;
    uint16_t flags;
    uint16_t qd_count;
    uint16_t an_count;
    uint16_t ns_count;
    uint16_t ar_count;
} dns_header_t;

// DNS Question Packet
typedef struct  __attribute__((__packed__))
{
    uint16_t type;
    uint16_t class_;
} dns_question_t;

// DNS Answer Packet
typedef struct __attribute__((__packed__))
{
    uint16_t ptr_offset;
    uint16_t type;
    uint16_t class_;
    uint32_t ttl;
    uint16_t addr_len;
    uint32_t ip_addr;
} dns_answer_t;

static char *parse_dns_name(char *raw_name, char *parsed_name, size_t parsed_name_max_len);
static int parse_dns_request(char *req, size_t req_len, char *dns_reply, size_t dns_reply_max_len);


cDNSServer* cDNSServer::m_instance = nullptr;

cDNSServer& cDNSServer::getInstance()
{
    if(m_instance == nullptr)
    {
        m_instance = new cDNSServer();
    }
    return *m_instance;
}

void cDNSServer::destroy()
{
    stop();
    if(m_instance != nullptr)
    {
        delete m_instance;
        m_instance = nullptr;
    }
}

void cDNSServer::start()
{
    ESP_LOGI(FNAME, "Starting DNS Server %p", m_instance);
    m_runTask = true;
    xTaskCreate(taskEntry, "DNSServer", 4096, this, 5, &m_taskHandle);
}

void cDNSServer::stop()
{
    m_runTask = false;
    if(m_socket > 0)
    {
        close(m_socket);
        m_socket = 0;
    }
}

void cDNSServer::taskEntry(void* arg)
{
    reinterpret_cast<cDNSServer*>(arg)->task();
}

void cDNSServer::task()
{
    char rx_buffer[128];
    char addr_str[128];
    int addr_family;
    int ip_protocol;

    while (m_runTask) {
        struct sockaddr_in dest_addr;
        dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(DNS_PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
        inet_ntoa_r(dest_addr.sin_addr, addr_str, sizeof(addr_str) - 1);

        int m_socket = socket(addr_family, SOCK_DGRAM, ip_protocol);
        if (m_socket < 0) {
            ESP_LOGE(FNAME, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(FNAME, "Socket created");

        int err = bind(m_socket, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err < 0) {
            ESP_LOGE(FNAME, "Socket unable to bind: errno %d", errno);
        }
        ESP_LOGI(FNAME, "Socket bound, port %d", DNS_PORT);

        while (m_runTask) {
            ESP_LOGI(FNAME, "Waiting for data");
            struct sockaddr_in6 source_addr; // Large enough for both IPv4 or IPv6
            socklen_t socklen = sizeof(source_addr);
            int len = recvfrom(m_socket, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

            // Error occurred during receiving
            if (len < 0) {
                ESP_LOGE(FNAME, "recvfrom failed: errno %d", errno);
                close(m_socket);
                break;
            }
            // Data received
            else {
                // Get the sender's ip address as string
                if (source_addr.sin6_family == PF_INET) {
                    inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
                } else if (source_addr.sin6_family == PF_INET6) {
                    inet6_ntoa_r(source_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
                }

                // Null-terminate whatever we received and treat like a string...
                rx_buffer[len] = 0;

                char reply[DNS_MAX_LEN];
                int reply_len = parse_dns_request(rx_buffer, len, reply, DNS_MAX_LEN);

                ESP_LOGI(FNAME, "Received %d bytes from %s | DNS reply with len: %d", len, addr_str, reply_len);
                if (reply_len <= 0) {
                    ESP_LOGE(FNAME, "Failed to prepare a DNS reply");
                } else {
                    int err = sendto(m_socket, reply, reply_len, 0, (struct sockaddr *)&source_addr, sizeof(source_addr));
                    if (err < 0) {
                        ESP_LOGE(FNAME, "Error occurred during sending: errno %d", errno);
                        break;
                    }
                }
            }
        }

        if (m_socket != -1) {
            ESP_LOGE(FNAME, "Shutting down socket");
            shutdown(m_socket, 0);
            close(m_socket);
        }
    }
    vTaskDelete(NULL);
}


/*
    Parse the name from the packet from the DNS name format to a regular .-seperated name
    returns the pointer to the next part of the packet
*/
static char *parse_dns_name(char *raw_name, char *parsed_name, size_t parsed_name_max_len)
{

    char *label = raw_name;
    char *name_itr = parsed_name;
    int name_len = 0;

    do {
        int sub_name_len = *label;
        // (len + 1) since we are adding  a '.'
        name_len += (sub_name_len + 1);
        if (name_len > parsed_name_max_len) {
            return NULL;
        }

        // Copy the sub name that follows the the label
        memcpy(name_itr, label + 1, sub_name_len);
        name_itr[sub_name_len] = '.';
        name_itr += (sub_name_len + 1);
        label += sub_name_len + 1;
    } while (*label != 0);

    // Terminate the final string, replacing the last '.'
    parsed_name[name_len - 1] = '\0';
    // Return pointer to first char after the name
    return label + 1;
}

// Parses the DNS request and prepares a DNS response with the IP of the softAP
static int parse_dns_request(char *req, size_t req_len, char *dns_reply, size_t dns_reply_max_len)
{
    if (req_len > dns_reply_max_len) {
        return -1;
    }

    // Prepare the reply
    memset(dns_reply, 0, dns_reply_max_len);
    memcpy(dns_reply, req, req_len);

    // Endianess of NW packet different from chip
    dns_header_t *header = (dns_header_t *)dns_reply;
    ESP_LOGD(FNAME, "DNS query with header id: 0x%X, flags: 0x%X, qd_count: %d", ntohs(header->id), ntohs(header->flags), ntohs(header->qd_count));

    // Not a standard query
    if ((header->flags & OPCODE_MASK) != 0) {
        return 0;
    }

    // Set question response flag
    header->flags |= QR_FLAG;

    uint16_t qd_count = ntohs(header->qd_count);
    header->an_count = htons(qd_count);

    int reply_len = qd_count * sizeof(dns_answer_t) + req_len;
    if (reply_len > dns_reply_max_len) {
        return -1;
    }

    // Pointer to current answer and question
    char *cur_ans_ptr = dns_reply + req_len;
    char *cur_qd_ptr = dns_reply + sizeof(dns_header_t);
    char name[128];

    // Respond to all questions with the ESP32's IP address
    for (int i = 0; i < qd_count; i++) {
        char *name_end_ptr = parse_dns_name(cur_qd_ptr, name, sizeof(name));
        if (name_end_ptr == NULL) {
            ESP_LOGE(FNAME, "Failed to parse DNS question: %s", cur_qd_ptr);
            return -1;
        }

        dns_question_t *question = (dns_question_t *)(name_end_ptr);
        uint16_t qd_type = ntohs(question->type);
        uint16_t qd_class = ntohs(question->class_);

        ESP_LOGI(FNAME, "Received type: %d | class: %d | Question for: %s", qd_type, qd_class, name);

        if (qd_type == QD_TYPE_A) {
            dns_answer_t *answer = (dns_answer_t *)cur_ans_ptr;

            answer->ptr_offset = htons(0xC000 | (cur_qd_ptr - dns_reply));
            answer->type = htons(qd_type);
            answer->class_ = htons(qd_class);
            answer->ttl = htonl(ANS_TTL_SEC);

            esp_netif_ip_info_t ip_info;
            if(strcmp("connectivitycheck.gstatic.com", name) == 0)  
            {
                ip_info.ip.addr = ESP_IP4TOUINT32(4,3,2,1); // Special case for (Samsung) Android - Private IP-Addresses won't trigger the login message
            }
            else
            {
                esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), &ip_info);    // Return ESPs IP for everybody else
            }
            ESP_LOGD(FNAME, "Answer with PTR offset: 0x%X and IP 0x%X", ntohs(answer->ptr_offset), ip_info.ip.addr);

            answer->addr_len = htons(sizeof(ip_info.ip.addr));
            answer->ip_addr = ip_info.ip.addr;
        }
    }
    return reply_len;
}
