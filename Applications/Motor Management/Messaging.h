#ifndef __MESSAGING_H__
#define __MESSAGING_H__


void HandleMessage(const char* jsonString);

void SendTextNotification(const char* name, const char* value);
void SendIntegerNotification(const char* name, int value);
void SendRealNotification(const char* name, double value);
void SendErrorNotification(const char* subject);

#endif /* __MESSAGING_H__ */
