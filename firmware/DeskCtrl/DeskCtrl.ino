#include "src/DeskCtrl.h"
#include <ESP8266WiFi.h>
#include <Wire.h>

void setup()
{
  DeskCtrl::GetInstance()->Init();
}

void loop()
{
  DeskCtrl::GetInstance()->Process();
  delay(1);
}
