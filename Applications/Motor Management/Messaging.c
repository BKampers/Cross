/*
** JSON messaging for Randd Motor Management system
** Copyright 2014, Bart Kampers
*/

#include "Messaging.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Communication.h"
#include "JsonMessage.h"

#include "Crank.h"
#include "Ignition.h"
#include "Injection.h"

#include "Controllers.h"
#include "Measurements.h"


const char* STATUS = "Status";

const char* MESSAGE  = "Message";
const char* RESPONSE = "Response";

const char* REQUEST      = "Request";
const char* MODIFY       = "Modify";
const char* NOTIFICATION = "Notification";

const char* SUBJECT  = "Subject";

const char* ERR = "Error";

const char* PROPERTIES = "Properties";
const char* VALUE      = "Value";
const char* SIMULATION = "Simulation";

const char* TABLE  = "Table";
const char* INDEX  = "Index";
const char* COLUMN = "Column";
const char* ROW    = "Row";

const char* FLASH_ELEMENTS = "FlashElements";
const char* FLASH_MEM      = "Flash";
const char* REFERENCE      = "Reference";
const char* TYPE_ID        = "TypeId";
const char* SIZE           = "Size";
const char* COUNT          = "Count";
const char* ELEMENTS       = "Elements";

const char* IGNITION_TIMER = "IgnitionTimer";


char response[255];		


char* AllocStateString(const struct jsonparse_state* parseState)
{
    char* value = malloc(parseState->vlen + 1);
    strncpy(value, parseState->json + parseState->vstart, parseState->vlen);
    value[parseState->vlen] = '\0';
    return value;
}


char* AllocString(const char* jsonString, const char* name)
{
    struct jsonparse_state parseState;
    Status status = FindPair(jsonString, name, &parseState);
    if (status == OK)
    {
        return AllocStateString(&parseState);
    }
    else
    {
        return NULL;
    }
}


void SendErrorResponse(const char* message, const char* error)
{
    sprintf(
        response,
        "{ \"%s\": \"%s\", \"%s\": \"%s\" }\r\n",
        RESPONSE, message,
        ERR, error);
    WriteString(response);
}


void SendUnknownSubjectResponse(const char* message, const char* subject)
{
    sprintf(
        response,
        "{ \"%s\": \"%s\", \"%s\": \"%s\", \"%s\": \"%s\" }\r\n",
        RESPONSE, message,
        SUBJECT, subject,
        ERR, "Unknown subject");
    WriteString(response);
}


void SendStatus(const char* message, const char* subject, const Status status)
{
    sprintf(
        response,
        "{ \"%s\": \"%s\", \"%s\": \"%s\", \"%s\": \"%s\" }\r\n",
        RESPONSE, message,
        SUBJECT, subject,
        STATUS, status);
    WriteString(response);
}


void SendValue(const char* subject, float value, const Status status)
{
    sprintf(
        response,
        "{ \"%s\": \"%s\", \"%s\": \"%s\", \"%s\": %f, \"%s\": \"%s\" }\r\n",
        RESPONSE, REQUEST,
        SUBJECT, subject,
        VALUE, value,
        STATUS, status);
    WriteString(response);
}


void SendIndexes(int columnIndex, int rowIndex)
{
    sprintf(response,
        ", \"%s\": %d, \"%s\": %d\r\n",
        COLUMN, columnIndex,
        ROW, rowIndex);
    WriteString(response);
}


Status SendTableFields(const TableController* tableController)
{
    Status status = OK;
    byte c, r;
    sprintf(response, ", \"%s\":\r\n  [\r\n", TABLE);
    WriteString(response);
    for (r = 0; (r < tableController->table.rows) && (status == OK); ++r)
    {
        WriteString("    [ ");
        for (c = 0; (c < tableController->table.columns) && (status == OK); ++c)
        {
            float field;
            Status getFieldStatus = GetTableControllerFieldValue(tableController, c, r, &field);
            if (getFieldStatus != OK)
            {
                status = getFieldStatus;
            }
            if ( c > 0)
            {
                WriteString(", ");
            }
            sprintf(response, "%f", field);
            WriteString(response);
        }
        WriteString(" ]");
        if (r < tableController->table.rows - 1)
        {
            WriteString(",");
        }
        WriteString("\r\n");
    }
    WriteString("  ]\r\n");
    return status;
}


void RespondTableControllerRequest(const char* jsonString, const TableController* tableController, const char* name)
{
    Status status = OK;
    bool sendTable = Contains(jsonString, PROPERTIES, TABLE);
    bool sendIndex = Contains(jsonString, PROPERTIES, INDEX);
    bool sendDefault = ! sendTable && ! sendIndex;
    sprintf(
        response,
        "{ \"%s\": \"%s\", \"%s\": \"%s\"",
        RESPONSE, REQUEST,
        SUBJECT, name);
        WriteString(response);
    if (sendTable || sendDefault)
    {
        status = SendTableFields(tableController);
    }
    if (sendIndex || sendDefault)
    {
        SendIndexes(tableController->columnIndex, tableController->rowIndex);
    }
    sprintf(response, ", \"%s\": \"%s\"}\r\n", STATUS, status);
    WriteString(response);
}


