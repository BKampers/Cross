#ifndef _INJECTION_H_
#define _INJECTION_H_

#include "Types.h"
#include "ApiStatus.h"


extern char INJECTION[];


Status InitInjection();

int GetInjectionColumnIndex();
int GetInjectionRowIndex();
float GetInjectionTime();

Status UpdateInjection();


#endif /* _INJECTION_H_ */
