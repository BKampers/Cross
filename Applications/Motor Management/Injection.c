/*
** Implementation of Injection
** Author: Bart Kampers
*/

#include "Injection.h"

#include <stdlib.h>
#include <string.h>

#include "MeasurementTable.h"

#include "HardwareSettings.h"
#include "Communication.h"
#include "JsonWriter.h"
#include "Crank.h"
#include "InjectionTimer.h"
#include "AnalogInput.h"


/*
** Private
*/

#define INJECTION_CHANNEL 1
#define CHANNEL_BUFFER_SIZE 64


#define UART_INJECTION  0x01
#define TIMER_INJECTION 0x02


typedef struct
{
    const char* measurementName;
    MeasurementTable* table;
} CorrectionConfiguration;


CorrectionConfiguration corrections[] =
{
    { WATER_TEMPERATURE, NULL },
    { AIR_TEMPERATURE, NULL },
    { BATTERY_VOLTAGE, NULL},
    { MAP_SENSOR , NULL }
};

#define CORRECTION_COUNT (sizeof(corrections) / sizeof(CorrectionConfiguration))


char CORRECTION_POSTFIX[] = "Correction";

MeasurementTable* injectionTable;

float injectionTime = 0.0f;

byte injectionMode = TIMER_INJECTION;


Status UpdateInjectionTimeUart()
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonRootStart(INJECTION_CHANNEL))
    VALIDATE(WriteJsonRealMember(INJECTION_CHANNEL, "InjectionTime", injectionTime))
    VALIDATE(WriteJsonObjectEnd(INJECTION_CHANNEL))
    return FinishTransmission(INJECTION_CHANNEL);
}


Status UpdateInjectionTime()
{
    Status uartStatus = OK;
    Status timerStatus = OK;
    if ((injectionMode & UART_INJECTION) != 0)
    {
        uartStatus = UpdateInjectionTimeUart();
    }
    if ((injectionMode & TIMER_INJECTION) != 0)
    {
        timerStatus = SetInjectionTimer(injectionTime);
    }
    return (uartStatus != OK) ? uartStatus : timerStatus;
}


Status CreateCorrectionTable(const char* measurementName, MeasurementTable** correctionTable)
{
    size_t nameLength = strlen(measurementName) + strlen(CORRECTION_POSTFIX);
    char* tableName = malloc(nameLength + 1);
    strcpy(tableName, measurementName);
    strcat(tableName, CORRECTION_POSTFIX);
    Status status = CreateMeasurementTable(tableName, measurementName, NULL, 10, 1, correctionTable);
    if (status == OK)
    {
        (*correctionTable)->precision = 1.0f;
        (*correctionTable)->minimum = -150.0f;
        (*correctionTable)->maximum = 150.0f;
        (*correctionTable)->decimals = 0;
    }
    return status;
}


/*
** Interface
*/

char INJECTION[] = "Injection";


Status InitInjection()
{
    Status status = CreateMeasurementTable(INJECTION, LOAD, RPM, 20, 20, &injectionTable);
    if (status == OK)
    {
        status = SetMeasurementTableEnabled(INJECTION, TRUE);
        if (status == OK)
        {
            int i;
            injectionTable->precision = 0.1f;
            injectionTable->minimum = 0.0f;
            injectionTable->maximum = 22.0f;
            injectionTable->decimals = 1;
            for (i = 0; (i < CORRECTION_COUNT) && (status == OK); ++i)
            {
                status = CreateCorrectionTable(corrections[i].measurementName, &(corrections[i].table));
            }
        }
    }
    if (status == OK)
    {
        Status uartStatus = OK;
        Status timerStatus = OK;
        if ((injectionMode & UART_INJECTION) != 0)
        {
            uartStatus = OpenCommunicationChannel(INJECTION_CHANNEL, CHANNEL_BUFFER_SIZE);
            if (uartStatus == OK)
            {
                uartStatus = UpdateInjectionTimeUart();
            }
        }
        if ((injectionMode & TIMER_INJECTION) != 0)
        {
            timerStatus = InitInjectionTimer();
            if (timerStatus == OK)
            {
                timerStatus = SetInjectionTimer(injectionTime);
            }
        }
        status = (uartStatus != OK) ? uartStatus : timerStatus;
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
    Status status = GetActualMeasurementTableField(injectionTable, &time);
    if (status == OK)
    {
        int i;
        float totalCorrectionPercentage = 0.0f;
        for (i = 0; (i < CORRECTION_COUNT) && (status == OK); ++i)
        {
            MeasurementTable* table = corrections[i].table;
            if (table != NULL)
            {
                bool enabled;
                status = GetMeasurementTableEnabled(table->name, &enabled);
                if ((status == OK) && enabled)
                {
                    float percentage;
                    status = GetActualMeasurementTableField(table, &percentage);
                    if (status == OK)
                    {
                        totalCorrectionPercentage += percentage;
                    }
                }
            }
        }
        time = time * (1.0f + totalCorrectionPercentage / 100.0f);
        if (time != injectionTime)
        {
            injectionTime = time;
            status = UpdateInjectionTime();
        }
    }
    return status;
}
