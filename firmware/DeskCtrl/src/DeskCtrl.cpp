#include "DeskCtrl.h"
#include "Arduino.h"

/*****************************************************************************************************************/
/*                                              HARDWARE PROFILE                                                 */
/*****************************************************************************************************************/
#define PIN_BUTTON_UP   (D1)
#define PIN_BUTTON_DOWN (D2)
#define PIN_LED_BOARD   (D4)
#define PIN_MOTOR_EN    (D5)
#define PIN_MOTOR_DIR   (D6)
#define PIN_MOTOR_PULL  (D7)


/*****************************************************************************************************************/
/*                                                  SINGLETON                                                    */
/*****************************************************************************************************************/
DeskCtrl* DeskCtrl::s_instance = nullptr;

DeskCtrl* DeskCtrl::GetInstance()
{
    if(s_instance == nullptr)
        s_instance = new DeskCtrl();

    return s_instance;
}

/*****************************************************************************************************************/
/*                                                     INIT                                                      */
/*****************************************************************************************************************/
DeskCtrl::DeskCtrl() : 

    m_buttonUp("Up", PIN_BUTTON_UP,
        [this](Button::Event evt)
        {
            if(evt == Button::Event::Pressed)
                this->OnUpButtonPressed();
            else
                this->OnUpButtonReleased();
        }
    ), 

    m_buttonDown("Down", PIN_BUTTON_DOWN, [this](Button::Event evt)
        {
            if(evt == Button::Event::Pressed)
                this->OnDownButtonPressed();
            else
                this->OnDownButtonReleased();
        }
    ), 

    m_ledBoard("Board", PIN_LED_BOARD, false), 

    m_motor(PIN_MOTOR_EN, PIN_MOTOR_DIR, PIN_MOTOR_PULL)
{

};

void DeskCtrl::Init()
{
  SyslogInit();
  delay(5000);

  SYSLOG("Desk Controller startup --------------------");
  // Memory.Init();
  Memory::Init();
  m_ledBoard.Init();
  m_buttonUp.Init();
  m_buttonDown.Init();
  m_motor.Init();
}

/*****************************************************************************************************************/
/*                                                     LOOP                                                      */
/*****************************************************************************************************************/
void DeskCtrl::Process()
{
    static uint32_t Counter = 0;

    Counter++;

    if((Counter % 1000) == 0)
    {
        m_ledBoard.Toggle();
    }

    if((Counter % 100) == 0)
    {
        m_buttonUp.Pool();
        m_buttonDown.Pool();
    }

    if((Counter % 10) == 0)
    {
        m_motor.Pool();
    }
}



/*****************************************************************************************************************/
/*                                                    EVENTS                                                     */
/*****************************************************************************************************************/
void DeskCtrl::OnUpButtonPressed()
{
    CmdDeskGoUp();
}

void DeskCtrl::OnUpButtonReleased()
{
    CmdDeskStop();
}

void DeskCtrl::OnDownButtonPressed()
{
    CmdDeskGoDown();
}

void DeskCtrl::OnDownButtonReleased()
{
    CmdDeskStop();
}

void DeskCtrl::OnMotorStart()
{

}

void DeskCtrl::OnMotorStop()
{


}

/*****************************************************************************************************************/
/*                                                   COMMANDS                                                    */
/*****************************************************************************************************************/
void DeskCtrl::CmdDeskGoUp()
{
    m_motor.Start(Motor::EDir::Up);
}

void DeskCtrl::CmdDeskGoUpToNext()
{

}

void DeskCtrl::CmdDeskGoDown()
{
    m_motor.Start(Motor::EDir::Down);
}

void DeskCtrl::CmdDeskGoDownToNext()
{

}

void DeskCtrl::CmdDeskGoTo(uint32_t height)
{

}

void DeskCtrl::CmdDeskStop()
{
    m_motor.Stop();
}