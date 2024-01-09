#line 1 "C:\\Workspace\\DeskCtrl\\firmware\\DeskCtrl\\src\\Config\\Config.h"
#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <vector>
#include <stdint.h>

class Config
{
public:
    struct Data
    {

        char wifiSsid[32];
        char wifiPass[32];
        char wifiIp[24];
        char wifiGateway[24];
        char wifiSubnet[24];
        uint32_t presets[10];


    };

    Data data;

    // void Save();
    // void Load();
private:

};

#endif //_CONFIG_H_