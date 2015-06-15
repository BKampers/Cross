#include "JsonComposer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Text.h"

#define EMPTY_SOURCE_SIZE 3


typedef enum
{
    LITERAL,
    STRING,
    INTEGER,
    REAL,
    NODE
} ValueType;


char JSON_NULL_LITERAL[] = "null";
char JSON_FALSE_LITERAL[] = "false";
char JSON_TRUE_LITERAL[] = "true";


char* EMPTY_OBJECT_SOURCE = "{}";
char* EMPTY_ARRAY_SOURCE = "[]";


bool IsEmpty(const JsonNode* node)
{
    return
        ((node->type == JSON_OBJECT) && (strcmp(node->source, EMPTY_OBJECT_SOURCE) == 0)) ||
        ((node->type == JSON_ARRAY) && (strcmp(node->source, EMPTY_ARRAY_SOURCE) == 0));
}


TextStatus PrepareBuffer(StringBuffer* buffer, const JsonNode* node)
{
    TextStatus status;
    InitializeStringBuffer(buffer);
    status = AppendSubstring(buffer, node->source, 0, strlen(node->source) - 1);
    if ((status == TEXT_OK) && ! IsEmpty(node))
    {
        status = AppendCharacter(buffer, ELEMENT_SEPARATOR);
    }
    return status;
}


TextStatus AppendUnicode(StringBuffer* buffer, char character)
{
    char unicode[7];
    sprintf(unicode, "\\u%04x", character);
    return AppendString(buffer, unicode);
}


TextStatus AppendControlCharacter(StringBuffer* buffer, char character)
{
    switch (character)
    {
        case '\b':
            return AppendString(buffer, "\\b");
        case '\f':
            return AppendString(buffer, "\\f");
        case '\r':
            return AppendString(buffer, "\\r");
        case '\n':
            return AppendString(buffer, "\\n");
        case '\t':
            return AppendString(buffer, "\\t");
        default:
            return AppendUnicode(buffer, character);
    }
}


TextStatus AppendQuoted(StringBuffer* buffer, const char* string)
{
    size_t length = strlen(string);
    size_t i = 0;
    TextStatus status = AppendCharacter(buffer, STRING_START);
    while ((i < length) && (status == TEXT_OK))
    {
        char character = string[i];
        if (IsUnicodeControl(character))
        {
            status = AppendControlCharacter(buffer, character);
        }
        else if (character == '\"')
        {
            status = AppendString(buffer, "\\\"");
        }
        else
        {
            status = AppendCharacter(buffer, character);
        }
        i++;
    }
    if (status == TEXT_OK)
    {
        status = AppendCharacter(buffer, STRING_END);
    }
    return status;
}


TextStatus AppendName(StringBuffer* buffer, const char* name)
{
    TextStatus status = AppendQuoted(buffer, name);
    if (status == TEXT_OK)
    {
        status = AppendCharacter(buffer, NAME_VALUE_SEPARATOR);
    }
    return status;
}


TextStatus AppendValue(StringBuffer* buffer, const void* value, ValueType type)
{
    switch (type)
    {
        case REAL:
        {
            char string[32];
            sprintf(string, "%.15g", *((double*) value));
            return AppendString(buffer, string);
        }
        case INTEGER:
        {
            char string[16];
            sprintf(string, "%ld", *((long*) value));
            return AppendString(buffer, string);
        }
        case STRING:
        {
            return AppendQuoted(buffer, (const char*) value);
        }
        case NODE:
        {
            return AppendSubstring(buffer, ((JsonNode*) value)->source, 0, ((JsonNode*) value)->length);
        }
        case LITERAL:
        {
            return AppendString(buffer, (const char*) value);
        }
    }
}


TextStatus BuildBuffer(StringBuffer* buffer, JsonNode* node, const char* name, const void* value, ValueType type)
{
    TextStatus status = PrepareBuffer(buffer, node);
    if ((status == TEXT_OK) && (name != NULL))
    {
        status = AppendName(buffer, name);
    }
    if (status == TEXT_OK)
    {
        status = AppendValue(buffer, value, type);
    }
    if (status == TEXT_OK)
    {
        status = AppendCharacter(buffer, (node->type == JSON_OBJECT) ? OBJECT_END : ARRAY_END);
    }
    return status;
}


