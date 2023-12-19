#include "Led.h"
#include "Arduino.h"
#include "../Syslog/Syslog.h"

const char* Led::GetName()
{
  return m_name;
}

void Led::Init(const char* name, uint8_t pin, bool log)
{
  m_name = name;
  m_pin = pin;
  m_log = log;

  SYSLOG("Led init (%s)", m_name);
  pinMode(m_pin, OUTPUT);
}

void Led::On()
{
  if(m_log)
    SYSLOG("Led on (%s)", m_name);
  digitalWrite(m_pin, HIGH);
}

void Led::Off()
{
  if(m_log)
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