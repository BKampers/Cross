#include "Timers.h"

#include <stdlib.h>

#include "misc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"

/* Timer prescaler and period to provide cycles times of 0 .. 22 ms */
uint16_t prescaler = 24;
uint16_t period = 0xFFFF;


/*
** Private
*/

#define TIMER_PERIOD 0xFFFF

void (*HandleIrq2) () = NULL;
void (*HandleIrq3) (int capture) = NULL;
void (*HandleIrq4) (int event) = NULL;


/*
** Interface
*/

void InitPeriodTimer(void (*InterruptService) ())
{
    HandleIrq2 = InterruptService;

    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* enable timer irq */
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_TimeBaseStructure.TIM_Prescaler = IGNITION_TIMER_PRESCALER - 1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = TIMER_PERIOD;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
}


void InitCompareTimer(void (*InterruptService) (int channel))
{
    HandleIrq4 = InterruptService;

    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    /* Enable timer irq */
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Configure timer */
    TIM_TimeBaseStructure.TIM_Prescaler = prescaler - 1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = period;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    /* Start timer */
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM4, ENABLE);
}



int GetCompareTimerPeriod()
{
    return period;
}


void InitExternalPulseTimer(void (*InterruptService) (int capture))
{
    HandleIrq3 = InterruptService;

    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;

    /* TIM3 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    /* GPIOA clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* Enable the TIM3 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* TIM3 channel 2 pin (PA.07) configuration */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* TIM3 prescale */
    TIM_TimeBaseInitStructure.TIM_Prescaler = EXTERNAL_PULSE_TIMER_PRESCALER - 1; /* clock slow down */
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 0xFFFF;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);

    /*
    ** TIM3 configuration: Input Capture mode
    ** The external signal is connected to TIM3 CH2 pin (PA.07)
    ** The Rising edge is used as active edge,
    ** The TIM3 CCR2 is used to compute the frequency value
    */
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x0;
    TIM_ICInit(TIM3, &TIM_ICInitStructure);
    /* TIM enable counter */
    TIM_Cmd(TIM3, ENABLE);
    /* Enable the CC2 Interrupt Request */
    TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);
}


Status StartPeriodTimer(int ticks)
{
    if ((0 < ticks) && (ticks < TIMER_PERIOD))
    {
        TIM_SetCounter(TIM2, TIMER_PERIOD - ticks);
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update); /* Clear interrupt to avoid immediate firing */
        TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); /* Enable interrupt */
        TIM_Cmd(TIM2, ENABLE); /* Start timer */
        return OK;
    }
    else
    {
        return "InvalidTimerPeriod";
    }
}


Status StartCompareTimer(int channel, int ticks)
{
    Status status = OK;
    switch (channel)
    {
        case TIMER_CHANNEL_1:
            TIM_SetCompare1(TIM4, TIM_GetCounter(TIM4) + ticks);
            break;
        case TIMER_CHANNEL_2:
            TIM_SetCompare2(TIM4, TIM_GetCounter(TIM4) + ticks);
            break;
        case TIMER_CHANNEL_3:
            TIM_SetCompare3(TIM4, TIM_GetCounter(TIM4) + ticks);
            break;
        case TIMER_CHANNEL_4:
            TIM_SetCompare4(TIM4, TIM_GetCounter(TIM4) + ticks);
            break;
        default:
            status = "InvalidComparatorId";
            break;
    }
    if (status == OK)
    {
        TIM_ClearITPendingBit(TIM4, channel);
        TIM_ITConfig(TIM4, channel, ENABLE);
    }
    return status;
}


/*
** Interrupt handling
*/

void TIM2_IRQHandler(void)
{
    TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
    TIM_Cmd(TIM2, DISABLE);
    if (HandleIrq2 != NULL)
    {
        HandleIrq2();
    }
}


void TIM3_IRQHandler(void)
{
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
    if (HandleIrq3 != NULL)
    {
        HandleIrq3(TIM_GetCapture2(TIM3));
    }
}


void TIM4_IRQHandler(void)
{
    int event = 0;
    uint16_t timerInterrupt = TIM_IT_Update;
    while (timerInterrupt <= TIM_IT_CC4)
    {
        if (TIM_GetITStatus(TIM4, timerInterrupt) != RESET)
        {
            event |= timerInterrupt;
            TIM_ITConfig(TIM4, timerInterrupt, DISABLE);
            TIM_ClearITPendingBit(TIM4, timerInterrupt);
        }
        timerInterrupt <<= 1;
    }
    if (HandleIrq4 != NULL)
    {
        HandleIrq4(event);
    }
}
