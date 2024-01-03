#include "Api.h"
#include <ArduinoJson.h>
#include "../Syslog/Syslog.h"
#include "../DeskCtrl.h"

using namespace ArduinoJson;

static ApiResult Api_POST_Calibration(const std::string& body)
{
    StaticJsonDocument<256> req;

    auto res = deserializeJson(req, body);

    if(res != DeserializationError::Code::Ok)
        return {400, ""};

    auto height = req["height"].as<float>();

    if((height < 70) || (height > 120))
        return {400, ""};

    DeskCtrl::GetInstance()->CmdDeskCalibrate(height);

    return {200, ""};
}

static ApiResult Api_POST_DriveHeight(const std::string& body)
{
    StaticJsonDocument<256> req;

    auto res = deserializeJson(req, body);

    if(res != DeserializationError::Code::Ok)
        return {400, ""};

    auto height = req["height"].as<float>();

    if((height < 70) || (height > 120))
        return {400, ""};

    DeskCtrl::GetInstance()->CmdDeskGoTo(height);

    return {200, ""};
}

static ApiResult Api_POST_DriveDirection(const std::string& body)
{
    StaticJsonDocument<256> req;

    auto res = deserializeJson(req, body);

    if(res != DeserializationError::Code::Ok)
        return {400, ""};

    auto dir = req["direction"].as<std::string>();

    if(dir == "up")
    {
        DeskCtrl::GetInstance()->CmdDeskGoUp();
    }
    else
    {
        DeskCtrl::GetInstance()->CmdDeskGoDown();
    }
    

    return {200, ""};
}


static ApiResult Api_POST_DriveStop(const std::string& body)
{
    DeskCtrl::GetInstance()->CmdDeskStop();

    return {200, ""};
}


Api::Api()
{
    m_requests.push_back({"POST", "/calibration", Api_POST_Calibration});
    m_requests.push_back({"POST", "/drive/height", Api_POST_DriveHeight});
    m_requests.push_back({"POST", "/drive/direction", Api_POST_DriveDirection});
    m_requests.push_back({"POST", "/drive/stop", Api_POST_DriveStop});
}

ApiResult Api::ProcessRequest(const std::string& method, const std::string& path, const std::string& body)
{
    for(auto req : m_requests)
    {
        if(req.method != method)
            continue;

        if(req.path != path)
            continue;

        return req.handler(body);
    }

    return ApiResult(404, "");
}