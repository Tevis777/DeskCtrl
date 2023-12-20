#ifndef _EEPROM_H_
#define _EEPROM_H_

#include <stdint.h>
#include <stdbool.h>

class Eeprom
{
public:
    static constexpr uint32_t SIZE = 128;

    void Init();
    bool Write2B(uint32_t addr, uint16_t data);
    bool Read2B(uint32_t addr, uint16_t* data);
    bool ReadAll(uint8_t* data);

private:
    static constexpr uint32_t DEV_ADDR = 0x50;
};


#endif //_EEPROM_H_