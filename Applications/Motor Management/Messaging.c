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

const char* ERROR = "Error";
const char* STATUS = "Status";


Status PutMeasurementAttribute(Measurement* measurement, char* attributeName)
{
    if (strcmp(attributeName, VALUE) == 0)
    {
        float value;
        Status status = GetMeasurementValue(measurement, &value);
        if (status == OK)
        {
            return WriteJsonRealMember(DEFAULT_CHANNEL, VALUE, value);
        }
        else
        {
            return WriteJsonStringMember(DEFAULT_CHANNEL, VALUE, status);
        }
    }
    else if (strcmp(attributeName, SIMULATION) == 0)
    {
        return WriteJsonBooleanMember(DEFAULT_CHANNEL, SIMULATION, measurement->simulationValue != NULL);
    }
    else
    {
        return WriteJsonStringMember(DEFAULT_CHANNEL, attributeName, "NoSuchAttribute");
    }
}


Status PutAttributes(Measurement* measurement, JsonNode* attributes)
{
    int attributeCount;
    Status status = OK;
    if (GetCount(attributes, &attributeCount) == JSON_OK)
    {
        int i;
        for (i = 0; (i < attributeCount) && (status == OK); ++i)
        {
            char* attributeName;
            if (AllocateStringAt(attributes, i, &attributeName) == JSON_OK)
            {
                status = PutMeasurementAttribute(measurement, attributeName);
                free(attributeName);
            }
        }
    }
    return status;
}


Status PutMeasurement(JsonNode* message, Measurement* measurement)
{
    Status status = WriteJsonMemberName(DEFAULT_CHANNEL, measurement->name);
    if (status == OK)
    {
        status = WriteJsonObjectStart(DEFAULT_CHANNEL);
        if (status == OK)
        {
            JsonNode attributes;
            if (GetArray(message, ATTRIBUTES, &attributes) == JSON_OK)
            {
                status = PutAttributes(measurement, &attributes);
            }
        }
        if (status == OK)
        {
            status = WriteJsonObjectEnd(DEFAULT_CHANNEL);
        }
    }
    return status;
}


Status PutMeasurements(JsonNode* message)
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
    return status;
}


Status HandleMeasurementRequest(JsonNode* message)
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


Status HandleMeasurementTableRequest(JsonNode* message)
{
    return NOT_IMPLEMENTED;
}


Status HandleEngineRequest(JsonNode* message)
{
    return NOT_IMPLEMENTED;
}


Status HandleMemoryRequest(JsonNode* message)
{
    return NOT_IMPLEMENTED;
}


Status HandleRequest(JsonNode* message)
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


Status HandleModify(JsonNode* message)
{
    Status status = WriteJsonStringMember(DEFAULT_CHANNEL, PROCEDURE, MODIFY);
    if (status == OK)
    {
        
    }
    return status;
}


Status HandleCall(JsonNode* message) 
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
                status = WriteJsonStringMember(DEFAULT_CHANNEL, ERROR, "InvalidDireciotn");
            }
        }
        else
        {
            status = WriteJsonStringMember(DEFAULT_CHANNEL, DIRECTION, NOTIFICATION);
            if (status == OK)
            {   
                WriteJsonStringMember(DEFAULT_CHANNEL, ERROR, "InvalidDireciotn");
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
