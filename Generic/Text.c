#include <stdlib.h>
#include <string.h>

#include "Types.h"

#include "Text.h"


#define STRING_BUFFER_INCREMENT 64


bool IsUnicodeControl(char character)
{
    return (character <= (char) 0x1F) || (((char) 0x7F <= character) && (character <= (char) 0x9F));
}


void InitializeStringBuffer(StringBuffer* stringBuffer)
{
    stringBuffer->buffer = NULL;
    stringBuffer->capacity = 0;
    stringBuffer->length = 0;
}


TextStatus AppendString(StringBuffer* stringBuffer, const char* string)
{
    return AppendSubstring(stringBuffer, string, 0, strlen(string));
}


TextStatus AppendSubstring(StringBuffer* stringBuffer, const char* string, size_t offset, size_t count)
{
    bool empty = stringBuffer->buffer == NULL;
    stringBuffer->length += count;
    if (stringBuffer->capacity < stringBuffer->length + 1)
    {
        stringBuffer->capacity = (stringBuffer->length / STRING_BUFFER_INCREMENT + 1) * STRING_BUFFER_INCREMENT;
        stringBuffer->buffer = realloc(stringBuffer->buffer, stringBuffer->capacity * sizeof(char));
    }
    if (stringBuffer->buffer != NULL)
    {
        if (empty)
        {
            strncpy(stringBuffer->buffer, string + offset, count);
            stringBuffer->buffer[count] = '\0';
        }
        else
        {
            strncat(stringBuffer->buffer, string + offset, count);    
        }
        return TEXT_OK;
    }
    else
    {
        return TEXT_OUT_OF_MEMORY;
    }    
}


TextStatus AppendCharacter(StringBuffer* stringBuffer, char character)
{
    return AppendSubstring(stringBuffer, &character, 0 , 1);
}


char* DetachString(StringBuffer* stringBuffer)
{
    char* string = NULL;
    if (stringBuffer->buffer != NULL)
    {
        string = malloc(stringBuffer->length + 1);
        if (string != NULL)
        {
            strcpy(string, stringBuffer->buffer);
            free(stringBuffer->buffer);
            InitializeStringBuffer(stringBuffer);
        }
    }
    return string;
}
