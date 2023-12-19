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

  using Handler = std::function<void(Event)>;

  Button(const char* name, uint8_t pin, Handler handler);

  void Init();
  void Pool();
  bool IsPressed();

private:
  const char* m_name;
  uint8_t m_pin;
  Handler m_handler;
  bool m_prev = false;
};

#endif // _BUTTON_H_