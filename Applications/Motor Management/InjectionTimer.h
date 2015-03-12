#ifndef _INJECTIONTIMER_H_
#define _INJECTIONTIMER_H_

#include "ApiStatus.h"

Status InitInjectionTimer();

Status StartInjection(int injectorId);

Status SetPrescaler(int scale);
int GetPrescaler();

Status SetPeriod(int ticks);
int GetPeriod();

Status SetInjectionTimer(float time);
Status SetInjectionTicks(int ticks);
int GetInjectionTicks();

#endif /* _INJECTIONTIMER_H_ */
