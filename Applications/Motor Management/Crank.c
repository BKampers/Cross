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


/*
** Private
*/


char* INVALID_COG_NUMBER = "InvalidCogNumber";


CogCountCallback* cogCountCallbacks = NULL;

int phase = -1;

int cogCount = 0;
int cogTicks = 0;
int gapTicks = 0;

int previousCapture = 0;
int previousDelta = 0;
bool captured = FALSE;


void PhaseShift()
{
    if (IsOutputPinSet(TEMP_PHASE_PIN))
    {
        ResetOutputPins(TEMP_PHASE_PIN);
    }
    else
    {
        SetOutputPins(TEMP_PHASE_PIN);
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
        if ((0 < cogCount) && (cogCount <= GetEffectiveCogCount()))
        {
            CogCountCallback callback = cogCountCallbacks[cogCount-1];
            if (callback != NULL)
            {
                callback(cogCount);
            }
        }
        previousDelta = delta;
    }
    previousCapture = capture;
    captured = TRUE;
}


/*
** Interface
*/

void InitCrank()
{
    InitCrankCallbacks();
    InitExternalInterrupt(&PhaseShift);
    InitExternalPulseTimer(&PulseDetected);
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
    if (ticks > 0)
    {
        float spr = ticks / 60000.0f;
        return 60.0f / spr;
    }
    else
    {
        return 0.0f;
    }
}


bool EngineIsRunning()
{
    return 50.0f < GetRpm();
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
