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

    SYSLOG("Motor initialized");
}

void Motor::Calibrate(uint32_t position)
{
    m_position = position;

    SYSLOG("Motor calibrated (position: %u/%u)", PosToHeight(m_position), m_position);
}

void Motor::StartDst(uint32_t height)
{
    Request req;
    req.type = EState::Starting;
    req.dir = EDir::Unknown;
    req.position = HeightToPos(height);
    m_request = std::make_shared<Request>(req);

    SYSLOG("Motor start request (dst=%u->%u)", PosToHeight(m_position), height);
}

void Motor::StartManual(EDir dir)
{
    Request req;
    req.type = EState::Starting;
    req.dir = dir;
    req.position = UINT32_MAX;
    m_request = std::make_shared<Request>(req);

    SYSLOG("Motor start request (dir=%s)", GetDirStr(dir));
}

bool Motor::IsActive()
{
    return (m_state != EState::Idle);
}

uint32_t Motor::GetHeight()
{
    return PosToHeight(m_position);
}

void Motor::Stop()
{
    Request req;
    req.type = EState::Stopping;
    req.dir = EDir::Unknown;
    req.position = UINT32_MAX;
    m_request = std::make_shared<Request>(req);

    SYSLOG("Motor stop request");
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
    if(m_request) //Handle external request
    {
        EDir reqDir;

        if(m_request->position == UINT32_MAX)
        {
            reqDir = m_request->dir;
        }
        else
        {
            reqDir = (m_request->position > m_position) ? EDir::Up : EDir::Down;
        }

        if(m_request->type == EState::Starting) //Request to start drive
        {
            if(m_state == EState::Idle) //Motor is idle
            {
                m_selectedPos = m_request->position;
                m_dir = reqDir;
                m_state = EState::Starting;
                m_freq = INIT_FREQ;
                PhySetDir(m_dir);
                SetSpeed(INIT_FREQ);
                m_request = {};
                SYSLOG("Motor starting");
            }
            else //Motor is running
            {
                if(reqDir == m_dir) //Direction matches, update position
                {
                    m_selectedPos = m_request->position;
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
            if((m_dir == EDir::Up) && (m_position >= m_selectedPos))
                m_state = EState::Stopping;

            if((m_dir == EDir::Down) && (m_position <= m_selectedPos))
                m_state = EState::Stopping;
        }
    }

    if(m_state != EState::Idle)
    {
        if(m_dir == EDir::Down) //Handle absolute limits
        {
            if(m_position <= LIMIT_MARGIN)
            {
                m_state = EState::Stopping;
                m_request = {};
            }
        }
        else
        {
            if((m_position + LIMIT_MARGIN) >= TOTAL_STEPS)
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
            SYSLOG("Motor stopped (height:%u)", PosToHeight(m_position));
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

const char* Motor::GetDirStr(EDir dir)
{
    return (dir == EDir::Down) ? "Down" : "Up";
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



