#ifndef _PINS_H_
#define _PINS_H_

void InitOutputPins();

void SetBits(uint16_t pins);
void ResetBits(uint16_t pins);

uint8_t WritePin(uint16_t pin, uint8_t state);
uint8_t ReadPin(uint16_t pin);


#endif /* _PINS_H_ */
