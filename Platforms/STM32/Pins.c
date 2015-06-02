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


void SetBits(uint16_t pins)
{
    GPIO_SetBits(GPIOB, pins);
}


void ResetBits(uint16_t pins)
{
    GPIO_ResetBits(GPIOB, pins);
}


void WritePin(uint16_t pin, uint8_t state)
{
    GPIO_WriteBit(GPIOB, pin, (state == 0) ? Bit_RESET : Bit_SET);
}


uint8_t ReadPin(uint16_t pin)
{
    return GPIO_ReadInputDataBit(GPIOB, pin);
}
