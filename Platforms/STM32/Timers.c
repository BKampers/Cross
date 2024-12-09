#include "Timers.h"

#include <stdlib.h>

#include "misc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"


#define PERIOD_TIMER TIM2
#define PERIOD_TIMER_PERIPHERAL RCC_APB1Periph_TIM2
#define PERIOD_TIMER_IRQ TIM2_IRQn

#define PULSE_TIMER TIM3
#define PULSE_TIMER_PERIPHERAL RCC_APB1Periph_TIM3
#define PULSE_TIMER_IRQ TIM3_IRQn

#define COMPARE_TIMER TIM4
#define COMPARE_TIMER_PERIPHERAL RCC_APB1Periph_TIM4
#define COMPARE_TIMER_IRQ TIM4_IRQn

/* Timer prescaler and period to provide cycles times of 0 .. 22 ms */
uint16_t compareTimerPrescaler = 24;
uint16_t compareTimerPeriod = 0xFFFF;


/*
** Private
*/

#define TIMER_PERIOD 0xFFFF

TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
uint16_t pwmDutyCycle = 0;

void (*PeriodExpiredService) () = NULL;
void (*CaptureService) (int capture) = NULL;
void (*CaptureOverflowService) () = NULL;
void (*TimerMatchService) (int event) = NULL;


/*
** Interface
*/

void InitPeriodTimer(void (*InterruptService) ())
{
    PeriodExpiredService = InterruptService;

    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(PERIOD_TIMER_PERIPHERAL, ENABLE);

    /* enable timer irq */
    NVIC_InitStructure.NVIC_IRQChannel = PERIOD_TIMER_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_TimeBaseStructure.TIM_Prescaler = PERIOD_TIMER_PRESCALER - 1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = TIMER_PERIOD;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(PERIOD_TIMER, &TIM_TimeBaseStructure);
}


void InitCompareTimer(void (*InterruptService) (int channel))
{
    TimerMatchService = InterruptService;

    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(COMPARE_TIMER_IRQ, ENABLE);

    /* Enable timer irq */
    NVIC_InitStructure.NVIC_IRQChannel = COMPARE_TIMER_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Configure timer */
    TIM_TimeBaseStructure.TIM_Prescaler = compareTimerPrescaler - 1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = compareTimerPeriod;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(COMPARE_TIMER, &TIM_TimeBaseStructure);

    /* Start timer */
    TIM_ClearITPendingBit(COMPARE_TIMER, TIM_IT_Update);
    TIM_ITConfig(COMPARE_TIMER, TIM_IT_Update, ENABLE);
    TIM_Cmd(COMPARE_TIMER, ENABLE);
}


int GetCompareTimerPeriod()
{
    return compareTimerPeriod;
}


void InitExternalPulseTimer(void (*InterruptService) (int capture), void (*OverflowService) ())
{
    CaptureService = InterruptService;
    CaptureOverflowService = OverflowService;

    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;

    /* PULSE_TIMER clock enable */
    RCC_APB1PeriphClockCmd(PULSE_TIMER_PERIPHERAL, ENABLE);
    /* GPIOA clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* Enable the PULSE_TIMER global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = PULSE_TIMER_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* PULSE_TIMER channel 2 pin (PA.07) configuration */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* PULSE_TIMER prescale */
    TIM_TimeBaseInitStructure.TIM_Prescaler = EXTERNAL_PULSE_TIMER_PRESCALER - 1; /* clock slow down */
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 0xFFFF;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(PULSE_TIMER, &TIM_TimeBaseInitStructure);

    /*
    ** PULSE_TIMER configuration: Input Capture mode
    ** The external signal is connected to PULSE_TIMER CH2 pin (PA.07)
    ** The Rising edge is used as active edge,
    ** The PULSE_TIMER CCR2 is used to compute the frequency value
    */
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x0;
    TIM_ICInit(PULSE_TIMER, &TIM_ICInitStructure);
    /* TIM enable counter */
    TIM_Cmd(PULSE_TIMER, ENABLE);
    /* Enable the CC2 Interrupt Request */
    TIM_ITConfig(PULSE_TIMER, TIM_IT_CC2, ENABLE);
    TIM_ITConfig(PULSE_TIMER, TIM_IT_CC3, ENABLE);
}


