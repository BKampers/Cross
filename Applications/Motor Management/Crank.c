/*
** Implementation of Crank
**
** Measures time period of the crank cogwheel signal and discovers the gap.
** Counts cogs for each revolution.
** Computes RPM.
**
** Author: Bart Kampers
*/

#include "Crank.h"

#include <stdlib.h>

#include "HardwareSettings.h"
#include "Engine.h"

#include "Pins.h"
#include "Timers.h"
#include "ExternalInterrupt.h"

#define MINUTES_PER_MILLI 60000.0f
#define SECONDS_PER_MINUTE 60.0f

/*
** Private
*/


char* INVALID_COG_NUMBER = "InvalidCogNumber";


CogCountCallback* cogCountCallbacks = NULL;

int phase = 0;

int cogCount = 0;
int cogTicks = 0;
int gapTicks = 0;

int previousCapture = 0;
int previousDelta = 0;

bool captured = FALSE;


void PhaseShift()
{
    if (IsMmOutputPinSet(TEMP_PHASE_PIN))
    {
        ResetMmOutputPins(TEMP_PHASE_PIN);
    }
    else
    {
        SetMmOutputPins(TEMP_PHASE_PIN);
    }
    phase = 1;
}


void PulseDetected(int capture)
{
    if (captured)
    {
        int delta = capture - previousCapture;
        if (delta < 0)
        {
            delta += 0x10000;
        }
        if (GetGapSize() > 0)
        {
			if (delta > 2 * previousDelta)
			{
				gapTicks = delta;
				cogCount = 1;
				if (phase == 1)
				{
					phase = 0;
				}
			}
			else
			{
				cogTicks = delta;
				if (cogCount > 0)
				{
					cogCount++;
					if (cogCount == 0)
					{
						cogCount = 1;
					}
				}
			}
        }
        else
        {
        	cogTicks = delta;
        	cogCount = (cogCount % GetCogTotal()) + 1;
        }
        if ((0 < cogCount) && (cogCount <= GetEffectiveCogCount()))
        {
            CogCountCallback callback = cogCountCallbacks[cogCount-1];
            if (callback != NULL)
            {
                callback(cogCount);
            }
        }
        captured = FALSE;
        previousDelta = delta;
    }
    previousCapture = capture;
    captured = TRUE;
}


void EngineStopped()
{
    captured = FALSE;
}


/*
** Interface
*/

void InitCrank()
{
    InitCrankCallbacks();
    InitExternalInterrupt(&PhaseShift);
    InitExternalPulseTimer(&PulseDetected, &EngineStopped);
}


void InitCrankCallbacks()
{
    int i;
    int count = GetEffectiveCogCount();
    if (cogCountCallbacks != NULL)
    {
        free(cogCountCallbacks);
    }
    cogCountCallbacks = malloc(sizeof(CogCountCallback) * count);
    for (i = 0; i < count; ++i)
    {
        cogCountCallbacks[i] = NULL;
    }
}


int GetPhase()
{
    return phase;
}


int GetCogCount()
{
    return cogCount;
}


int GetCogTicks()
{
    return cogTicks;
}


int GetGapTicks()
{
    return gapTicks;
}


float GetRpm()
{
    int ticks = cogTicks;
    if (captured && (ticks > 0))
    {
    	return 26901000.0f / ticks;
    }
    return 0.0f;
}


bool EngineIsRunning()
{
    return captured;
}


Status SetCogCountCallback(CogCountCallback callback, int cogNumber)
{
    if ((0 < cogNumber) && (cogNumber <= GetEffectiveCogCount()))
    {
        cogCountCallbacks[cogNumber - 1] = callback;
        return OK;
    }
    else
    {
        return INVALID_COG_NUMBER;
    }
}
