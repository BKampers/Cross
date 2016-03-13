/*
** Randd Motor Management System
** Author: Bart Kampers
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Communication.h"
#include "Lcd.h"
#include "PersistentElementManager.h"
#include "Pins.h"

#include "AnalogInput.h"
#include "Messaging.h"

#include "Engine.h"
#include "MeasurementTable.h"
#include "Table.h"
#include "Measurements.h"

#include "Ignition.h"
#include "Injection.h"

#define INPUT_BUFFER_SIZE 0xFF

#define DISPLAY_STATE_IGNITION  0
#define DISPLAY_STATE_INJECTION 1
#define DISPLAY_STATE_COUNT 2

#define DISPLAY_REFRESH_RATE 1000
#define DISPLAY_CYCLE_MAX (DISPLAY_REFRESH_RATE * DISPLAY_STATE_COUNT)


char SYSTEM_NAME[] = "Randd MM32";

char input[INPUT_BUFFER_SIZE];
char text[LCD_LINE_LENGTH + 1];

Status memoryStatus = UNINITIALIZED;
Status ignitionStatus = UNINITIALIZED;
Status injectionStatus = UNINITIALIZED;
Status communicationStatus = UNINITIALIZED;

int displayCylceCount = 0;


Measurement* rpmMeasurement;
Measurement* loadMeasurement;


void checkPersistentMemoryNotifyCallback(Reference reference)
{
    if ((reference % 0x10) == 0)
    {
        sprintf(text, "Init memory %04X", reference);
        PutLcd(0, 1, text);
        UpdateLcd();
    }
}


void UpdateDisplay()
{
    if ((displayCylceCount % DISPLAY_REFRESH_RATE) == 0)
    {
        float load = -1.0f;
        float rpm = -1.0f;
        if (rpmMeasurement != NULL)
        {
            GetMeasurementValue(rpmMeasurement, &rpm);
        }
        if (loadMeasurement != NULL)
        {
            GetMeasurementValue(loadMeasurement, &load);
        }
        switch (displayCylceCount / DISPLAY_REFRESH_RATE)
        {
            case DISPLAY_STATE_IGNITION:
                sprintf(text, "%5.0f%6.1f%5d", rpm, load, GetIgnitionAngle());
                PutLcd(0, 0, "  RPM  LOAD  IGN");
                break;
            case DISPLAY_STATE_INJECTION:
                sprintf(text, "%5.0f%6.1f%5.1f", rpm, load, GetInjectionTime());
                PutLcd(0, 0, "  RPM  LOAD  INJ");
                break;
        }
        PutLcd(0, 1, text);
        UpdateLcd();
    }
    displayCylceCount = (displayCylceCount + 1) % DISPLAY_CYCLE_MAX;
}


void StatusToDisplay(Status status)
{
    PutLcdLine(0, status);
    if (strlen(text) > LCD_LINE_LENGTH)
    {
        PutLcdLine(1, &(status[LCD_LINE_LENGTH]));
    }
    UpdateLcd();
}


void ShowIfError(Status status)
{
    if (status != OK)
    {
        StatusToDisplay(status);
    }
}


void ShowStatus(char* name, Status status)
{
    FireTextEvent(name, status);
    if (status != OK)
    {
        StatusToDisplay(status);
    }
}


int main(void)
{
    communicationStatus = OpenCommunicationChannel(DEFAULT_CHANNEL, INPUT_BUFFER_SIZE);
    InitOutputPins();
    InitLcd();

    FireTextEvent("System", SYSTEM_NAME);
    FireTextEvent("Memory", PersistentMemoryType());
    PutLcdLine(0, SYSTEM_NAME);
    UpdateLcd();

    InitPersistentDataManager();

    RegisterTableTypes();
    RegisterEngineType();
    
    memoryStatus = CheckPersistentMemory(&checkPersistentMemoryNotifyCallback);
    ShowStatus("Check memory", memoryStatus);

    if (memoryStatus == OK)
    {
        Status status = InitEngine();
        ShowStatus("Initialize engine", status);
        
        if (status == OK)
        {
            InitAnalogInput();
            ignitionStatus = InitIgnition();
            ShowStatus("Initialize ignition", ignitionStatus);
            injectionStatus = InitInjection();
            ShowStatus("Initialize injection", injectionStatus);
        }
    }

    if (FindMeasurement(RPM, &rpmMeasurement) != OK)
    {
        rpmMeasurement = NULL;
    }
    if (FindMeasurement(LOAD, &loadMeasurement) != OK)
    {
        loadMeasurement = NULL;
    }

    for (;;)
    {
        ShowIfError(communicationStatus);
        communicationStatus = ReadString(input);
        if ((communicationStatus == OK) && (strlen(input) > 0))
        {
            communicationStatus = HandleMessage(input);
        }

        if (memoryStatus == OK)
        {
            Status status = UpdateIgnition();
            ShowIfError(status);
            if (status != ignitionStatus)
            {
                FireTextEvent(IGNITION, status);
                ignitionStatus = status;
            }

            status = UpdateInjection();
            ShowIfError(status);
            if (status != injectionStatus)
            {
                FireTextEvent(INJECTION, status);
                injectionStatus = status;
            }

            if ((ignitionStatus == OK) && (injectionStatus == OK))
            {
                UpdateDisplay();
            }
        }
    }

}
