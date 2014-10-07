/*
** Implementation of Ignition
** Copyright 2014, Bart Kampers
*/

#include "Ignition.h"

#include <stdlib.h>

#include "HardwareSettings.h"

#include "Engine.h"
#include "Crank.h"
#include "AnalogInput.h"

//#include "Measurements.h"
//#include "Table.h"
#include "Controllers.h"


#include "Timers.h"
#include "Leds.h"


#define IGNITION_ANGLE_BASE 60

#define DEGREES_PER_COG (360 / COG_TOTAL)


/*
** Crank timer and ignition timer run with different prescalers.
** Use PRESCALER_RATIO to convert between these two.
*/
#define PRESCALER_RATIO ((float) IGNITION_TIMER_PRESCALER / EXTERNAL_PULSE_TIMER_PRESCALER)


/*
** Private
*/

#define COLUMN_COUNT 20
#define ROW_COUNT 20


char INVALID_IGNITION_ANGLE[] = "Invalid ignition angle";


TableController ignitionController;

TypeId timerSettingsTypeId;

Status ignitionTimeStatus = UNINITIALIZED;

int ignitionAngle;


/*
** Ratio to compute ignitionTicks for the desired ignition angle.
**
** Computed by SetIgnitionAngle(int angle):
** angleTimeRatio = PRESCALER_RATIO * DEGREES_PER_COG / (IGNITION_ANGLE_BASE - angle);
*/
float angleTimeRatio;

/*
** Number of ticks for the ignition timer to count in order to accomplish the desired ignition angle.
** This value depends on the rotation speed, that is the number of ticks per cog.
**
** Computed by StartIgnition():
** ignitionTicks = GetCogTicks() / angleTimeRatio
*/
int ignitionTicks = 0;


void StartIgnition()
{
    LedsOn(IGNITION_LED);
	   ignitionTicks = (int) (GetCogTicks() / angleTimeRatio);
	   ignitionTimeStatus = StartPeriodTimer(ignitionTicks);
}


void StopIgnition()
{
    LedsOff(IGNITION_LED);
}


Status SetIgnitionAngle(int angle)
{
    if ((0 <= angle) && (angle < IGNITION_ANGLE_BASE))
    {
        ignitionAngle = angle;
        angleTimeRatio = PRESCALER_RATIO * DEGREES_PER_COG / (IGNITION_ANGLE_BASE - angle);
        return OK;
    }
    else
    {
        return INVALID_IGNITION_ANGLE;
    }
}


/*
** Interface
*/

char IGNITION[] = "Ignition";


void RegisterIgnitionTypes()
{
    RegisterType(&timerSettingsTypeId);
}


Status InitIgnition()
{
    TIMER_SETTINGS timerSettings;
    GetIgnitionTimerSettings(&timerSettings);
    ignitionTicks = timerSettings.counter;
    ignitionController.name = IGNITION;
    Status status = FindTable(IGNITION, &(ignitionController.table));
    if (status != OK)
    {
        status = CreateTable(IGNITION, COLUMN_COUNT, ROW_COUNT, &(ignitionController.table));
    }
    if (status == OK)
    {
        status = FindMeasurement(LOAD, &(ignitionController.columnMeasurement));
    }
    if (status == OK)
    {
        status = FindMeasurement(RPM, &(ignitionController.rowMeasurement));
    }
    ignitionController.columnIndex = 0;
    ignitionController.rowIndex = 0;
    SetCogCountCallback(&StartIgnition, /*cog number*/20);
    SetCogCountCallback(&StartIgnition, /*cog number*/50);
    SetIgnitionAngle(0);
    InitPeriodTimer(&StopIgnition);
    return status;
}


int GetIgnitionColumnIndex()
{
    return ignitionController.columnIndex;
}


int GetIgnitionRowIndex()
{
    return ignitionController.rowIndex;
}


int GetIgnitionAngle()
{
    return ignitionAngle;
}


Status UpdateIgnition()
{
    TableField angle;
    Status status = GetTableControllerValue(&ignitionController, &angle);
    if (status == OK)
    {
        status = SetIgnitionAngle(angle);
    }
    return status;
}


void GetIgnitionTimerSettings(TIMER_SETTINGS* timerSettings)
{
    Reference reference = NULL_REFERENCE;
    Status status = FindFirst(timerSettingsTypeId, &reference);
    if (status == OK)
    {
        status = GetElement(reference, sizeof(TIMER_SETTINGS), timerSettings);
    }
    if (status != OK)
    {
        timerSettings->prescaler = IGNITION_TIMER_PRESCALER;
        timerSettings->period = 0xFFFF;
        timerSettings->counter = 0xFFFF;
    }
}


Status SetIgnitionTimerSettings(TIMER_SETTINGS* timerSettings)
{
    SetIgnitionAngle(timerSettings->counter);
    Reference reference = NULL_REFERENCE;
    Status status = FindFirst(timerSettingsTypeId, &reference);
    if (status != OK)
    {
        status = AllocateElement(timerSettingsTypeId, sizeof(TIMER_SETTINGS), &reference);
    }
    if (status == OK)
    {
        status = StoreElement(timerSettings, reference, sizeof(TIMER_SETTINGS));
    }
    return status;
}


int GetIgnitionTicks()
{
    return (int) ignitionTicks;
}
