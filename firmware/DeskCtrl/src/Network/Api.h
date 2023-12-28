#ifndef _API_H_
#define _API_H_

#include <stdint.h>
#include <vector>
#include <string>
#include <functional>

struct ApiResult
{
    ApiResult() {};
    ApiResult(uint32_t cd, const std::string& bd) : code(cd), body(bd) {};

    uint32_t code = 0;
    std::string body;
};


class Api
{
public:
    Api();
    ApiResult ProcessRequest(const std::string& method, const std::string& path, const std::string& body);

private:

    using Handler = std::function<ApiResult(const std::string& body)>;

    struct Request
    {
        std::string method;
        std::string path;
        Handler handler;
    };


    std::vector<Request> m_requests;
};


#endif //_API_H_