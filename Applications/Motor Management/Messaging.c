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

/*
** Private
*/

const char* DIRECTION = "Direction";
const char* CALL = "Call";
const char* RETURN = "Return";
const char* NOTIFICATION = "Notification";

const char* PROCEDURE = "Procedure";
const char* REQUEST = "Request";
const char* MODIFY = "Modify";

const char* DATA_TYPE = "DataType";
const char* MEASUREMENT = "Measurement";
const char* MEASUREMENT_TABLE = "MeasurementTable";
const char* ENGINE = "Engine";
const char* MEMORY = "Memory";

const char* INSTANCES = "Instances";
const char* ATTRIBUTES = "Attributes";
const char* VALUES = "Values";

const char* VALUE = "Value";
const char* SIMULATION = "Simulation";
const char* CURRENT_ROW = "CurrentRow";
const char* CURRENT_COLUMN = "CurrentColumn";
const char* TABLE = "Table";

const char* ERROR = "Error";
const char* STATUS = "Status";


bool AttributeRequested(const JsonNode* attributes, const char* attributeName)
{
    int index;
    return (attributes == NULL) || (IndexOfString(attributes, attributeName, &index) == JSON_OK);
}


Status PutMeasurementValue(const Measurement* measurement)
{
    float value;
    Status status = GetMeasurementValue(measurement, &value);
    if (status == OK)
    {
        status = WriteJsonRealMember(DEFAULT_CHANNEL, VALUE, value);
    }
    else
    {
        status =  WriteJsonStringMember(DEFAULT_CHANNEL, VALUE, status);
    }
    return status;
}


Status PutMeasurementAttributes(const Measurement* measurement, const JsonNode* attributes)
{
    Status status = OK;
    if (AttributeRequested(attributes, VALUE))
    {
       status = PutMeasurementValue(measurement); 
    }
    if (AttributeRequested(attributes, SIMULATION))
    {
       status = WriteJsonBooleanMember(DEFAULT_CHANNEL, SIMULATION, measurement->simulationValue != NULL);
    }
    return status;
}


Status PutMeasurement(const JsonNode* message, const Measurement* measurement)
{
    Status status = WriteJsonMemberName(DEFAULT_CHANNEL, measurement->name);
    if (status == OK)
    {
        status = WriteJsonObjectStart(DEFAULT_CHANNEL);
        if (status == OK)
        {
            JsonNode attributes;
            JsonStatus jsonStatus = GetArray(message, ATTRIBUTES, &attributes);
            if (jsonStatus == JSON_OK)
            {
                status = PutMeasurementAttributes(measurement, &attributes);
            }
            else if (jsonStatus == JSON_NAME_NOT_PRESENT)
            {
                status = PutMeasurementAttributes(measurement, NULL);
            }
        }
        if (status == OK)
        {
            status = WriteJsonObjectEnd(DEFAULT_CHANNEL);
        }
    }
    return status;
}


Status PutMeasurements(const JsonNode* message)
{
    JsonNode instances;
    Status status = OK;
    JsonStatus jsonStatus = GetArray(message, INSTANCES, &instances);
    if (jsonStatus == JSON_OK)
    {
        int instanceCount;
        jsonStatus = GetCount(&instances, &instanceCount);
        if (jsonStatus == JSON_OK)
        {
            int i;
            for (i = 0; (i < instanceCount) && (status == OK); ++i)
            {
                char* measurementName;
                jsonStatus = AllocateStringAt(&instances, i, &measurementName);
                if (jsonStatus == JSON_OK)
                {
                    Measurement* measurement;
                    if (FindMeasurement(measurementName, &measurement) == OK)
                    {
                        status = PutMeasurement(message, measurement);
                    }
                    free(measurementName);
                }
            }
        }
    }
    else if (jsonStatus == JSON_NAME_NOT_PRESENT)
    {
        int i;
        int count = GetMeasurementCount();
        for (i = 0; (i < count) && (status == OK); ++i)
        {
            Measurement* measurement;
            if (GetMeasurement(i, &measurement) == OK)
            {
                status = PutMeasurement(message, measurement);
            }
        }
    }
    return status;
}


Status HandleMeasurementRequest(const JsonNode* message)
{
    Status status = WriteJsonMemberName(DEFAULT_CHANNEL, VALUES);
    if (status == OK)
    {
        status = WriteJsonObjectStart(DEFAULT_CHANNEL);
        if (status == OK)
        {
            status = PutMeasurements(message);
            if (status == OK)
            {
                status = WriteJsonObjectEnd(DEFAULT_CHANNEL);
            }
        }
    }
    return status;
}


