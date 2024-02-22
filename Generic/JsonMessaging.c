/*
 * Copyright © Bart Kampers
 */

#include "JsonMessaging.h"

#include <stdlib.h>
#include <string.h>

#include "JsonWriter.h"
#include "JsonParser.h"


const char* DIRECTION = "Direction";
const char* CALL = "Call";
const char* RETURN = "Return";
const char* FIRE = "Fire";

const char* FUNCTION = "Function";
const char* PARAMETERS = "Parameters";
const char* RETURN_VALUE = "ReturnValue";

const char* STATUS = "Status";
const char* ERROR = "Error";


JsonStatus EmptyObject(JsonNode* node)
{
    node->source = "{}";
    node->type = JSON_OBJECT;
    node->length = 2;
    return JSON_OK;
}


Function* FindFunction(const char* name, JsonRpcApi* api) 
{
    int i;
    for (i = 0; i < api->functionCount; ++i)
    {
        if (strcmp(name, api->functions[i].name) == 0)
        {
            return &(api->functions[i]);
        }
    }
    return NULL;
}


Status HandleCall(const JsonNode* message, JsonRpcApi* api, int channelId, Status* error) 
{
    Status transportStatus = OK;
    char* functionName;
    if (AllocateString(message, FUNCTION, &functionName) == JSON_OK)
    {
        transportStatus = WriteJsonStringMember(channelId, FUNCTION, functionName);
        if (transportStatus == OK)
        {
            Function* function = FindFunction(functionName, api);
            if (function != NULL)
            {
                JsonNode parameters;
                JsonStatus jsonStatus = GetObject(message, PARAMETERS, &parameters);
                if (jsonStatus == JSON_NAME_NOT_PRESENT)
                {
                    jsonStatus = EmptyObject(&parameters);
                }
                if (jsonStatus == JSON_OK)
                {
                    transportStatus = WriteJsonMemberName(channelId, RETURN_VALUE);
                    if (transportStatus == OK)
                    {
                        transportStatus = function->Call(&parameters, error);
                    }
                }
            }
            else 
            {
                *error = "UnknownFunction";
            }
        }
        free(functionName);
    }
    else
    {
        *error = "NoFunction";
    }
    return transportStatus;
}


/*
** Interface
*/

Status HandleMessage(char* jsonString, JsonRpcApi* api, int channelId)
{
    JsonNode message;
    char* direction;
    Status status = UNINITIALIZED;
    Status transportStatus = WriteJsonRootStart(channelId);
    if (transportStatus == OK)
    {
        Initialize(jsonString, &message);
        if (AllocateString(&message, DIRECTION, &direction) == JSON_OK)
        {
            if (strcmp(direction, CALL) == 0)
            {
                transportStatus = WriteJsonStringMember(channelId, DIRECTION, RETURN);
                if (transportStatus == OK) 
                {
                    transportStatus = HandleCall(&message, api, channelId, &status);
                }
            }
            else
            {
                transportStatus = WriteJsonStringMember(channelId, DIRECTION, FIRE);
                status = "InvalidDirection";
            }
            free(direction);
        }
        else
        {
            transportStatus = WriteJsonStringMember(channelId, DIRECTION, FIRE);
            status =  "InvalidMessageReceived";
        }
        if ((transportStatus == OK) && (status != UNINITIALIZED))
        {
            transportStatus = WriteJsonStringMember(channelId, STATUS, status);
        }
        if (transportStatus == OK)
        {
            transportStatus = WriteJsonRootEnd(channelId);
        }
    }
    return transportStatus;
}


Status FireTextEvent(const char* name, const char* value, int channelId)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonRootStart(channelId))
    VALIDATE(WriteJsonStringMember(channelId, DIRECTION, FIRE))
    VALIDATE(WriteJsonStringMember(channelId, name, value))
    return WriteJsonRootEnd(channelId);
}


Status FireIntegerEvent(const char* name, int value, int channelId)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonRootStart(channelId))
    VALIDATE(WriteJsonStringMember(channelId, DIRECTION, FIRE))
    VALIDATE(WriteJsonIntegerMember(channelId, name, value))
    return WriteJsonRootEnd(channelId);
}


Status FireRealEvent(const char* name, double value, int channelId)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonRootStart(channelId))
    VALIDATE(WriteJsonStringMember(channelId, DIRECTION, FIRE))
    VALIDATE(WriteJsonRealMember(channelId, name, value))
    return WriteJsonRootEnd(channelId);
}


Status FireErrorEvent(const char* error, int channelId)
{
    return FireTextEvent(ERROR, error, channelId);
}
