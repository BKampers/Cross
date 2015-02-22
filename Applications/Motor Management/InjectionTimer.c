#include "Injection.h"

#include "misc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#include "Types.h"

#include "Timers.h"

#include "Messaging.h"


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
    { &TIM_SetCompare1, TIMER_CHANNEL_1, GPIO_Pin_8  },
    { &TIM_SetCompare2, TIMER_CHANNEL_2, GPIO_Pin_9 },
    { &TIM_SetCompare3, TIMER_CHANNEL_3, GPIO_Pin_10 },
    { &TIM_SetCompare4, TIMER_CHANNEL_4, GPIO_Pin_11 }
};


//float injectionTime = 0.0f;
uint16_t injectionTicks = 0;


/*
** private
*/


//void InitOutputPins()
//{
//    GPIO_InitTypeDef  GPIO_InitStructure;
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_11 | GPIO_Pin_10 | GPIO_Pin_9 | GPIO_Pin_8;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);
//}


void EndInjection(int events)
{
    Injector* injector = injectors;
    if ((events & TIMER_UPDATE) != 0)
    {
        BitAction pin = (BitAction) GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_8);
        GPIO_WriteBit(GPIOB, GPIO_Pin_8, ! pin);
    }
    while (injector <= &(injectors[INJECTOR_COUNT - 1]))
    {
        if ((events & injector->timerInterrupt) != 0)
        {
            /* End injection */
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
//    InitOutputPins();
    InitCompareTimer(&EndInjection);
    return OK;
}


Status StartInjection(int injectorId)
{
    if ((0 <= injectorId) && (injectorId < INJECTOR_COUNT))
    {
        Injector* injector = &(injectors[injectorId]);
        GPIO_SetBits(GPIOB, injector->pinId);
        injector->SetTimerCompare(TIM4, TIM_GetCounter(TIM4) + injectionTicks);
        TIM_ClearITPendingBit(TIM4, injector->timerInterrupt);
        TIM_ITConfig(TIM4, injector->timerInterrupt, ENABLE);
        return OK;
    }
    else
    {
        return "InvalidInjectorId";
    }
}


Status SetInjectionTicks(int ticks)
{
    if ((0 <= ticks) && (ticks <= GetCompareTimerPeriod()))
    {
        injectionTicks = (uint16_t) ticks;
//        injectionTime = ticks / TICKS_PER_MILLI;
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
