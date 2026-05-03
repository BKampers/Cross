#include "VoltageDetector.h"

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_exti.h"
#include "misc.h"


/*
** Private
*/

bool lowVoltageDetected = FALSE;


/*
** Interface
*/

void EnableLowVoltageDetection()
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    /* PVD to 2.9V */
    PWR_PVDLevelConfig(PWR_PVDLevel_2V9);
    PWR_PVDCmd(ENABLE);

    /* EXTI16 */
    EXTI_ClearITPendingBit(EXTI_Line16);
    EXTI_InitStructure.EXTI_Line = EXTI_Line16;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* NVIC */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


bool LowVoltageDetected()
{
	return lowVoltageDetected || (PWR_GetFlagStatus(PWR_FLAG_PVDO) != RESET);
}


/*
** Interrupt handling
*/

void PVD_IRQHandler(void)
{
	lowVoltageDetected = TRUE;
    if (EXTI_GetITStatus(EXTI_Line16) != RESET)
    {
        if (PWR_GetFlagStatus(PWR_FLAG_PVDO) == RESET)
        {
            GPIO_SetBits(GPIOC, GPIO_Pin_13);
        }
        else
        {
            GPIO_ResetBits(GPIOC, GPIO_Pin_13);
        }
        EXTI_ClearITPendingBit(EXTI_Line16);
    }
}
