#include "JsonWriter.h"

#include <stdio.h>

#include "Json.h"
#include "Communication.h"


/*
** private
*/

bool elementWritten = FALSE;


Status WriteJsonBoolean(int channelId, bool value)
{
    return WriteChannel(channelId, value ? JSON_TRUE_LITERAL : JSON_FALSE_LITERAL);
}


Status WriteJsonInteger(int channelId, int value)
{
    char valueString[16];
    sprintf(valueString, "%d", value);
    return WriteChannel(channelId, valueString);
}


Status WriteJsonReal(int channelId, double value)
{
    char valueString[32];
    sprintf(valueString, "%.15g", value);
    return WriteChannel(channelId, valueString);
}


Status WriteJsonString(int channelId, const char* string)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteCharacter(channelId, STRING_START))
    VALIDATE(WriteChannel(channelId, string))
    return WriteCharacter(channelId, STRING_END);
}


/*
** Interface
*/

Status WriteJsonObjectStart(int channelId)
{
    elementWritten = FALSE;
    return WriteCharacter(channelId, OBJECT_START);
}


Status WriteJsonObjectEnd(int channelId)
{
    elementWritten = TRUE;
    return WriteCharacter(channelId, OBJECT_END);
}


Status WriteJsonMemberName(int channelId, const char* name)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonSeparator(channelId))
    return WriteJsonString(channelId, name);
}


Status WriteJsonBooleanMember(int channelId, const char* name, bool value)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonMemberName(channelId, name))
    return WriteJsonBoolean(channelId, value);
}


Status WriteJsonIntegerMember(int channelId, const char* name, int value)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonMemberName(channelId, name))
    return WriteJsonInteger(channelId, value);
}


Status WriteJsonRealMember(int channelId, const char* name, double value)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonMemberName(channelId, name))
    return WriteJsonReal(channelId, value);
}


Status WriteJsonStringMember(int channelId, const char* name, const char* value)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonMemberName(channelId, name))
    return WriteJsonString(channelId, value);
}


Status WriteJsonArrayStart(int channelId)
{
    elementWritten = FALSE;
    return WriteCharacter(channelId, ARRAY_START);
}


Status WriteJsonArrayEnd(int channelId)
{
    elementWritten = TRUE;
    return WriteCharacter(channelId, ARRAY_END);
}


Status WriteJsonBooleanElement(int channelId, bool value)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonSeparator(channelId))
    return WriteJsonBoolean(channelId, value);
}


Status WriteJsonIntegerElement(int channelId, int value)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonSeparator(channelId))
    return WriteJsonInteger(channelId, value);
}


Status WriteJsonRealElement(int channelId, double value)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonSeparator(channelId))
    return WriteJsonReal(channelId, value);
}


Status WriteJsonStringElement(int channelId, const char* value)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonSeparator(channelId))
    return WriteJsonString(channelId, value);
}


Status WriteJsonSeparator(int channelId)
{
    if (elementWritten)
    {
        return WriteCharacter(channelId, ELEMENT_SEPARATOR);
    }
    else
    {
        return OK;
    }
}
