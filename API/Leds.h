#ifndef _LEDS_H_
#define _LEDS_H_

#include <stdint.h>

void InitLeds(void);

void SetLeds(uint8_t leds);

void LedsOn(uint8_t mask);
void LedsOff(uint8_t mask);
void ToggleLeds(uint8_t mask);

#endif // _LEDS_H_
