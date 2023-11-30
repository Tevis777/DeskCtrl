#ifndef _MOTOR_H_
#define _MOTOR_H_

#include <stdint.h>

class Motor
{
public:
  enum class Dir
  {
    Right,
    Left
  };

  Motor(uint8_t pin);

  void Init();
  void Start(Dir dir);
  void Stop();
  void Pool();

private:
  enum class EState
  {
    Starting,
    Running,
    Stopping,
    Idle,
  };

  static constexpr uint32_t START_TIME = 512; //[ms]

  static constexpr uint32_t INIT_FREQ = 1024;
  static constexpr uint32_t WORK_FREQ = 2048;

  static constexpr uint32_t PWM_ON = UINT8_MAX / 2; //50% duty cycle
  static constexpr uint32_t PWM_OFF = 0;
  
  uint8_t m_pin;
  uint32_t m_freq;
  EState m_state;
};

#endif // _MOTOR_H_