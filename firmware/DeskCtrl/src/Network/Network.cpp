#include "Network.h"
#include "../Syslog/Syslog.h"
#include <ESP8266WiFi.h>
#include <string>

static std::string IpToStr(IPAddress ip)
{
    uint32_t cursor = 0;
    char buff[48] = {0};

    for(int i = 0; i < 4; i++)
    {
        if(i > 0)
            cursor += snprintf(buff + cursor, sizeof(buff) - cursor, ".");

        cursor += snprintf(buff + cursor, sizeof(buff) - cursor, "%u", ip[i]);
    }
    
    return std::string(buff);
}

void Network::Connect(const char* ssid, const char* pass)
{
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(AP_SSID, AP_PASS);

    SYSLOG("Wifi AP launched on %s", IpToStr(WiFi.softAPIP()).c_str());

    WiFi.begin(ssid, pass);
}

std::string Network::GetAddr()
{
    return IpToStr(WiFi.localIP());
}

void Network::Pool()
{
    if(WiFi.status() != WL_CONNECTED)
    {
        if(m_connected)
        {
            SYSLOG("Wifi STA connection lost");
            m_connected = false;
        }

        return;
    }
    else
    {
        if(!m_connected)
        {
            SYSLOG("Wifi STA connection established (%s)", GetAddr().c_str());
            m_connected = true;
            m_server.Init();
        }
    }

    m_server.Pool();
}