/*
** STM32 implementation for ExternalInterrupt
** Author: Bart Kampers
*/

#include "ExternalInterrupt.h"

#include <stdlib.h>


/*
** Privvate
*/

InterruptService service = NULL;


/*
** Interface
*/

void InitExternalInterrupt(InterruptService interruptService)
{
    service = interruptService;
}
