/*
** Implementation of Injection
** Copyright 2014, Bart Kampers
*/

#include "Injection.h"

#include <stdio.h>

#include "MeasurementTable.h"

#include "HardwareSettings.h"
#include "Communication.h"
#include "Crank.h"
#include "AnalogInput.h"


/*
** Private
*/

#define INJECTION_CHANNEL 1
#define CHANNEL_BUFFER_SIZE 64


MeasurementTable* injectionTable;

MeasurementTable* waterTemperatureTable;

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

char INJECTION[] = "Injection";
char WATER_TEMPERATURE_CORRECTION[] = "WaterTemperatureCorrection"; 


Status InitInjection()
{
    Status status = CreateTableController(INJECTION, LOAD, RPM, 20, 20, &injectionTable);
    if (status == OK)
    {
        injectionTable->precision = 0.1f;
        injectionTable->minimum = 0.0f;
        injectionTable->maximum = 22.0f;
        injectionTable->decimals = 1;
        status = CreateTableController(WATER_TEMPERATURE_CORRECTION, WATER_TEMPERATURE, NULL, 15, 1, &waterTemperatureTable);
        if (status == OK)
        {
            waterTemperatureTable->precision = 1.0f;
            waterTemperatureTable->minimum = -150.0f;
            waterTemperatureTable->maximum = 150.0f;
            waterTemperatureTable->decimals = 0;
        }
    }
    if (status == OK)
    {
        status = OpenCommunicationChannel(INJECTION_CHANNEL, CHANNEL_BUFFER_SIZE);
        if (status == OK)
        {
            status = SendInjectionTime();
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
    float time;
    Status status = GetActualTableControllerFieldValue(injectionTable, &time);
    if (status == OK)
    {
        float totalCorrectionPercentage = 0.0f;
        if (waterTemperatureTable != NULL)
        {
            float percentage;
            if (GetActualTableControllerFieldValue(waterTemperatureTable, &percentage) == OK)
            {
                totalCorrectionPercentage += percentage;
            }
        }
        time = time * (1.0f + totalCorrectionPercentage / 100.0f);
        if (time != injectionTime)
        {
            injectionTime = time;
            status = SendInjectionTime();
        }
    }
    return status;
}
