#line 1 "C:\\Workspace\\DeskCtrl\\firmware\\DeskCtrl\\src\\DeskCtrl.cpp"
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
void DeskCtrl::Init()
{
  SyslogInit();
  delay(5000);

  SYSLOG("Desk Controller startup --------------------");
  // Memory.Init();
  Memory::Init();


  uint32_t position;

  Memory::Load(0, &position, sizeof(uint32_t));

  m_motor.Init(PIN_MOTOR_EN, PIN_MOTOR_DIR, PIN_MOTOR_PULL);
  m_motor.Calibrate(position);
  m_ledBoard.Init("Board", PIN_LED_BOARD, false);
  m_buttonUp.Init("Up", PIN_BUTTON_UP, [this](Button::Event evt)
                                        {
                                            if(evt == Button::Event::Pressed)
                                                this->OnUpButtonPressed();
                                            else
                                                this->OnUpButtonReleased();
                                        });

  m_buttonDown.Init("Down", PIN_BUTTON_DOWN, [this](Button::Event evt)
                                        {
                                            if(evt == Button::Event::Pressed)
                                                this->OnDownButtonPressed();
                                            else
                                                this->OnDownButtonReleased();
                                        });
}

/*****************************************************************************************************************/
/*                                                     LOOP                                                      */
/*****************************************************************************************************************/
void DeskCtrl::Process()
{
    static uint32_t counter = 0;

    counter++;

    if((counter % 1000) == 0)
    {
        m_ledBoard.Toggle();
    }

    if((counter % 100) == 0)
    {
        m_buttonUp.Pool();
        m_buttonDown.Pool();
    }

    if((counter % 10) == 0)
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

void DeskCtrl::OnMotorStop(uint32_t position)
{
    Memory::Save(0, &position, sizeof(uint32_t));
}

/*****************************************************************************************************************/
/*                                                   COMMANDS                                                    */
/*****************************************************************************************************************/
void DeskCtrl::CmdDeskGoUp()
{
    m_motor.Start(Motor::EDir::Up);
}

void DeskCtrl::CmdDeskGoUpToNextLevel()
{

}

void DeskCtrl::CmdDeskGoDown()
{
    m_motor.Start(Motor::EDir::Down);
}

void DeskCtrl::CmdDeskGoDownToNextLevel()
{

}

void DeskCtrl::CmdDeskGoTo(uint32_t height)
{

}

void DeskCtrl::CmdDeskStop()
{
    m_motor.Stop();
}

void DeskCtrl::CmdDeskCalibrate(uint32_t height)
{


}