/*
** Implementation of Ignition
** Copyright 2015, Bart Kampers
*/

#include "Ignition.h"

#include <stdlib.h>

#include "MeasurementTable.h"

#include "HardwareSettings.h"
#include "Engine.h"
#include "Crank.h"
#include "AnalogInput.h"

#include "Timers.h"
#include "Leds.h"



#define IGNITION_ANGLE_BASE 60

#define DEGREES_PER_COG (360.0f / GetCogTotal())


/*
** Crank timer and ignition timer run with different prescalers.
** Use PRESCALER_RATIO to convert between these two.
*/
#define PRESCALER_RATIO ((float) IGNITION_TIMER_PRESCALER / EXTERNAL_PULSE_TIMER_PRESCALER)


/*
** Private
*/

char INVALID_IGNITION_ANGLE[] = "InvalidIgnitionAngle";


TypeId timerSettingsTypeId;

Status ignitionTimeStatus = UNINITIALIZED;

MeasurementTable* ignitionTable;
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
    Status status;
    TIMER_SETTINGS timerSettings;
    
    GetIgnitionTimerSettings(&timerSettings);
    ignitionTicks = timerSettings.counter;
    
    status = InitIgnitionStartCogs();
    if (status == OK)
    {
        SetIgnitionAngle(0);
        InitPeriodTimer(&StopIgnition);
        status = CreateMeasurementTable(IGNITION, LOAD, RPM, 20, 20, &ignitionTable);
        if (status == OK)
        {
            ignitionTable->precision = 1.0f;
            ignitionTable->minimum = 0.0f;
            ignitionTable->maximum = 59.0f;
            ignitionTable->decimals = 0;
            status = SetMeasurementTableEnabled(IGNITION, TRUE);
        }
    }
    return status;
}


int GetIgnitionAngle()
{
    return ignitionAngle;
}


Status UpdateIgnition()
{
    float angle;
    Status status = GetActualMeasurementTableField(ignitionTable, &angle);
    if (status == OK)
    {
        status = SetIgnitionAngle((int) angle);
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


Status InitIgnitionStartCogs()
{
    Status status = OK;
    int count = GetDeadPointCount();
    int i;
    for (i = 0; (i < count) && (status == OK); ++i)
    {
        status = SetCogCountCallback(&StartIgnition, GetDeadPointCog(i));
    }
    return status;
}
