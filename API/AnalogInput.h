#ifndef _ANALOGINPUT_H_
#define _ANALOGINPUT_H_

#include "ApiStatus.h"


#define ADC_MAX 0xFFF


extern const char* AnalogPorts[];


void InitAnalogInput();
Status GetAnalogValue(int index, int* value);


#endif // _ANALOGINPUT_H_
