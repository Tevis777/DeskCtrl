#line 1 "C:\\Workspace\\DeskCtrl\\firmware\\DeskCtrl\\src\\DeskCtrl.h"
#ifndef _DESKCTRL_H_
#define _DESKCTRL_H_

#include "Buttons/Button.h"
#include "Motor/Motor.h"
#include "Led/Led.h"
#include "Memory/Memory.h"
#include "Syslog/Syslog.h"

class DeskCtrl
{
public:
    DeskCtrl();
    
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
    void OnMotorStop();

    //Commands
    void CmdDeskGoUp();
    void CmdDeskGoUpToNext();
    void CmdDeskGoDown();
    void CmdDeskGoDownToNext();
    void CmdDeskGoTo(uint32_t height);
    void CmdDeskStop();




    static DeskCtrl* s_instance;
    Led m_ledBoard;
    Button m_buttonUp;
    Button m_buttonDown;
    Motor m_motor;






private:

};

#endif //_DESKCTRL_H_