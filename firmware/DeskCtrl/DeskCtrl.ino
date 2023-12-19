#include "src/DeskCtrl.h"

void setup()
{
  DeskCtrl::GetInstance()->Init();
}

void loop()
{
  DeskCtrl::GetInstance()->Process();
  delay(1);
}
