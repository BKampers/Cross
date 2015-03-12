#ifndef _IGNITION_H_
#define _IGNITION_H_

#include "Types.h"
#include "ApiStatus.h"


typedef struct
{
	uint16_t prescaler;
	uint16_t period;
	uint16_t counter;
} TIMER_SETTINGS;


extern char IGNITION[];


void RegisterIgnitionTypes();
Status InitIgnition();

void GetIgnitionTimerSettings(TIMER_SETTINGS* timerSettings);
Status SetIgnitionTimerSettings(TIMER_SETTINGS* timerSettings);

int GetIgnitionAngle();
int GetIgnitionTicks();

Status UpdateIgnition();

void StartIgnition(int cogNumber);


#endif /* _IGNITION_H_ */
