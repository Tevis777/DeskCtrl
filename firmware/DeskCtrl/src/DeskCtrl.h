#ifndef _DESKCTRL_H_
#define _DESKCTRL_H_

#include "Buttons/Button.h"
#include "Motor/Motor.h"
#include "Motor/MotorPower.h"
#include "Led/Led.h"
#include "Syslog/Syslog.h"
#include "Storage/Storage.h"
#include "Network/Network.h"
#include "Config/Config.h"

#include <vector>

class DeskCtrl
{
public:
    static DeskCtrl* GetInstance();

    //Main control
    void Init();
    void Process();

    //Events
    void OnUpButtonPressed();
    void OnUpButtonReleased();
    void OnDownButtonPressed();
    void OnDownButtonReleased();
    void OnMotorStart();
    void OnMotorStop(uint32_t position);

    //Commands
    void CmdDeskGoUp();
    void CmdDeskGoUpToNextPreset();
    void CmdDeskGoDown();
    void CmdDeskGoDownToNextPreset();
    void CmdDeskGoTo(Motor::Height height);
    void CmdDeskStop();
    void CmdDeskCalibrate(Motor::Height height);
    void CmdResetConfig();

    Motor& GetMotor();
    MotorPower& GetMotorPower();
    Network& GetNetwork();
    Config& GetConfig();

private:
    static DeskCtrl* s_instance;

    Config m_config;
    Led m_ledBoard;
    Button m_buttonUp;
    Button m_buttonDown;
    Motor m_motor;
    MotorPower m_motorPower;
    Storage m_storage;
    Network m_network;

    std::vector<uint32_t> m_presets;
};

#endif //_DESKCTRL_H_