#include "Pins.h"

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "HardwareSettings.h"


/*
** Private
*/

void InitOutputPort(GPIO_TypeDef* port, uint16_t pins)
{
    GPIO_InitTypeDef GPIO_InitStructure = { pins, GPIO_Speed_50MHz, GPIO_Mode_Out_PP };
    GPIO_Init(port, &GPIO_InitStructure);
}


/*
** Interface
*/

void InitOutputPins()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	InitOutputPort(OUTPUT_PORT_MM, GPIO_Pin_All);
	InitOutputPort(OUTPUT_PORT_RPM, RPM_INDICATOR_1 | RPM_INDICATOR_2 | RPM_INDICATOR_3 | RPM_INDICATOR_4);

//    GPIO_InitTypeDef GPIO_InitStructure;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//
//    RCC_APB2PeriphClockCmd(OUTPUT_REGISTER_MM, ENABLE);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
//    GPIO_Init(OUTPUT_PORT_MM, &GPIO_InitStructure);
//
//    RCC_APB2PeriphClockCmd(OUTPUT_REGISTER_RPM, ENABLE);
//    GPIO_InitStructure.GPIO_Pin = RPM_INDICATOR_1 | RPM_INDICATOR_2 | RPM_INDICATOR_3 | RPM_INDICATOR_4;
//    GPIO_Init(OUTPUT_PORT_RPM, &GPIO_InitStructure);
}


void SetMmOutputPins(uint16_t pins)
{
    GPIO_SetBits(OUTPUT_PORT_MM, pins);
}


void ResetMmOutputPins(uint16_t pins)
{
    GPIO_ResetBits(OUTPUT_PORT_MM, pins);
}


bool IsMmOutputPinSet(uint16_t pin)
{
    return GPIO_ReadOutputDataBit(OUTPUT_PORT_MM, pin);
}


void SetOutputPin(Pin* pin)
{
	GPIO_SetBits((GPIO_TypeDef*) pin->peripheral, pin->port);
}

void ResetOutputPin(Pin* pin)
{
	GPIO_ResetBits((GPIO_TypeDef*) pin->peripheral, pin->port);
}

bool IsPinSet(Pin* pin)
{
	return GPIO_ReadOutputDataBit((GPIO_TypeDef*) pin->peripheral, pin->port);
}
