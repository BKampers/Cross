/*
** JSON messaging for Randd Motor Management system
** Copyright 2015, Bart Kampers
*/

#include "Messaging.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Communication.h"
#include "JsonComposer.h"
#include "JsonParser.h"

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

char* INVALID_MESSAGE = "Invalid message";
char* INVALID_SUBJECT = "Invalid subject";


void SendEot()
{
    char eot[2];
    eot[0] = '~';
    eot[1] = 0;
    WriteString(eot);
}


void SendErrorResponse(const char* message, const char* error)
{
    JsonNode response;
    ComposeObject(&response);
    PutStringMember(&response, RESPONSE, message);
    PutStringMember(&response, ERR, error);
    WriteString(response.source);
    free(response.source);
    SendEot();
    
}


void SendUnknownSubjectResponse(const char* message, const char* subject)
{
    JsonNode response;
    ComposeObject(&response);
    PutStringMember(&response, RESPONSE, message);
    PutStringMember(&response, SUBJECT, subject);
    PutStringMember(&response, ERR, "Unknown subject");
    WriteString(response.source);
    free(response.source);
    SendEot();
}


void SendStatus(const char* message, const char* subject, const Status status)
{
    JsonNode response;
    ComposeObject(&response);
    PutStringMember(&response, RESPONSE, message);
    PutStringMember(&response, SUBJECT, subject);
    PutStringMember(&response, STATUS, status);
    WriteString(response.source);
    free(response.source);
    SendEot();
}


void SendMeasurementValue(const char* name, const Measurement* measurement)
{
    float value;
    Status status = GetMeasurementValue(measurement, &value);
    JsonNode response;
    ComposeObject(&response);
    PutStringMember(&response, RESPONSE, REQUEST);
    PutStringMember(&response, SUBJECT, name);
    PutRealMember(&response, VALUE, value);
    PutBooleanMember(&response, SIMULATION, (measurement->simulationValue != NULL));
    PutStringMember(&response, FORMAT, measurement->format);
    PutRealMember(&response, MINIMUM, measurement->minimum);
    PutRealMember(&response, MAXIMUM, measurement->maximum);
    PutStringMember(&response, STATUS, status);
    WriteString(response.source);
    free(response.source);
    SendEot();
}


void PutIndexes(JsonNode* object, int columnIndex, int rowIndex)
{
    PutIntegerMember(object, COLUMN, columnIndex);
    PutIntegerMember(object, ROW, rowIndex);
}


Status PutTableFields(JsonNode* object, const MeasurementTable* measurementTable)
{
    Status status = OK;
    JsonNode tableArray;
    byte c, r;
    if (measurementTable->columnMeasurement != NULL)
    {
        PutStringMember(object, "ColumnMeasurement", measurementTable->columnMeasurement->name);
    }
    if (measurementTable->rowMeasurement != NULL)
    {
        PutStringMember(object, "RowMeasurement", measurementTable->rowMeasurement->name);
    }
    PutRealMember(object, MINIMUM, measurementTable->minimum);
    PutRealMember(object, MAXIMUM, measurementTable->maximum);
    PutIntegerMember(object, DECIMALS, measurementTable->decimals);
    ComposeArray(&tableArray);
    for (r = 0; (r < measurementTable->table.rows) && (status == OK); ++r)
    {
        JsonNode rowArray;
        ComposeArray(&rowArray);
        for (c = 0; (c < measurementTable->table.columns) && (status == OK); ++c)
        {
            float field;
            Status getFieldStatus = GetMeasurementTableField(measurementTable, c, r, &field);
            if (getFieldStatus != OK)
            {
                status = getFieldStatus;
            }
            AddRealElement(&rowArray, field);
        }
        AddArrayElement(&tableArray, &rowArray);
        free(rowArray.source);
    }
    PutArrayMember(object, TABLE, &tableArray);
    free(tableArray.source);
    return status;
}


bool HasString(JsonNode* array, const char* value)
{
    int index;
    JsonStatus jsonStatus = IndexOfString(array, value, &index);
    return jsonStatus == JSON_OK;
}


