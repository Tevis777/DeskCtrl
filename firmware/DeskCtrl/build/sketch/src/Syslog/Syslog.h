#line 1 "C:\\Workspace\\DeskCtrl\\firmware\\DeskCtrl\\src\\Syslog\\Syslog.h"
#ifndef _SYSLOG_H_
#define _SYSLOG_H_

#include <stdarg.h>

void SyslogInit();
void SYSLOG(const char* txt, ...);

#endif //_SYSLOG_H_