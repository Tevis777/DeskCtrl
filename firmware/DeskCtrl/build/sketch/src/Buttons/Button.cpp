#line 1 "C:\\Workspace\\DeskCtrl\\firmware\\DeskCtrl\\src\\Buttons\\Button.cpp"
#include "Button.h"
#include "Arduino.h"
#include "../Syslog/Syslog.h"

void Button::Init(const char* name, uint8_t pin, EActive active, Handler handler)
{
  m_name = name;
  m_pin = pin;
  m_handler = handler;
  m_active = active;

  SYSLOG("Button init (%s)", m_name);
  pinMode(m_pin, INPUT);
  m_prev = IsPressed();
}

void Button::Pool(uint32_t interval)
{
    if(m_timer >= interval)
    {
        m_timer -= interval;
        return;
    }

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

    m_timer = DEBOUNCE_TIMEOUT;
    m_prev = IsPressed();
}

bool Button::IsPressed()
{
    if(m_active == EActive::High)
    {
        return digitalRead(m_pin);
    }
    else
    {
        return !digitalRead(m_pin);
    }
}