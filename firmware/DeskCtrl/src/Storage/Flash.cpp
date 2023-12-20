#include "Flash.h"
#include <EEPROM.h>
#include <Arduino.h>

void Flash::Init(void)
{
    EEPROM.begin(SIZE);
    delay(100);
}

bool Flash::Save(uint32_t addr, const void* data, uint32_t size)
{
    const uint8_t* ptr = static_cast<const uint8_t*>(data);
    int32_t i;

    if((addr + size) >= SIZE)
        return false;

    for(i = 0; i < size; i++)
    {
        EEPROM.write(addr + i, ptr[i]);
    }

    EEPROM.commit();

    return false;
}

bool Flash::Load(uint32_t addr, void* data, uint32_t size)
{
    uint8_t* ptr = static_cast<uint8_t*>(data);
    int32_t i;

    if((addr + size) >= SIZE)
        return false;

    for(i = 0; i < size; i++)
    {
        ptr[i] = EEPROM.read(addr + i);
    }

    return false;
}
