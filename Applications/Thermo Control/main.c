#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Communication.h"
#include "PersistentElementManager.h"
#include "Pins.h"
#include "AnalogInput.h"
#include "JsonMessaging.h"
#include "Table.h"
#include "MeasurementTable.h"

/*#include "Messaging.h"*/

/*
#include "MeasurementTable.h"
#include "Table.h"
#include "Measurements.h"
*/

#define INPUT_BUFFER_SIZE 0xFF

char SYSTEM_NAME[] = "Temperature Control";

char TEMPERATURE_TABLE[] = "TemperatureTable";

Status communicationStatus = UNINITIALIZED;
Status memoryStatus = UNINITIALIZED;


void checkPersistentMemoryNotifyCallback(Reference reference)
{/*
    if ((reference % 0x10) == 0)
    {
        sprintf(text, "Init memory %04X", reference);
        PutLcd(0, 1, text);
        UpdateLcd();
    }
*/}


MeasurementTable* actualTemperatureTable;


Status InitTables()
{
    Status status = CreateMeasurementTable(TEMPERATURE_TABLE, TEMPERATURE, NULL, 20, 20, &actualTemperatureTable);
    if (status == OK)
    {
    	actualTemperatureTable->precision = 1.0f;
    	actualTemperatureTable->minimum = 0.0f;
    	actualTemperatureTable->maximum = 59.0f;
    	actualTemperatureTable->decimals = 0;
        status = SetMeasurementTableEnabled(TEMPERATURE_TABLE, TRUE);
    }
    return status;
}


int main(void)
{

    communicationStatus = OpenCommunicationChannel(DEFAULT_CHANNEL, INPUT_BUFFER_SIZE);
    InitOutputPins();

    FireTextEvent("System", SYSTEM_NAME, DEFAULT_CHANNEL);
    FireTextEvent("Memory", PersistentMemoryType(), DEFAULT_CHANNEL);

    InitPersistentDataManager();

    RegisterTableTypes();

    memoryStatus = CheckPersistentMemory(&checkPersistentMemoryNotifyCallback);

    for (;;)
    {
    }
}
