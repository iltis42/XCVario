#include <esp_http_server.h>


    enum class otaStatus : int8_t
    {
        ERROR = -1,
        IDLE = 0,
        DONE = 1,
    };


class cWebserver
{
public:
    cWebserver(cWebserver const&) = delete;
    void operator=(cWebserver const&) = delete;

    static cWebserver& getInstance();
    void destroy();

    void start();
    void stop();

    otaStatus getOtaStatus(){return m_otaStatus;}
    void setOtaStatus(otaStatus status){m_otaStatus=status;}
    int getOtaProgress(){return m_otaProgress;}
    void setOtaProgress(int progress){ m_otaProgress = progress;}

private:
    cWebserver() {};
    static cWebserver* m_instance;

    httpd_handle_t m_httpHandle= nullptr;
    otaStatus m_otaStatus = otaStatus::IDLE;
    int m_otaProgress = 0;
};

#define Webserver (cWebserver::getInstance())