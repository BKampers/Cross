#ifndef _TIMERS_H_
#define _TIMERS_H_

#include "ApiStatus.h"

#define TIMER_UPDATE    0x01
#define TIMER_CHANNEL_1 0x02
#define TIMER_CHANNEL_2 0x04
#define TIMER_CHANNEL_3 0x08
#define TIMER_CHANNEL_4 0x10

#define EXTERNAL_PULSE_TIMER_PRESCALER 80
#define PERIOD_TIMER_PRESCALER 40

#define MAX_PULSE_NANOS 25000000L


void InitPeriodTimer(void (*InterruptService) ());
void InitCompareTimer(void (*InterruptService) (int channel));
void InitExternalPulseTimer(void (*InterruptService) (int capture), void (*OverflowService) ());

Status StartPeriodTimer(int ticks);
Status StartCompareTimer(int channel, int ticks);
void StartPwmTimer();
void SetPwmDutyCycle(float percentage);
int GetPwmDutyCycle();

int GetCompareTimerPeriod();


#endif /* _TIMERS_H_ */
