#ifndef _SERVER_H_
#define _SERVER_H_

#include <ESP8266WiFi.h>
#include <stdint.h>
#include "Api.h"

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
        Handler,
        RespLine,
        RespHeaders,
        RespBody,
    };



    WiFiServer m_server{80};
    Api m_api;
};

#endif //_SERVER_H_