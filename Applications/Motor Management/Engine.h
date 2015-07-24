#ifndef _ENGINE_H_
#define _ENGINE_H_

#include "ApiStatus.h"


#define DEAD_POINT_MAX 4
#define PHASE_MAX 2


extern char ENGINE_IS_RUNNING[];


void RegisterEngineType();
Status InitEngine();

int GetCogTotal();
int GetGapSize();
int GetEffectiveCogCount();

int GetDeadPointOffset();
int GetDeadPointCount();
int GetDeadPointCog(int index);
int GetDeadPointIndex(int cogNumber);

int GetCylinderCount();

Status SetGogwheelProperties(int cogTotal, int gapSize, int offset);
Status SetCylinderCount(int count);

#endif /* _ENGINE_H_ */
