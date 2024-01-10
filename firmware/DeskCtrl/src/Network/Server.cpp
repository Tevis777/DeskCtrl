#include "Server.h"
#include "Api.h"
#include <string>
#include <map>
#include "../Syslog/Syslog.h"

static const std::map<uint32_t, const char*> RESP_TEXTS = 
{
    {200, "OK"},
    {400, "Bad Request"},
    {404, "Not found"},
    {500, "Internal Server Error"},
};


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
    uint32_t contentLen = 0;
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

                SYSLOG("Http req: %s %s", reqMethod.c_str(), reqPath.c_str());

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
                    if(contentLen > 0)
                    {
                        state = EState::ReqBody;
                    }
                    else
                    {
                        state = EState::Handler;
                    }
                    
                    tmp = "";
                    break;
                }

                //SYSLOG("Http: %s: %s", name.c_str(), value.c_str());

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

                SYSLOG("Http req: %s", reqBody.c_str());
                state = EState::Handler;
                continue;
            }
            case EState::Handler:
            {   
                result = m_api.ProcessRequest(reqMethod, reqPath, reqBody);

                if(result.html)
                {
                    SYSLOG("Http resp: %u <html>", result.code);
                }
                else
                {
                    SYSLOG("Http resp: %u %s", result.code, result.body.c_str());
                }
                
                state = EState::RespLine;
                continue;
            }
            case EState::RespLine:
            {
                char buff[256];
                snprintf(buff, sizeof(buff), "HTTP/1.1 %u %s", result.code, RESP_TEXTS.at(result.code));
                client.println(buff);
                state = EState::RespHeaders;
                continue;
            }
            case EState::RespHeaders:
            {
                char buff[256];
                snprintf(buff, sizeof(buff), "Content-Length: %u", result.body.length());
                client.println(buff);

                if(result.html)
                {
                    client.println("Content-Type: text/html");
                }
                else
                {
                    client.println("Content-Type: application/json");
                }
                
                client.println("Access-Control-Allow-Origin: *");
                client.println("Connection: close");
                client.println();
                state = EState::RespBody;
                continue;
            }
            case EState::RespBody:
            {
                client.print(result.body.c_str());
                state = EState::Disconnect;
                continue;
            }
            case EState::Disconnect:
            default:
            {
                client.stop();
                return;
            }
        }
    }
}
