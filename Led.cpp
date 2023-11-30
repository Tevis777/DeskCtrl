#include "Led.h"
#include "Arduino.h"
#include "Syslog.h"

Led::Led(const char* name, uint8_t pin, bool log) : m_name(name), m_pin(pin), m_log(log) {}

const char* Led::GetName()
{
  return m_name;
}

void Led::Init()
{
  SYSLOG("Led init (%s)", m_name);
  pinMode(m_pin, OUTPUT);
}

void Led::On()
{
  SYSLOG("Led on (%s)", m_name);
  digitalWrite(m_pin, HIGH);
}

void Led::Off()
{
  SYSLOG("Led off (%s)", m_name);
  digitalWrite(m_pin, LOW);
}

void Led::Toggle()
{
  GetState() ? Off() : On();
}

bool Led::GetState()
{
  return digitalRead(m_pin);
}