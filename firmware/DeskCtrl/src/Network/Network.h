#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "Server.h"

class Network
{
public:
    void Connect(const char* ssid, const char* pass);
    void Pool();
    std::string GetAddr();

private:
    static constexpr const char* AP_SSID = "DeskCtrl";
    static constexpr const char* AP_PASS = "LubiePlacki666";

    bool m_connected = false;
    HttpServer m_server;
};

#endif //_NETWORK_H_