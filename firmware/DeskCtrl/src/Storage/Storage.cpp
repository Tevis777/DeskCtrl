#include "Storage.h"
#include "Memory.h"


void Storage::Init(uint8_t sda, uint8_t scl)
{
    Memory::Init();
    m_eeprom.Init(sda, scl);


}

void Storage::SavePosition(uint32_t position)
{
    Memory::Save(0, &position, sizeof(uint32_t));
}

void Storage::LoadPosition(uint32_t& position)
{
    Memory::Load(0, &position, sizeof(uint32_t));
}