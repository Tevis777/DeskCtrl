#ifndef _MOTOR_H_
#define _MOTOR_H_

#include <stdint.h>
#include <memory>

class Motor
{
public:
    enum class EDir
    {
        Unknown,
        Down,
        Up
    };

    void Init(uint8_t pinEn, uint8_t pinDir, uint8_t pinPull);
    void Stop();
    void Pool(uint32_t interval);

    void StartDst(uint32_t height);
    void StartManual(EDir dir);

    bool IsActive();

    void Calibrate(uint32_t position);
  

private:
    static Motor* s_instance;

    enum class EState
    {
        Starting,
        Running,
        Stopping,
        Idle,
    };

    struct Request
    {
        EState type;
        EDir dir;
        uint32_t position;
    };



    static constexpr uint32_t HEIGHT_MAX = 120; //[cm]
    static constexpr uint32_t HEIGHT_MIN = 70; //[cm]

    static constexpr uint32_t TOTAL_STEPS = 44800;

    static constexpr uint32_t STEPS_PER_CM = (44800 / (HEIGHT_MAX - HEIGHT_MIN));

    static constexpr uint32_t START_TIME = 500; //[ms]

    static constexpr uint32_t INIT_FREQ = 128;
    static constexpr uint32_t WORK_FREQ = 896;
    static constexpr uint32_t PRESCALER = 256;

    static uint32_t FreqToTimerVal(uint32_t freq);

    static void OnTimerISR();

    void HandleTick();
    void SetSpeed(uint32_t freq);

    volatile uint32_t m_position;
    uint32_t m_selectedPos = UINT32_MAX;

    uint32_t PosToHeight(uint32_t pos);
    uint32_t HeightToPos(uint32_t height);

    const char* GetDirStr();



    void PhySetup();
    void PhyEnable(bool enabled);
    void PhySetDir(EDir dir);
    void PhyPull();


    std::shared_ptr<Request> m_request;

    EDir m_dir;


    uint8_t m_pinPull;
    uint8_t m_pinEn;
    uint8_t m_pinDir;
    uint32_t m_freq;
    bool m_restart;

    EState m_state = EState::Idle;
};

#endif // _MOTOR_H_