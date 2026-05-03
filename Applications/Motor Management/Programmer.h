#ifndef _PROGRAMMER_H_
#define _PROGRAMMER_H_

#include "Types.h"
#include "ApiStatus.h"

Status GetTableProgrammable(const char* tableName, bool* programmable);
Status SetProgrammerActivated(const char* tableName, bool activated);
bool IsProgrammerActivated(const char* tableName);
Status ApplyProgrammerValue(const char* tableName, int* column, int* row, float* value);

#endif /* _PROGRAMMER_H_ */