void RespondMeasurementTableRequest(const JsonNode* object, const MeasurementTable* measurementTable, const char* name)
{
    Status status = OK;
    JsonNode response, properties;
    JsonStatus jsonStatus = GetArray(object, PROPERTIES, &properties);
    bool sendTable = HasString(&properties, TABLE);
    bool sendIndex = HasString(&properties, INDEX);
    bool sendEnabled = HasString(&properties, ENABLED);
    bool sendDefault = ! sendTable && ! sendIndex && ! sendEnabled;
    ComposeObject(&response);
    PutStringMember(&response, RESPONSE, REQUEST);
    PutStringMember(&response, SUBJECT, name);
    if (sendTable || sendDefault)
    {
        status = PutTableFields(&response, measurementTable);
    }
    if (sendIndex || sendDefault)
    {
        PutIndexes(&response, measurementTable->columnIndex, measurementTable->rowIndex);
    }
    if (sendEnabled || sendDefault)
    {
        bool enabled;
        GetMeasurementTableEnabled(name, &enabled);
        PutBooleanMember(&response, ENABLED, enabled);
    }
    PutStringMember(&response, STATUS, status);
    WriteString(response.source);
    free(response.source);
    SendEot();
}


void RespondRequest(const JsonNode* object, const char* subjectString)
{
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
            RespondMeasurementTableRequest(object, measurementTable, subjectString);
            responded = TRUE;
        }
    }
    if (! responded)
    {
        char* messageString;
        AllocateString(object, MESSAGE, &messageString);
        SendUnknownSubjectResponse(messageString, subjectString);
        free(messageString);
    }
}


void SendMeasurementNames()
{
    JsonNode response, names;
    ComposeArray(&names);
    AddStringElement(&names, RPM);
    AddStringElement(&names, LOAD);
    AddStringElement(&names, WATER_TEMPERATURE);
    AddStringElement(&names, AIR_TEMPERATURE);
    AddStringElement(&names, BATTERY_VOLTAGE);
    AddStringElement(&names, MAP_SENSOR);
    ComposeObject(&response);
    PutStringMember(&response, RESPONSE, REQUEST);
    PutStringMember(&response, SUBJECT, MEASUREMENTS);
    PutArrayMember(&response, NAMES, &names);
    WriteString(response.source);
    free(names.source);
    free(response.source);
    SendEot();
}


void SendMeasurementTableNames()
{
    JsonNode response, names;
    int i;
    int count = GetMeasurementTableCount();
    ComposeArray(&names);
    ComposeObject(&response);
    PutStringMember(&response, RESPONSE, REQUEST);
    PutStringMember(&response, SUBJECT, MEASUREMENT_TABLES);
    for (i = 0; i < count; ++i)
    {
        AddStringElement(&names, GetMeasurementTableName(i));
    }
    PutArrayMember(&response, NAMES, &names);
    free(names.source);
    WriteString(response.source);
    SendEot();
    free(response.source);
}


void SendEngineProperties()
{
    JsonNode response, cogwheel, deadPoints;
    int i;
    ComposeObject(&response);
    ComposeObject(&cogwheel);
    PutIntegerMember(&cogwheel, COG_TOTAL, GetCogTotal());
    PutIntegerMember(&cogwheel, GAP_SIZE, GetGapSize());
    PutIntegerMember(&cogwheel, OFFSET, GetDeadPointOffset());
    ComposeArray(&deadPoints);
    for (i = 0; i < GetDeadPointCount(); ++i)
    {
        AddIntegerElement(&deadPoints, GetDeadPointCog(i));
    }
    PutStringMember(&response, RESPONSE, REQUEST);
    PutStringMember(&response, SUBJECT, ENGINE);
    PutIntegerMember(&response, CYLINDER_COUNT, GetCylinderCount());
    PutObjectMember(&response, COGWHEEL, &cogwheel);
    PutArrayMember(&response, DEAD_POINTS, &deadPoints);
    free(cogwheel.source);
    free(deadPoints.source);
    WriteString(response.source);
    SendEot();
    free(response.source);
}


