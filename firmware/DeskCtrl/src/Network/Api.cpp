#include "Api.h"
#include "WebPage.h"
#include <ArduinoJson.h>
#include "../Syslog/Syslog.h"
#include "../DeskCtrl.h"
#include <cmath>

using namespace ArduinoJson;

static ApiResult Api_OPTIONS_Preflight(const std::string& body)
{
    return {200, ""};
}

static ApiResult Api_GET_WebPage(const std::string& body)
{
    return {200, GetWebPage(), true};
}


static ApiResult Api_GET_Health(const std::string& body)
{
    StaticJsonDocument<1024> resp;
    std::string respTxt;

    //Obtain data to access
    auto& config = DeskCtrl::GetInstance()->GetConfig();
    auto& motor = DeskCtrl::GetInstance()->GetMotor();
  
    //Status
    auto statusObj = resp.createNestedObject("status");
    statusObj["height"] = std::round(motor.GetHeight() * 100.0) / 100.0;


    //Config
    auto configObj = resp.createNestedObject("config");

    auto staObj = configObj.createNestedObject("wifiSTA");
    staObj["ssid"] = config.wifiSTA.ssid;
    staObj["pass"] = config.wifiSTA.pass;
    staObj["ip"] = config.wifiSTA.ip;
    staObj["gateway"] = config.wifiSTA.gateway;
    staObj["subnet"] = config.wifiSTA.subnet;


    auto apObj = configObj.createNestedObject("wifiAP");
    apObj["ssid"] = config.wifiAP.ssid;
    apObj["pass"] = config.wifiAP.pass;
    apObj["ip"] = config.wifiAP.ip;
    apObj["gateway"] = config.wifiAP.gateway;
    apObj["subnet"] = config.wifiAP.subnet;

    auto powerObj = configObj.createNestedObject("power");
    powerObj["timeout"] = config.power.timeout;

    auto driveObj = configObj.createNestedObject("drive");

    driveObj["presetsMax"] = Config::MAX_PRESETS;

    auto presetArr = driveObj.createNestedArray("presets");

    for(const auto& preset : config.drive.presets)
    {
        presetArr.add(preset);
    }

    serializeJson(resp, respTxt);

    return {200, respTxt};
}

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
    m_requests.push_back({"GET", "/", Api_GET_WebPage});
    m_requests.push_back({"GET", "/health", Api_GET_Health});
    m_requests.push_back({"POST", "/calibration", Api_POST_Calibration});
    m_requests.push_back({"POST", "/drive/height", Api_POST_DriveHeight});
    m_requests.push_back({"POST", "/drive/direction", Api_POST_DriveDirection});
    m_requests.push_back({"POST", "/drive/stop", Api_POST_DriveStop});
    m_requests.push_back({"OPTIONS", "/", Api_OPTIONS_Preflight});
    m_requests.push_back({"OPTIONS", "/health", Api_OPTIONS_Preflight});
    m_requests.push_back({"OPTIONS", "/calibration", Api_OPTIONS_Preflight});
    m_requests.push_back({"OPTIONS", "/drive/height", Api_OPTIONS_Preflight});
    m_requests.push_back({"OPTIONS", "/drive/direction", Api_OPTIONS_Preflight});
    m_requests.push_back({"OPTIONS", "/drive/stop", Api_OPTIONS_Preflight});
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