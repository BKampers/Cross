#ifndef _JSONPARSER_H_
#define	_JSONPARSER_H_

#include <stdlib.h>

#include "Types.h"
#include "Json.h"

void Initialize(const char* source, JsonNode* node);

JsonStatus AllocateString(const JsonNode* object, const char* name, char** value);
JsonStatus GetDouble(const JsonNode* object, const char* name, double* value);
JsonStatus GetFloat(const JsonNode* object, const char* name, float* value);
JsonStatus GetInt(const JsonNode* object, const char* name, int* value);
JsonStatus GetBoolean(const JsonNode* object, const char* name, bool* value);
JsonStatus GetObject(const JsonNode* object, const char* name, JsonNode* value);
JsonStatus GetArray(const JsonNode* object, const char* name, JsonNode* value);


JsonStatus GetCount(const JsonNode* array, int* count);
JsonStatus GetDoubletAt(const JsonNode* array, int index, double* element);
JsonStatus GetFloatAt(const JsonNode* array, int index, float* element);
JsonStatus GetIntAt(const JsonNode* array, int index, int* element);
JsonStatus GetObjectAt(const JsonNode* array, int index, JsonNode* element);
JsonStatus GetArrayAt(const JsonNode* array, int index, JsonNode* element);
JsonStatus IndexOfString(const JsonNode* array, const char* value, int* index);

#endif	/* _JSONPARSER_H_ */

