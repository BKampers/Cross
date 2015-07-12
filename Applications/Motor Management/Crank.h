#ifndef _CRANK_H_
#define _CRANK_H_

#include "ApiStatus.h"

#include "Types.h"

typedef void (*CogCountCallback) (int cogNumber);

void InitCrank();
void InitCrankCallbacks();

bool SignalDetected();
int GetPhase();

int GetCogCount();
int GetCogTicks();
int GetGapTicks();

float GetRpm();

Status SetCogCountCallback(CogCountCallback callback, int cogNumber);

#endif /* _CRANK_H_ */