JsonStatus AddToNode(JsonNode* node, const char* name, const void* value, ValueType type)
{
    JsonStatus jsonStatus;
    if (node == NULL)
    {
        jsonStatus = JSON_INVALID_PARAMETER;
    }
    else if ((name != NULL) && (node->type != JSON_OBJECT))
    {
        jsonStatus = JSON_OBJECT_EXPECTED;
    }
    else if ((name == NULL) && (node->type != JSON_ARRAY))
    {
        jsonStatus = JSON_ARRAY_EXPECTED;
    }
    else
    {
        StringBuffer buffer;
        TextStatus status = BuildBuffer(&buffer, node, name, value, type);
        char* newSource = DetachString(&buffer);
        if (status == TEXT_OK)
        {
            free(node->source);
            node->source = newSource;
            node->length = strlen(newSource);
            jsonStatus = JSON_OK;
        }
        else
        {
            free(newSource);
            jsonStatus = JSON_OUT_OF_MEMORY;
        }
    }
    return jsonStatus;
}


/*
** Interface
*/

JsonStatus ComposeObject(JsonNode* object)
{
    object->source = malloc(EMPTY_SOURCE_SIZE);
    if (object->source != NULL)
    {
        strcpy(object->source, EMPTY_OBJECT_SOURCE);
        object->type = JSON_OBJECT;
        object->length = EMPTY_SOURCE_SIZE - 1;
        return JSON_OK;
    }
    else
    {
        return JSON_OUT_OF_MEMORY;
    }
}


JsonStatus PutObjectMember(JsonNode* object, const char* name, const JsonNode* value)
{
    if ((value != NULL) && (value->type == JSON_OBJECT))
    {
        return AddToNode(object, name, (const void*) value, NODE);
    }
    else
    {
        return JSON_INVALID_PARAMETER;
    }
}


JsonStatus PutArrayMember(JsonNode* object, const char* name, const JsonNode* value)
{
    if ((value != NULL) && (value->type == JSON_ARRAY))
    {
        return AddToNode(object, name, (const void*) value, NODE);
    }
    else
    {
        return JSON_INVALID_PARAMETER;
    }
}


JsonStatus PutStringMember(JsonNode* object, const char* name, const char* value)
{
    return AddToNode(object, name, value, STRING);
}


JsonStatus PutIntegerMember(JsonNode* object, const char* name, long value)
{
    return AddToNode(object, name, (const void*) &value, INTEGER);
}


JsonStatus PutRealMember(JsonNode* object, const char* name, double value)
{
    return AddToNode(object, name, (const void*) &value, REAL);
}


JsonStatus PutBooleanMember(JsonNode* object, const char* name, bool value)
{
    return AddToNode(object, name, (value) ? JSON_TRUE_LITERAL : JSON_FALSE_LITERAL, LITERAL);
}


JsonStatus PutNullMember(JsonNode* object, const char* name)
{
    return AddToNode(object, name, JSON_NULL_LITERAL, LITERAL);
}


JsonStatus ComposeArray(JsonNode* array)
{
    array->source = malloc(EMPTY_SOURCE_SIZE);
    if (array->source != NULL)
    {
        strcpy(array->source, EMPTY_ARRAY_SOURCE);
        array->type = JSON_ARRAY;
        array->length = EMPTY_SOURCE_SIZE - 1;
        return JSON_OK;
    }
    else
    {
        return JSON_OUT_OF_MEMORY;
    }
}


JsonStatus AddObjectElement(JsonNode* array, const JsonNode* element)
{
    if ((element != NULL) && (element->type == JSON_OBJECT))
    {
        return AddToNode(array, NULL, (const void*) element, NODE);
    }
    else
    {
        return JSON_INVALID_PARAMETER;
    }
}


JsonStatus AddArrayElement(JsonNode* array, const JsonNode* element)
{
    if ((element != NULL) && (element->type == JSON_ARRAY))
    {
        return AddToNode(array, NULL, (const void*) element, NODE);
    }
    else
    {
        return JSON_INVALID_PARAMETER;
    }
}


JsonStatus AddStringElement(JsonNode* array, const char* element)
{
    return AddToNode(array, NULL, element, STRING);
}


JsonStatus AddIntegerElement(JsonNode* array, long value)
{
    return AddToNode(array, NULL, (const void*) &value, INTEGER);
}


JsonStatus AddRealElement(JsonNode* array, double value)
{
    return AddToNode(array, NULL, (const void*) &value, REAL);
}


JsonStatus AddBooleanElement(JsonNode* array, bool value)
{
    char* valueString = (value) ? JSON_TRUE_LITERAL : JSON_FALSE_LITERAL;
    return AddToNode(array, NULL, valueString, LITERAL);
}


JsonStatus AddNullElement(JsonNode* array)
{
    return AddToNode(array, NULL, JSON_NULL_LITERAL, LITERAL);
}
