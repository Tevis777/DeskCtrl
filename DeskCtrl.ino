#include "Led.h"
#include "Syslog.h"
#include "Button.h"
#include "Motor.h"

#define PIN_BUTTON_UP (D1)
#define PIN_BUTTON_DOWN (D2)
#define PIN_MOTOR_SIGNAL (D3)
#define PIN_LED_BOARD (D4)

void OnButtonUp(Button::Event event);
void OnButtonDown(Button::Event event);




uint32_t Counter = 0;

Led LedBoard("Board", PIN_LED_BOARD, true);
Motor Motor(PIN_MOTOR_SIGNAL);
Button ButtonUp("Up", PIN_BUTTON_UP, OnButtonUp);
Button ButtonDown("Up", PIN_BUTTON_DOWN, OnButtonDown);



void setup() {
  SyslogInit();
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

  Motor.Pool();

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
  SYSLOG("Button handler DOWN: %u", event);
}

