#include "Led.h"
#include "Syslog.h"
#include "Button.h"

#define PIN_LED_BOARD (D4)
#define PIN_BUTTON_UP (D1)
#define PIN_BUTTON_DOWN (D2)


void OnButtonUp(Button::Event event)
{
  SYSLOG("Button handler UP: %u", event);
}

void OnButtonDown(Button::Event event)
{
  SYSLOG("Button handler DOWN: %u", event);

}

uint32_t Counter = 0;

Led LedBoard("Board", PIN_LED_BOARD, true);
Button ButtonUp("Up", PIN_BUTTON_UP, OnButtonUp);
Button ButtonDown("Up", PIN_BUTTON_DOWN, OnButtonDown);

void setup() {
  SyslogInit();
  LedBoard.Init();
  ButtonUp.Init();
  ButtonDown.Init();

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

  delay(1);
}
