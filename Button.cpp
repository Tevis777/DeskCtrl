#include "Button.h"
#include "Arduino.h"
#include "Syslog.h"

Button::Button(const char* name, uint8_t pin, Handler handler) : m_name(name), m_pin(pin), m_handler(handler) {}

void Button::Init()
{
  pinMode(m_pin, INPUT);
  m_prev = IsPressed();
}

void Button::Pool()
{
  if(IsPressed() == m_prev)
    return;

  if(IsPressed())
  {
    SYSLOG("Button pressed (%s)", m_name);
    m_handler(Event::Pressed);
  }
  else
  {
    SYSLOG("Button released (%s)", m_name);
    m_handler(Event::Released);
  }

  m_prev = IsPressed();
}

bool Button::IsPressed()
{
  return digitalRead(m_pin);
}