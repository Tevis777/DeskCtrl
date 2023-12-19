#include "Storage.h"
#include "Memory.h"


void Storage::Init()
{
    Memory::Init();
}

void Storage::SavePosition(uint32_t position)
{
    Memory::Save(0, &position, sizeof(uint32_t));
}

void Storage::LoadPosition(uint32_t& position)
{
    Memory::Load(0, &position, sizeof(uint32_t));
}