Status PutMeasurmentTableFields(const MeasurementTable* measurementTable) 
{
    Status error = UNINITIALIZED;
    Status status = WriteJsonMemberName(DEFAULT_CHANNEL, TABLE);
    if (status == OK)
    {
        byte column, row;
        status = WriteJsonArrayStart(DEFAULT_CHANNEL);
        for (row = 0; (row < measurementTable->table.rows) && (status == OK); ++row)
        {
            status = WriteJsonArrayStart(DEFAULT_CHANNEL);
            for (column = 0; (column < measurementTable->table.columns) && (status == OK); ++column)
            {
                float value;
                Status valueStatus = GetMeasurementTableField(measurementTable, column, row, &value);
                if (valueStatus == OK)
                {
                    status = WriteJsonRealElement(DEFAULT_CHANNEL, value);
                }
                else 
                {
                    error = valueStatus;
                    status = WriteJsonNullElement(DEFAULT_CHANNEL);
                }
            }
            if (status == OK)
            {
                status = WriteJsonArrayEnd(DEFAULT_CHANNEL);
            }
        }
        if (status == OK)
        {
            status = WriteJsonArrayEnd(DEFAULT_CHANNEL);
        }
    }
    if (error != UNINITIALIZED)
    {
        status = WriteJsonStringMember(DEFAULT_CHANNEL, ERROR, error);
    }
    return status; 
}


Status PutMeasurementTableAttributes(const MeasurementTable* measurementTable, const JsonNode* attributes)
{
    Status status = OK;
    if (AttributeRequested(attributes, CURRENT_ROW))
    {
        status = WriteJsonIntegerMember(DEFAULT_CHANNEL, CURRENT_ROW, measurementTable->rowIndex);
    }
    if (AttributeRequested(attributes, CURRENT_COLUMN))
    {
        status = WriteJsonIntegerMember(DEFAULT_CHANNEL, CURRENT_COLUMN, measurementTable->columnIndex);
    }
    if (AttributeRequested(attributes, TABLE))
    {
        status = PutMeasurmentTableFields(measurementTable);
    }
    return status;
}


Status PutMeasurementTable(const JsonNode* message, const MeasurementTable* measurementTable)
{
    Status status = WriteJsonMemberName(DEFAULT_CHANNEL, measurementTable->name);
    if (status == OK)
    {
        status = WriteJsonObjectStart(DEFAULT_CHANNEL);
        if (status == OK)
        {
            JsonNode attributes;
            JsonStatus jsonStatus = GetArray(message, ATTRIBUTES, &attributes);
            if (jsonStatus == JSON_OK)
            {
                status = PutMeasurementTableAttributes(measurementTable, &attributes);
            }
            else if (jsonStatus == JSON_NAME_NOT_PRESENT)
            {
                status = PutMeasurementTableAttributes(measurementTable, NULL);
            }
        }
    }
    if (status == OK)
    {
        status = WriteJsonObjectEnd(DEFAULT_CHANNEL);
    }
    return status;
}


Status PutMeasurementTables(const JsonNode* message)
{
    JsonNode instances;
    Status status = OK;
    JsonStatus jsonStatus = GetArray(message, INSTANCES, &instances);
    if (jsonStatus == JSON_OK)
    {
        int instanceCount;
        jsonStatus = GetCount(&instances, &instanceCount);
        if (jsonStatus == JSON_OK)
        {
            int i;
            for (i = 0; (i < instanceCount) && (status == OK); ++i)
            {
                char* tableName;
                jsonStatus = AllocateStringAt(&instances, i, &tableName);
                if (jsonStatus == JSON_OK)
                {
                    MeasurementTable* measurementTable;
                    if (FindMeasurementTable(tableName, &measurementTable) == OK)
                    {
                        status = PutMeasurementTable(message, measurementTable);
                    }
                    free(tableName);
                }
            }
        }
    }
    else if (jsonStatus == JSON_NAME_NOT_PRESENT)
    {
        int i;
        int count = GetMeasurementTableCount();
        for (i = 0; (i < count) && (status == OK); ++i)
        {
            MeasurementTable* measurementTable;
            if (GetMeasurementTable(i, &measurementTable) == OK)
            {
                status = PutMeasurementTable(message, measurementTable);
            }
        }
    }
    return status;
}


Status HandleMeasurementTableRequest(const JsonNode* message)
{
    Status status = WriteJsonMemberName(DEFAULT_CHANNEL, VALUES);
    if (status == OK)
    {
        status = WriteJsonObjectStart(DEFAULT_CHANNEL);
        if (status == OK)
        {
            status = PutMeasurementTables(message);
            if (status == OK)
            {
                status = WriteJsonObjectEnd(DEFAULT_CHANNEL);
            }
        }
    }
    return status;
}


