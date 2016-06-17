#ifndef _JSONMESSAGING_H_
#define _JSONMESSAGING_H_

#include "ApiStatus.h"
#include "Json.h"


typedef struct
{
    const char* name;
    Status (*Call) (const JsonNode* parameters, Status* status);
} Function;


typedef struct
{
    Function* functions;
    int functionCount;
} JsonRpcApi;


Status HandleMessage(const char* jsonString, JsonRpcApi* api, int channelId);

Status FireTextEvent(const char* name, const char* value, int channelId);
Status FireIntegerEvent(const char* name, int value, int channelId);
Status FireRealEvent(const char* name, double value, int channelId);
Status FireErrorEvent(const char* error, int channelId);


#endif /* _JSONMESSAGING_H_ */

