#include "Pins.h"

#include <stdio.h>

uint16_t ODR = 0;


void showLeds()
{
    /*
    printf("LEDS: ");
    uint32_t mask = 0x00000100;
    int i;
    for (i = 0; i < 8; ++i)
    {
        printf(((ODR & mask) == 0) ? "." : "!");
        mask = mask << 1;
    }
    printf("\r\n");
    */
}


void InitOutputPins(void)
{
    showLeds();
}


void SetOutputPins(uint16_t pins)
{
    ODR |= pins;
    showLeds();
}


void ResetOutputPins(uint16_t pins)
{
    ODR &= pins ^ 0xFF;
    showLeds();
}


bool IsOutputPinSet(uint16_t pin)
{
    return (ODR & pin) != 0;
}