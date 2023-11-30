#include "Motor.h"
#include "Arduino.h"
#include "Syslog.h"

Motor::Motor(uint8_t pin) : m_pin(pin){}

void Motor::Init()
{
  analogWrite(m_pin, PWM_OFF); 
}

void Motor::Start(Dir dir)
{
  if(m_state != EState::Idle)
    return;

  SYSLOG("Motor start");
  m_state = EState::Starting;
  m_freq = INIT_FREQ;
  analogWriteFreq(INIT_FREQ);
  analogWriteRange(PWM_ON);
}

void Motor::Stop()
{
  if(m_state == EState::Idle)
    return;

  SYSLOG("Motor stop");
  m_state = EState::Stopping;
}

void Motor::Pool() //Every 1ms
{
  if(m_state == EState::Starting)
  {
    m_freq += (WORK_FREQ - INIT_FREQ) / START_TIME;

    analogWriteFreq(m_freq);

    if(m_freq >= WORK_FREQ)
    {
      m_state = EState::Running;
    }
  }

  if(m_state == EState::Stopping)
  {
    m_freq -= (WORK_FREQ - INIT_FREQ) / START_TIME;

    if(m_freq <= INIT_FREQ)
    {
      m_state = EState::Idle;
      analogWriteRange(PWM_OFF);
    }
  }
}