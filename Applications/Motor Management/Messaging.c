/*
** JSON messaging for Randd Motor Management system
** Copyright 2015, Bart Kampers
*/

#include "Messaging.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ApiStatus.h"

#include "Communication.h"
#include "JsonWriter.h"
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


Status SendErrorResponse(const char* message, const char* error)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonRootStart(DEFAULT_CHANNEL))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, RESPONSE, message))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, ERR, error))
    VALIDATE(WriteJsonObjectEnd(DEFAULT_CHANNEL))
    return FinishTransmission(DEFAULT_CHANNEL);
}


Status SendUnknownSubjectResponse(const char* message, const char* subject)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonRootStart(DEFAULT_CHANNEL))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, RESPONSE, message))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, SUBJECT, subject))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, ERR, "Unknown subject"))
    VALIDATE(WriteJsonObjectEnd(DEFAULT_CHANNEL))
    return FinishTransmission(DEFAULT_CHANNEL);
}


Status SendStatus(const char* message, const char* subject, const Status status)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonRootStart(DEFAULT_CHANNEL))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, RESPONSE, message))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, SUBJECT, subject))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, STATUS, status))
    VALIDATE(WriteJsonObjectEnd(DEFAULT_CHANNEL))
    return FinishTransmission(DEFAULT_CHANNEL);
}


Status SendMeasurementValue(const char* name, const Measurement* measurement)
{
    RETURN_WHEN_INVALID
    float value;
    Status status = GetMeasurementValue(measurement, &value);
    VALIDATE(WriteJsonRootStart(DEFAULT_CHANNEL))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, RESPONSE, REQUEST))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, SUBJECT, name))
    VALIDATE(WriteJsonRealMember(DEFAULT_CHANNEL, VALUE, value))
    VALIDATE(WriteJsonBooleanMember(DEFAULT_CHANNEL, SIMULATION, (measurement->simulationValue != NULL)))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, FORMAT, measurement->format))
    VALIDATE(WriteJsonRealMember(DEFAULT_CHANNEL, MINIMUM, measurement->minimum))
    VALIDATE(WriteJsonRealMember(DEFAULT_CHANNEL, MAXIMUM, measurement->maximum))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, STATUS, status))
    VALIDATE(WriteJsonObjectEnd(DEFAULT_CHANNEL))
    return FinishTransmission(DEFAULT_CHANNEL);
}


Status SendTableFields(const MeasurementTable* measurementTable)
{
    RETURN_WHEN_INVALID
    Status status = OK;
    byte c, r;
    if (measurementTable->columnMeasurement != NULL)
    {
        VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, "ColumnMeasurement", measurementTable->columnMeasurement->name))
    }
    if (measurementTable->rowMeasurement != NULL)
    {
        VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, "RowMeasurement", measurementTable->rowMeasurement->name))
    }
    VALIDATE(WriteJsonRealMember(DEFAULT_CHANNEL, MINIMUM, measurementTable->minimum))
    VALIDATE(WriteJsonRealMember(DEFAULT_CHANNEL, MAXIMUM, measurementTable->maximum))
    VALIDATE(WriteJsonIntegerMember(DEFAULT_CHANNEL, DECIMALS, measurementTable->decimals))
    VALIDATE(WriteJsonMemberName(DEFAULT_CHANNEL, TABLE))
    VALIDATE(WriteJsonArrayStart(DEFAULT_CHANNEL))
    for (r = 0; (r < measurementTable->table.rows) && (status == OK); ++r)
    {
        VALIDATE(WriteJsonArrayStart(DEFAULT_CHANNEL))
        for (c = 0; (c < measurementTable->table.columns) && (status == OK); ++c)
        {
            float field;
            status = GetMeasurementTableField(measurementTable, c, r, &field);
            VALIDATE(WriteJsonRealElement(DEFAULT_CHANNEL, field))
        }
        VALIDATE(WriteJsonArrayEnd(DEFAULT_CHANNEL))
    }
    VALIDATE(WriteJsonArrayEnd(DEFAULT_CHANNEL))
    return status;
}


