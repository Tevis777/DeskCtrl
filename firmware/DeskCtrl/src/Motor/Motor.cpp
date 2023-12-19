#include "Motor.h"
#include "Arduino.h"
#include "../Syslog/Syslog.h"
#include "../DeskCtrl.h"

Motor* Motor::s_instance = nullptr;

void Motor::OnTimerISR()
{
    s_instance->HandleTick();
}

void Motor::Init(uint8_t pinEn, uint8_t pinDir, uint8_t pinPull)
{
    s_instance = this;

    m_pinEn = pinEn;
    m_pinDir = pinDir;
    m_pinPull = pinPull;

    PhySetup();

    timer1_attachInterrupt(OnTimerISR);
    timer1_disable();

    SYSLOG("Motor init (position: %u/%u)", PosToHeight(m_position), m_position);
}

void Motor::Calibrate(uint32_t position)
{
    m_position = position;
}

void Motor::StartDst(uint32_t height)
{
    m_selectedPos = HeightToPos(height);

    if(m_selectedPos == m_position)
        return;

    if(m_selectedPos > m_position)
    {
        Start(EDir::Up);
    }
    else
    {
        Start(EDir::Down);
    }
}

void Motor::StartManual(EDir dir)
{
    m_selectedPos = UINT32_MAX;
    Start(dir);
}


void Motor::Start(EDir dir)
{
    if(m_state != EState::Idle)
        return;


    m_dir = dir;
    m_state = EState::Starting;
    m_freq = INIT_FREQ;
    PhySetDir(m_dir);
    SetSpeed(INIT_FREQ);

    DeskCtrl::GetInstance()->OnMotorStart();
}

void Motor::Stop()
{
    if(m_state == EState::Idle)
        return;

    m_state = EState::Stopping;
}

void Motor::HandleTick()
{
    PhyPull();

    if(m_dir == EDir::Down)
    {
        m_position--;
    }
    else
    {
        m_position++;
    }
}

uint32_t Motor::FreqToTimerVal(uint32_t freq)
{
    return ((F_CPU / PRESCALER) / freq);
}

void Motor::SetSpeed(uint32_t freq)
{
    if(freq == 0)
    {
        timer1_disable(); //Disable interrupt
        PhyEnable(false);
    }
    else
    {
        PhyEnable(true);
        timer1_write(FreqToTimerVal(freq));
        timer1_enable(TIM_DIV256, TIM_EDGE, TIM_LOOP);
    }
}

void Motor::Pool(uint32_t interval)
{
    if(m_selectedPos != UINT32_MAX) //Handle selected position
    {
        if((m_state == EState::Idle) || (m_state == EState::Running))
        {
            if((m_dir == EDir::Up) && (m_position >= m_selectedPos))
                Stop();

            if((m_dir == EDir::Down) && (m_position <= m_selectedPos))
                Stop();
        }
    }

    if(m_state == EState::Starting) //Handle soft start
    {
        m_freq += (WORK_FREQ - INIT_FREQ) / (START_TIME / interval);

        SetSpeed(m_freq);

        if(m_freq >= WORK_FREQ)
        {
            m_state = EState::Running;
        }
    }

    if(m_state == EState::Stopping) //Handle soft stop
    {
        m_freq -= (WORK_FREQ - INIT_FREQ) / (START_TIME / interval);

        SetSpeed(m_freq);

        if(m_freq <= INIT_FREQ)
        {
            SetSpeed(0);
            m_state = EState::Idle;
            DeskCtrl::GetInstance()->OnMotorStop(m_position);
        }
    }
}

uint32_t Motor::PosToHeight(uint32_t pos)
{
    return HEIGHT_MIN + (pos / STEPS_PER_CM);
}

uint32_t Motor::HeightToPos(uint32_t height)
{
    return (height - HEIGHT_MIN) * STEPS_PER_CM;
}

const char* Motor::GetDirStr()
{
    return (m_dir == EDir::Down) ? "Down" : "Up";
}


/*****************************************************************************************************************/
/*                                                   PHYSICAL                                                    */
/*****************************************************************************************************************/
void Motor::PhySetup()
{
    pinMode(m_pinPull, OUTPUT);
    pinMode(m_pinDir, OUTPUT);
    pinMode(m_pinEn, OUTPUT);

    PhyEnable(false);
}

void Motor::PhyEnable(bool enabled)
{
    digitalWrite(m_pinEn, enabled ? LOW : HIGH);
}

void Motor::PhySetDir(EDir dir)
{
    digitalWrite(m_pinDir, dir == EDir::Down ? LOW : HIGH);
}

void Motor::PhyPull()
{
    digitalWrite(m_pinPull, digitalRead(m_pinPull) ? LOW : HIGH);
}



