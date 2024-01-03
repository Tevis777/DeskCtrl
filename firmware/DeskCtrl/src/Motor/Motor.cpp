#include "Motor.h"
#include "Arduino.h"
#include "../Syslog/Syslog.h"
#include "../DeskCtrl.h"

Motor* Motor::s_instance = nullptr;

volatile static IRAM_ATTR Motor::EDirection Direction;
volatile static IRAM_ATTR Motor::Steps StepPos;

volatile static uint8_t PinPull;
volatile static uint8_t PinEnable;
volatile static uint8_t PinDir;

/*****************************************************************************************************************/
/*                                                   PHYSICAL                                                    */
/*****************************************************************************************************************/
static inline void PhyEnable(bool enabled)
{
    digitalWrite(PinEnable, enabled ? LOW : HIGH);
}

static inline void PhySetDir(Motor::EDirection dir)
{
    digitalWrite(PinDir, dir == Motor::EDirection::Down ? LOW : HIGH);
}

static inline void PhyPull()
{
    digitalWrite(PinPull, digitalRead(PinPull) ? LOW : HIGH);
}

static inline void PhySetup(uint8_t pinEn, uint8_t pinDir, uint8_t pinPull)
{
    PinEnable = pinEn;
    PinDir = pinDir;
    PinPull = pinPull;
    pinMode(PinPull, OUTPUT);
    pinMode(PinDir, OUTPUT);
    pinMode(PinEnable, OUTPUT);
    PhyEnable(false);
}

/*****************************************************************************************************************/
/*                                                  TIMER ISR                                                    */
/*****************************************************************************************************************/
static void ICACHE_RAM_ATTR MotorISRHandler()
{
    PhyPull();

    if(Direction == Motor::EDirection::Down)
    {
        StepPos--;
    }
    else
    {
        StepPos++;
    }
}

static inline uint32_t MotorISRFreqToVal(uint32_t freq)
{
    static constexpr uint32_t PRESCALER = 256;

    return ((F_CPU / PRESCALER) / freq);
}

static inline void MotorISRInit()
{
    timer1_attachInterrupt(MotorISRHandler);
    timer1_disable();
}

static inline void MotorISRDisable()
{
    timer1_disable();
}

static inline void MotorISRSetup(uint32_t freq)
{       
    timer1_write(MotorISRFreqToVal(freq));
    timer1_enable(TIM_DIV256, TIM_EDGE, TIM_LOOP);
}

/*****************************************************************************************************************/
/*                                                     API                                                       */
/*****************************************************************************************************************/
void Motor::Init(uint8_t pinEn, uint8_t pinDir, uint8_t pinPull)
{
    s_instance = this;

    PhySetup(pinEn, pinDir, pinPull);
    MotorISRInit();

    SYSLOG("Motor initialized");
}

void Motor::StartDst(Height height)
{
    Request req;
    req.type = EState::Starting;
    req.dir = EDirection::Unknown;
    req.stepPos = HeightToSteps(height);
    m_request = std::make_shared<Request>(req);

    SYSLOG("Motor start request (dst=%.1f->%.1f)", StepsToHeight(StepPos), height);
}

void Motor::StartManual(EDirection dir)
{
    Request req;
    req.type = EState::Starting;
    req.dir = dir;
    req.stepPos = STEPS_NONE;
    m_request = std::make_shared<Request>(req);

    SYSLOG("Motor start request (dir=%s)", dir == EDirection::Down ? "down" : "up");
}

void Motor::Stop()
{
    Request req;
    req.type = EState::Stopping;
    req.dir = EDirection::Unknown;
    req.stepPos = STEPS_NONE;
    m_request = std::make_shared<Request>(req);

    SYSLOG("Motor stop request");
}

Motor::Height Motor::StepsToHeight(Steps steps)
{
    return HEIGHT_MIN + ((float)steps / STEPS_PER_CM);
}

Motor::Steps Motor::HeightToSteps(Height height)
{
    return (height - HEIGHT_MIN) * STEPS_PER_CM;
}

