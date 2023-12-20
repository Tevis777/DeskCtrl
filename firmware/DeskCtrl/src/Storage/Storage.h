#ifndef _STORAGE_H_
#define _STORAGE_H_

#include "Eeprom.h"
#include "Flash.h"
#include <stdint.h>

class Storage
{
public:
    void Init();
    void SavePosition(uint32_t position);
    void LoadPosition(uint32_t& position);
    
private:
    uint32_t m_storageAddr = 0;

    Eeprom m_eeprom;
    Flash m_flash;
};


#endif //_STORAGE_H_