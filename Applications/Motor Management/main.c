/*
** Randd Motor Management System
** Author: Bart Kampers
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#include "Crank.h"
#include "Ignition.h"
#include "Injection.h"

#include "stm32f10x_gpio.h"

#define INPUT_BUFFER_SIZE 0xFF
#define MESSAGE_TEXT_LENGTH 64

#define DISPLAY_STATE_IGNITION  0
#define DISPLAY_STATE_INJECTION 1
#define DISPLAY_STATE_COUNT 2

#define DISPLAY_REFRESH_RATE 100
#define DISPLAY_CYCLE_MAX (DISPLAY_REFRESH_RATE * DISPLAY_STATE_COUNT)


char SYSTEM_NAME[] = "Randd MM32";
char VERSION[] = "2024-01";

char input[INPUT_BUFFER_SIZE];
char message[MESSAGE_TEXT_LENGTH];
char text[LCD_LINE_LENGTH + 1];

Status memoryStatus = UNINITIALIZED;
Status ignitionStatus = UNINITIALIZED;
Status injectionStatus = UNINITIALIZED;
Status communicationStatus = UNINITIALIZED;

int displayCylceCount = 0;


Measurement* rpmMeasurement;
Measurement* loadMeasurement;


void CheckPersistentMemoryNotifyCallback(Reference reference)
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
//        switch (displayCylceCount / DISPLAY_REFRESH_RATE)
//        {
//            case DISPLAY_STATE_IGNITION:
//                sprintf(text, "%5.0f%6.1f%5d", rpm, load, GetIgnitionAngle());
//                PutLcd(0, 0, "  RPM  LOAD  IGN");
//                SetOutputPins(0x0000);
//                break;
//            case DISPLAY_STATE_INJECTION:
//                sprintf(text, "%5.0f%6.1f%5.1f", rpm, load, GetInjectionTime());
//                PutLcd(0, 0, "  RPM  LOAD  INJ");
//                ResetOutputPins(0xFFFF);
//                break;
//        }
//        PutLcd(0, 1, text);
        sprintf(text, "%16f", GetIgnitionDutyCycle());
        PutLcd(0, 0, text);
        sprintf(text, " %7d %7d", GetCogTicks(), GetPwmDutyCycle());
        PutLcd(0, 1, text);
        UpdateLcd();
        snprintf(message, MESSAGE_TEXT_LENGTH, "cog=%d; gap=%d; rpm=%.0f; %s", GetCogTicks(), GetGapTicks(), GetRpm(), ((EngineIsRunning()) ? "run" : "stop"));
//        FireTextEvent("Ticks", message, DEFAULT_CHANNEL);
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

//uint8_t count = 0;

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
//    	uint16_t bits = count++;
//    	bits <<= 8;
//    	GPIO_Write(GPIOB, bits);
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

            if (IsValid(ignitionStatus) && IsValid(injectionStatus))
            {
                UpdateDisplay();
            }
        }
    }

}
