#ifndef _TIMERS_H_
#define _TIMERS_H_

#include "ApiStatus.h"


#define EXTERNAL_PULSE_TIMER_PRESCALER 20
#define IGNITION_TIMER_PRESCALER 40


void InitPeriodTimer(void (*InterruptService) ());
void InitExternalPulseTimer(void (*InterruptService) (int capture));

Status StartPeriodTimer(int ticks);


#endif /* _TIMERS_H_ */