void RespondRequest(const struct jsonparse_state* subject)
{
    char* subjectString = AllocStateString(subject);
    bool responded = FALSE;
    Measurement* measurement;
    Status status = FindMeasurement(subjectString, &measurement);
    if (status == OK)
    {
        float value;
        status = GetMeasurementValue(measurement, &value);
        SendValue(subjectString, value, status);
        responded = TRUE;
    }
    if (! responded)
    {
        TableController* tableController = FindTableController(subjectString);
        if (tableController != NULL)
        {
            RespondTableControllerRequest(subject->json,tableController, subjectString);
            responded = TRUE;
        }
    }
    if (! responded)
    {
        char* messageString = AllocString(subject->json, MESSAGE);
        SendUnknownSubjectResponse(messageString, subjectString);
        free(messageString);
    }
    free(subjectString);
}


void SendFlashMemory(const char* jsonString)
{
    Status status = OK;
    int reference;
    int count;
    int i;

    if (GetIntValue(jsonString, REFERENCE, &reference) != OK)
    {
        reference = 0;
    }
    if (GetIntValue(jsonString, COUNT, &count) != OK)
    {
        count = (int) PersistentMemoryLimit();
    }

    sprintf(
        response,
        "{ \"%s\": \"%s\", \"%s\": \"%s\", \"%s\": %d, \"%s\": [ ",
        RESPONSE, REQUEST,
        SUBJECT, FLASH_MEM,
        REFERENCE, reference,
        VALUE);
    WriteString(response);
    for (i = 0; (i < count) && (status == OK); ++i)
    {
        if (i > 0)
        {
            WriteString(", ");
        }
        char buffer;
        status = ReadPersistentMemory(reference + i, 1, &buffer);
        sprintf(response, "%d", buffer);
        WriteString(response);
    }
    sprintf(response, " ], \"%s\": \"%s\" }\r\n", STATUS, status);
    WriteString(response);
}


void SendFlashElements()
{
    bool first = TRUE;
    int typeId;
    sprintf(
    response,
    "{ \"%s\": \"%s\", \"%s\": \"%s\", \"%s\": [\r\n",
    RESPONSE, REQUEST,
    SUBJECT, FLASH_ELEMENTS,
    ELEMENTS);
    WriteString(response);
    Status status = OK;
    for (typeId = 1; typeId <= GetTypeCount(); ++typeId)
    {
        Reference reference;
        status = FindFirst(typeId, &reference);
        while (status == OK)
        {
            ElementSize size;
            status = GetSize(reference, &size);
            if (status == OK)
            {
                if (! first)
                {
                    WriteString(",\r\n");
                }
                first = FALSE;
                sprintf(
                    response,
                    "  { \"%s\" : \"%d\", \"%s\" : \"%d\", \"%s\" : \"%d\" }",
                    TYPE_ID, typeId,
                    REFERENCE, reference,
                    SIZE, size);
                WriteString(response);
            }
            status = FindNext(typeId, &reference);
        }
    }
    if (status == INVALID_ID)
    {
        /* first or next not found, not an error */
        status = OK;
    }
    sprintf(response, "],\r\n \"%s\" : \"%s\" }\r\n", STATUS, status);
    WriteString(response);
}


void ModifyTable(const char* jsonString, const char* name)
{
    int row, column;
    float value;
    Status status = GetIntValue(jsonString, ROW, &row);
    if (status == OK)
    {
        status = GetIntValue(jsonString, COLUMN, &column);
        if (status == OK)
        {
            status = GetFloatValue(jsonString, VALUE, &value);
            if (status == OK)
            {
                status = SetTableControllerFieldValue(name, column, row, value);
            }
        }
    }
    SendStatus(MODIFY, name, status);
}


void ModifyIgnitionTimer(const char* jsonString)
{
    int value;
    TIMER_SETTINGS timerSettings;
    GetIgnitionTimerSettings(&timerSettings);
    if (GetIntValue(jsonString, "Prescaler", &value) == OK)
    {
        timerSettings.prescaler = (uint16_t) value;
    }
    if (GetIntValue(jsonString, "Period", &value) == OK)
    {
        timerSettings.period = (uint16_t) value;
    }
    if (GetIntValue(jsonString, "Counter", &value) == OK)
    {
        timerSettings.counter = (uint16_t) value;
    }
    SetIgnitionTimerSettings(&timerSettings);
    SendStatus(MODIFY, IGNITION_TIMER, OK);
}


