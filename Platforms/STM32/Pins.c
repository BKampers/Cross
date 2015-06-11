#include "Pins.h"

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"


void InitOutputPins()
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}


void SetOutputPins(uint16_t pins)
{
    GPIO_SetBits(GPIOB, pins);
}


void ResetOutputPins(uint16_t pins)
{
    GPIO_ResetBits(GPIOB, pins);
}


bool IsOutputPinSet(uint16_t pin)
{
    return GPIO_ReadOutputDataBit(GPIOB, pin);//(GPIOB->ODR & pin) != 0;
}
