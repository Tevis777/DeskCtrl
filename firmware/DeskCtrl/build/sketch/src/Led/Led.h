#line 1 "C:\\Workspace\\DeskCtrl\\firmware\\DeskCtrl\\src\\Led\\Led.h"
#ifndef _LED_H_
#define _LED_H_

#include <stdint.h>

class Led
{
public:
  Led(const char* name, uint8_t pin, bool log);

  void Init();
  void On();
  void Off();
  void Toggle();
  bool GetState();
  const char* GetName();


private:
  const char* m_name;
  uint8_t m_pin;
  bool m_log;
};

#endif //_LED_H_