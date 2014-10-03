#include "Leds.h"

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"


void InitLeds(void)
{
	/* Initialize Leds mounted on STM32 board */
	GPIO_InitTypeDef  GPIO_InitStructure;
	/* Initialize LED which connected to PB15 .. PB8; Enable the Clock*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	/* Configure the GPIO_LED pin */
	GPIO_InitStructure.GPIO_Pin =
	  GPIO_Pin_15 |
	  GPIO_Pin_14 |
	  GPIO_Pin_13 |
	  GPIO_Pin_12 |
	  GPIO_Pin_11 |
	  GPIO_Pin_10 |
	  GPIO_Pin_9  |
	  GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}


void SetLeds(uint8_t leds)
{
	GPIOB->ODR = (GPIOB->ODR & 0xFFFF00FF) | ((uint32_t) leds) << 8;
}


void LedsOn(uint8_t mask)
{
	GPIOB->ODR |= ((uint32_t) mask) << 8;
}


void LedsOff(uint8_t mask)
{
	GPIOB->ODR &= (((uint32_t) (mask ^ 0xFF)) << 8) | 0xFFFF00FF;
}


void ToggleLeds(uint8_t mask)
{
	GPIOB->ODR ^= ((uint32_t) mask) << 8;
}
