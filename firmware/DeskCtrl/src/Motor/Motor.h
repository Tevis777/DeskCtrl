#ifndef _MOTOR_H_
#define _MOTOR_H_

#include <stdint.h>
#include <memory>

class Motor
{
public:
    enum class EDirection
    {
        Unknown,
        Down,
        Up
    };

    using Steps = uint32_t;
    using Height = float;

    static constexpr Steps STEPS_NONE = UINT32_MAX;

    void Init(uint8_t pinEn, uint8_t pinDir, uint8_t pinPull);
    void Pool(uint32_t interval);

    void StartDst(Height height);
    void StartManual(EDirection direction);
    void Stop();

    void Calibrate(Steps steps);
    void Calibrate(Height height);

    bool IsActive() const;

    Height GetHeight() const;
    Steps GetSteps() const;

    static Height StepsToHeight(Steps steps);
    static Steps HeightToSteps(Height height);

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
        EDirection dir;
        Steps stepPos;
    };

    static constexpr Height HEIGHT_MAX = 120; //[cm]
    static constexpr Height HEIGHT_MIN = 70; //[cm]

    static constexpr Steps TOTAL_STEPS = 44800;

    static constexpr Steps STEPS_PER_CM = (44800 / (HEIGHT_MAX - HEIGHT_MIN));
    static constexpr Steps LIMIT_MARGIN = 2000;

    static constexpr uint32_t START_TIME = 500; //[ms]

    static constexpr uint32_t INIT_FREQ = 128;
    static constexpr uint32_t WORK_FREQ = 896;

    std::shared_ptr<Request> m_request;

    EState m_state = EState::Idle;
    Steps m_selectedPos = STEPS_NONE;
    uint32_t m_freq;
};

#endif // _MOTOR_H_