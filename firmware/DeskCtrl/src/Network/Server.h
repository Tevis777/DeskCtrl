#ifndef _SERVER_H_
#define _SERVER_H_

#include <ESP8266WiFi.h>
#include <stdint.h>

class HttpServer
{
public:
    void Init();
    void Pool();

private:

    enum class EState
    {
        ReqLine,
        ReqHeaders,
        ReqBody,
        RespHeaders,
        RespBody,
    };



    WiFiServer m_server{80};
};

#endif //_SERVER_H_