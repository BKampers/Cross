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
    char message[64];
    sprintf(message, "{ \"InjectionTime\" : %f }\r\n", injectionTime);
    return WriteChannel(INJECTION_CHANNEL, message);
}


/*
** Interface
*/

Status InitInjection()
{
    Status status = OpenCommunicationChannel(INJECTION_CHANNEL, CHANNEL_BUFFER_SIZE);
    if (status == OK)
    {
        SendInjectionTime();
        injectionController = FindTableController(INJECTION);
        if (injectionController == NULL)
        {
            status = "NoInjectionControllerFound";
        }
    }
    return status;
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
