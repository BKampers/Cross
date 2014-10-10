#ifndef _IGNITION_H_
#define _IGNITION_H_

#include "Types.h"
#include "ApiStatus.h"
#include "Controllers.h"


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

TableController* GetIgnitionTableController();

int GetIgnitionAngle();

Status UpdateIgnition();
//Status SetIgnitionAngle(int angle /*degrees*/);


int GetIgnitionTicks();


#endif /* _IGNITION_H_ */
