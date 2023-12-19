#ifndef _DESKCTRL_H_
#define _DESKCTRL_H_

#include "Buttons/Button.h"
#include "Motor/Motor.h"
#include "Led/Led.h"
#include "Memory/Memory.h"
#include "Syslog/Syslog.h"

#include <memory.h>

class DeskCtrl
{
public:
    DeskCtrl();
    
    static DeskCtrl* GetInstance();

    
    void Init();
    void Process();


    static DeskCtrl* s_instance;
    Led m_ledBoard;
    Button m_buttonUp;
    Button m_buttonDown;
    Motor m_motor;


private:



};

#endif //_DESKCTRL_H_