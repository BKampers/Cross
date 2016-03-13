#include "AnalogInput.h"

#include <stdint.h>


#define NUMBER_OF_ADC_PORTS 8


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
