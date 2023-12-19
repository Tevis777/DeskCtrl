#include "DeskCtrl.h"
#include "Arduino.h"

//Hardware profile
#define PIN_BUTTON_UP   (D1)
#define PIN_BUTTON_DOWN (D2)
#define PIN_LED_BOARD   (D4)

#define PIN_MOTOR_EN    (D5)
#define PIN_MOTOR_DIR   (D6)
#define PIN_MOTOR_PULL  (D7)


DeskCtrl* DeskCtrl::s_instance = nullptr;



void OnButtonUp(Button::Event event)
{
  switch(event)
  {
    case Button::Event::Pressed:
    {
      DeskCtrl::GetInstance()->m_motor.Start(Motor::EDir::Up);
      //Motor.GoTo(90);
      return;
    }
    case Button::Event::Released:
    default:
    {
      DeskCtrl::GetInstance()->m_motor.Stop();
      return;
    }
  }
}

void OnButtonDown(Button::Event event)
{
  switch(event)
  {
    case Button::Event::Pressed:
    {
      DeskCtrl::GetInstance()->m_motor.Start(Motor::EDir::Down);
      //Motor.GoTo(85);
      return;
    }
    case Button::Event::Released:
    default:
    {
      DeskCtrl::GetInstance()->m_motor.Stop();
      return;
    }
  }
}

DeskCtrl::DeskCtrl() : 
    m_buttonUp("Up", PIN_BUTTON_UP, OnButtonUp), 
    m_ledBoard("Board", PIN_LED_BOARD, false), 
    m_buttonDown("Down", PIN_BUTTON_DOWN, OnButtonDown), 
    m_motor(PIN_MOTOR_EN, PIN_MOTOR_DIR, PIN_MOTOR_PULL)
{

};







DeskCtrl* DeskCtrl::GetInstance()
{
    if(s_instance == nullptr)
        s_instance = new DeskCtrl();

    return s_instance;
}

void DeskCtrl::Init()
{
  SyslogInit();
  delay(5000);

  SYSLOG("Desk Controller startup --------------------");
  // Memory.Init();
  Memory::Init();
  m_ledBoard.Init();
  m_buttonUp.Init();
  m_buttonDown.Init();
  m_motor.Init();
}


void DeskCtrl::Process()
{
    static uint32_t Counter = 0;

    Counter++;

    if((Counter % 1000) == 0)
    {
        m_ledBoard.Toggle();
    }

    if((Counter % 100) == 0)
    {
        m_buttonUp.Pool();
        m_buttonDown.Pool();
    }

    if((Counter % 10) == 0)
    {
        m_motor.Pool();
    }
}
