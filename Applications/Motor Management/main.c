/*
** Randd Motor Management System
** Author: Bart Kampers
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Version.h"
#include "Timers.h"
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

#include "Configuration.h"
#include "Crank.h"
#include "Ignition.h"
#include "Injection.h"
#include "RpmIndication.h"

#include "stm32f10x_gpio.h"

#define INPUT_BUFFER_SIZE 0xFF
#define MESSAGE_TEXT_LENGTH 64

#define DISPLAY_STATE_COUNT 2

#define DISPLAY_REFRESH_RATE 1000
#define DISPLAY_CYCLE_MAX (DISPLAY_REFRESH_RATE * DISPLAY_STATE_COUNT)

typedef enum
{
	DISPLAY_STATE_IGNITION,
	DISPLAY_STATE_INJECTION
} DisplayState;

char SYSTEM_NAME[] = "Randd MM32";

char input[INPUT_BUFFER_SIZE];
char message[MESSAGE_TEXT_LENGTH];
char text[LCD_LINE_LENGTH + 1];

Status memoryStatus = UNINITIALIZED;
Status ignitionStatus = UNINITIALIZED;
Status injectionStatus = UNINITIALIZED;
Status rpmIndicationStatus = UNINITIALIZED;
Status communicationStatus = UNINITIALIZED;

int displayCylceCount = 0;
DisplayState displayState = DISPLAY_STATE_IGNITION;

Measurement* rpmMeasurement;
Measurement* loadMeasurement;


void CheckPersistentMemoryNotifyCallback(Reference reference)
{
    if ((reference % 0x100) == 0)
    {
        sprintf(text, "Init memory %04X", reference);
        PutLcd(0, 1, text);
        UpdateLcd();
    }
}


void nextDisplayState() {
	switch (displayState)
	{
		case DISPLAY_STATE_IGNITION:
			displayState = (GetInjectionMode() == INJECTION_DISABLED) ? DISPLAY_STATE_IGNITION : DISPLAY_STATE_INJECTION;
			break;
		case DISPLAY_STATE_INJECTION:
		default:
			displayState = DISPLAY_STATE_IGNITION;
			break;
	}
}


void UpdateDisplay()
{
    if ((displayCylceCount % DISPLAY_REFRESH_RATE) == 0)
    {
        float load = NAN;
        float rpm = NAN;
        if (rpmMeasurement != NULL)
        {
            GetMeasurementValue(rpmMeasurement, &rpm);
        }
        if (loadMeasurement != NULL)
        {
            GetMeasurementValue(loadMeasurement, &load);
        }
        switch (displayState)
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
        nextDisplayState();
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

bool IsValid(Status status)
{
	return (status == OK) || (status == DISABLED);
}


void ShowIfError(Status status)
{
    if (!IsValid(status))
    {
        StatusToDisplay(status);
    }
}


void ShowStatus(char* name, Status status)
{
    FireTextEvent(name, status, DEFAULT_CHANNEL);
    ShowIfError(status);
}


int main(void)
{
    communicationStatus = OpenCommunicationChannel(DEFAULT_CHANNEL, INPUT_BUFFER_SIZE);
    InitOutputPins();
    InitLcd();

    FireTextEvent("System", SYSTEM_NAME, DEFAULT_CHANNEL);
    FireTextEvent("Version", VERSION, DEFAULT_CHANNEL);
    FireTextEvent("Memory", PersistentMemoryType(), DEFAULT_CHANNEL);
    sprintf(text, "%d", GetCogTotal());
    FireTextEvent("Cogs", text, DEFAULT_CHANNEL);
    sprintf(text, "%d", GetGapSize());
    FireTextEvent("Gap",text, DEFAULT_CHANNEL);
    PutLcdLine(0, SYSTEM_NAME);
    UpdateLcd();

    InitPersistentDataManager();

    RegisterTableTypes();
    RegisterEngineType();
    
    memoryStatus = CheckPersistentMemory(&CheckPersistentMemoryNotifyCallback);
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
            rpmIndicationStatus = InitRpmIndication();
            ShowStatus("Initialize RPM indication", rpmIndicationStatus);
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
            communicationStatus = HandleMessage(input, &motorManagementApi, DEFAULT_CHANNEL);
        }

        if (memoryStatus == OK)
        {
            Status status = UpdateIgnition();
            ShowIfError(status);
            if (status != ignitionStatus)
            {
                FireTextEvent(IGNITION, status, DEFAULT_CHANNEL);
                ignitionStatus = status;
            }

            status = UpdateInjection();
            ShowIfError(status);
            if (status != injectionStatus)
            {
                FireTextEvent(INJECTION, status, DEFAULT_CHANNEL);
                injectionStatus = status;
            }

            status = UpdateRpmIndication();
            if (status != rpmIndicationStatus)
			{
				FireTextEvent(RPM_INDICATION, status, DEFAULT_CHANNEL);
				rpmIndicationStatus = status;
			}

            if (IsValid(ignitionStatus) && IsValid(injectionStatus) && IsValid(rpmIndicationStatus))
            {
                UpdateDisplay();
            }
        }
    }

}
