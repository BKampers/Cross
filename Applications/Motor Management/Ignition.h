#ifndef __IGNITION_H__
#define __IGNITION_H__

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

int GetColumnIndex();
int GetRowIndex();
int GetIgnitionAngle();

Status UpdateIgnition();
//Status SetIgnitionAngle(int angle /*degrees*/);


int GetIgnitionTicks();


#endif /* __IGNITION_H__ */
