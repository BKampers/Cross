/*
** JSON messaging for Randd Motor Management system
** Copyright 2015, Bart Kampers
*/

#include "Messaging.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Communication.h"
#include "JsonMessage.h"

#include "Engine.h"
#include "Crank.h"
#include "Ignition.h"
#include "Injection.h"

#include "MeasurementTable.h"
#include "Measurements.h"


const char* STATUS = "Status";

const char* MESSAGE = "Message";
const char* RESPONSE = "Response";

const char* REQUEST = "Request";
const char* MODIFY = "Modify";
const char* NOTIFICATION = "Notification";

const char* SUBJECT = "Subject";

const char* ERR = "Error";

const char* PROPERTIES = "Properties";
const char* VALUE = "Value";
const char* SIMULATION = "Simulation";

const char* MINIMUM = "Minimum";
const char* MAXIMUM = "Maximum";

const char* MEASUREMENTS = "Measurements";
const char* MEASUREMENT_TABLES = "MeasurementTables";
const char* NAMES = "Names";

const char* ENABLED = "Enabled";
const char* TABLE = "Table";
const char* INDEX = "Index";
const char* COLUMN = "Column";
const char* ROW = "Row";
const char* DECIMALS = "Decimals";
const char* FORMAT = "Format";

const char* ENGINE = "Engine";
const char* COGWHEEL = "Cogwheel";
const char* COG_TOTAL = "CogTotal";
const char* GAP_SIZE = "GapSize";
const char* OFFSET = "Offset";
const char* DEAD_POINTS = "DeadPoints";
const char* CYLINDER_COUNT = "CylinderCount";

const char* FLASH_ELEMENTS = "FlashElements";
const char* FLASH_MEM = "Flash";
const char* REFERENCE = "Reference";
const char* TYPE_ID = "TypeId";
const char* SIZE = "Size";
const char* COUNT = "Count";
const char* ELEMENTS = "Elements";

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


void SendEot()
{
    char eot[2];
    eot[0] = '~';
    eot[1] = 0;
    WriteString(eot);
}


void SendErrorResponse(const char* message, const char* error)
{
    sprintf(
        response,
        "{ \"%s\": \"%s\", \"%s\": \"%s\" }\r\n",
        RESPONSE, message,
        ERR, error);
    WriteString(response);
    SendEot();
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
    SendEot();
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
    SendEot();
}


void SendMeasurementValue(const char* name, const Measurement* measurement)
{
    float value;
    Status status = GetMeasurementValue(measurement, &value);
    sprintf(
        response,
        "{ \"%s\": \"%s\", \"%s\": \"%s\", \"%s\": %f, \"%s\": %s, \"%s\": \"%s\", \"%s\": %f, \"%s\": %f, \"%s\": \"%s\" }\r\n",
        RESPONSE, REQUEST,
        SUBJECT, name,
        VALUE, value,
        SIMULATION, (measurement->simulationValue != NULL) ? TRUE_LITERAL : FALSE_LITERAL,
        FORMAT, measurement->format,
        MINIMUM, measurement->minimum,
        MAXIMUM, measurement->maximum,
        STATUS, status);
    WriteString(response);
    SendEot();
}


void SendIndexes(int columnIndex, int rowIndex)
{
    sprintf(response,
        ", \"%s\": %d, \"%s\": %d\r\n",
        COLUMN, columnIndex,
        ROW, rowIndex);
    WriteString(response);
}


