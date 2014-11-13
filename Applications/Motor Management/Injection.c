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

TableController* waterTemperatureCorrectonController;

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
    injectionController = FindTableController(INJECTION);
    if (injectionController != NULL)
    {
        waterTemperatureCorrectonController = FindTableController(WATER_TEMPERATURE_CORRECTION);
        Status status = OpenCommunicationChannel(INJECTION_CHANNEL, CHANNEL_BUFFER_SIZE);
        if (status == OK)
        {
            status = SendInjectionTime();
        }
        return status;
    }
    else
    {
        return "NoInjectionControllerFound";
    }
}


float GetInjectionTime()
{
    return injectionTime;
}


Status UpdateInjection()
{
    float time;
    Status status = GetActualTableControllerFieldValue(injectionController, &time);
    if (status == OK)
    {
        float totalCorrectionPercentage = 0.0f;
        if (waterTemperatureCorrectonController != NULL)
        {
            float percentage;
            status = GetActualTableControllerFieldValue(waterTemperatureCorrectonController, &percentage);
            if (status == OK)
            {
                totalCorrectionPercentage += percentage;
            }
        }
        injectionTime = time * (1.0f + totalCorrectionPercentage / 100.0f);
        status = SendInjectionTime();
    }
    return status;
}
