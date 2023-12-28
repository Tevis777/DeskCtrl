#include "Server.h"
#include "Api.h"
#include <string>
#include "../Syslog/Syslog.h"


void HttpServer::Init()
{
    m_server.begin();
}

static void ParseReqLine(const std::string& line, std::string& method, std::string& path)
{
    uint32_t spaces = 0;

    method = "";
    path = "";

    for(const auto& chr : line)
    {
        if(chr == ' ')
        {
            spaces++;
            continue;
        }

        if(spaces == 0)
        {
            method += chr;
        }
        else if(spaces == 1)
        {
            path += chr;
        }
        else
        {
            return;
        }
    }
}

static void ParseHeader(const std::string& line, std::string& name, std::string& value)
{
    bool printVal = false;
    bool firstVal = false;

    name = "";
    value = "";

    for(const auto& chr : line)
    {
        if((chr == '\r') || (chr == '\n'))
            return;

        if(!printVal)
        {
            if((chr == ':'))
            {
                printVal = true;
                firstVal = true;
                continue;
            }

            name += chr;
        }
        else
        {
            if((chr == ' ' && firstVal))
            {
                firstVal = false;
                continue;
            }

            value += chr;
        }
    }
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
    uint32_t contentLen;
    ApiResult result;

    while(client.connected())
    {
        switch(state)
        {
            case EState::ReqLine:
            {
                if(!client.available())
                    continue;

                tmp += client.read();

                if(tmp.back() != '\n')
                    continue;

                ParseReqLine(tmp, reqMethod, reqPath);

                SYSLOG("%s %s", reqMethod.c_str(), reqPath.c_str());

                state = EState::ReqHeaders;
                tmp = "";
                break;
            }
            case EState::ReqHeaders:
            {
                if(!client.available())
                    continue;

                tmp += client.read();

                if(tmp.back() != '\n')
                    continue;

                std::string name;
                std::string value;

                ParseHeader(tmp, name, value);

                if(name.empty())
                {
                    state = EState::ReqBody;
                    tmp = "";
                    break;
                }

                //SYSLOG("%s: %s", name.c_str(), value.c_str());

                if(name == "Content-Length")
                {
                    contentLen = atoi(value.c_str());
                }

                tmp = "";
                continue;
            }
            case EState::ReqBody:
            {
                if(!client.available())
                    continue;

                reqBody += client.read();

                if(reqBody.length() < contentLen)
                    continue;

                SYSLOG("%s", reqBody.c_str());
                state = EState::Handler;
                continue;
            }
            case EState::Handler:
            {   
                m_api.ProcessRequest(reqMethod, reqPath, reqBody);
                return;
            }
            case EState::RespLine:
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