#include "Leds.h"

#include <stdio.h>

uint32_t ODR = 0;


void showLeds()
{
    printf("LEDS: ");
    uint32_t mask = 0x00000100;
    int i;
    for (i = 0; i < 8; ++i)
    {
        printf(((ODR & mask) == 0) ? "." : "!");
        mask = mask << 1;
    }
    printf("\r\n");
}


void InitLeds(void)
{
    showLeds();
}


void SetLeds(uint8_t leds)
{
    ODR = (ODR & 0xFFFF00FF) | ((uint32_t) leds) << 8;
    showLeds();
}


void LedsOn(uint8_t mask)
{
    ODR |= ((uint32_t) mask) << 8;
    showLeds();
}


void LedsOff(uint8_t mask)
{
    ODR &= (((uint32_t) (mask ^ 0xFF)) << 8) | 0xFFFF00FF;
    showLeds();
}


void ToggleLeds(uint8_t mask)
{
    ODR ^= ((uint32_t) mask) << 8;
    showLeds();
}
