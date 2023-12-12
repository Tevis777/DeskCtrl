#include "Led.h"
#include "Syslog.h"
#include "Button.h"
#include "Motor.h"
#include "Memory.h"

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
Motor Motor(PIN_MOTOR_EN, PIN_MOTOR_DIR, PIN_MOTOR_PULL);
Button ButtonUp("Up", PIN_BUTTON_UP, OnButtonUp);
Button ButtonDown("Up", PIN_BUTTON_DOWN, OnButtonDown);
Memory Memory(1024);

void setup() {
  SyslogInit();
  SYSLOG("Desk Controller startup --------------------");
  Memory.Init();
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
      Motor.Start(Motor::Dir::Right);
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
      Motor.Start(Motor::Dir::Left);
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

