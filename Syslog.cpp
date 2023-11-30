#include "Syslog.h"
#include "Arduino.h"
#include <stdio.h>

#define SYSLOG_BUFF_SIZE (512)

void SyslogInit()
{
  Serial.begin(9600);
}

void SYSLOG(const char* txt, ...)
{
  char buffer[SYSLOG_BUFF_SIZE];

  va_list args;
  va_start(args, txt);
  vsnprintf(buffer, sizeof(buffer), txt, args);
  va_end(args);

  Serial.println(buffer);
}