#include "Engine.h"

#include <stdint.h>

/*
** Private 
*/

uint8_t cogTotal = 60;
uint8_t gapCogs = 2;
uint8_t ignitionReferenceCog = 29;

uint8_t cylinderCount = 8;

/*
** Interface
*/

int GetCogTotal()
{
    return cogTotal;
}


int GetGapCogs()
{
    return gapCogs;
}


int GetEffectiveCogCount()
{
    return cogTotal - gapCogs;
}


int GetIgnitionReferenceCog()
{
    return ignitionReferenceCog;
}


int GetDeadPointCount()
{
    switch (cylinderCount)
    {
        case 8:
            return 4;
        case 6:
            return 3;
        default:
            return 2;
    }
}