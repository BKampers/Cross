#ifndef _MESSAGING_H_
#define _MESSAGING_H_

#include "ApiStatus.h"


void HandleMessage(const char* jsonString);

Status SendTextNotification(const char* name, const char* value);
Status SendIntegerNotification(const char* name, int value);
Status SendRealNotification(const char* name, double value);
Status SendErrorNotification(const char* subject);

#endif /* _MESSAGING_H_ */
