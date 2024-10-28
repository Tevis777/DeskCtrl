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
    StaticJsonDocument<1024> root;
    std::string respTxt;

    //Obtain data to access
    auto& config = DeskCtrl::GetInstance()->GetConfig();
    auto& motor = DeskCtrl::GetInstance()->GetMotor();
  
    //Status
    auto statusObj = root.createNestedObject("status");
    statusObj["height"] = std::round(motor.GetHeight() * 100.0) / 100.0;


    //Config
    auto configObj = root.createNestedObject("config");

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

    serializeJson(root, respTxt);

    return {200, respTxt};
}

static ApiResult Api_POST_Restart(const std::string& body)
{
    DeskCtrl::GetInstance()->CmdDeskStop();
    ESP.restart();
    return {200, ""};
}

static ApiResult Api_POST_Config(const std::string& body)
{
    StaticJsonDocument<1024> root;

    auto res = deserializeJson(root, body);

    if(res != DeserializationError::Code::Ok)
        return {400, ""};

    auto& config = DeskCtrl::GetInstance()->GetConfig();

    auto configObj = root["config"].as<JsonObject>();

    if(configObj.containsKey("wifiSTA"))
    {
        auto obj = configObj["wifiSTA"].as<JsonObject>();
        if(obj.containsKey("ssid"))     config.wifiSTA.ssid     = obj["ssid"].as<std::string>();
        if(obj.containsKey("pass"))     config.wifiSTA.pass     = obj["pass"].as<std::string>();
        if(obj.containsKey("ip"))       config.wifiSTA.ip       = obj["ip"].as<std::string>();
        if(obj.containsKey("gateway"))  config.wifiSTA.gateway  = obj["gateway"].as<std::string>();
        if(obj.containsKey("subnet"))   config.wifiSTA.subnet   = obj["subnet"].as<std::string>();
    }

    if(configObj.containsKey("wifiAP"))
    {
        auto obj = configObj["wifiAP"].as<JsonObject>();
        if(obj.containsKey("ssid"))     config.wifiAP.ssid     = obj["ssid"].as<std::string>();
        if(obj.containsKey("pass"))     config.wifiAP.pass     = obj["pass"].as<std::string>();
        if(obj.containsKey("ip"))       config.wifiAP.ip       = obj["ip"].as<std::string>();
        if(obj.containsKey("gateway"))  config.wifiAP.gateway  = obj["gateway"].as<std::string>();
        if(obj.containsKey("subnet"))   config.wifiAP.subnet   = obj["subnet"].as<std::string>();
    }

    if(configObj.containsKey("power"))
    {
        auto obj = configObj["power"].as<JsonObject>();

        if(obj.containsKey("timeout"))
            config.power.timeout = obj["timeout"].as<uint32_t>();
    }

    if(configObj.containsKey("drive"))
    {
        auto obj = configObj["drive"].as<JsonObject>();

        if(obj.containsKey("presets"))
        {
            config.drive.presets.clear();

            for (JsonVariant value : obj["presets"].as<JsonArray>())
            {   
                if(value == nullptr)
                    continue;

                config.drive.presets.push_back(value);
            }

            std::sort(config.drive.presets.begin(), config.drive.presets.end());
        }       
    }

    DeskCtrl::GetInstance()->CmdConfigSave();

    return {200, ""};
}

static ApiResult Api_POST_DriveCalibration(const std::string& body)
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
    m_requests.push_back({"POST", "/restart", Api_POST_Restart});
    m_requests.push_back({"POST", "/config", Api_POST_Config});
    m_requests.push_back({"POST", "/drive/calibration", Api_POST_DriveCalibration});
    m_requests.push_back({"POST", "/drive/height", Api_POST_DriveHeight});
    m_requests.push_back({"POST", "/drive/direction", Api_POST_DriveDirection});
    m_requests.push_back({"POST", "/drive/stop", Api_POST_DriveStop});
    m_requests.push_back({"OPTIONS", "/", Api_OPTIONS_Preflight});
    m_requests.push_back({"OPTIONS", "/health", Api_OPTIONS_Preflight});
    m_requests.push_back({"OPTIONS", "/config", Api_OPTIONS_Preflight});
    m_requests.push_back({"OPTIONS", "/drive/calibration", Api_OPTIONS_Preflight});
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