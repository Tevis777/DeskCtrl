#include "Api.h"
#include <ArduinoJson.h>
#include "../Syslog/Syslog.h"
#include "../DeskCtrl.h"

using namespace ArduinoJson;

static ApiResult Api_Calibration(const std::string& body)
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

Api::Api()
{
    m_requests.push_back({"POST", "/calibration", Api_Calibration});
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