bool HasString(JsonNode* array, const char* value)
{
    int index;
    return IndexOfString(array, value, &index) == JSON_OK;
}


Status RespondMeasurementTableRequest(const JsonNode* object, const MeasurementTable* measurementTable, const char* name)
{
    RETURN_WHEN_INVALID
    Status status = OK;
    JsonNode properties;
    bool sendTable, sendIndex, sendEnabled, sendDefault;
    if (GetArray(object, PROPERTIES, &properties) == JSON_OK)
    {
        sendTable = HasString(&properties, TABLE);
        sendIndex = HasString(&properties, INDEX);
        sendEnabled = HasString(&properties, ENABLED);
        sendDefault = ! sendTable && ! sendIndex && ! sendEnabled;
    }
    else
    {
        sendTable = TRUE;
        sendIndex = TRUE;
        sendEnabled = TRUE;
        sendDefault = TRUE;
    }
    VALIDATE(WriteJsonRootStart(DEFAULT_CHANNEL))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, RESPONSE, REQUEST))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, SUBJECT, measurementTable->name))
    if (sendTable || sendDefault)
    {
        status = SendTableFields(measurementTable);
    }
    if ((sendIndex || sendDefault) && (status == OK))
    {
        status = WriteJsonIntegerMember(DEFAULT_CHANNEL, COLUMN, measurementTable->columnIndex);
        if (status == OK)
        {
            status = WriteJsonIntegerMember(DEFAULT_CHANNEL, ROW, measurementTable->rowIndex);
        }
    }
    if ((sendEnabled || sendDefault) && (status == OK))
    {
        bool enabled;
        GetMeasurementTableEnabled(name, &enabled);
        status = WriteJsonBooleanMember(DEFAULT_CHANNEL, ENABLED, enabled);
    }
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, STATUS, status))
    VALIDATE(WriteJsonObjectEnd(DEFAULT_CHANNEL))
    return FinishTransmission(DEFAULT_CHANNEL);
}


Status RespondRequest(const JsonNode* object, const char* subjectString)
{
    RETURN_WHEN_INVALID
    bool responded = FALSE;
    Measurement* measurement;
    Status status = FindMeasurement(subjectString, &measurement);
    if (status == OK)
    {
        VALIDATE(SendMeasurementValue(subjectString, measurement))
        responded = TRUE;
    }
    if (! responded)
    {
        MeasurementTable* measurementTable;
        status = FindMeasurementTable(subjectString, &measurementTable);
        if (status == OK)
        {
            VALIDATE(RespondMeasurementTableRequest(object, measurementTable, subjectString))
            responded = TRUE;
        }
    }
    if (! responded)
    {
        char* messageString;
        AllocateString(object, MESSAGE, &messageString);
        VALIDATE(SendUnknownSubjectResponse(messageString, subjectString))
        free(messageString);
    }
    return OK;
}


Status SendMeasurementNames()
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonRootStart(DEFAULT_CHANNEL))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, SUBJECT, MEASUREMENTS))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, RESPONSE, REQUEST))
    VALIDATE(WriteJsonMemberName(DEFAULT_CHANNEL, NAMES))
    VALIDATE(WriteJsonArrayStart(DEFAULT_CHANNEL))
    VALIDATE(WriteJsonStringElement(DEFAULT_CHANNEL, RPM))
    VALIDATE(WriteJsonStringElement(DEFAULT_CHANNEL, LOAD))
    VALIDATE(WriteJsonStringElement(DEFAULT_CHANNEL, WATER_TEMPERATURE))
    VALIDATE(WriteJsonStringElement(DEFAULT_CHANNEL, AIR_TEMPERATURE))
    VALIDATE(WriteJsonStringElement(DEFAULT_CHANNEL, BATTERY_VOLTAGE))
    VALIDATE(WriteJsonStringElement(DEFAULT_CHANNEL, MAP_SENSOR))
    VALIDATE(WriteJsonArrayEnd(DEFAULT_CHANNEL))
    VALIDATE(WriteJsonObjectEnd(DEFAULT_CHANNEL))
    return FinishTransmission(DEFAULT_CHANNEL);
}


