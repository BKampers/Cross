#ifndef _CRANK_H_
#define _CRANK_H_

#include "ApiStatus.h"

#include "Types.h"


void InitCrank();

bool SignalDetected();
int GetRotationCount();
int GetCogCount();
int GetCogTicks();
int GetGapTicks();

float GetRpm();

Status SetCogCountCallback(void (*callback) (), int cogNumber);

#endif /* _CRANK_H_ */
