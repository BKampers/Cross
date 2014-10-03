/*
** Implementation of Injection
** Copyright 2014, Bart Kampers
*/

#include "Injection.h"

#include <stdio.h>

#include "HardwareSettings.h"
#include "Communication.h"

#include "Crank.h"
#include "AnalogInput.h"
#include "Measurements.h"
#include "Table.h"


/*
** Private
*/

#define INJECTION_CHANNEL 1
#define CHANNEL_BUFFER_SIZE 64

#define COLUMN_COUNT 20
#define ROW_COUNT 20


Measurement* rpmMeasurement;
Measurement* loadMeasurement;
Table injectionTable;

float injectionTime;

byte injectionRowIndex = 0;
byte injectionColumnIndex = 0;


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

char INJECTION[] = "Injection";


void RegisterInjectionTypes()
{
}


Status InitInjection()
{
    Status status = FindTable(INJECTION, &injectionTable);
    if (status != OK)
    {
        status = CreateTable(INJECTION, COLUMN_COUNT, ROW_COUNT);
    }
    if (status == OK)
    {
        status = OpenCommunicationChannel(INJECTION_CHANNEL, CHANNEL_BUFFER_SIZE);
    }
    SetInjectionTime(0.0f);
    if (status == OK)
    {
        status = FindMeasurement(RPM, &rpmMeasurement);
    }
    if (status == OK)
    {
        status = FindMeasurement("Load", &loadMeasurement);
    }
    return status;
}


int GetInjectionColumnIndex()
{
    return injectionColumnIndex;
}


int GetInjectionRowIndex()
{
    return injectionRowIndex;
}


float GetInjectionTime()
{
    return injectionTime;
}


Status UpdateInjection()
{
    Status status;
    if ((rpmMeasurement != NULL) && (loadMeasurement != NULL))
    {
        float rpmValue;
        float loadValue;
        status = GetMeasurementValue(rpmMeasurement, &rpmValue);
        if (status == OK)
        {
            status = GetMeasurementValue(loadMeasurement, &loadValue);
        }
        if (status == OK)
        {
            TableField time;
            float rpmRange = rpmMeasurement->maximum - rpmMeasurement->minimum;
            float loadRange = loadMeasurement->maximum - loadMeasurement->minimum;
//            injectionRowIndex = (byte) ((rpmValue - rpmMeasurement->minimum) / (rpmRange / injectionTable.rows));
//            injectionColumnIndex = (byte) ((loadValue - loadMeasurement->minimum) / (loadRange / injectionTable.columns));
            injectionRowIndex = (byte) (rpmValue / (rpmRange / injectionTable.rows));
            injectionColumnIndex = (byte) (loadValue / (loadRange / injectionTable.columns));
            status = GetTableField(INJECTION, injectionColumnIndex, injectionRowIndex, &time);
            if (status == OK)
            {
                status = SetInjectionTime(time);
            }
        }
    }
    else
    {
        status = "MeasurementNotFound";
    }
    return status;
}
