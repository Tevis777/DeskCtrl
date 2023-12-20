#include "src/DeskCtrl.h"
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
