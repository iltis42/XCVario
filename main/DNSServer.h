/**
 * @file DNSServer.h
 * 
 * @author DeeFuse
 * @brief DNS-Server implementation for CaptivePortal functionality
 * @date 2023-05-08
 * 
 */

class cDNSServer
{
public:
    cDNSServer(cDNSServer const&) = delete;
    void operator=(cDNSServer const&) = delete;

    static cDNSServer& getInstance();
    void destroy();

    void start();
    void stop();

private:
    cDNSServer() {};

    static void taskEntry(void* arg);
    void task();

    static cDNSServer* m_instance;
    TaskHandle_t m_taskHandle = nullptr;
    int m_socket = 0;
    bool m_runTask = false;
};

#define DNSServer (cDNSServer::getInstance())