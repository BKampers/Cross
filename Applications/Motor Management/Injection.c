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

#include "Controllers.h"
#include "Measurements.h"
#include "Table.h"


/*
** Private
*/

#define INJECTION_CHANNEL 1
#define CHANNEL_BUFFER_SIZE 64

#define COLUMN_COUNT 20
#define ROW_COUNT 20


TableController injectionController;

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

char INJECTION[] = "Injection";


Status InitInjection()
{
    injectionController.name = INJECTION;
    Status status = FindTable(INJECTION, &(injectionController.table));
    if (status != OK)
    {
        status = CreateTable(INJECTION, COLUMN_COUNT, ROW_COUNT, &(injectionController.table));
    }
    if (status == OK)
    {
        status = FindMeasurement(LOAD, &(injectionController.columnMeasurement));
    }
    if (status == OK)
    {
        status = FindMeasurement(RPM, &(injectionController.rowMeasurement));
    }
    if (status == OK)
    {
        status = OpenCommunicationChannel(INJECTION_CHANNEL, CHANNEL_BUFFER_SIZE);
    }
    injectionController.columnIndex = 0;
    injectionController.rowIndex = 0;
    SetInjectionTime(0.0f);
    return status;
}


int GetInjectionColumnIndex()
{
    return injectionController.columnIndex;
}


int GetInjectionRowIndex()
{
    return injectionController.rowIndex;
}


float GetInjectionTime()
{
    return injectionTime;
}


Status UpdateInjection()
{
    TableField time;
    Status status = GetTableControllerValue(&injectionController, &time);
    if (status == OK)
    {
        status = SetInjectionTime(time * 0.1f);
    }
    return status;
}
