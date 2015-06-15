#ifndef __JSONCOMPOSER_H__
#define	__JSONCOMPOSER_H__

#include <stdlib.h>

#include "Types.h"
#include "Json.h"

JsonStatus ComposeObject(JsonNode* object);
JsonStatus PutObjectMember(JsonNode* object, const char* name, const JsonNode* value);
JsonStatus PutArrayMember(JsonNode* object, const char* name, const JsonNode* value);
JsonStatus PutStringMember(JsonNode* object, const char* name, const char* value);
JsonStatus PutIntegerMember(JsonNode* object, const char* name, long value);
JsonStatus PutRealMember(JsonNode* object, const char* name, double value);
JsonStatus PutBooleanMember(JsonNode* object, const char* name, bool value);
JsonStatus PutNullMember(JsonNode* object, const char* name);

JsonStatus ComposeArray(JsonNode* array);
JsonStatus AddObjectElement(JsonNode* array, const JsonNode* element);
JsonStatus AddArrayElement(JsonNode* array, const JsonNode* element);
JsonStatus AddStringElement(JsonNode* array, const char* element);
JsonStatus AddIntegerElement(JsonNode* array, long value);
JsonStatus AddRealElement(JsonNode* array, double value);
JsonStatus AddBooleanElement(JsonNode* array, bool value);
JsonStatus AddNullElement(JsonNode* array);

#endif	/* __JSONCOMPOSER_H__ */

