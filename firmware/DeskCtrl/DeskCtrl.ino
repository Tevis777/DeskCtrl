#include "src/Led/Led.h"
#include "src/Syslog/Syslog.h"
#include "src/Buttons/Button.h"
#include "src/Motor/Motor.h"
#include "src/Memory/Memory.h"

//GPIO profile
#define PIN_BUTTON_UP (D1)
#define PIN_BUTTON_DOWN (D2)
#define PIN_LED_BOARD (D4)

#define PIN_MOTOR_EN (D5)
#define PIN_MOTOR_DIR (D6)
#define PIN_MOTOR_PULL (D7)


void OnButtonUp(Button::Event event);
void OnButtonDown(Button::Event event);




uint32_t Counter = 0;

Led LedBoard("Board", PIN_LED_BOARD, false);

Button ButtonUp("Up", PIN_BUTTON_UP, OnButtonUp);
Button ButtonDown("Down", PIN_BUTTON_DOWN, OnButtonDown);
Motor Motor(PIN_MOTOR_EN, PIN_MOTOR_DIR, PIN_MOTOR_PULL);

void setup() {
  SyslogInit();
  delay(5000);

  SYSLOG("Desk Controller startup --------------------");
  // Memory.Init();
  Memory::Init();
  LedBoard.Init();
  ButtonUp.Init();
  ButtonDown.Init();
  Motor.Init();

}

void loop() {


  Counter++;

  if((Counter % 1000) == 0)
  {
    LedBoard.Toggle();
  }

  if((Counter % 100) == 0)
  {
    ButtonUp.Pool();
    ButtonDown.Pool();
  }

  if((Counter % 10) == 0)
  {
    Motor.Pool();
  }

  delay(1);
}


void OnButtonUp(Button::Event event)
{
  switch(event)
  {
    case Button::Event::Pressed:
    {
      Motor.Start(Motor::EDir::Up);
      //Motor.GoTo(90);
      return;
    }
    case Button::Event::Released:
    default:
    {
      Motor.Stop();
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
      Motor.Start(Motor::EDir::Down);
      //Motor.GoTo(85);
      return;
    }
    case Button::Event::Released:
    default:
    {
      Motor.Stop();
      return;
    }
  }
}

