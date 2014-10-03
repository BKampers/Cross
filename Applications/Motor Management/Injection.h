#ifndef __INJECTION_H__
#define __INJECTION_H__

#include "Types.h"
#include "ApiStatus.h"


extern char INJECTION[];

void RegisterInjectionTypes();
Status InitInjection();

int GetInjectionColumnIndex();
int GetInjectionRowIndex();
float GetInjectionTime();

Status UpdateInjection();


#endif /* __INJECTION_H__ */
