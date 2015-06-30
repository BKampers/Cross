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

Status WriteCharacter(int channelId, char character);
Status WriteString(int channelId, const char* string);

Status FinishTransmission(int channelId);

#endif /* _COMMUNICATION_H_ */