Status SendMeasurementTableNames()
{
    RETURN_WHEN_INVALID
    int i;
    int count = GetMeasurementTableCount();
    VALIDATE(WriteJsonRootStart(DEFAULT_CHANNEL))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, RESPONSE, REQUEST))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, SUBJECT, MEASUREMENT_TABLES))
    VALIDATE(WriteJsonMemberName(DEFAULT_CHANNEL, NAMES))
    VALIDATE(WriteJsonArrayStart(DEFAULT_CHANNEL))
    for (i = 0; i < count; ++i)
    {
        VALIDATE(WriteJsonStringElement(DEFAULT_CHANNEL, GetMeasurementTableName(i)))
    }
    VALIDATE(WriteJsonArrayEnd(DEFAULT_CHANNEL))
    VALIDATE(WriteJsonObjectEnd(DEFAULT_CHANNEL))
    return FinishTransmission(DEFAULT_CHANNEL);
}


Status SendEngineProperties()
{
    RETURN_WHEN_INVALID
    int i;
    VALIDATE(WriteJsonRootStart(DEFAULT_CHANNEL))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, RESPONSE, REQUEST))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, SUBJECT, ENGINE))
    VALIDATE(WriteJsonIntegerMember(DEFAULT_CHANNEL, CYLINDER_COUNT, GetCylinderCount()))
    VALIDATE(WriteJsonMemberName(DEFAULT_CHANNEL, COGWHEEL))
    VALIDATE(WriteJsonObjectStart(DEFAULT_CHANNEL))
    VALIDATE(WriteJsonIntegerMember(DEFAULT_CHANNEL, COG_TOTAL, GetCogTotal()))
    VALIDATE(WriteJsonIntegerMember(DEFAULT_CHANNEL,  GAP_SIZE, GetGapSize()))
    VALIDATE(WriteJsonIntegerMember(DEFAULT_CHANNEL,  OFFSET, GetDeadPointOffset()))
    VALIDATE(WriteJsonObjectEnd(DEFAULT_CHANNEL))
    VALIDATE(WriteJsonMemberName(DEFAULT_CHANNEL, DEAD_POINTS))
    VALIDATE(WriteJsonArrayStart(DEFAULT_CHANNEL))
    for (i = 0; i < GetDeadPointCount(); ++i)
    {
        VALIDATE(WriteJsonIntegerElement(DEFAULT_CHANNEL, GetDeadPointCog(i)))
    }
    VALIDATE(WriteJsonArrayEnd(DEFAULT_CHANNEL))
    VALIDATE(WriteJsonObjectEnd(DEFAULT_CHANNEL))
    return FinishTransmission(DEFAULT_CHANNEL);
}


Status SendFlashMemory(const JsonNode* object)
{
    RETURN_WHEN_INVALID
    Status status = OK;
    int reference;
    int count;
    int i;
    VALIDATE(WriteJsonRootStart(DEFAULT_CHANNEL))
    if (GetInt(object, REFERENCE, &reference) != JSON_OK)
    {
        reference = 0;
    }
    if (GetInt(object, COUNT, &count) != JSON_OK)
    {
        count = (int) PersistentMemoryLimit();
    }
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, RESPONSE, REQUEST))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, SUBJECT, FLASH_MEM))
    VALIDATE(WriteJsonIntegerMember(DEFAULT_CHANNEL, REFERENCE, reference))
    VALIDATE(WriteJsonMemberName(DEFAULT_CHANNEL, VALUE));
    VALIDATE(WriteJsonArrayStart(DEFAULT_CHANNEL))
    for (i = 0; (i < count) && (status == OK); ++i)
    {
        byte buffer;
        status = ReadPersistentMemory(reference + i, 1, &buffer);
        VALIDATE(WriteJsonIntegerElement(DEFAULT_CHANNEL, ((int) buffer) & 0xFF))
    }
    VALIDATE(WriteJsonArrayEnd(DEFAULT_CHANNEL))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, STATUS, status))
    VALIDATE(WriteJsonObjectEnd(DEFAULT_CHANNEL))
    return FinishTransmission(DEFAULT_CHANNEL);
}


