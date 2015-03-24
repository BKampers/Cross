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

#include "Types.h"

#include "stm32f10x_gpio.h"
#include "HardwareSettings.h"
#include "Engine.h"
#include "Timers.h"
#include "ExternalInterrupt.h"


/*
** Private
*/


#define MICROS_PER_TICK 10.24f


char* INVALID_COG_NUMBER = "InvalidCogNumber";


CogCountCallback* cogCountCallbacks = NULL;

int cogCount = 0;
int cogTicks = 0;
int gapTicks = 0;

int previousCapture = 0;
int previousDelta = 0;
bool captured = FALSE;


void PhaseShift()
{
    uint8_t state = GPIO_ReadInputDataBit(GPIOB, TEMP_PHASE_PIN);
    GPIO_WriteBit(GPIOB, TEMP_PHASE_PIN, (state == Bit_RESET) ? Bit_SET : Bit_RESET);
}


void InitCrankCallbacks()
{
    int i;
    int count = GetEffectiveCogCount();
    cogCountCallbacks = malloc(sizeof(CogCountCallback) * count);
    for (i = 0; i < count; ++i)
    {
        cogCountCallbacks[i] = NULL;
    }
    InitExternalInterrupt(&PhaseShift);
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
    InitExternalPulseTimer(&PulseDetected);
}


bool SignalDetected()
{
    return captured;
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


void RemoveCogCountCallback(CogCountCallback callback)
{
    int count = GetEffectiveCogCount();
    int i;
    for (i = 0; i < count; ++i)
    {
        if (cogCountCallbacks[i] == callback)
        {
            cogCountCallbacks[i] = NULL;
        }
    }
}
