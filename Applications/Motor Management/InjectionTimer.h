#ifndef _INJECTIONTIMER_H_
#define _INJECTIONTIMER_H_

#include "ApiStatus.h"

Status InitInjectionTimer();

Status SetInjectorCog(int injectorIndex, int cogNumber);

Status StartInjection(int injectorId);

Status SetPrescaler(int scale);
int GetPrescaler();

Status SetPeriod(int ticks);
int GetPeriod();

Status SetInjectionTime(float time);
Status SetInjectionTicks(int ticks);
int GetInjectionTicks();

#endif /* _INJECTIONTIMER_H_ */
