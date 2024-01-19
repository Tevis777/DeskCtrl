#ifndef _STORAGE_H_
#define _STORAGE_H_

#include "Eeprom.h"
#include "Flash.h"
#include "../Config/Config.h"
#include <stdint.h>

class Storage
{
public:
    void Init();
    void SavePosition(uint32_t position);
    void LoadPosition(uint32_t& position);
    void SaveConfig(const Config& cfg);
    void LoadConfig(Config& cfg);
    
private:
    uint32_t m_storageAddr = 0;

    static constexpr uint32_t IP_MAX_LEN = 15;
    static constexpr uint32_t SSID_MAX_LEN = 31;
    static constexpr uint32_t PASS_MAX_LEN = 31;

    struct ConfigStruct
    {
        char staSsid[SSID_MAX_LEN + 1];
        char staPass[PASS_MAX_LEN + 1];
        char staIp[IP_MAX_LEN + 1];
        char staGateway[IP_MAX_LEN + 1];
        char staSubnet[IP_MAX_LEN + 1];

        char apSsid[SSID_MAX_LEN + 1];
        char apPass[PASS_MAX_LEN + 1];
        char apIp[IP_MAX_LEN + 1];
        char apGateway[IP_MAX_LEN + 1];
        char apSubnet[IP_MAX_LEN + 1];

        uint32_t powerTimeout;
        uint32_t presets[Config::MAX_PRESETS];
    };

    ConfigStruct ConfigEncode(const Config& cfg);
    Config ConfigDecode(const ConfigStruct& mem);

    Eeprom m_eeprom;
    Flash m_flash;
};


#endif //_STORAGE_H_