Status SendTableFields(const MeasurementTable* measurementTable)
{
    Status status = OK;
    byte c, r;
    if (measurementTable->columnMeasurement != NULL)
    {
        sprintf(response, ", \"%s\": \"%s\"", "ColumnMeasurement", measurementTable->columnMeasurement->name);
        WriteString(response);
    }
    if (measurementTable->rowMeasurement != NULL)
    {
        sprintf(response, ", \"%s\": \"%s\"", "RowMeasurement", measurementTable->rowMeasurement->name);
        WriteString(response);
    }
    sprintf(
        response,
        ", \"%s\": %f, \"%s\": %f, \"%s\": %d, \"%s\":\r\n  [\r\n",
        MINIMUM, measurementTable->minimum,
        MAXIMUM, measurementTable->maximum,
        DECIMALS, measurementTable->decimals,
        TABLE);
    WriteString(response);
    for (r = 0; (r < measurementTable->table.rows) && (status == OK); ++r)
    {
        WriteString("    [ ");
        for (c = 0; (c < measurementTable->table.columns) && (status == OK); ++c)
        {
            float field;
            Status getFieldStatus = GetMeasurementTableField(measurementTable, c, r, &field);
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
        if (r < measurementTable->table.rows - 1)
        {
            WriteString(",");
        }
        WriteString("\r\n");
    }
    WriteString("  ]\r\n");
    return status;
}


void RespondMeasurementTableRequest(const char* jsonString, const MeasurementTable* measurementTable, const char* name)
{
    Status status = OK;
    bool sendTable = Contains(jsonString, PROPERTIES, TABLE);
    bool sendIndex = Contains(jsonString, PROPERTIES, INDEX);
    bool sendEnabled = Contains(jsonString, PROPERTIES, ENABLED);
    bool sendDefault = ! sendTable && ! sendIndex && ! sendEnabled;
    sprintf(
        response,
        "{ \"%s\": \"%s\", \"%s\": \"%s\"",
        RESPONSE, REQUEST,
        SUBJECT, name);
        WriteString(response);
    if (sendTable || sendDefault)
    {
        status = SendTableFields(measurementTable);
    }
    if (sendIndex || sendDefault)
    {
        SendIndexes(measurementTable->columnIndex, measurementTable->rowIndex);
    }
    if (sendEnabled || sendDefault)
    {
        bool enabled;
        GetMeasurementTableEnabled(name, &enabled);
        sprintf(response, ", \"%s\": %s", ENABLED, (enabled ? TRUE_LITERAL : FALSE_LITERAL));
        WriteString(response);
    }
    sprintf(response, ", \"%s\": \"%s\"}\r\n", STATUS, status);
    WriteString(response);
    SendEot();
}


void RespondRequest(const struct jsonparse_state* subject)
{
    char* subjectString = AllocStateString(subject);
    bool responded = FALSE;
    Measurement* measurement;
    Status status = FindMeasurement(subjectString, &measurement);
    if (status == OK)
    {
        SendMeasurementValue(subjectString, measurement);
        responded = TRUE;
    }
    if (! responded)
    {
        MeasurementTable* measurementTable;
        status = FindMeasurementTable(subjectString, &measurementTable);
        if (status == OK)
        {
            RespondMeasurementTableRequest(subject->json, measurementTable, subjectString);
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


void SendMeasurementNames()
{
    sprintf(
        response,
        "{ \"%s\": \"%s\", \"%s\": \"%s\", \"%s\": [\r\n",
        RESPONSE, REQUEST,
        SUBJECT, MEASUREMENTS,
        NAMES);
    WriteString(response);
    sprintf(response, "  \"%s\",\r\n", RPM);
    WriteString(response);
    sprintf(response, "  \"%s\",\r\n", LOAD);
    WriteString(response);
    sprintf(response, "  \"%s\",\r\n", WATER_TEMPERATURE);
    WriteString(response);
    sprintf(response, "  \"%s\",\r\n", AIR_TEMPERATURE);
    WriteString(response);
    sprintf(response, "  \"%s\",\r\n", BATTERY_VOLTAGE);
    WriteString(response);
    sprintf(response, "  \"%s\"]\r\n}\r\n", MAP_SENSOR);
    WriteString(response);
}


void SendMeasurementTableNames()
{
    int i;
    int count = GetMeasurementTableCount();
    sprintf(
        response,
        "{ \"%s\": \"%s\", \"%s\": \"%s\", \"%s\": [",
        RESPONSE, REQUEST,
        SUBJECT, MEASUREMENT_TABLES,
        NAMES);
    WriteString(response);
    for (i = 0; i < count; ++i)
    {
        if (i > 0)
        {
            WriteString(",");
        }
        sprintf(response, "\r\n  \"%s\"", GetMeasurementTableName(i));
        WriteString(response);
    }
    WriteString("]\r\n}\r\n");
    SendEot();
}


void SendEngineProperties()
{
    int i;
    sprintf(
        response,
        "{ \"%s\": \"%s\", \"%s\": \"%s\", \"%s\": %d, \"%s\": { \"%s\": %d,\"%s\": %d,\"%s\": %d }, \"%s\": [",
        RESPONSE, REQUEST,
        SUBJECT, ENGINE,
        CYLINDER_COUNT, GetCylinderCount(),
        COGWHEEL, COG_TOTAL, GetCogTotal(), GAP_SIZE, GetGapSize(), OFFSET, GetDeadPointOffset(),
        DEAD_POINTS);
    WriteString(response);
    for (i = 0; i < GetDeadPointCount(); ++i)
    {
        if (i > 0)
        {
            WriteString(",");
        }
        sprintf(response, "%d", GetDeadPointCog(i));
        WriteString(response);
    }
    sprintf(response, "] }");
    WriteString(response);
    SendEot();
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
    SendEot();
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
    bool enabled;
    Status fieldStatus = OK;
    Status enableStatus = OK;
    Status parseStatus = GetIntValue(jsonString, ROW, &row);
    if (parseStatus == OK)
    {
        fieldStatus = GetIntValue(jsonString, COLUMN, &column);
        if (fieldStatus == OK)
        {
            fieldStatus = GetFloatValue(jsonString, VALUE, &value);
            if (fieldStatus == OK)
            {
                fieldStatus = SetMeasurementTableField(name, column, row, value);
            }
        }
    }
    parseStatus = GetBoolValue(jsonString, ENABLED, &enabled);
    if (parseStatus == OK)
    {
        enableStatus = SetMeasurementTableEnabled(name, enabled);
    }
    SendStatus(MODIFY, name, (fieldStatus != OK) ? fieldStatus : enableStatus);
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


void ModifyCogwheel(const char* jsonString)
{
    int cogTotal, gapSize, offset;
    Status status = GetIntValue(jsonString, COG_TOTAL, &cogTotal);
    if (status == OK)
    {
        status = GetIntValue(jsonString, GAP_SIZE, &gapSize);
        if (status == OK)
        {
            status = GetIntValue(jsonString, OFFSET, &offset);
            if (status == OK)
            {
                if (status == OK)
                {
                    status = SetGogwheelProperties(cogTotal, gapSize, offset);
                }
            }
        }
    }
    SendStatus(MODIFY, COGWHEEL, status);
}


void ModifyCylinderCount(const char* jsonString)
{
    int value;
    Status status = GetIntValue(jsonString, VALUE, &value);
    if (status == OK)
    {
        status = SetCylinderCount(value);
    }
    SendStatus(MODIFY, CYLINDER_COUNT, status);
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
    else if (EqualString(subject, MEASUREMENTS))
    {
        SendMeasurementNames();
    }
    else if (EqualString(subject, MEASUREMENT_TABLES))
    {
        SendMeasurementTableNames();
    }
    else if (EqualString(subject, ENGINE))
    {
        SendEngineProperties();
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
        if (strcmp(subjectString, COGWHEEL) == 0)
        {
            ModifyCogwheel(subject->json);
            handled = TRUE;
        }
        else if (strcmp(subjectString, CYLINDER_COUNT) == 0)
        {
            ModifyCylinderCount(subject->json);
            handled= TRUE;
        }
        else if (strcmp(subjectString, FLASH_MEM) == 0)
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
    SendEot();
}


void SendIntegerNotification(const char* name, int value)
{
    sprintf(response, "{ \"%s\" : \"%s\", \"%s\" : %d }\r\n", MESSAGE, NOTIFICATION, name, value);
    WriteString(response);
    SendEot();
}


void SendRealNotification(const char* name, double value)
{
    sprintf(response, "{ \"%s\" : \"%s\", \"%s\" : %f }\r\n", MESSAGE, NOTIFICATION, name, value);
    WriteString(response);
    SendEot();
}


void SendErrorNotification(const char* error)
{
     SendTextNotification(ERR, error);
}
