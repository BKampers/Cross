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
float injectionTime;


Status SetInjectionTime(float time)
{
    char message[64];
    injectionTime = time;
    sprintf(message, "{ \"InjectionTime\" : %f }\r\n", time);
    WriteChannel(INJECTION_CHANNEL, message);
    return OK;
}


/*
** Interface
*/

Status InitInjection()
{
    SetInjectionTime(0.0f);
    injectionController = FindTableController(INJECTION);
    return (injectionController != NULL) ? OK : "NoInjectionControllerFound";
}


float GetInjectionTime()
{
    return injectionTime;
}


Status UpdateInjection()
{
    TableField time;
    Status status = GetActualTableControllerValue(injectionController, &time);
    if (status == OK)
    {
        status = SetInjectionTime(time * 0.1f);
    }
    return status;
}
