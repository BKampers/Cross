#ifndef __JSONMESSAGE_H__
#define __JSONMESSAGE_H__

#include "ApiStatus.h"
#include "Types.h"
#include "jsonparse.h"


Status FindPair(const char* jsonString, const char* name, struct jsonparse_state* state);

Status GetBoolValue(const char* jsonString, const char* name, bool* value);
Status GetIntValue(const char* jsonString, const char* name, int* value);
Status GetFloatValue(const char* jsonString, const char* name, float* value);

Status AsBool(const struct jsonparse_state* state, bool* value);
Status AsInt(const struct jsonparse_state* state, int* value);
Status AsFloat(const struct jsonparse_state* state, float* value);

bool EqualString(const struct jsonparse_state* state, const char* value);
bool Contains(const char* jsonString, const char* name, const char* value);


#endif /* __JSONMESSAGE_H__ */
