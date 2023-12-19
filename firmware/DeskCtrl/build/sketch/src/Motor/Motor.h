#line 1 "C:\\Workspace\\DeskCtrl\\firmware\\DeskCtrl\\src\\Motor\\Motor.h"
#ifndef _MOTOR_H_
#define _MOTOR_H_

#include <stdint.h>

class Motor
{
public:
  enum class EDir
  {
    Down,
    Up
  };

  Motor(uint8_t pinEn, uint8_t pinDir, uint8_t pinPull);

  void Init();
  void Start(EDir dir);
  void Stop();
  void Pool();

  void GoTo(uint32_t height);

  

private:
  static Motor* s_instance;

  enum class EState
  {
    Starting,
    Running,
    Stopping,
    Idle,
  };


  static constexpr uint32_t HEIGHT_MAX = 120; //[cm]
  static constexpr uint32_t HEIGHT_MIN = 70; //[cm]

  static constexpr uint32_t TOTAL_STEPS = 44800;

  static constexpr uint32_t STEPS_PER_CM = (44800 / (HEIGHT_MAX - HEIGHT_MIN));


  static constexpr uint32_t START_TIME = 1000; //[ms]

  static constexpr uint32_t FREQ_DIV_32 = (12 * 1024);
  static constexpr uint32_t FREQ_DIV_16 = (6 * 1024);
  static constexpr uint32_t FREQ_DIV_8 = (3 * 1024);
  static constexpr uint32_t FREQ_DIV_4 = (3 * 512);
  static constexpr uint32_t FREQ = 5 * 128;

  static constexpr uint32_t INIT_FREQ = 128;
  static constexpr uint32_t WORK_FREQ = 896;
  static constexpr uint32_t PRESCALER = 256;

  static uint32_t FreqToTimerVal(uint32_t freq);

  static void OnTimerISR();
  
  void HandleTick();
  void SetSpeed(uint32_t freq);

  volatile uint32_t m_pos;
  uint32_t m_selectedPos = UINT32_MAX;

  uint32_t PosToHeight(uint32_t pos);
  uint32_t HeightToPos(uint32_t height);

  const char* GetDirStr();

  void StepperSetup();
  void StepperSetEn(bool enabled);
  void StepperSetDir(EDir dir);
  void StepperPull();
  void ResetPos();

  EDir m_dir;
  uint8_t m_pinPull;
  uint8_t m_pinEn;
  uint8_t m_pinDir;
  uint32_t m_freq;
  
  EState m_state = EState::Idle;
};

#endif // _MOTOR_H_