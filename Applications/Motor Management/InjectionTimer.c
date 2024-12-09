/*
** Service for 4 injectors
** to be started at cog number and stopped after provided injection time.
** Author: Bart Kampers
*/

#include "Injection.h"

#include <stdlib.h>

#include "Types.h"

#include "Pins.h"
#include "Timers.h"

#include "HardwareSettings.h"
#include "Engine.h"


#define INJECTOR_COUNT (sizeof(injectors) / sizeof(Injector))

#define TICKS_PER_MILLI (0xFFFF / 22.0f)

#define VALID_UINT16(number) ((0 <= number) && (number <= 0xFFFF))


typedef struct
{
    uint16_t pinId;
    uint16_t channel;
} Injector;


Injector injectors[] =
{
    { INJECTION_1_PIN, TIMER_CHANNEL_1 },
    { INJECTION_2_PIN, TIMER_CHANNEL_2 },
    { INJECTION_3_PIN, TIMER_CHANNEL_3 },
    { INJECTION_4_PIN, TIMER_CHANNEL_4 }
};


uint16_t injectionTicks = 0;


/*
** private
*/

void EndInjection(int channel)
{
    Injector* injector = injectors;
    while (injector <= &(injectors[INJECTOR_COUNT - 1]))
    {
        if ((channel & injector->channel) != 0)
        {
            ResetMmOutputPins(injector->pinId);
        }
        injector++;
    }
}


/*
** Interface
*/

Status InitInjectionTimer()
{
    InitCompareTimer(&EndInjection);
    return OK;
}


Status StartInjection(int cogNumber)
{
    int index = GetDeadPointIndex(cogNumber);
    if ((0 <= index) && (index < INJECTOR_COUNT))
    {
        Injector* injector = &(injectors[index]);
        SetMmOutputPins(injector->pinId);
        return StartCompareTimer(injector->channel, injectionTicks);
    }
    else
    {
        return "NoInjectorForCog";
    }
}


Status SetInjectionTicks(int ticks)
{
    if ((0 <= ticks) && (ticks <= GetCompareTimerPeriod()))
    {
        injectionTicks = (uint16_t) ticks;
        return OK;
    }
    else
    {
        return "InvalidInjectionTicks";
    }
}


int GetInjectionTicks()
{
    return injectionTicks;
}


Status SetInjectionTimer(float time)
{
    float ticks = TICKS_PER_MILLI * time;
    if (VALID_UINT16(ticks))
    {
        return SetInjectionTicks((int) ticks);
    }
    else
    {
        return "InvalidInjectionTime";
    }
}
