#include "Eeprom.h"
#include <Wire.h>
#include "../Syslog/Syslog.h"

void Eeprom::Init()
{
    Wire.begin();
}

bool Eeprom::Write2B(uint32_t addr, uint16_t data)
{
    uint8_t* ptr = (uint8_t*)&data;

    Wire.beginTransmission((int)DEV_ADDR);
    Wire.write(addr);
    Wire.write(ptr, 2);

    if(Wire.endTransmission() != 0)
    {
        SYSLOG("Eeprom write error!");
        return false;
    }

    delay(5);

    return true;
}

bool Eeprom::Read2B(uint32_t addr, uint16_t* data)
{
    uint8_t* ptr = (uint8_t*)data;
    int32_t i;

    Wire.beginTransmission((int)DEV_ADDR);
    Wire.write(addr);

    if(Wire.endTransmission() != 0)
    {
        SYSLOG("Eeprom read error!");
        return false;
    }

    delay(5);

    if(Wire.requestFrom((int)DEV_ADDR, 2) != 2)
    {
        SYSLOG("Eeprom request error!");
        return false;
    }

    for(i = 0; i < 2; i++)
    {
        ptr[i] = Wire.read();
    }

    delay(5);

    return true;
}


bool Eeprom::ReadAll(uint8_t* data)
{
    uint8_t* ptr = (uint8_t*)data;
    int32_t i;

    Wire.beginTransmission((int)DEV_ADDR);
    Wire.write(0);

    if(Wire.endTransmission() != 0)
    {
        SYSLOG("Eeprom read error!");
        return false;
    }

    delay(5);

    if(Wire.requestFrom((int)DEV_ADDR, SIZE) != SIZE)
    {
        SYSLOG("Eeprom request error!");
        return false;
    }

    for(i = 0; i < SIZE; i++)
    {
        ptr[i] = Wire.read();
    }

    delay(5);

    return true;
}
