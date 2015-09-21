#include "Pins.h"

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"



/*
** Private
*/


//uint16_t gpioPins(uint16_t pins)
//{
//    // Do not use pins PB4 and PB5
//    return ((pins & 0x3FF0) << 2) | (pins & 0x000F);
//}


/*
** Interface
*/

void InitOutputPins()
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}


void SetOutputPins(uint16_t pins)
{
    GPIO_SetBits(GPIOE, pins);
}


void ResetOutputPins(uint16_t pins)
{
    GPIO_ResetBits(GPIOE, pins);
}


bool IsOutputPinSet(uint16_t pin)
{
    return GPIO_ReadOutputDataBit(GPIOE, pin);
}
