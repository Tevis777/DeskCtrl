#include "Storage.h"
#include "Memory.h"
#include "../Syslog/Syslog.h"

void Storage::Init()
{
    uint16_t buff[Eeprom::SIZE / 2];
    int32_t i;

    m_flash.Init();
    m_eeprom.Init();
    m_eeprom.ReadAll((uint8_t*)buff);

    for(i = 0; i < Eeprom::SIZE / 2; i++)
    {
        if(buff[i] != 0xFFFF)
        {
            m_storageAddr = i * 2;
            break;
        }
    }

    SYSLOG("Detected position on addr %u", m_storageAddr);
}

void Storage::SavePosition(uint32_t position)
{
    uint16_t val = position;

    m_eeprom.Write2B(m_storageAddr, 0xFFFF);
    m_storageAddr = ((m_storageAddr + 2) % (Eeprom::SIZE / 2));
    m_eeprom.Write2B(m_storageAddr, val);

    SYSLOG("Saving position on addr %u", m_storageAddr);
}

void Storage::LoadPosition(uint32_t& position)
{
    uint16_t val;

    m_eeprom.Read2B(m_storageAddr, &val);

    if(val == 0xFFFFF)
        val = 0;

    position = val;
}

Storage::ConfigStruct Storage::ConfigEncode(const Config& data)
{
    uint32_t idx = 0;
    ConfigStruct res;

    strncpy(res.staSsid, data.wifiSTA.ssid.c_str(), SSID_MAX_LEN);
    strncpy(res.staPass, data.wifiSTA.pass.c_str(), PASS_MAX_LEN);
    strncpy(res.staIp, data.wifiSTA.ip.c_str(), IP_MAX_LEN);
    strncpy(res.staGateway, data.wifiSTA.gateway.c_str(), IP_MAX_LEN);
    strncpy(res.staSubnet, data.wifiSTA.subnet.c_str(), IP_MAX_LEN);

    strncpy(res.apSsid, data.wifiAP.ssid.c_str(), SSID_MAX_LEN);
    strncpy(res.apPass, data.wifiAP.pass.c_str(), PASS_MAX_LEN);
    strncpy(res.apIp, data.wifiAP.ip.c_str(), IP_MAX_LEN);
    strncpy(res.apGateway, data.wifiAP.gateway.c_str(), IP_MAX_LEN);
    strncpy(res.apSubnet, data.wifiAP.subnet.c_str(), IP_MAX_LEN);

    res.powerTimeout = data.power.timeout;

    memset(&res.presets, 0, sizeof(res.presets));

    for(const auto& preset : data.drive.presets)
    {
        res.presets[idx++] = preset;
    }

    return res;
}

Config Storage::ConfigDecode(const ConfigStruct& mem)
{
    uint32_t idx = 0;
    Config res;

    res.wifiSTA.ssid = mem.staSsid;
    res.wifiSTA.pass = mem.staPass;
    res.wifiSTA.ip = mem.staIp;
    res.wifiSTA.gateway = mem.staGateway;
    res.wifiSTA.subnet = mem.staSubnet;

    res.wifiAP.ssid = mem.apSsid;
    res.wifiAP.pass = mem.apPass;
    res.wifiAP.ip = mem.apIp;
    res.wifiAP.gateway = mem.apGateway;
    res.wifiAP.subnet = mem.apSubnet;

    res.power.timeout = mem.powerTimeout;

    for(idx = 0; idx < Config::MAX_PRESETS; idx++)
    {
        if(mem.presets[idx] == 0)
            break;

        res.drive.presets.push_back(mem.presets[idx]);
    }
    
    return res;
}

void Storage::SaveConfig(const Config& cfg)
{
    ConfigStruct memStruct = ConfigEncode(cfg);

    m_flash.Save(0, &memStruct, sizeof(ConfigStruct));
}

void Storage::LoadConfig(Config& cfg)
{
    ConfigStruct memStruct;

    m_flash.Load(0, &memStruct, sizeof(ConfigStruct));

    cfg = ConfigDecode(memStruct);
}
