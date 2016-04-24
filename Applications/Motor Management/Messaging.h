#ifndef _MESSAGING_H_
#define _MESSAGING_H_

#include "ApiStatus.h"


Status HandleMessage(const char* jsonString);

Status FireTextEvent(const char* name, const char* value);
Status FireIntegerEvent(const char* name, int value);
Status FireRealEvent(const char* name, double value);
Status FireErrorEvent(const char* subject);

#endif /* _MESSAGING_H_ */