void Motor::Calibrate(Steps steps)
{
    StepPos = steps;

    SYSLOG("Motor calibrated (position: %.1f/%u)", StepsToHeight(StepPos), StepPos);
}

void Motor::Calibrate(Height height)
{
    Calibrate(HeightToSteps(height));
}

bool Motor::IsActive() const
{
    return (m_state != EState::Idle);
}

Motor::Height Motor::GetHeight() const
{
    return StepsToHeight(StepPos);
}

Motor::Steps Motor::GetSteps() const
{
    return StepPos;
}



/*****************************************************************************************************************/
/*                                                   PROCESSING                                                  */
/*****************************************************************************************************************/
void Motor::SetSpeed(uint32_t freq)
{
    if(freq == 0)
    {
        MotorISRDisable();
        PhyEnable(false);
    }
    else
    {
        PhyEnable(true);
        MotorISRSetup(freq);
    }
}

void Motor::Pool(uint32_t interval)
{
    if(m_request) //Handle external request
    {
        EDirection reqDir;

        if(m_request->stepPos == STEPS_NONE) //By manual selection
        {
            reqDir = m_request->dir;
        }
        else //By destination
        {
            reqDir = (m_request->stepPos > StepPos) ? EDirection::Up : EDirection::Down;
        }

        if(m_request->type == EState::Starting) //Request to start drive
        {
            if(m_state == EState::Idle) //Motor is idle
            {
                m_selectedPos = m_request->stepPos;
                m_state = EState::Starting;
                m_freq = INIT_FREQ;
                Direction = reqDir;
                PhySetDir(Direction);
                SetSpeed(INIT_FREQ);
                m_request = {};
                SYSLOG("Motor starting");
            }
            else //Motor is running
            {
                if(reqDir == Direction) //Direction matches, update position
                {
                    m_selectedPos = m_request->stepPos;
                    m_request = {};
                }
                else //Direction mismatch, wait until stopped
                {
                    m_state = EState::Stopping;
                    SYSLOG("Motor stopping");
                }
            }
        }
        else if(m_request->type == EState::Stopping) 
        {
            if(m_state != EState::Idle)
            {
                m_state = EState::Stopping;
                SYSLOG("Motor stopping");
            }

            m_request = {};
        }
    }    

    if(m_selectedPos != UINT32_MAX) //Handle selected position
    {
        if((m_state == EState::Idle) || (m_state == EState::Running))
        {
            if((Direction == EDirection::Up) && (StepPos >= m_selectedPos))
                m_state = EState::Stopping;

            if((Direction == EDirection::Down) && (StepPos <= m_selectedPos))
                m_state = EState::Stopping;
        }
    }

    if(m_state != EState::Idle)
    {
        if(Direction == EDirection::Down) //Handle absolute limits
        {
            if(StepPos <= LIMIT_MARGIN)
            {
                m_state = EState::Stopping;
                m_request = {};
            }
        }
        else
        {
            if((StepPos + LIMIT_MARGIN) >= TOTAL_STEPS)
            {
                m_state = EState::Stopping;
                m_request = {};
            }
        }
    }


    if(m_state == EState::Starting) //Handle soft start
    {
        if(m_freq < WORK_FREQ)
        {
            m_freq += (WORK_FREQ - INIT_FREQ) / (START_TIME / interval);
            SetSpeed(m_freq);
        }

        if(m_freq >= WORK_FREQ)
        {
            m_state = EState::Running;
        }
    }

    if(m_state == EState::Stopping) //Handle soft stop
    {
        if(m_freq > INIT_FREQ)
        {
            m_freq -= (WORK_FREQ - INIT_FREQ) / (START_TIME / interval);
            SetSpeed(m_freq);
        }

        if(m_freq <= INIT_FREQ)
        {
            SetSpeed(0);
            m_state = EState::Idle;
            m_selectedPos = UINT32_MAX;
            SYSLOG("Motor stopped (height:%.1f)", StepsToHeight(StepPos));
            DeskCtrl::GetInstance()->OnMotorStop(StepPos);
        }
    }
}


