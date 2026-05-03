#include "Programmer.h"

#include <stdlib.h>
#include <string.h>

#include "Ignition.h"
#include "Injection.h"
#include "Measurements.h"

/*
** Private
*/

typedef struct
{
	const char* tableName;
	Status (*apply) (int* column, int* row, float* value);
	bool activated;
} Programmer;


Programmer programmers[] =
{
	{ IGNITION, &IgnitionApplyProgrammerValue, FALSE },
	{ INJECTION, NULL, FALSE },
	{ "MapCorrection", NULL, FALSE },
	{ "SpareCorrection", NULL, FALSE }
};

#define PROGRAMMER_COUNT (sizeof(programmers) / sizeof(Programmer))


Programmer* GetProgrammer(const char* tableName)
{
	int i;
	for (i = 0; i < PROGRAMMER_COUNT; ++i)
	{
		if (strcmp(programmers[i].tableName, tableName) == 0)
		{
			return &(programmers[i]);
		}
	}
	return NULL;
}


/*
** Interface
*/

Status GetTableProgrammable(const char* tableName, bool* programmable)
{
	Programmer* programmer = GetProgrammer(tableName);
	if (programmer == NULL)
	{
		return INVALID_PARAMETER;
	}
	*programmable = programmer->apply != NULL;
	return OK;
}


Status SetProgrammerActivated(const char* tableName, bool activated)
{
	Programmer* programmer = GetProgrammer(tableName);
	if ((programmer == NULL) || (programmer->apply == NULL))
	{
		return INVALID_PARAMETER;
	}
	programmer->activated = activated;
	return OK;
}


bool IsProgrammerActivated(const char* tableName)
{
	Programmer* programmer = GetProgrammer(tableName);
	if ((programmer == NULL) || (programmer->apply == NULL))
	{
		return FALSE;
	}
	return programmer->activated;
}

Status ApplyProgrammerValue(const char* tableName, int* column, int* row, float* value)
{
	Programmer* programmer = GetProgrammer(tableName);
	if ((programmer == NULL) || (programmer->apply == NULL))
	{
		return INVALID_PARAMETER;
	}
	return programmer->apply(column, row, value);
}
