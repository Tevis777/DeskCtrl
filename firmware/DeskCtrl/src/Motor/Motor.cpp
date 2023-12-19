#include "Motor.h"
#include "Arduino.h"
#include "../Syslog/Syslog.h"
#include "../Memory/Memory.h"

#include "../DeskCtrl.h"

Motor* Motor::s_instance = nullptr;

void Motor::OnTimerISR()
{
  s_instance->HandleTick();
}



void Motor::Init(uint8_t pinEn, uint8_t pinDir, uint8_t pinPull)
{
  s_instance = this;

  m_pinEn = pinEn;
  m_pinDir = pinDir;
  m_pinPull = pinPull;

  StepperSetup();
  timer1_attachInterrupt(OnTimerISR);
  timer1_disable();

  SYSLOG("Motor init (position: %u/%u)", PosToHeight(m_pos), m_pos);
}

void Motor::GoTo(uint32_t height)
{
  m_selectedPos = HeightToPos(height);

  if(m_selectedPos == m_pos)
    return;

  if(m_selectedPos > m_pos)
    Start(EDir::Up);
  else
    Start(EDir::Down);

}

void Motor::Calibrate(uint32_t position)
{
  m_pos = position;
}


void Motor::Start(EDir dir)
{
  if(m_state != EState::Idle)
    return;


  m_dir = dir;
  m_state = EState::Starting;
  m_freq = INIT_FREQ;
  SYSLOG("Motor start (%s - %u->%u)", GetDirStr(), m_pos, m_selectedPos);
  StepperSetDir(m_dir);
  SetSpeed(INIT_FREQ);

  DeskCtrl::GetInstance()->OnMotorStart();
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
  digitalWrite(m_pinPull, digitalRead(m_pinPull) ? LOW : HIGH);

  if(m_dir == EDir::Down)
    m_pos--;
  else
    m_pos++;
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
    StepperSetEn(false);
  }
  else
  {
    StepperSetEn(true);
    timer1_write(FreqToTimerVal(freq));
    timer1_enable(TIM_DIV256, TIM_EDGE, TIM_LOOP);
  }
}

void Motor::Pool() //Every 10ms
{
  if(m_selectedPos != UINT32_MAX)
  {
    if((m_state == EState::Idle) || (m_state == EState::Running))
    {
      if((m_dir == EDir::Up) && (m_pos >= m_selectedPos))
        Stop();

      if((m_dir == EDir::Down) && (m_pos <= m_selectedPos))
        Stop();
    }
  }



  if(m_state == EState::Starting)
  {
    m_freq += (WORK_FREQ - INIT_FREQ) / (START_TIME / 10);

    SetSpeed(m_freq);

    SYSLOG("FREQ: %u", m_freq);

    if(m_freq >= WORK_FREQ)
    {
      m_state = EState::Running;
    }
  }

  if(m_state == EState::Stopping)
  {
    m_freq -= (WORK_FREQ - INIT_FREQ) / (START_TIME / 10);

    SetSpeed(m_freq);

    SYSLOG("FREQ: %u", m_freq);

    if(m_freq <= INIT_FREQ)
    {
      SetSpeed(0);
      m_state = EState::Idle;

      uint32_t val = m_pos;
      
      SYSLOG("Position: %u/%u", PosToHeight(m_pos), m_pos);
      DeskCtrl::GetInstance()->OnMotorStop(m_pos);
    }
  }
}

uint32_t Motor::PosToHeight(uint32_t pos)
{
  return HEIGHT_MIN + (pos / STEPS_PER_CM);
}

uint32_t Motor::HeightToPos(uint32_t height)
{
  return (height - HEIGHT_MIN) * STEPS_PER_CM;
}

const char* Motor::GetDirStr()
{
  return m_dir == EDir::Down ? "Down" : "Up";
}


void Motor::StepperSetup()
{
  pinMode(m_pinPull, OUTPUT);
  pinMode(m_pinDir, OUTPUT);
  pinMode(m_pinEn, OUTPUT);

  StepperSetEn(false);
}

void Motor::StepperSetEn(bool enabled)
{
  digitalWrite(m_pinEn, enabled ? LOW : HIGH);
}

void Motor::StepperSetDir(EDir dir)
{
  digitalWrite(m_pinDir, dir == EDir::Down ? LOW : HIGH);
}

void Motor::StepperPull()
{
  digitalWrite(m_pinPull, digitalRead(m_pinPull) ? LOW : HIGH);
}



