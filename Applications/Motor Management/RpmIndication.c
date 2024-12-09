/*
** Indicate RPM level.
** Ports set high when RPM of engine is above a programmable value.
**
** Copyright 2024, Bart Kampers
*/

#include "RpmIndication.h"

#include <stdint.h>

#include "Pins.h"
#include "HardwareSettings.h"
#include "Table.h"
#include "Crank.h"

/* Private */

Table rpmIndicationTable;
Pin pins[] =
{
	{ OUTPUT_PORT_RPM, RPM_INDICATOR_1 },
	{ OUTPUT_PORT_RPM, RPM_INDICATOR_2 },
	{ OUTPUT_PORT_RPM, RPM_INDICATOR_3 },
	{ OUTPUT_PORT_RPM, RPM_INDICATOR_4 }
};

#define RPM_COLUMN 0
#define COLUMN_COUNT 1
#define ROW_COUNT (sizeof(pins) / sizeof(Pin))


/* Interface */

char RPM_INDICATION[] = "RpmIndication";

Status InitRpmIndication()
{
	if (FindTable(RPM_INDICATION, &rpmIndicationTable) != OK)
	{
		return CreateTable(RPM_INDICATION, COLUMN_COUNT, ROW_COUNT, &rpmIndicationTable);
	}
	return OK;
}


Status UpdateRpmIndication()
{
	byte row;
	for (row = 0; row < ROW_COUNT; ++row)
	{
		TableField value;
		Status status = GetTableField(RPM_INDICATION, RPM_COLUMN, row, &value);
		if (status != OK)
		{
			return status;
		}
		if (value < GetRpm())
		{
			SetOutputPin(&(pins[row]));
		}
		else
		{
			ResetOutputPin(&(pins[row]));
		}
	}
	return OK;
}
