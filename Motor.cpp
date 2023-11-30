#include "Motor.h"
#include "Arduino.h"
#include "Syslog.h"

Motor* Motor::s_instance = nullptr;

void Motor::OnTimerISR()
{
  s_instance->HandleTick();
}

Motor::Motor(uint8_t pin) : m_pin(pin)
{
  s_instance = this;
}

void Motor::Init()
{
  SYSLOG("Motor init");
  pinMode(m_pin, OUTPUT);
  digitalWrite(m_pin, LOW);
  timer1_attachInterrupt(OnTimerISR);
  timer1_disable();


  // timer1_enable(TIM_DIV256, TIM_EDGE, TIM_LOOP);
  // timer1_write(FreqToTimerVal(INIT_FREQ));

  // timer1_enable(TIM_DIV256, TIM_EDGE, TIM_LOOP);
  // timer1_write(FreqToTimerVal(WORK_FREQ));
}

void Motor::Start(Dir dir)
{
  if(m_state != EState::Idle)
    return;

  SYSLOG("Motor start");
  m_state = EState::Starting;
  m_freq = INIT_FREQ;
  SetSpeed(INIT_FREQ);
}

void Motor::Stop()
{
  if(m_state == EState::Idle)
    return;

  SYSLOG("Motor stop");
  m_state = EState::Stopping;
}

void Motor::HandleTick()
{
  digitalWrite(m_pin, digitalRead(m_pin) ? LOW : HIGH);
}

uint32_t Motor::FreqToTimerVal(uint32_t freq)
{
  return ((F_CPU / PRESCALER) / freq);
}

void Motor::SetSpeed(uint32_t freq)
{
  if(freq == 0)
  {
    timer1_disable(); //Disable interrupt
    digitalWrite(m_pin, LOW);
  }
  else
  {
    timer1_write(FreqToTimerVal(freq));
    timer1_enable(TIM_DIV256, TIM_EDGE, TIM_LOOP);
  }
}

void Motor::Pool() //Every 10ms
{
  if(m_state == EState::Starting)
  {
    m_freq += (WORK_FREQ - INIT_FREQ) / (START_TIME / 100);

    SetSpeed(m_freq);

    SYSLOG("FREQ: %u", m_freq);

    if(m_freq >= WORK_FREQ)
    {
      m_state = EState::Running;
    }
  }

  if(m_state == EState::Stopping)
  {
    m_freq -= (WORK_FREQ - INIT_FREQ) / (START_TIME / 100);

    SetSpeed(m_freq);

    SYSLOG("FREQ: %u", m_freq);

    if(m_freq <= INIT_FREQ)
    {
      SetSpeed(0);
      m_state = EState::Idle;
    }
  }
}