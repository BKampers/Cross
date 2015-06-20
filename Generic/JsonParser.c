#include "JsonParser.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "Text.h"


const char* WHITE_SPACES = " \f\n\r\t";
const char* NUMBER_CHARACTERS = ".+-Ee";


void ScanNext(const char* source, JsonNode* node);


void InitializeNode(JsonNode* node, const char* source)
{
    node->source = source;
    node->type = JSON_INVALID;
    node->length = 0;
}


bool IsWhiteSpace(char character)
{
    return strchr(WHITE_SPACES, character) != NULL;
}


bool IsValue(const JsonNode* node)
{
    switch (node->type)
    {
        case JSON_OBJECT:
        case JSON_ARRAY:
        case JSON_STRING:
        case JSON_NUMBER:
        case JSON_BOOLEAN:
        case JSON_NULL:
            return TRUE;
        default:
            return FALSE;
    }
}


void ScanPairValue(JsonNode* nameNode, JsonNode* pairNode)
{
    JsonNode separatorNode;
    ScanNext(nameNode->source + nameNode->length, &separatorNode);
    if (separatorNode.type == JSON_NAME_VALUE_SEPARATOR)
    {
        JsonNode valueNode;
        ScanNext(separatorNode.source + separatorNode.length, &valueNode);
        if (IsValue(&valueNode))
        {
            pairNode->type = JSON_PAIR;
            pairNode->source = nameNode->source;
            pairNode->length = (size_t) (valueNode.source - nameNode->source) + valueNode.length;
        }
    }    
}


void ScanObject(JsonNode* node)
{
    JsonNode next;
    ScanNext(node->source + 1, &next);
    while (next.type == JSON_STRING)
    {
        ScanPairValue(&next, &next);
        if (next.type == JSON_PAIR)
        {
            ScanNext(next.source + next.length, &next);
            if (next.type == JSON_ELEMENT_SEPARATOR)
            {
                ScanNext(next.source + next.length, &next);            
            }
        }
        else
        {
            next.type = JSON_INVALID;
        }
    }
    if (next.type == JSON_OBJECT_END)
    {
        node->type = JSON_OBJECT;
        node->length = (size_t) (next.source - node->source) + next.length;
    }
}


void ScanArray(JsonNode* node)
{
    JsonNode next;
    ScanNext(node->source + 1, &next);
    while (IsValue(&next))
    {
        ScanNext(next.source + next.length, &next);
        if (next.type == JSON_ELEMENT_SEPARATOR)
        {
            ScanNext(next.source + next.length, &next);            
        }
    }
    if (next.type == JSON_ARRAY_END)
    {
        node->type = JSON_ARRAY;
        node->length = (size_t) (next.source - node->source) + next.length;
    }
}


void ScanString(JsonNode* node)
{
    bool scanning = TRUE;
    node->length = 1;
    while (scanning)
    {
        char character = *(node->source + node->length);
        if (character != '\0')
        {
            node->length++;
            if (character == ESCAPE_CHARACTER)
            {
                node->length++;
            }
            else if (IsUnicodeControl(character))
            {
                scanning = FALSE;
            }
            else if (character == STRING_END)
            {
                node->type = JSON_STRING;
                scanning = FALSE;
            }
        }
        else
        {
            scanning = FALSE;
        }
    }
}


