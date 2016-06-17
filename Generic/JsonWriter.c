#include "JsonWriter.h"

#include <stdio.h>
#include <string.h>

#include "Json.h"
#include "Communication.h"


/*
** private
*/

bool valueWritten = FALSE;


Status WriteUnicode(int channelId, char character)
{
    char unicode[7];
    sprintf(unicode, "\\u%04x", character);
    return WriteString(channelId, unicode);
}


Status WriteControlCharacter(int channelId, char character)
{
    switch (character)
    {
        case '\b':
            return WriteString(channelId, "\\b");
        case '\f':
            return WriteString(channelId, "\\f");
        case '\r':
            return WriteString(channelId, "\\r");
        case '\n':
            return WriteString(channelId, "\\n");
        case '\t':
            return WriteString(channelId, "\\t");
        default:
            return WriteUnicode(channelId, character);
    }
}


Status WriteStringContent(int channelId, const char* string)
{
    RETURN_WHEN_INVALID
    size_t length = strlen(string);
    size_t i = 0;
    while (i < length)
    {
        char character = string[i];
        if (IsUnicodeControl(character))
        {
            VALIDATE(WriteControlCharacter(channelId, character))
        }
        else if (character == '\"')
        {
            VALIDATE(WriteString(channelId, "\\\""))
        }
        else if (character == '\\')
        {
            VALIDATE(WriteString(channelId, "\\\\"))
        }
        else
        {
            VALIDATE(WriteCharacter(channelId, character))
        }
        i++;
    }
    return OK;
}


Status WriteJsonSeparator(int channelId)
{
    if (valueWritten)
    {
        return WriteCharacter(channelId, ELEMENT_SEPARATOR);
    }
    else
    {
        return OK;
    }
}


/*
** Interface
*/

Status WriteJsonNull(int channelId)
{
    valueWritten = TRUE;
    return WriteString(channelId, JSON_NULL_LITERAL);
}


Status WriteJsonBoolean(int channelId, bool value)
{
    valueWritten = TRUE;
    return WriteString(channelId, value ? JSON_TRUE_LITERAL : JSON_FALSE_LITERAL);
}


Status WriteJsonInteger(int channelId, int value)
{
    char valueString[16];
    sprintf(valueString, "%d", value);
    valueWritten = TRUE;
    return WriteString(channelId, valueString);
}


Status WriteJsonReal(int channelId, double value)
{
    char valueString[32];
    sprintf(valueString, "%.15g", value);
    valueWritten = TRUE;
    return WriteString(channelId, valueString);
}


Status WriteJsonString(int channelId, const char* string)
{
    RETURN_WHEN_INVALID
    valueWritten = TRUE;
    VALIDATE(WriteCharacter(channelId, STRING_START))
    VALIDATE(WriteString(channelId, string))
    return WriteCharacter(channelId, STRING_END);
}


Status WriteJsonRootStart(int channelId)
{
    valueWritten = FALSE;
    return WriteCharacter(channelId, OBJECT_START);
}


Status WriteJsonRootEnd(int channelId)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonObjectEnd(channelId))
    return FinishTransmission(channelId);
}


Status WriteJsonObjectStart(int channelId)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonSeparator(channelId))
    valueWritten = FALSE;
    return WriteCharacter(channelId, OBJECT_START);
}


Status WriteJsonObjectEnd(int channelId)
{
    valueWritten = TRUE;
    return WriteCharacter(channelId, OBJECT_END);
}


Status WriteJsonMemberName(int channelId, const char* name)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonSeparator(channelId))
    VALIDATE(WriteJsonString(channelId, name))
    valueWritten = FALSE;
    return WriteCharacter(channelId, NAME_VALUE_SEPARATOR);
}


Status WriteJsonBooleanMember(int channelId, const char* name, bool value)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonMemberName(channelId, name))
    return WriteJsonBoolean(channelId, value);
}


Status WriteJsonNullMember(int channelId, const char* name)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonMemberName(channelId, name))
    return WriteJsonNull(channelId);
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
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonSeparator(channelId))
    valueWritten = FALSE;
    return WriteCharacter(channelId, ARRAY_START);
}


Status WriteJsonArrayEnd(int channelId)
{
    valueWritten = TRUE;
    return WriteCharacter(channelId, ARRAY_END);
}


Status WriteJsonNullElement(int channelId)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonSeparator(channelId))
    return WriteJsonNull(channelId);    
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