void ModifyFlash(const char* jsonString)
{
    int reference;
    int value;
    Status status = GetIntValue(jsonString, REFERENCE, &reference);
    if (status == OK)
    {
        int count;
        status = GetIntValue(jsonString, COUNT, &count);
        if (status != OK)
        {
            count = 1;
        }
        if (count > 0)
        {
            status = GetIntValue(jsonString, VALUE, &value);
            if (status == OK)
            {
                if ((0x00 <= value) && (value <= 0xFF))
                {
                    status = FillPersistentMemory(reference, count, (byte) value);
                }
                else
                {
                    status = "InvalidByteValue";
                }
            }
        }
        else
        {
            status = INVALID_PARAMETER;
        }
    }
    SendStatus(MODIFY, FLASH_MEM, status);
}


void HandleRequest(const struct jsonparse_state* subject)
{
    if (EqualString(subject, FLASH_MEM))
    {
        SendFlashMemory(subject->json);
    }
    else if (EqualString(subject, FLASH_ELEMENTS))
    {
        SendFlashElements();
    }
    else
    {
        RespondRequest(subject);
    }
}


bool HandleModifySimulation(const char* jsonString, const char* subjectString)
{
    bool simulation;
    Status status = GetBoolValue(jsonString, SIMULATION, &simulation);
    if (status == OK)
    {
        if (simulation)
        {
            float simulationValue;
            status = GetFloatValue(jsonString, VALUE, &simulationValue);
            if (status == OK)
            {
                status = SetMeasurementSimulation(subjectString, simulationValue);
            }
        }
        else
        {
            status = ResetMeasurementSimulation(subjectString);
        }
        SendStatus(MODIFY, subjectString, status);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


void HandleModification(const struct jsonparse_state* subject)
{
    char* subjectString = AllocStateString(subject);
    bool handled = FALSE;
    Table table;
    if (FindTable(subjectString, &table) == OK)
    {
        ModifyTable(subject->json, subjectString);
        handled = TRUE;
    }
    if (! handled)
    {
        handled = HandleModifySimulation(subject->json, subjectString);
    }
    if (! handled)
    {
        if (strcmp(subjectString, FLASH_MEM) == 0)
        {
            ModifyFlash(subject->json);
            handled = TRUE;
        }
        else if (strcmp(subjectString, IGNITION_TIMER) == 0)
        {
            ModifyIgnitionTimer(subject->json);
            handled = TRUE;
        }
    }
    if (! handled)
    {
        char* messageString = AllocString(subject->json, MESSAGE);
        SendUnknownSubjectResponse(messageString, subjectString);
        free(messageString);
    }
    free(subjectString);
}


/*
** Message examples
** { "Message" : "Request", "Subject" : "RPM" }
** { "Message" : "Request", "Subject" : "Ignition", "Properties" : ["Table", "Index"] }
** { "Message" : "Request", "Subject" : "Flash", "Reference" : <integer>, "Count" : <integer> }
** { "Message" : "Modify", "Subject" : "Flash", "Reference" : <integer>, "Value" : <byte> }
** { "Message" : "Modify", "Subject" : "Ignition", "Column" : <integer>, "Row" : <integer>, "Value" : <integer> }
** { "Message" : "Modify", "Subject" : "IgnitionTimer", "Prescaler" : intValue, "Period" : <integer>, "Counter" : <integer> }
** { "Message" : "Modify", "Subject" : "RPM", "Simulation" : <boolean>, "Value" : <integer> }
*/
void HandleMessage(const char* jsonString)
{
    struct jsonparse_state message;
    Status status = FindPair(jsonString, MESSAGE, &message);
    if (status == OK)
    {
        struct jsonparse_state subject;
        status = FindPair(jsonString, SUBJECT, &subject);
        if (status == OK)
        {
            if (EqualString(&message, REQUEST))
            {
                HandleRequest(&subject);
            }
            else if (EqualString(&message, MODIFY))
            {
                HandleModification(&subject);
            }
            else
            {
                SendErrorNotification("Unknown message");
            }
        }
        else
        {
            char* messageString = AllocString(jsonString, MESSAGE);
            SendErrorResponse(messageString, "No subject");
            free(messageString);
        }
    }
    else
    {
        SendErrorNotification("Not a message");
    }
}


void SendTextNotification(const char* name, const char* value)
{
    sprintf(response, "{ \"%s\" : \"%s\", \"%s\" : \"%s\" }\r\n", MESSAGE, NOTIFICATION, name, value);
    WriteString(response);
}


void SendIntegerNotification(const char* name, int value)
{
    sprintf(response, "{ \"%s\" : \"%s\", \"%s\" : %d }\r\n", MESSAGE, NOTIFICATION, name, value);
    WriteString(response);
}


void SendRealNotification(const char* name, double value)
{
    sprintf(response, "{ \"%s\" : \"%s\", \"%s\" : %f }\r\n", MESSAGE, NOTIFICATION, name, value);
    WriteString(response);
}


void SendErrorNotification(const char* error)
{
     SendTextNotification(ERR, error);
}
