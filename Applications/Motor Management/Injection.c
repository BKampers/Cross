/*
** Implementation of Injection
** Copyright 2014, Bart Kampers
*/

#include "Injection.h"

#include <stdio.h>

#include "Controllers.h"

#include "HardwareSettings.h"
#include "Communication.h"
#include "Crank.h"
#include "AnalogInput.h"


/*
** Private
*/

#define INJECTION_CHANNEL 1
#define CHANNEL_BUFFER_SIZE 64


TableController* injectionController;
float injectionTime = 0.0f;


Status SendInjectionTime()
{
    /*char message[64];
    sprintf(message, "{ \"InjectionTime\" : %f }\r\n", injectionTime);
    WriteChannel(INJECTION_CHANNEL, message);*/
    return OK;
}


/*
** Interface
*/

Status InitInjection()
{
    SendInjectionTime();
    injectionController = FindTableController(INJECTION);
    return (injectionController != NULL) ? OK : "NoInjectionControllerFound";
}


float GetInjectionTime()
{
    return injectionTime;
}


Status UpdateInjection()
{
    Status status = GetActualTableControllerFieldValue(injectionController, &injectionTime);
    if (status == OK)
    {
        status = SendInjectionTime();
    }
    return status;
}