void SendFlashMemory(const JsonNode* object)
{
    JsonNode response;
    JsonNode memory;
    Status status = OK;
    int reference;
    int count;
    int i;
    ComposeObject(&response);
    if (GetInt(object, REFERENCE, &reference) != JSON_OK)
    {
        reference = 0;
    }
    if (GetInt(object, COUNT, &count) != JSON_OK)
    {
        count = (int) PersistentMemoryLimit();
    }
    PutStringMember(&response, RESPONSE, REQUEST);
    PutStringMember(&response, SUBJECT, FLASH_MEM);
    PutIntegerMember(&response, REFERENCE, reference);
    ComposeArray(&memory);
    for (i = 0; (i < count) && (status == OK); ++i)
    {
        char buffer;
        status = ReadPersistentMemory(reference + i, 1, &buffer);
        AddIntegerElement(&memory, buffer);
    }
    PutArrayMember(&response, VALUE, &memory);
    free(memory.source);
    PutStringMember(&response, STATUS, status);
    WriteString(response.source);
    SendEot();
    free(response.source);
}


void SendFlashElements()
{
    Status status = OK;
    JsonNode response, elements, element;
    int typeId;
    ComposeObject(&response);
    ComposeArray(&elements);
    PutStringMember(&response, RESPONSE, REQUEST);
    PutStringMember(&response, SUBJECT, FLASH_ELEMENTS);
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
                ComposeObject(&element);
                PutIntegerMember(&element, TYPE_ID, typeId);
                PutIntegerMember(&element, REFERENCE, reference);
                PutIntegerMember(&element, SIZE, size);
                AddObjectElement(&elements, &element);
                free(element.source);
            }
            status = FindNext(typeId, &reference);
        }
    }
    if (status == INVALID_ID)
    {
        /* first or next not found, not an error */
        status = OK;
    }
    PutArrayMember(&response, ELEMENTS, &elements);
    free(elements.source);
    PutStringMember(&response, STATUS, status);
    WriteString(response.source);
    free(response.source);
    SendEot();
}


void ModifyTable(JsonNode* object, const char* name)
{
    int row, column;
    float value;
    bool enabled;
    Status fieldStatus = OK;
    Status enableStatus = OK;
    if (GetInt(object, ROW, &row) == JSON_OK)
    {
        if (GetInt(object, COLUMN, &column) == JSON_OK)
        {
            if (GetFloat(object, VALUE, &value) == JSON_OK)
            {
                fieldStatus = SetMeasurementTableField(name, column, row, value);
            }
        }
    }
    if (GetBoolean(object, ENABLED, &enabled) == JSON_OK)
    {
        enableStatus = SetMeasurementTableEnabled(name, enabled);
    }
    SendStatus(MODIFY, name, (fieldStatus != JSON_OK) ? fieldStatus : enableStatus);
}


void ModifyIgnitionTimer(const JsonNode* object)
{
    int value;
    TIMER_SETTINGS timerSettings;
    GetIgnitionTimerSettings(&timerSettings);
    if (GetInt(object, "Prescaler", &value) == JSON_OK)
    {
        timerSettings.prescaler = (uint16_t) value;
    }
    if (GetInt(object, "Period", &value) == JSON_OK)
    {
        timerSettings.period = (uint16_t) value;
    }
    if (GetInt(object, "Counter", &value) == JSON_OK)
    {
        timerSettings.counter = (uint16_t) value;
    }
    SetIgnitionTimerSettings(&timerSettings);
    SendStatus(MODIFY, IGNITION_TIMER, OK);
}


void ModifyCogwheel(JsonNode* object)
{
    int cogTotal, gapSize, offset;
    Status status = INVALID_MESSAGE;
    if (GetInt(object, COG_TOTAL, &cogTotal) == JSON_OK)
    {
        if (GetInt(object, GAP_SIZE, &gapSize) == JSON_OK)
        {
            if (GetInt(object, OFFSET, &offset) == JSON_OK)
            {
                status = SetGogwheelProperties(cogTotal, gapSize, offset);
            }
        }
    }
    SendStatus(MODIFY, COGWHEEL, status);
}


void ModifyCylinderCount(const JsonNode* object)
{
    int value;
    Status status = INVALID_MESSAGE;
    if (GetInt(object, VALUE, &value) == JSON_OK)
    {
        status = SetCylinderCount(value);
    }
    SendStatus(MODIFY, CYLINDER_COUNT, status);
}


