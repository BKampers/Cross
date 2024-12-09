#ifndef _PINS_H_
#define _PINS_H_

#include "Types.h"

typedef void* Peripheral;

typedef struct
{
	Peripheral peripheral;
	uint16_t port;
} Pin;

void InitOutputPins();

void SetMmOutputPins(uint16_t pins);
void ResetMmOutputPins(uint16_t pins);

bool IsMmOutputPinSet(uint16_t pin);

void SetOutputPin(Pin* pin);
void ResetOutputPin(Pin* pin);
bool IsPinSet(Pin* pin);



#endif /* _PINS_H_ */
