#line 1 "C:\\Workspace\\DeskCtrl\\firmware\\DeskCtrl\\src\\Buttons\\Button.h"
#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <stdint.h>
#include <functional>

class Button
{
public:
  enum class Event
  {
    Pressed,
    Released
  };

  enum class EActive
  {
    Low,
    High
  };

  using Handler = std::function<void(Event)>;

  void Init(const char* name, uint8_t pin, EActive active, Handler handler);
  void Pool();
  bool IsPressed();

private:
  const char* m_name;
  uint8_t m_pin;
  Handler m_handler;
  bool m_prev = false;
  EActive m_active;
};

#endif // _BUTTON_H_