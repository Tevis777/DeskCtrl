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

  Motor(uint8_t pinEn, uint8_t pinDir, uint8_t pinPull);

  void Init();
  void Start(Dir dir);
  void Stop();
  void Pool();

private:
  static Motor* s_instance;

  enum class EState
  {
    Starting,
    Running,
    Stopping,
    Idle,
  };

  static constexpr uint32_t START_TIME = 1 * 1000; //[ms]

  static constexpr uint32_t FREQ_DIV_32 = (12 * 1024);
  static constexpr uint32_t FREQ_DIV_16 = (6 * 1024);
  static constexpr uint32_t FREQ_DIV_8 = (3 * 1024);
  static constexpr uint32_t FREQ_DIV_4 = (3 * 512);
  static constexpr uint32_t FREQ = 5 * 128;

  static constexpr uint32_t INIT_FREQ = 2 * 1024;
  static constexpr uint32_t WORK_FREQ = 8 * 1024;
  static constexpr uint32_t PRESCALER = 256;

  static uint32_t FreqToTimerVal(uint32_t freq);

  static void OnTimerISR();
  
  void HandleTick();
  void SetSpeed(uint32_t freq);



  
  uint8_t m_pinPull;
  uint8_t m_pinEn;
  uint8_t m_pinDir;
  uint32_t m_freq;
  EState m_state = EState::Idle;
};

#endif // _MOTOR_H_