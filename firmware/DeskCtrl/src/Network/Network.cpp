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

static IPAddress StrToIp(const std::string& str)
{
    uint32_t counter = 0;
    uint32_t cursor = 0;
    uint8_t vals[4] = {0};
    char tmp[4] = {0};

    for(const auto& chr : str)
    {
        if(chr == '.')
        {
            vals[counter] = atoi(tmp);
            counter++;
            cursor = 0;
            tmp[0] = 0;
            continue;
        }

        cursor += snprintf(tmp + cursor, sizeof(tmp) - cursor, "%c", chr);
    }

    vals[counter] = atoi(tmp);

    return IPAddress(vals[0], vals[1], vals[2], vals[3]);
}

void Network::Connect(const char* ssid, const char* pass, const char* ip, const char* gateway, const char* subnet)
{
    WiFi.mode(WIFI_AP_STA);

    WiFi.softAPConfig(StrToIp(AP_IP), StrToIp(AP_IP), StrToIp(AP_SUBNET));
    WiFi.softAP(AP_SSID, AP_PASS);

    WiFi.config(StrToIp(ip), StrToIp(gateway), StrToIp(subnet), StrToIp(gateway), StrToIp(gateway));
    WiFi.begin(ssid, pass);

    SYSLOG("Wifi AP launched on %s", IpToStr(WiFi.softAPIP()).c_str());
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