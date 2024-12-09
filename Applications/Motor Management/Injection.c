/*
** Implementation of Injection
** Author: Bart Kampers
*/

#include "Injection.h"

#include <stdlib.h>
#include <string.h>

#include "Configuration.h"
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

#define MIN_INJECTION_TIME 0.0f
#define MAX_INJECTION_TIME 22.0f


CorrectionConfiguration injectionCorrections[] =
{
};

#define CORRECTION_COUNT (sizeof(injectionCorrections) / sizeof(CorrectionConfiguration))


MeasurementTable* injectionTable = NULL;

float injectionTime = 0.0f;


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
    if ((GetInjectionMode() & UART_INJECTION) != 0)
    {
        uartStatus = UpdateInjectionTimeUart();
    }
    if ((GetInjectionMode() & TIMER_INJECTION) != 0)
    {
        timerStatus = SetInjectionTimer(injectionTime);
    }
    return (uartStatus != OK) ? uartStatus : timerStatus;
}


/*
** Interface
*/

char INJECTION[] = "Injection";


Status InitInjection()
{
	if (GetInjectionMode() == INJECTION_DISABLED)
	{
		return DISABLED;
	}
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
                status = CreateCorrectionTable(injectionCorrections[i].measurementName, &(injectionCorrections[i].table));
            }
        }
    }
    if (status == OK)
    {
        Status channelStatus = OK;
        Status timerStatus = OK;
        if ((GetInjectionMode() & UART_INJECTION) != 0)
        {
            channelStatus = OpenCommunicationChannel(INJECTION_CHANNEL, CHANNEL_BUFFER_SIZE);
            if (channelStatus == OK)
            {
                channelStatus = UpdateInjectionTimeUart();
            }
        }
        if ((GetInjectionMode() & TIMER_INJECTION) != 0)
        {
            timerStatus = InitInjectionTimer();
            if (timerStatus == OK)
            {
                timerStatus = SetInjectionTimer(injectionTime);
            }
        }
        status = (channelStatus != OK) ? channelStatus : timerStatus;
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
	if (GetInjectionMode() == INJECTION_DISABLED)
	{
		return DISABLED;
	}
    Status status = GetActualMeasurementTableField(injectionTable, &time);
    if (status == OK)
    {
        int i;
        float totalCorrectionPercentage = 0.0f;
        for (i = 0; (i < CORRECTION_COUNT) && (status == OK); ++i)
        {
            MeasurementTable* table = injectionCorrections[i].table;
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
        time = maxf(MIN_INJECTION_TIME, minf(time, MAX_INJECTION_TIME));
        if (time != injectionTime)
        {
            injectionTime = time;
            status = UpdateInjectionTime();
        }
    }
    return status;
}
