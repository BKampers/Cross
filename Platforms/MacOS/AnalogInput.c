#include "AnalogInput.h"

#include <stdint.h>

#define ADC1_DR_Address ((uint32_t) 0x4001244C)

#define NUMBER_OF_ADC_PORTS 5


char* INVALID_ADC_INDEX = "Invalid ADC index";

uint16_t ADCConvertedValues[NUMBER_OF_ADC_PORTS];


void InitAnalogInput()
{
}


Status GetAnalogValue(int index, int* value)
{
	if ((0 <= index) && (index < NUMBER_OF_ADC_PORTS))
	{
		*value = (int) ADCConvertedValues[index];
		return OK;
	}
	else
	{
		return INVALID_ADC_INDEX;
	}
}