void ModifyFlash(JsonNode* object)
{
    int reference, value, count;
    Status status = INVALID_MESSAGE;
    if (GetInt(object, REFERENCE, &reference) == JSON_OK)
    {
        if (GetInt(object, COUNT, &count) != JSON_OK)
        {
            count = 1;
        }
        if (count > 0)
        {
            if (GetInt(object, VALUE, &value) == JSON_OK)
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


Status HandleRequest(const JsonNode* object)
{
    char* subject;
    Status status = (AllocateString(object, SUBJECT, &subject) == JSON_OK) ? OK : INVALID_SUBJECT;
    if (status == OK)
    {
        if (strcmp(subject, FLASH_MEM) == 0)
        {
            SendFlashMemory(object);
        }
        else if (strcmp(subject, FLASH_ELEMENTS) == 0)
        {
            SendFlashElements();
        }
        else if (strcmp(subject, MEASUREMENTS) == 0)
        {
            SendMeasurementNames();
        }
        else if (strcmp(subject, MEASUREMENT_TABLES) == 0)
        {
            SendMeasurementTableNames();
        }
        else if (strcmp(subject, ENGINE) == 0)
        {
            SendEngineProperties();
        }
        else
        {
            RespondRequest(object, subject);
        }
    }
    free(subject);
    return status;
}


bool HandleModifySimulation(JsonNode* object, const char* subjectString)
{
    bool simulation;
    Status status = INVALID_MESSAGE;
    if (GetBoolean(object, SIMULATION, &simulation) == JSON_OK)
    {
        if (simulation)
        {
            float simulationValue;
            if (GetFloat(object, VALUE, &simulationValue) == JSON_OK)
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


Status HandleModification(JsonNode* object)
{
    char* subjectString;
    Status status = (AllocateString(object, SUBJECT, &subjectString) == JSON_OK) ? OK : INVALID_SUBJECT;
    if (status == OK)
    {
        bool handled = FALSE;
        Table table;
        if (FindTable(subjectString, &table) == OK)
        {
            ModifyTable(object, subjectString);
            handled = TRUE;
        }
        if (! handled)
        {
            handled = HandleModifySimulation(object, subjectString);
        }
        if (! handled)
        {
            if (strcmp(subjectString, COGWHEEL) == 0)
            {
                ModifyCogwheel(object);
            }
            else if (strcmp(subjectString, CYLINDER_COUNT) == 0)
            {
                ModifyCylinderCount(object);
            }
            else if (strcmp(subjectString, FLASH_MEM) == 0)
            {
                ModifyFlash(object);
            }
            else if (strcmp(subjectString, IGNITION_TIMER) == 0)
            {
                ModifyIgnitionTimer(object);
            }
        }
    }
    free(subjectString);
    return status;
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
    JsonNode object;
    char* message;
    Status status = OK;
    Initialize(jsonString, &object);
    if (AllocateString(&object, MESSAGE, &message) == JSON_OK)
    {
        if (strcmp(message, REQUEST) == 0)
        {
            status = HandleRequest(&object);
        }
        else if (strcmp(message, MODIFY) == 0)
        {
            status = HandleModification(&object);
        }
        else
        {
            SendErrorNotification("Unknown message");
        }
        if (status != OK)
        {
            SendErrorResponse(message, status);
        }
    }
    else
    {
        SendErrorNotification("Not a message");
    }
    free(message);
}


void SendTextNotification(const char* name, const char* value)
{
    JsonNode notification;
    ComposeObject(&notification);
    PutStringMember(&notification, MESSAGE, NOTIFICATION);
    PutStringMember(&notification, name, value);
    WriteString(notification.source);
    free(notification.source);
    SendEot();
}


void SendIntegerNotification(const char* name, int value)
{
    JsonNode notification;
    ComposeObject(&notification);
    PutStringMember(&notification, MESSAGE, NOTIFICATION);
    PutIntegerMember(&notification, name, value);
    WriteString(notification.source);
    free(notification.source);
    SendEot();
}


void SendRealNotification(const char* name, double value)
{
    JsonNode notification;
    ComposeObject(&notification);
    PutStringMember(&notification, MESSAGE, NOTIFICATION);
    PutRealMember(&notification, name, value);
    WriteString(notification.source);
    free(notification.source);
    SendEot();
}


void SendErrorNotification(const char* error)
{
     SendTextNotification(ERR, error);
}
