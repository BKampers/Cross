#include "Pins.h"

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "HardwareSettings.h"


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
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(OUTPUT_REGISTER, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(OUTPUT_PORT, &GPIO_InitStructure);
}


void SetOutputPins(uint16_t pins)
{
    GPIO_SetBits(OUTPUT_PORT, pins);
}


void ResetOutputPins(uint16_t pins)
{
    GPIO_ResetBits(OUTPUT_PORT, pins);
}


bool IsOutputPinSet(uint16_t pin)
{
    return GPIO_ReadOutputDataBit(OUTPUT_PORT, pin);
}
