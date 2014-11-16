#ifndef _INJECTION_H_
#define _INJECTION_H_

#include "Types.h"
#include "ApiStatus.h"

extern char INJECTION[];
extern char WATER_TEMPERATURE_CORRECTION[]; 


Status InitInjection();

float GetInjectionTime();

Status UpdateInjection();


#endif /* _INJECTION_H_ */