Status StartPeriodTimer(int ticks)
{
    if ((0 < ticks) && (ticks < TIMER_PERIOD))
    {
        TIM_SetCounter(PERIOD_TIMER, TIMER_PERIOD - ticks);
        TIM_ClearITPendingBit(PERIOD_TIMER, TIM_IT_Update); /* Clear interrupt to avoid immediate firing */
        TIM_ITConfig(PERIOD_TIMER, TIM_IT_Update, ENABLE); /* Enable interrupt */
        TIM_Cmd(PERIOD_TIMER, ENABLE); /* Start timer */
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
            TIM_SetCompare1(COMPARE_TIMER, (uint16_t) (TIM_GetCounter(COMPARE_TIMER) + ticks));
            break;
        case TIMER_CHANNEL_2:
            TIM_SetCompare2(COMPARE_TIMER, (uint16_t) (TIM_GetCounter(COMPARE_TIMER) + ticks));
            break;
        case TIMER_CHANNEL_3:
            TIM_SetCompare3(COMPARE_TIMER, (uint16_t) (TIM_GetCounter(COMPARE_TIMER) + ticks));
            break;
        case TIMER_CHANNEL_4:
            TIM_SetCompare4(COMPARE_TIMER, (uint16_t) (TIM_GetCounter(COMPARE_TIMER) + ticks));
            break;
        default:
            status = "InvalidComparatorId";
            break;
    }
    if (status == OK)
    {
        TIM_ClearITPendingBit(COMPARE_TIMER, channel);
        TIM_ITConfig(COMPARE_TIMER, channel, ENABLE);
    }
    return status;
}


Status StartPwmTimer(int period)
{
	if ((period < 0) || (0xFFFF < period))
	{
		return INVALID_PARAMETER;
	}
	uint16_t PrescalerValue = (uint16_t) (SystemCoreClock / 24000000) - 1;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	/* GPIOA and GPIOB clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6/* | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9*/;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    TIM_TimeBaseStructure.TIM_Period = (uint16_t) period;
    TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    /* PWM1 Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = period;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    TIM_OC1Init(TIM4, &TIM_OCInitStructure);

    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

    /* TIM4 enable counter */
    TIM_Cmd(TIM4, ENABLE);
    return OK;
}


Status SetPwmDutyCycle(int dutyCycle)
{
	if (TIM_OCInitStructure.TIM_Pulse == dutyCycle)
	{
		return OK;
	}
	if ((dutyCycle < 0 ) || (TIM_TimeBaseStructure.TIM_Period < dutyCycle))
	{
		return INVALID_PARAMETER;
	}
	pwmDutyCycle = (uint16_t) dutyCycle;
	TIM_OCInitStructure.TIM_Pulse = pwmDutyCycle;
    TIM_OC1Init(TIM4, &TIM_OCInitStructure);
    return OK;
}

int GetPwmDutyCycle()
{
	return pwmDutyCycle;
}


/*
** Interrupt handling
*/

void TIM2_IRQHandler(void)
{
    TIM_ITConfig(PERIOD_TIMER, TIM_IT_Update, DISABLE);
    TIM_Cmd(PERIOD_TIMER, DISABLE);
    if (PeriodExpiredService != NULL)
    {
        PeriodExpiredService();
    }
}


void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(PULSE_TIMER, TIM_IT_CC2) != RESET)
    {
        int capture = TIM_GetCapture2(PULSE_TIMER);
        TIM_SetCompare3(PULSE_TIMER, (uint16_t) (capture - 1));
        TIM_ClearITPendingBit(PULSE_TIMER, TIM_IT_CC2);
        if (CaptureService != NULL)
        {
            CaptureService(capture);
        }
    }
    if (TIM_GetITStatus(PULSE_TIMER, TIM_IT_CC3) != RESET)
    {
        TIM_ClearITPendingBit(PULSE_TIMER, TIM_IT_CC3);
        if (CaptureOverflowService != NULL)
        {
            CaptureOverflowService();
        }
    }
}


void TIM4_IRQHandler(void)
{
    int event = 0;
    uint16_t timerInterrupt = TIM_IT_Update;
    while (timerInterrupt <= TIM_IT_CC4)
    {
        if (TIM_GetITStatus(COMPARE_TIMER, timerInterrupt) != RESET)
        {
            event |= timerInterrupt;
            TIM_ITConfig(COMPARE_TIMER, timerInterrupt, DISABLE);
            TIM_ClearITPendingBit(COMPARE_TIMER, timerInterrupt);
        }
        timerInterrupt <<= 1;
    }
    if (TimerMatchService != NULL)
    {
        TimerMatchService(event);
    }
}
