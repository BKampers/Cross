#ifndef _INJECTION_H_
#define _INJECTION_H_

#include "Types.h"
#include "ApiStatus.h"
#include "Controllers.h"


extern char INJECTION[];


Status InitInjection();

TableController* GetInjectionTableController();
float GetInjectionTime();

Status UpdateInjection();


#endif /* _INJECTION_H_ */
