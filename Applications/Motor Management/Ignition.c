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
#include "Measurements.h"
#include "Table.h"

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


Measurement* rpmMeasurement;
Measurement* loadMeasurement;
Table ignitionTable;
TypeId timerSettingsTypeId;

Status ignitionTimeStatus = UNINITIALIZED;

int ignitionAngle;

byte ignitionRowIndex = 0;
byte ignitionColumnIndex = 0;


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
    Status status = FindTable(IGNITION, &ignitionTable);
    if (status != OK)
    {
        status = CreateTable(IGNITION, COLUMN_COUNT, ROW_COUNT);
    }
    SetCogCountCallback(&StartIgnition, /*cog number*/20);
    SetCogCountCallback(&StartIgnition, /*cog number*/50);
    SetIgnitionAngle(0);
    InitPeriodTimer(&StopIgnition);
    if (status == OK)
    {
        status = FindMeasurement(RPM, &rpmMeasurement);
    }
    if (status == OK)
    {
        status = FindMeasurement("Load", &loadMeasurement);
    }
    return status;
}


int GetColumnIndex()
{
    return ignitionColumnIndex;
}


int GetRowIndex()
{
    return ignitionRowIndex;
}


int GetIgnitionAngle()
{
    return ignitionAngle;
}

//#include "Messaging.h"

Status UpdateIgnition()
{
    Status status;
    if ((rpmMeasurement != NULL) && (loadMeasurement != NULL))
    {
        float rpmValue;
        float loadValue;
        status = GetMeasurementValue(rpmMeasurement, &rpmValue);
        if (status == OK)
        {
            status = GetMeasurementValue(loadMeasurement, &loadValue);
        }
        if (status == OK)
        {
//            SendRealNotification("rpmValue", rpmValue);
//            SendRealNotification("loadValue", loadValue);
            TableField angle;
            float rpmRange = rpmMeasurement->maximum - rpmMeasurement->minimum;
            float loadRange = loadMeasurement->maximum - loadMeasurement->minimum;
//            SendRealNotification("rpmRange", rpmRange);
//            SendRealNotification("loadRange", loadRange);
//            ignitionRowIndex = (byte) ((rpmValue - rpmMeasurement->minimum) / (rpmRange / ignitionTable.rows));
//            ignitionColumnIndex = (byte) ((loadValue - loadMeasurement->minimum) / (loadRange / ignitionTable.columns));
            ignitionRowIndex = (byte) (rpmValue / (rpmRange / ignitionTable.rows));
            ignitionColumnIndex = (byte) (loadValue / (loadRange / ignitionTable.columns));
//            SendIntegerNotification("ignitionRowIndex", ignitionRowIndex);
//            SendIntegerNotification("ignitionColumnIndex", ignitionColumnIndex);
            status = GetTableField(IGNITION, ignitionColumnIndex, ignitionRowIndex, &angle);
            if (status == OK)
            {
                status = SetIgnitionAngle(angle);
            }
        }
    }
    else
    {
        status = "MeasurementNotFound";
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
