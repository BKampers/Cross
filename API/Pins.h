#ifndef _PINS_H_
#define _PINS_H_

#include "Types.h"

void InitOutputPins();

void SetOutputPins(uint16_t pins);
void ResetOutputPins(uint16_t pins);

bool IsOutputPinSet(uint16_t pin);


#endif /* _PINS_H_ */
