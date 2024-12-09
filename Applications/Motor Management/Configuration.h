#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

#include "Types.h"

#define INJECTION_DISABLED 0x00
#define UART_INJECTION 0x01
#define TIMER_INJECTION 0x02

byte GetInjectionMode();

bool PwmIgnitionEnabled();
bool TimerIgnitionEnabled();

int GetDefaultCogTotal();
int GetDefaultGapSize();
int GetDefaultDeadPointOffset();
int GetDefaultCylinderCount();

bool IsCogwheelMutable();


#endif /* _CONFIGURATION_H_ */