Status HandleEngineRequest(const JsonNode* message)
{
    return NOT_IMPLEMENTED;
}


Status HandleMemoryRequest(const JsonNode* message)
{
    return NOT_IMPLEMENTED;
}


Status HandleRequest(const JsonNode* message)
{
    char* dataType;
    Status status = WriteJsonStringMember(DEFAULT_CHANNEL, PROCEDURE, REQUEST);
    if (status == OK)
    {
        if (AllocateString(message, DATA_TYPE, &dataType) == JSON_OK)
        {
            status = WriteJsonStringMember(DEFAULT_CHANNEL, DATA_TYPE, dataType);
            if (status == OK)
            {
                if (strcmp(dataType, MEASUREMENT) == 0)
                {
                    status = HandleMeasurementRequest(message);
                }
                else if (strcmp(dataType, MEASUREMENT_TABLE) == 0)
                {
                    status = HandleMeasurementTableRequest(message);
                }
                else if (strcmp(dataType, ENGINE) == 0)
                {
                    status = HandleEngineRequest(message);
                }
                else if (strcmp(dataType, MEMORY) == 0)
                {
                    status = HandleMemoryRequest(message);
                }
                else
                {
                    status = WriteJsonStringMember(DEFAULT_CHANNEL, "Error", "InvalidDataType");
                }
            }
            free(dataType);
        }
        else
        {
            status = WriteJsonStringMember(DEFAULT_CHANNEL, "Error", "INoDataType");
        }
    }
    return status;
}


Status HandleModify(const JsonNode* message)
{
    Status status = WriteJsonStringMember(DEFAULT_CHANNEL, PROCEDURE, MODIFY);
    if (status == OK)
    {
        
    }
    return status;
}


Status HandleCall(const JsonNode* message) 
{
    Status status;
    char* procedure;
    if (AllocateString(message, PROCEDURE, &procedure) == JSON_OK)
    {
        if (strcmp(procedure, REQUEST) == 0)
        {
            status = HandleRequest(message);
        }
        else if (strcmp(procedure, MODIFY) == 0)
        {
            status = HandleModify(message);
        }
        else
        {
            status = WriteJsonStringMember(DEFAULT_CHANNEL, "Error", "InvalidProcedure");
        }
        free(procedure);
    }
    else
    {
        status = WriteJsonStringMember(DEFAULT_CHANNEL, "Error", "NoProcedure");
    }
    return status;
}


/*
** Interface
*/

void HandleMessage(const char* jsonString)
{
    JsonNode message;
    char* direction;
    Status status = WriteJsonRootStart(DEFAULT_CHANNEL);
    if (status == OK)
    {
        Initialize(jsonString, &message);
        if (AllocateString(&message, DIRECTION, &direction) == JSON_OK)
        {
            if (strcmp(direction, CALL) == 0)
            {
                status = WriteJsonStringMember(DEFAULT_CHANNEL, DIRECTION, RETURN);
                if (status == OK) 
                {
                    status = HandleCall(&message);
                }
            }
            else
            {
                status = WriteJsonStringMember(DEFAULT_CHANNEL, ERROR, "InvalidDirection");
            }
        }
        else
        {
            status = WriteJsonStringMember(DEFAULT_CHANNEL, DIRECTION, NOTIFICATION);
            if (status == OK)
            {   
                WriteJsonStringMember(DEFAULT_CHANNEL, ERROR, "InvalidDirection");
            }
        }
        free(direction);
        status = WriteJsonRootEnd(DEFAULT_CHANNEL);
    }
}


Status SendTextNotification(const char* name, const char* value)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonRootStart(DEFAULT_CHANNEL))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, DIRECTION, NOTIFICATION))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, name, value))
    return WriteJsonRootEnd(DEFAULT_CHANNEL);
}


Status SendIntegerNotification(const char* name, int value)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonRootStart(DEFAULT_CHANNEL))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, DIRECTION, NOTIFICATION))
    VALIDATE(WriteJsonIntegerMember(DEFAULT_CHANNEL, name, value))
    return WriteJsonRootEnd(DEFAULT_CHANNEL);
}


Status SendRealNotification(const char* name, double value)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonRootStart(DEFAULT_CHANNEL))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, DIRECTION, NOTIFICATION))
    VALIDATE(WriteJsonRealMember(DEFAULT_CHANNEL, name, value))
    return WriteJsonRootEnd(DEFAULT_CHANNEL);
}


Status SendErrorNotification(const char* error)
{
     return SendTextNotification(ERROR, error);
}