bool ScanMinus(JsonNode* node)
{
    char character = *(node->source + node->length);
    if (character == '-')
    {
        node->length++;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


bool ScanSign(JsonNode* node)
{
    char character = *(node->source + node->length);
    if ((character == '-') || (character == '+'))
    {
        node->length++;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


bool ScanDecimalSeparator(JsonNode* node)
{
    char character = *(node->source + node->length);
    if (character == '.')
    {
        node->length++;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


bool ScanExponentSeparator(JsonNode* node)
{
    char character = *(node->source + node->length);
    if ((character == 'e') || (character == 'E'))
    {
        node->length++;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


bool ScanZero(JsonNode* node)
{
    char character = *(node->source + node->length);
    if (character == '0')
    {
        node->length++;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


bool ScanPositiveDigit(JsonNode* node)
{
    char character = *(node->source + node->length);
    if (('1' <= character) && (character <= '9'))
    {
        node->length++;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


bool ScanDigit(JsonNode* node)
{
    if (isdigit(*(node->source + node->length)))
    {
        node->length++;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


bool ScanDigits(JsonNode* node)
{
    int count = 0;
    while (ScanDigit(node))
    {
        count++;
    }
    return count > 0;
}


bool ScanInteger(JsonNode* node)
{
    ScanMinus(node);
    if (ScanZero(node))
    {
        node->type = JSON_NUMBER;
    }
    else
    {
        if (ScanPositiveDigit(node))
        {
            node->type = JSON_NUMBER;
            ScanDigits(node);
        }    
    }
    return node->type == JSON_NUMBER;
}


void ScanFraction(JsonNode* node)
{
    if (ScanDecimalSeparator(node))
    {
        if (! ScanDigits(node))
        {
            node->type = JSON_INVALID;
        }
    }
}


void ScanExponent(JsonNode* node)
{
    if (ScanExponentSeparator(node))
    {
        ScanSign(node);
        if (! ScanDigits(node))
        {
            node->type = JSON_INVALID;
        }
    }
}


void ScanNumber(JsonNode* node)
{
    if (ScanInteger(node))
    {
        ScanFraction(node);
        ScanExponent(node);
    }
}


bool ScanLiteral(JsonNode* node, const char* literal)
{
    size_t length = strlen(literal);
    if (strncmp(node->source, literal, length) == 0)
    {
        node->length = length;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


void ScanBoolean(JsonNode* node)
{
    if (ScanLiteral(node, JSON_FALSE_LITERAL) || ScanLiteral(node, JSON_TRUE_LITERAL))
    {
        node->type = JSON_BOOLEAN;
    }
}


void ScanNull(JsonNode* node)
{
    if (ScanLiteral(node, JSON_NULL_LITERAL))
    {
        node->type = JSON_NULL;
    }
}


void ScanNext(const char* source, JsonNode* node)
{
    char character;
    InitializeNode(node, source);
    while (IsWhiteSpace(*(node->source)))
    {
        node->source++;
    }
    character = *(node->source);
    if (character == OBJECT_START)
    {
        ScanObject(node);
    }
    else if (character == ARRAY_START)
    {
        ScanArray(node);
    }
    else if (character == STRING_START)
    {
        ScanString(node);
    }
    else if ((character == '-') || isdigit(character))
    {
        ScanNumber(node);
    }
    else if ((character == *JSON_FALSE_LITERAL) || (character == *JSON_TRUE_LITERAL))
    {
        ScanBoolean(node);
    }
    else if (character == *JSON_NULL_LITERAL)
    {
        ScanNull(node);
    }
    else if (character == NAME_VALUE_SEPARATOR)
    {
        node->type = JSON_NAME_VALUE_SEPARATOR;
        node->length = 1;
    }
    else if (character == ELEMENT_SEPARATOR)
    {
        node->type = JSON_ELEMENT_SEPARATOR;
        node->length = 1;
    }
    else if (character == ARRAY_END)
    {
        node->type = JSON_ARRAY_END;
        node->length = 1;
    }
    else if (character == OBJECT_END)
    {
        node->type = JSON_OBJECT_END;
        node->length = 1;
    }
}


void ScanPair(char* source, JsonNode* pairNode)
{
    JsonNode nameNode;
    ScanNext(source, &nameNode);
    if (nameNode.type == JSON_STRING)
    {
        ScanPairValue(&nameNode, pairNode);
    }
}


void ScanNextPair(const JsonNode* offsetPairNode, JsonNode* nextPairNode)
{
    JsonNode elementSeparatorNode;
    InitializeNode(nextPairNode, offsetPairNode->source);
    ScanNext(offsetPairNode->source + offsetPairNode->length, &elementSeparatorNode);
    if (elementSeparatorNode.type == JSON_ELEMENT_SEPARATOR)
    {
        ScanPair(elementSeparatorNode.source + elementSeparatorNode.length, nextPairNode);
    }
    else if (elementSeparatorNode.type == JSON_OBJECT_END)
    {
        *nextPairNode = elementSeparatorNode;
    }
}


JsonStatus FindPair(const JsonNode* object, const char* name, JsonNode* pair)
{
    if ((object != NULL) && (object->type == JSON_OBJECT))
    {
        InitializeNode(pair, object->source);
        ScanPair(object->source + 1, pair);
        while (pair->type == JSON_PAIR)
        {
            JsonNode nameNode;
            ScanNext(pair->source, &nameNode);
            if (nameNode.type == JSON_STRING)
            {
                if (strncmp(name, nameNode.source + 1, nameNode.length - 2) == 0)
                {
                    return JSON_OK;
                }
            }
            JsonNode nextNode;
            ScanNextPair(pair, &nextNode);
            *pair = nextNode;
        }
        return JSON_NAME_NOT_PRESENT;
    }
    else
    {
        return JSON_OBJECT_EXPECTED;
    }
}


void GetValue(const JsonNode* pair, JsonNode* value)
{
    InitializeNode(value, pair->source);
    if ((pair != NULL) && (pair->type == JSON_PAIR))
    {
        JsonNode name;
        ScanNext(pair->source, &name);
        if (name.type == JSON_STRING)
        {
            JsonNode separator;
            ScanNext(name.source + name.length, &separator);
            if (separator.type == JSON_NAME_VALUE_SEPARATOR)
            {
                ScanNext(separator.source + separator.length, value);
            }
        }
    }
}


JsonStatus GetNode(const JsonNode* object, const char* name, JsonType type, JsonNode* value)
{
    JsonNode pair;
    JsonStatus status = FindPair(object, name, &pair);
    if (status == JSON_OK)
    {
        JsonNode node;
        GetValue(&pair, &node);
        if (node.type == type)
        {
            *value = node;
        }
        else
        {
            status = JSON_TYPE_MISMATCH;
        }
    }
    return status;
}


JsonStatus GetNodeAt(const JsonNode* array, int index, JsonType type, JsonNode* element)
{
    if ((array != NULL) && (array->type == JSON_ARRAY))
    {
        char* elementSource = array->source + 1;
        while (index >= 0)
        {
            ScanNext(elementSource, element);
            if (element->type == JSON_ARRAY_END)
            {
                return JSON_OUT_OF_BOUNDS;
            }
            else if (index == 0)
            {
                return (element->type == type) ? JSON_OK : JSON_TYPE_MISMATCH;
            }
            else if (element->type == JSON_ELEMENT_SEPARATOR)
            {
                index--;
            }
            elementSource = element->source + element->length;
        }
        return JSON_INVALID_PARAMETER;
    }
    else
    {
        return JSON_ARRAY_EXPECTED;
    }
}


/* 
** Interface
*/

void Initialize(const char* source, JsonNode* node)
{
    ScanNext(source, node);
}


JsonStatus ProcessUnicode(const JsonNode* node, size_t* index, char* character)
{
    JsonStatus status = JSON_OK;
    int unicode = 0;
    int i;
    for (i = 1; (i <= 4) && (status == JSON_OK); ++i)
    {
        (*index)++;
        if (*index < node->length - 1)
        {
            char digit = *(node->source + *index);
            unicode <<= 4;
            if (('0' <= digit) && (digit <= '9'))
            {
                unicode |= digit - '0';
            }
            else if (('A' <= digit) && (digit <= 'F'))
            {
                unicode |= digit - 'A' + 10;
            }
            else if (('a' <= digit) && (digit <= 'f'))
            {
                unicode |= digit - 'a' + 10;
            }
            else 
            {
                status = JSON_INVALID_STRING;
            }
        }
        else
        {
            status = JSON_INVALID_STRING;
        }
    }
    *character = ((0x00 < unicode) && (unicode < 0xFF)) ? (char) unicode : 0xFF;
    return status;
}


JsonStatus ProcessEscapeCharacter(const JsonNode* node, size_t* index, char* character)
{
    JsonStatus status = JSON_OK;
    (*index)++;
    if (*index < node->length - 1)
    {
        char escaped = *(node->source + *index);
        switch (escaped)
        {
            case '\"':
            case '\\':
            case '/':
                *character = escaped;
                break;
            case 'b':
                *character = '\b';
                break;
            case 'f':
                *character = '\f';
                break;
            case 'n':
                *character = '\n';
                break;
            case 'r':
                *character = '\r';
                break;
            case 't':
                *character = '\t';
                break;
            case 'u':
                status = ProcessUnicode(node, index, character);
                break;
            default: 
                status = JSON_INVALID_STRING;
                break;
        }
    }
    else
    {
        status = JSON_INVALID_STRING;
    }
    return status;
}


JsonStatus AllocateString(const JsonNode* object, const char* name, char** value)
{
    JsonNode node;
    JsonStatus status = GetNode(object, name, JSON_STRING, &node);
    if (status == JSON_OK)
    {
        size_t i;
        size_t j = 0;
        *value = malloc(node.length - 2);
        for (i = 1; (i < node.length - 1) && status == JSON_OK; ++i)
        {
            char character = *(node.source + i);
            if (character == '\\')
            {
                status = ProcessEscapeCharacter(&node, &i, &character);
            }
            (*value)[j] = character;
            j++;
        }
        (*value)[j] = '\0';
    }
    else
    {
        *value = NULL;
    }
    return status;
}


JsonStatus GetDouble(const JsonNode* object, const char* name, double* value)
{
    JsonNode node;
    JsonStatus status = GetNode(object, name, JSON_NUMBER, &node);
    if (status == JSON_OK)
    {
        *value = atof(node.source);
    }
    return status;
}


JsonStatus GetFloat(const JsonNode* object, const char* name, float* value)
{
    double doubleValue;
    JsonStatus status = GetDouble(object, name, &doubleValue);
    if (status == JSON_OK)
    {
        *value = (float) doubleValue;
    }
    return status;
}


JsonStatus GetInt(const JsonNode* object, const char* name, int* value)
{
    double doubleValue;
    JsonStatus status = GetDouble(object, name, &doubleValue);
    if (status == JSON_OK)
    {
        *value = (int) doubleValue;
    }
    return status;
}


JsonStatus GetBoolean(const JsonNode* object, const char* name, bool* value)
{
    JsonNode node;
    JsonStatus status = GetNode(object, name, JSON_BOOLEAN, &node);
    if (status == JSON_OK)
    {
        *value = (strncmp(JSON_TRUE_LITERAL, node.source, node.length) == 0);
    }
    return status;
}


JsonStatus GetObject(const JsonNode* object, const char* name, JsonNode* value)
{
    return GetNode(object, name, JSON_OBJECT, value);
}


JsonStatus GetArray(const JsonNode* object, const char* name, JsonNode* value)
{
    return GetNode(object, name, JSON_ARRAY, value);
}


JsonStatus GetDoubleAt(const JsonNode* array, int index, double* element)
{
    JsonNode node;
    JsonStatus status = GetNodeAt(array, index, JSON_NUMBER, &node);
    if (status == JSON_OK)
    {
        *element = atof(node.source);
    }
    return status;
}


JsonStatus GetFloatAt(const JsonNode* array, int index, float* element)
{
    double doubleElement;
    JsonStatus status = GetDoubleAt(array, index, &doubleElement);
    if (status == JSON_OK)
    {
        *element = (float) doubleElement;
    }
    return status;
}


JsonStatus GetIntAt(const JsonNode* array, int index, int* element)
{
    double doubleElement;
    JsonStatus status = GetDoubleAt(array, index, &doubleElement);
    if (status == JSON_OK)
    {
        *element = (int) doubleElement;
    }
    return status;
}


JsonStatus GetObjectAt(const JsonNode* array, int index, JsonNode* element)
{
    return GetNodeAt(array, index, JSON_OBJECT, element);
}


JsonStatus GetArrayAt(const JsonNode* array, int index, JsonNode* element)
{
    return GetNodeAt(array, index, JSON_ARRAY, element);
}


JsonStatus IndexOfString(const JsonNode* array, const char* value, int* index)
{
    JsonStatus status = JSON_ARRAY_EXPECTED;
    if ((array != NULL) && (array->type == JSON_ARRAY))
    {
        char* elementSource = array->source + 1;
        *index = 0;
        while ((status != JSON_OK) && (status != JSON_VALUE_NOT_PRESENT))
        {
            JsonNode element;
            ScanNext(elementSource, &element);
            if (element.type == JSON_ARRAY_END)
            {
                status = JSON_VALUE_NOT_PRESENT;
            }
            else if ((element.type == JSON_STRING) && (strncmp(value, element.source + 1, element.length - 2) == 0))
            {
                status = JSON_OK;
            }
            else if (element.type == JSON_ELEMENT_SEPARATOR)
            {
                (*index)++;
            }
            elementSource = element.source + element.length;
        }
    }
    return status;
}