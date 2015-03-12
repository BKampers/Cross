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

#include "HardwareSettings.h"
#include "Engine.h"


#define INJECTOR_COUNT (sizeof(injectors) / sizeof(Injector))

#define TICKS_PER_MILLI (0xFFFF / 22.0f)

#define VALID_UINT16(number) ((0 <= number) && (number <= 0xFFFF))


typedef struct
{
    void (*SetTimerCompare) (TIM_TypeDef* TIMx, uint16_t compare);
    uint16_t timerInterrupt;
    uint16_t pinId;
} Injector;


Injector injectors[] =
{
    { &TIM_SetCompare1, TIMER_CHANNEL_1, INJECTION_1_PIN },
    { &TIM_SetCompare2, TIMER_CHANNEL_2, INJECTION_2_PIN },
    { &TIM_SetCompare3, TIMER_CHANNEL_3, INJECTION_3_PIN },
    { &TIM_SetCompare4, TIMER_CHANNEL_4, INJECTION_4_PIN }
};


uint16_t injectionTicks = 0;


/*
** private
*/

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


Status StartInjection(int cogNumber)
{
    int index = GetDeadPointIndex(cogNumber);
    if ((0 <= index) && (index < INJECTOR_COUNT))
    {
        Injector* injector = &(injectors[index]);
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
