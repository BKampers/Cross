/*
** Platform dependent settings
** Author: Bart Kampers
*/

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"


/*
** Interface
*/

void InitOutputPins()
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    /* Initialize LED which connected to PB15 .. PB8; Enable the Clock*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    /* Configure the GPIO_LED pin */
    GPIO_InitStructure.GPIO_Pin =
      /* Ignition */  GPIO_Pin_12 | GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_5 | GPIO_Pin_4 | GPIO_Pin_3 | GPIO_Pin_2 | GPIO_Pin_1 | GPIO_Pin_0 |
      /* Injection */ GPIO_Pin_11 | GPIO_Pin_10 | GPIO_Pin_9 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}