Status SendFlashElements()
{
    RETURN_WHEN_INVALID
    Status status = OK;
    int typeId;
    VALIDATE(WriteJsonRootStart(DEFAULT_CHANNEL))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, RESPONSE, REQUEST))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, SUBJECT, FLASH_ELEMENTS))
    VALIDATE(WriteJsonMemberName(DEFAULT_CHANNEL, ELEMENTS))
    VALIDATE(WriteJsonArrayStart(DEFAULT_CHANNEL))
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
                VALIDATE(WriteJsonObjectStart(DEFAULT_CHANNEL))
                VALIDATE(WriteJsonIntegerMember(DEFAULT_CHANNEL, TYPE_ID, typeId))
                VALIDATE(WriteJsonIntegerMember(DEFAULT_CHANNEL, REFERENCE, reference))
                VALIDATE(WriteJsonIntegerMember(DEFAULT_CHANNEL, SIZE, size))
                VALIDATE(WriteJsonObjectEnd(DEFAULT_CHANNEL))
            }
            status = FindNext(typeId, &reference);
        }
    }
    VALIDATE(WriteJsonArrayEnd(DEFAULT_CHANNEL))
    if (status == INVALID_ID)
    {
        /* first or next not found, not an error */
        status = OK;
    }
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, STATUS, status))
    VALIDATE(WriteJsonObjectEnd(DEFAULT_CHANNEL))
    return FinishTransmission(DEFAULT_CHANNEL);
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
    SendStatus(MODIFY, name, (fieldStatus != OK) ? fieldStatus : enableStatus);
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
    int reference;
    Status status = INVALID_MESSAGE;
    if (GetInt(object, REFERENCE, &reference) == JSON_OK)
    {
        JsonNode values;
        int count, value;
        if ((GetArray(object, VALUE, &values) == JSON_OK) && (GetCount(&values, &count) == JSON_OK))
        {
            byte* buffer = malloc(count * sizeof(byte));
            int index;
            for (index = 0; index < count; ++index)
            {
                if (GetIntAt(&values, index, &value) == JSON_OK)
                {
                    buffer[index] = (byte) value;
                }
            }
            status = WritePersistentMemory(reference, count, buffer);
            free(buffer);
        }
        else if (GetInt(object, COUNT, &count) != JSON_OK)
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


Status SendTextNotification(const char* name, const char* value)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonRootStart(DEFAULT_CHANNEL))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, MESSAGE, NOTIFICATION))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, name, value))
    VALIDATE(WriteJsonObjectEnd(DEFAULT_CHANNEL))
    return FinishTransmission(DEFAULT_CHANNEL);
}


Status SendIntegerNotification(const char* name, int value)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonRootStart(DEFAULT_CHANNEL))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, MESSAGE, NOTIFICATION))
    VALIDATE(WriteJsonIntegerMember(DEFAULT_CHANNEL, name, value))
    VALIDATE(WriteJsonObjectEnd(DEFAULT_CHANNEL))
    return FinishTransmission(DEFAULT_CHANNEL);
}


Status SendRealNotification(const char* name, double value)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonRootStart(DEFAULT_CHANNEL))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, MESSAGE, NOTIFICATION))
    VALIDATE(WriteJsonRealMember(DEFAULT_CHANNEL, name, value))
    VALIDATE(WriteJsonObjectEnd(DEFAULT_CHANNEL))
    return FinishTransmission(DEFAULT_CHANNEL);
}


Status SendErrorNotification(const char* error)
{
     return SendTextNotification(ERR, error);
}
