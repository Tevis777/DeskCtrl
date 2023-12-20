#ifndef _STORAGE_H_
#define _STORAGE_H_

#include "Eeprom.h"
#include <stdint.h>

class Storage
{
public:
    void Init();
    void SavePosition(uint32_t position);
    void LoadPosition(uint32_t& position);

    Eeprom m_eeprom;
private:
    uint32_t m_storageAddr = 0;
    


};


#endif //_STORAGE_H_