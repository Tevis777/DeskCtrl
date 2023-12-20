#ifndef _STORAGE_H_
#define _STORAGE_H_

#include "Eeprom.h"
#include <stdint.h>

class Storage
{
public:
    void Init(uint8_t sda, uint8_t scl);
    void SavePosition(uint32_t position);
    void LoadPosition(uint32_t& position);

    Eeprom m_eeprom;
private:
    


};


#endif //_STORAGE_H_