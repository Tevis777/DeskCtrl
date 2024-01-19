#line 1 "C:\\Workspace\\DeskCtrl\\firmware\\DeskCtrl\\src\\Config\\Config.h"
#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <vector>
#include <stdint.h>
#include <string>

struct Config
{

    static constexpr uint32_t MAX_PRESETS = 5;


    struct Wifi
    {
        std::string ssid;
        std::string pass;
        std::string ip;
        std::string gateway;
        std::string subnet;
    };

    struct Power
    {
        uint32_t timeout; //[min]
    };

    struct Drive
    {
        std::vector<uint32_t> presets; //[cm]
    };

    Wifi wifiAP;
    Wifi wifiSTA;
    Power power;
    Drive drive;
};

#endif //_CONFIG_H_