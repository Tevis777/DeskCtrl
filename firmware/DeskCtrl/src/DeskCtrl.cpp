#include "DeskCtrl.h"
#include "Arduino.h"
#include <algorithm>

/*****************************************************************************************************************/
/*                                              HARDWARE PROFILE                                                 */
/*****************************************************************************************************************/
#define PIN_MOTOR_EN        (D0)
#define PIN_EEPROM_SCL      (D1)
#define PIN_EEPROM_SDA      (D2)
#define PIN_BUTTON_UP       (D3)
#define PIN_LED_BOARD       (D4)
#define PIN_MOTOR_POWER     (D5)
#define PIN_BUTTON_DOWN     (D6)
#define PIN_MOTOR_PULL      (D7)
#define PIN_MOTOR_DIR       (D8)


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
  //delay(500);

  SYSLOG("Desk Controller startup --------------------");
  // Memory.Init();
  m_storage.Init();

  uint32_t position;

  //CmdResetConfig();

  m_storage.LoadPosition(position);
  m_storage.LoadConfig(m_config);
  
  m_motorPower.Init(PIN_MOTOR_POWER);
  m_motorPower.On();
  m_motor.Init(PIN_MOTOR_EN, PIN_MOTOR_DIR, PIN_MOTOR_PULL);
  m_motor.Calibrate(position);


  m_ledBoard.Init("Board", PIN_LED_BOARD, false);
  m_buttonUp.Init("Up", PIN_BUTTON_UP, Button::EActive::Low, [this](Button::Event evt)
                                        {
                                            if(evt == Button::Event::Pressed)
                                                this->OnUpButtonPressed();
                                            else
                                                this->OnUpButtonReleased();
                                        });

  m_buttonDown.Init("Down", PIN_BUTTON_DOWN, Button::EActive::Low, [this](Button::Event evt)
                                        {
                                            if(evt == Button::Event::Pressed)
                                                this->OnDownButtonPressed();
                                            else
                                                this->OnDownButtonReleased();
                                        });

    
    auto ip = "192.168.0.115";
    auto gateway = "192.168.0.1";
    auto subnet = "255.255.255.0";

    m_network.Connect("UPC2012", "LubiePlacki666", ip, gateway, subnet);

    m_presets.push_back(80);
    m_presets.push_back(95);
    m_presets.push_back(115);

    SYSLOG("Config:");
    SYSLOG("Wifi STA ssid:%s, pass:%s, ip:%s, gateway:%s, subnet:%s", m_config.wifiSTA.ssid.c_str(), 
                                                                      m_config.wifiSTA.pass.c_str(), 
                                                                      m_config.wifiSTA.ip.c_str(),
                                                                      m_config.wifiSTA.gateway.c_str(), 
                                                                      m_config.wifiSTA.subnet.c_str());

    SYSLOG("Wifi AP ssid:%s, pass:%s, ip:%s, gateway:%s, subnet:%s", m_config.wifiAP.ssid.c_str(), 
                                                                     m_config.wifiAP.pass.c_str(), 
                                                                     m_config.wifiAP.ip.c_str(),
                                                                     m_config.wifiAP.gateway.c_str(), 
                                                                     m_config.wifiAP.subnet.c_str());

    SYSLOG("Power timeout: %u", m_config.power.timeout);

    for(const auto& preset : m_config.drive.presets)
    {
        SYSLOG("Preset: %u", preset);
    }    
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
        m_network.Pool();
    }

    if((counter % 10) == 0)
    {
        m_buttonUp.Pool(10);
        m_buttonDown.Pool(10);
        m_motor.Pool(10);
    }
}



/*****************************************************************************************************************/
/*                                                    EVENTS                                                     */
/*****************************************************************************************************************/
void DeskCtrl::OnUpButtonPressed()
{
    if(m_buttonDown.IsPressed())
    {
        CmdDeskGoDownToNextPreset();
    }
    else if(m_motor.IsActive())
    {
        CmdDeskStop();
    }
    else
    {
        CmdDeskGoUp();
    }
}

void DeskCtrl::OnUpButtonReleased()
{

}

void DeskCtrl::OnDownButtonPressed()
{
    if(m_buttonUp.IsPressed())
    {
        CmdDeskGoUpToNextPreset();
    }
    else if(m_motor.IsActive())
    {
        CmdDeskStop();
    }
    else
    {
        CmdDeskGoDown();
    }
}

void DeskCtrl::OnDownButtonReleased()
{

}

void DeskCtrl::OnMotorStart()
{

}

void DeskCtrl::OnMotorStop(Motor::Steps steps)
{
    m_storage.SavePosition(steps);
}

/*****************************************************************************************************************/
/*                                                   COMMANDS                                                    */
/*****************************************************************************************************************/
void DeskCtrl::CmdDeskGoUp()
{
    m_motor.StartManual(Motor::EDirection::Up);
}

void DeskCtrl::CmdDeskGoUpToNextPreset()
{
    uint32_t height = m_motor.GetHeight();

    for(const auto& preset : m_presets)
    {
        if(preset <= height)
            continue;

        m_motor.StartDst(preset);
        break;
    }
}

void DeskCtrl::CmdDeskGoDown()
{
    m_motor.StartManual(Motor::EDirection::Down);
}

void DeskCtrl::CmdDeskGoDownToNextPreset()
{
    uint32_t height = m_motor.GetHeight();
    auto presets = m_presets;

    std::reverse(presets.begin(), presets.end());

    for(const auto& preset : presets)
    {
        if(preset >= height)
            continue;

        m_motor.StartDst(preset);
        break;
    }
}

void DeskCtrl::CmdDeskGoTo(Motor::Height height)
{
    m_motor.StartDst(height);
}

void DeskCtrl::CmdDeskStop()
{
    m_motor.Stop();
}

void DeskCtrl::CmdDeskCalibrate(Motor::Height height)
{
    m_motor.Calibrate(height);
    m_storage.SavePosition(Motor::HeightToSteps(height));
}

void DeskCtrl::CmdConfigReset()
{
    m_config.wifiSTA.ssid = "UPC2012";
    m_config.wifiSTA.pass = "LubiePlacki666";
    m_config.wifiSTA.ip = "192.168.0.115";
    m_config.wifiSTA.gateway = "192.168.0.1";
    m_config.wifiSTA.subnet = "255.255.255.0";

    m_config.wifiAP.ssid = "DeskCtrl";
    m_config.wifiAP.pass = "LubiePlacki666";
    m_config.wifiAP.ip = "192.168.10.1";
    m_config.wifiAP.gateway = "192.168.10.1";
    m_config.wifiAP.subnet = "255.255.255.0";

    m_config.power.timeout = 60;

    m_config.drive.presets = {80, 95, 120};

    m_storage.SaveConfig(m_config);
}

void DeskCtrl::CmdConfigSave()
{
    m_storage.SaveConfig(m_config);
}

/*****************************************************************************************************************/
/*                                                   GETTERS                                                     */
/*****************************************************************************************************************/
Motor& DeskCtrl::GetMotor()
{
    return m_motor;
}

MotorPower& DeskCtrl::GetMotorPower()
{
    return m_motorPower;
}

Network& DeskCtrl::GetNetwork()
{
    return m_network;
}

Config& DeskCtrl::GetConfig()
{
    return m_config;
}