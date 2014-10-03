/*
** Implementation of Crank
**
** Measures time period of the crank cogwheel signal and discovers the gap.
** Counts cogs for each revolution.
** Computes RPM.
**
** Copyright 2014, Bart Kampers
*/

#include "Crank.h"

#include <stdlib.h>

#include "Types.h"

#include "Engine.h"
#include "Timers.h"


/*
** Private
*/


#define MICROS_PER_TICK 10.24f


char* INVALID_COG_NUMBER = "Invalid cog number";


void (*cogCountCallbacks[EFFECTIVE_COG_COUNT]) ();

int32_t rotationCount = -1;

int cogCount = 0;
int cogTicks = 0;
int gapTicks = 0;

int previousCapture = 0;
int previousDelta = 0;
bool captured = FALSE;


void InitCrankCallbacks()
{
    int i;
    for (i = 0; i < EFFECTIVE_COG_COUNT; ++i)
    {
        cogCountCallbacks[i] = NULL;
    }
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
            rotationCount++;
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
        if ((0 < cogCount) && (cogCount <= EFFECTIVE_COG_COUNT))
        {
            void (*callback) () = cogCountCallbacks[cogCount-1];
            if (callback != NULL)
            {
                callback();
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
    InitExternalPulseTimer(&PulseDetected);
}


bool SignalDetected()
{
    return captured;
}


int GetRotationCount()
{
    return rotationCount;
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
        float rps = 1.0f / spr;
        return rps * 60.0f;
    }
    else
    {
        return 0.0f;
    }
}


Status SetCogCountCallback(void (*callback) (), int cogNumber)
{
    if ((0 <= cogNumber) && (cogNumber < EFFECTIVE_COG_COUNT))
    {
        cogCountCallbacks[cogNumber] = callback;
        return OK;
    }
    else
    {
        return INVALID_COG_NUMBER;
    }
}
