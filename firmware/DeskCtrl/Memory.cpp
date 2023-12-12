#include "Memory.h"
#include <EEPROM.h>
#include <Arduino.h>

void Memory::Init(void)
{
  EEPROM.begin(SIZE);
  delay(100);
}

bool Memory::Save(uint32_t addr, const uint8_t* data, uint32_t size)
{
  int32_t i;

  if((addr + size) >= SIZE)
    return false;

  for(i = 0; i < size; i++)
  {
    EEPROM.write(addr + i, data[i]);
  }

  EEPROM.commit();

  return false;
}

bool Memory::Load(uint32_t addr, uint8_t* data, uint32_t size)
{
  int32_t i;

  if((addr + size) >= SIZE)
    return false;

  for(i = 0; i < size; i++)
  {
    data[i] = EEPROM.read(addr + i);
  }

  return false;
}
