/*
** Implementation of Ignition
** Author: Bart Kampers
*/

#include "Ignition.h"

#include <math.h>
#include <stdlib.h>

#include "Types.h"
#include "Pins.h"
#include "Timers.h"

#include "Configuration.h"
#include "MeasurementTable.h"
#include "HardwareSettings.h"
#include "Engine.h"
#include "Crank.h"
#include "AnalogInput.h"

#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#define IGNITION_ANGLE_BASE 60
#define IGNITION_PWM_PERIOD 500
#define IGNITION_PWM_RESOLUTION 0.5f

#define DEGREES_PER_COG (360.0f / GetCogTotal())


/*
** Crank timer and ignition timer run with different prescalers.
** Use PRESCALER_RATIO to convert between these two.
*/
#define PRESCALER_RATIO ((float) PERIOD_TIMER_PRESCALER / EXTERNAL_PULSE_TIMER_PRESCALER)


/*
** Private
*/

#define ALL_IGNITION_PINS (GLOBAL_IGNITION_PIN | IGNITION_1_PIN | IGNITION_2_PIN | IGNITION_3_PIN | IGNITION_4_PIN | IGNITION_5_PIN | IGNITION_6_PIN | IGNITION_7_PIN | IGNITION_8_PIN)

char INVALID_IGNITION_ANGLE[] = "InvalidIgnitionAngle";

CorrectionConfiguration ignitionCorrections[] =
{
    { MAP_SENSOR, NULL },
    { SPARE, NULL }
};
#define CORRECTION_COUNT (sizeof(ignitionCorrections) / sizeof(CorrectionConfiguration))



uint16_t ignitionPins[DEAD_POINT_MAX][PHASE_MAX] =
{
    { IGNITION_1_PIN, IGNITION_2_PIN },
    { IGNITION_3_PIN, IGNITION_4_PIN },
    { IGNITION_5_PIN, IGNITION_6_PIN },
    { IGNITION_7_PIN, IGNITION_8_PIN }
};


TypeId timerSettingsTypeId;

Status ignitionTimeStatus = UNINITIALIZED;

MeasurementTable* ignitionTable;
float ignitionDutyCycle;
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


void StopIgnition()
{
    ResetOutputPins(ALL_IGNITION_PINS);
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
    int i;
    
    if (PwmIgnitionEnabled())
    {
    	status = StartPwmTimer(IGNITION_PWM_PERIOD);
    	if (status != OK)
    	{
    		return status;
    	}
    }

    if (TimerIgnitionEnabled())
    {
		GetIgnitionTimerSettings(&timerSettings);
		ignitionTicks = timerSettings.counter;
	    InitPeriodTimer(&StopIgnition);
    }
    
    SetIgnitionAngle(0);

    status = CreateMeasurementTable(IGNITION, LOAD, RPM, 20, 20, &ignitionTable);
    if (status == OK)
    {
        ignitionTable->precision = 0.5f;
        ignitionTable->minimum = 0.0f;
        ignitionTable->maximum = 50.0f;
        ignitionTable->decimals = 1;
        status = SetMeasurementTableEnabled(IGNITION, TRUE);
        for (i = 0; (i < CORRECTION_COUNT) && (status == OK); ++i)
        {
            status = CreateCorrectionTable(ignitionCorrections[i].measurementName, &(ignitionCorrections[i].table));
        }
    }
    return status;
}


float GetIgnitionDutyCycle()
{
	return ignitionDutyCycle;
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
        timerSettings->prescaler = PERIOD_TIMER_PRESCALER;
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


int GetIgnitionAngle()
{
    return ignitionAngle;
}


int GetIgnitionTicks()
{
    return (int) ignitionTicks;
}


Status UpdateIgnition()
{
    float angle;
    int i;
    Status pwmStatus = OK;
    Status timerStatus = OK;
    Status status = GetActualMeasurementTableField(ignitionTable, &angle);
    for (i = 0; (i < CORRECTION_COUNT) && (status == OK); ++i)
    {
        MeasurementTable* table = ignitionCorrections[i].table;
        if (table != NULL)
        {
            bool enabled;
            status = GetMeasurementTableEnabled(table->name, &enabled);
            if ((status == OK) && enabled)
            {
                float correction;
                status = GetActualMeasurementTableField(table, &correction);
                if (status == OK)
                {
                    angle += correction;
                }
            }
        }
    }
    if (status != OK)
    {
    	return status;
    }
    if (PwmIgnitionEnabled())
    {
    	ignitionDutyCycle = angle;
    	pwmStatus = SetPwmDutyCycle(max(0, min(IGNITION_PWM_PERIOD, roundf(angle / 50 * IGNITION_PWM_PERIOD))));
    }
    if (TimerIgnitionEnabled())
    {
        timerStatus = SetIgnitionAngle((int) angle);
    }
    return (timerStatus != OK) ? timerStatus : pwmStatus;
}


void StartIgnition(int cogNumber)
{
	if (!TimerIgnitionEnabled())
	{
		return;
	}
	int phase = GetPhase();
	if ((0 <= phase) && (phase < PHASE_MAX))
	{
		int deadPointIndex = GetDeadPointIndex(cogNumber);
		if ((0 <= deadPointIndex) && (deadPointIndex < DEAD_POINT_MAX))
		{
			uint16_t cylinderPin = ignitionPins[deadPointIndex][phase];
			SetOutputPins(GLOBAL_IGNITION_PIN | cylinderPin);
		}
		ignitionTicks = (int) (GetCogTicks() / angleTimeRatio);
		ignitionTimeStatus = StartPeriodTimer(ignitionTicks);
	}
}
