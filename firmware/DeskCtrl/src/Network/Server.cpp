#include "Server.h"
#include "Api.h"
#include <string>
#include "../Syslog/Syslog.h"


void HttpServer::Init()
{
    m_server.begin();
}

static std::string ParseReqLineMethod(const std::string& reqLine)
{
    auto pos = reqLine.find(" ");
    auto res = reqLine.substr(0, pos);
    return res;
}

static std::string ParseReqLinePath(const std::string& reqLine)
{
    auto pos = reqLine.find(" ");
    auto res = reqLine.substr(pos);
    pos = res.find(" ");
    res = res.substr(0, pos);
    return res;
}


void HttpServer::Pool()
{
    WiFiClient client = m_server.available();
    EState state = EState::ReqLine;

    if(!client)
        return;

    std::string tmp;

    std::string reqMethod;
    std::string reqPath;
    std::string reqBody;

    while(client.connected())
    {
        switch(state)
        {
            case EState::ReqLine:
            {
                if(!client.available())
                    continue;

                tmp += client.read();

                SYSLOG("new char %c", tmp.back());

                if(tmp.back() != '\n')
                    continue;

                reqMethod = ParseReqLineMethod(tmp);
                reqPath = ParseReqLinePath(tmp);

                SYSLOG("New request %s %s", reqMethod.c_str(), reqPath.c_str());
                return;
            }
            case EState::ReqHeaders:
            {


            }
            case EState::ReqBody:
            {


            }
            case EState::RespHeaders:
            {


            }
            case EState::RespBody:
            {


            }



        }
    }
}