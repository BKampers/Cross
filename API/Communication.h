/*
** Interface for the UART driver recognizing JSON objects
** Copyright 2014, Bart Kampers
*/

#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_

#include <stdint.h>
#include "ApiStatus.h"


#define DEFAULT_CHANNEL 0


Status OpenCommunicationChannel(int channelId, uint8_t bufferSize);
Status CloseCommunicationChannel(int channelId);

Status ReadChannel(int channelId, char* string);
Status ReadString(char* string);

Status WriteChannel(int channelId, char*);
Status WriteString(char* string);

#endif /* _COMMUNICATION_ */
