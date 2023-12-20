#ifndef _MOTORPOWER_H_
#define _MOTORPOWER_H_

#include <stdint.h>

class MotorPower
{
public:
    void Init(uint8_t pin);

    void On();
    void Off();

private:
    uint8_t m_pin;
};

#endif