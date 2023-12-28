#include "Api.h"
#include "../Syslog/Syslog.h"

static ApiResult Api_Calibration(const std::string& body)
{
    SYSLOG("API CALIBRATION CALLED!!!!");

    return {200, "My response body hehe"};
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