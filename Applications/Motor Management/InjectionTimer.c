/*
** Service for 4 injectors on PB8, PB9, PB10 and PB11
** to be started at cog number and stopped after provided injection time.
** Author: Bart Kampers
*/

#include "Injection.h"

#include <stdlib.h>

#include "misc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#include "Types.h"

#include "Timers.h"


#define INJECTOR_COUNT (sizeof(injectors) / sizeof(Injector))

#define TICKS_PER_MILLI (0xFFFF / 22.0f)

#define VALID_UINT16(number) ((0 <= number) && (number <= 0xFFFF))

typedef struct
{
    void (*SetTimerCompare) (TIM_TypeDef* TIMx, uint16_t compare);
    int cogNumber;
    uint16_t timerInterrupt;
    uint16_t pinId;
} Injector;


Injector injectors[] =
{
    { &TIM_SetCompare1, 0, TIMER_CHANNEL_1, GPIO_Pin_8  },
    { &TIM_SetCompare2, 0, TIMER_CHANNEL_2, GPIO_Pin_9  },
    { &TIM_SetCompare3, 0, TIMER_CHANNEL_3, GPIO_Pin_10 },
    { &TIM_SetCompare4, 0, TIMER_CHANNEL_4, GPIO_Pin_11 }
};


uint16_t injectionTicks = 0;


/*
** private
*/

Injector* FindInjector(int cogNumber)
{
    int i;
    for (i = 0; i < INJECTOR_COUNT; ++i)
    {
        Injector* injector = &(injectors[i]);
        if (injector->cogNumber == cogNumber)
        {
            return injector;
        }
    }
    return NULL;
}


void EndInjection(int events)
{
    Injector* injector = injectors;
    while (injector <= &(injectors[INJECTOR_COUNT - 1]))
    {
        if ((events & injector->timerInterrupt) != 0)
        {
            GPIO_ResetBits(GPIOB, injector->pinId);
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


Status SetInjectorCog(int injectorIndex, int cogNumber)
{
    if ((0 <= injectorIndex) && (injectorIndex < INJECTOR_COUNT))
    {
        injectors[injectorIndex].cogNumber = cogNumber;
        return OK;
    }
    else
    {
        return "InvalidInjectorId";
    }
}


Status StartInjection(int cogNumber)
{
    Injector* injector = FindInjector(cogNumber);
    if (injector != NULL)
    {
        GPIO_SetBits(GPIOB, injector->pinId);
        injector->SetTimerCompare(TIM4, TIM_GetCounter(TIM4) + injectionTicks);
        TIM_ClearITPendingBit(TIM4, injector->timerInterrupt);
        TIM_ITConfig(TIM4, injector->timerInterrupt, ENABLE);
        return OK;
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


Status SetInjectionTime(float time)
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
