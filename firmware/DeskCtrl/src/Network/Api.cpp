#include "Api.h"
#include <ArduinoJson.h>
#include "../Syslog/Syslog.h"

using namespace ArduinoJson;

static ApiResult Api_Calibration(const std::string& body)
{
    StaticJsonDocument<256> req;

    auto res = deserializeJson(req, body);

    if(res != DeserializationError::Code::Ok)
        return {400, ""};

    auto val1 = req["val1"].as<std::string>();
    auto val2 = req["val2"].as<int>();

    SYSLOG("Deserialized val1: %s, val2: %u", val1.c_str(), val2);

    StaticJsonDocument<256> resp;
    std::string respStr;

    resp["sensor"] = "gps";
    resp["time"]   = 1351824120;
    resp["data"][0] = 48.756080;
    resp["data"][1] = 2.302038;

    serializeJson(resp, respStr);

    return {200, respStr};
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