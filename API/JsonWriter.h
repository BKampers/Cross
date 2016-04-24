#ifndef _JSONWRITER_H_
#define _JSONWRITER_H_

#include "Types.h"
#include "ApiStatus.h"

Status WriteJsonRootStart(int channelId);
Status WriteJsonRootEnd(int channelId);

Status WriteJsonObjectStart(int channelId);
Status WriteJsonObjectEnd(int channelId);
Status WriteJsonMemberName(int channelId, const char* name);
Status WriteJsonNullMember(int channelId, const char* name);
Status WriteJsonBooleanMember(int channelId, const char* name, bool value);
Status WriteJsonIntegerMember(int channelId, const char* name, int value);
Status WriteJsonRealMember(int channelId, const char* name, double value);
Status WriteJsonStringMember(int channelId, const char* name, const char* value);

Status WriteJsonArrayStart(int channelId);
Status WriteJsonArrayEnd(int channelId);
Status WriteJsonNullElement(int channelId);
Status WriteJsonBooleanElement(int channelId, bool value);
Status WriteJsonIntegerElement(int channelId, int value);
Status WriteJsonRealElement(int channelId, double value);
Status WriteJsonStringElement(int channelId, const char* value);

#endif /* _JSONWRITER_H_ */
