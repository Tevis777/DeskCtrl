#include "MotorPower.h"
#include "../Syslog/Syslog.h"
#include "Arduino.h"

void MotorPower::Init(uint8_t pin)
{
    m_pin = pin;
    pinMode(m_pin, OUTPUT);
    digitalWrite(m_pin, LOW);
}

void MotorPower::On()
{
    SYSLOG("Motor power on");
    digitalWrite(m_pin, HIGH);
}

void MotorPower::Off()
{
    SYSLOG("Motor power off");
    digitalWrite(m_pin, LOW);
}