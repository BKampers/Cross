/*
** Indicate RPM level.
** Ports set high when RPM of engine is above a programmable value.
**
** Copyright 2025, Bart Kampers
*/

#include "RpmIndication.h"

#include <stdint.h>

#include "Pins.h"
#include "HardwareSettings.h"
#include "Table.h"
#include "Crank.h"

/* Private */

Table rpmIndicationTable;

uint16_t indicatorPins[] = { RPM_INDICATOR_1, RPM_INDICATOR_2, RPM_INDICATOR_3, RPM_INDICATOR_4 };

#define RPM_COLUMN 0
#define COLUMN_COUNT 1
#define ROW_COUNT (sizeof(indicatorPins) / sizeof(uint16_t))


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
	uint16_t pinsToSet = 0;
	uint16_t pinsToReset = 0;
	byte row;
	for (row = 0; row < ROW_COUNT; ++row)
	{
		TableField rpmTreshold;
		Status status = GetTableField(RPM_INDICATION, RPM_COLUMN, row, &rpmTreshold);
		if (status != OK)
		{
			return status;
		}
		if (rpmTreshold < GetRpm())
		{
			pinsToSet |= indicatorPins[row];
		}
		else
		{
			pinsToReset |= indicatorPins[row];
		}
	}
	SetRpmOutputPins(pinsToSet);
	ResetRpmOutputPins(pinsToReset);
	return OK;
}
