#include "Storage.h"
#include "Memory.h"
#include "../Syslog/Syslog.h"

void Storage::Init()
{
    uint16_t buff[Eeprom::SIZE / 2];
    int32_t i;

    Memory::Init();
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