#ifndef _JSON_H_
#define	_JSON_H_

#include <stdlib.h>
#include "Types.h"

extern char JSON_NULL_LITERAL[];
extern char JSON_FALSE_LITERAL[];
extern char JSON_TRUE_LITERAL[];

#define OBJECT_START '{'
#define OBJECT_END   '}'

#define ARRAY_START '['
#define ARRAY_END   ']'

#define STRING_START '\"'
#define STRING_END   '\"'

#define NAME_VALUE_SEPARATOR ':'
#define ELEMENT_SEPARATOR ','

#define ESCAPE_CHARACTER '\\'
#define UNICODE_CHARACTER 'u'


typedef enum
{
    JSON_OK,
    JSON_INVALID_PARAMETER,
    JSON_OUT_OF_MEMORY,
    JSON_OBJECT_EXPECTED,
    JSON_ARRAY_EXPECTED,
    JSON_NAME_NOT_PRESENT,
    JSON_VALUE_NOT_PRESENT,
    JSON_OUT_OF_BOUNDS,
    JSON_TYPE_MISMATCH,
    JSON_INVALID_STRING
} JsonStatus;


typedef enum
{ 
    JSON_INVALID,
    JSON_PAIR,
    JSON_NAME,
    JSON_VALUE,
    JSON_STRING,
    JSON_NUMBER,
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_BOOLEAN,
    JSON_NULL,
    JSON_ELEMENT_SEPARATOR,
    JSON_NAME_VALUE_SEPARATOR,
    JSON_ARRAY_END,
    JSON_OBJECT_END
} JsonType;


typedef struct 
{
    char* source;
    size_t length;
    JsonType type;
} JsonNode;


#endif	/* _JSON_H_ */

