/*
** JSON messaging for Randd Motor Management system
** Author: Bart Kampers
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

const char* FUNCTION = "Function";
const char* PARAMETERS = "Parameters";
const char* RETURN_VALUE = "ReturnValue";
const char* STATUS = "Status";

const char* MEASUREMENT_NAME = "MeasurmentName";
const char* TABLE_NAME = "TableName";
const char* VALUE = "Value";
const char* SIMULATION = "Simulation";
const char* COLUMN = "Column";
const char* ROW = "Row";
const char* CURRENT_COLUMN = "CurrentColumn";
const char* CURRENT_ROW = "CurrentRow";
const char* ENABLED = "Enabled";
const char* MINIMUM = "Minimum";
const char* MAXIMUM = "Maximum";
const char* SIMULATION_VALUE = "SimulationValue";
const char* FORMAT = "Format";
const char* COGWHEEL = "Cogwheel";
const char* CYLINDER_COUNT = "CylinderCount";
const char* DEAD_POINTS = "DeadPoints";
const char* COG_TOTAL = "CogTotal";
const char* GAP_SIZE = "GapSize";
const char* OFFSET = "Offset";

const char* TYPE_ID = "TypeId";
const char* REFERENCE = "Reference";
const char* SIZE = "Size";

const char* FIRE = "Fire";
const char* ERROR = "Error";


typedef struct
{
    const char* name;
    Status (*Call) (const JsonNode* parameters, Status* status);
} Function;


Status PutMeasurementTableProperties(const char* tableName, Status* status)
{
    RETURN_WHEN_INVALID
    MeasurementTable* measurementTable;
    VALIDATE(WriteJsonMemberName(DEFAULT_CHANNEL, RETURN_VALUE));
    VALIDATE(WriteJsonObjectStart(DEFAULT_CHANNEL));
    *status = FindMeasurementTable(tableName, &measurementTable);
    if (*status == OK)
    {
        bool enabled;
        *status = GetMeasurementTableEnabled(measurementTable->name, &enabled);
        if (*status == OK)
        {
            VALIDATE(WriteJsonBooleanMember(DEFAULT_CHANNEL, ENABLED, enabled));
        }
        VALIDATE(WriteJsonIntegerMember(DEFAULT_CHANNEL, CURRENT_ROW, measurementTable->rowIndex));
        VALIDATE(WriteJsonIntegerMember(DEFAULT_CHANNEL, CURRENT_COLUMN, measurementTable->columnIndex));
    }
    return (WriteJsonObjectEnd(DEFAULT_CHANNEL));    
}


Status PutMeasurementProperties(const char* measurmentName, Status* status)
{
    RETURN_WHEN_INVALID
    Measurement* measurement;
    VALIDATE(WriteJsonMemberName(DEFAULT_CHANNEL, RETURN_VALUE));
    VALIDATE(WriteJsonObjectStart(DEFAULT_CHANNEL));
    *status = FindMeasurement(measurmentName, &measurement);
    if (*status == OK)
    {
        VALIDATE(WriteJsonRealMember(DEFAULT_CHANNEL, MINIMUM, measurement->minimum));
        VALIDATE(WriteJsonRealMember(DEFAULT_CHANNEL, MAXIMUM, measurement->maximum));
        VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, FORMAT, measurement->format));
        if (measurement->simulationValue == NULL)
        {
            VALIDATE(WriteJsonNullMember(DEFAULT_CHANNEL, SIMULATION_VALUE));
        }
        else
        {
            VALIDATE(WriteJsonRealMember(DEFAULT_CHANNEL, SIMULATION_VALUE, *(measurement->simulationValue)));
        }
    }
    return (WriteJsonObjectEnd(DEFAULT_CHANNEL));    
}


Status PutMeasurementTableFields(const char* tableName, Status* status)
{
    RETURN_WHEN_INVALID
    MeasurementTable* measurementTable;
    VALIDATE(WriteJsonMemberName(DEFAULT_CHANNEL, RETURN_VALUE));
    VALIDATE(WriteJsonArrayStart(DEFAULT_CHANNEL));
    *status = FindMeasurementTable(tableName, &measurementTable);
    if (*status == OK)
    {
        byte column, row;
        for (row = 0; row < measurementTable->table.rows; ++row)
        {
            VALIDATE(WriteJsonArrayStart(DEFAULT_CHANNEL));
            for (column = 0; column < measurementTable->table.columns; ++column)
            {
                float value;
                *status = GetMeasurementTableField(measurementTable, column, row, &value);
                if (*status == OK)
                {
                    VALIDATE(WriteJsonRealElement(DEFAULT_CHANNEL, value));
                }
                else 
                {
                    VALIDATE(WriteJsonNullElement(DEFAULT_CHANNEL));
                }
            }
            VALIDATE(WriteJsonArrayEnd(DEFAULT_CHANNEL));
        }
    }
    return (WriteJsonArrayEnd(DEFAULT_CHANNEL));    
}


Status PutDeadPoints()
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonMemberName(DEFAULT_CHANNEL, DEAD_POINTS));
    VALIDATE(WriteJsonArrayStart(DEFAULT_CHANNEL));
    int i;
    for (i = 0; i < GetDeadPointCount(); ++i)
    {
        VALIDATE(WriteJsonIntegerElement(DEFAULT_CHANNEL, GetDeadPointCog(i)));
    }
    return WriteJsonArrayEnd(DEFAULT_CHANNEL);
}


Status PutCogwheel()
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonMemberName(DEFAULT_CHANNEL, COGWHEEL));
    VALIDATE(WriteJsonObjectStart(DEFAULT_CHANNEL));
    VALIDATE(WriteJsonIntegerMember(DEFAULT_CHANNEL, COG_TOTAL, GetCogTotal()));
    VALIDATE(WriteJsonIntegerMember(DEFAULT_CHANNEL, GAP_SIZE, GetGapSize()));
    VALIDATE(WriteJsonIntegerMember(DEFAULT_CHANNEL, OFFSET, GetDeadPointOffset()));
    return WriteJsonObjectEnd(DEFAULT_CHANNEL);
}


Status PutElement(TypeId typeId, Reference reference, ElementSize size)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonObjectStart(DEFAULT_CHANNEL));
    VALIDATE(WriteJsonIntegerMember(DEFAULT_CHANNEL, TYPE_ID, typeId));
    VALIDATE(WriteJsonIntegerMember(DEFAULT_CHANNEL, REFERENCE, reference));
    VALIDATE(WriteJsonIntegerMember(DEFAULT_CHANNEL, SIZE, size));
    return WriteJsonObjectEnd(DEFAULT_CHANNEL);
}


Status PutElementTypes(TypeId typeId, Status* status)
{
    Status transportStatus = OK;
    Reference reference;
    Status memoryStatus = FindFirst(typeId, &reference);
    while ((memoryStatus == OK) && (transportStatus == OK))
    {
        ElementSize size;
        memoryStatus = GetSize(reference, &size);
        if (memoryStatus == OK)
        {
            transportStatus = PutElement(typeId, reference, size);
            memoryStatus = FindNext(typeId, &reference);
        }
        else {
            *status = memoryStatus;
        }
    }
    return transportStatus;
}


Status PutPersistentElements(Status* status)
{
    RETURN_WHEN_INVALID
    int i;
    int count = GetTypeCount();
    VALIDATE(WriteJsonMemberName(DEFAULT_CHANNEL, RETURN_VALUE));
    VALIDATE(WriteJsonArrayStart(DEFAULT_CHANNEL));
    for (i = 0; i < count; ++i)
    {
        VALIDATE( PutElementTypes((TypeId) i, status));
    }
    return WriteJsonArrayEnd(DEFAULT_CHANNEL);
}


Status CallGetMeasurements(const JsonNode* parameters, Status* status)
{
    RETURN_WHEN_INVALID
    int i;
    int count = GetMeasurementCount();
    VALIDATE(WriteJsonMemberName(DEFAULT_CHANNEL, RETURN_VALUE));
    VALIDATE(WriteJsonObjectStart(DEFAULT_CHANNEL));
    for (i = 0; i < count; ++i)
    {
        Measurement* measurement;
        if (GetMeasurement(i, &measurement) == OK)
        {
            float value;
            VALIDATE(WriteJsonMemberName(DEFAULT_CHANNEL, measurement->name));
            VALIDATE(WriteJsonObjectStart(DEFAULT_CHANNEL));
            *status = GetMeasurementValue(measurement, &value);
            if (*status == OK)
            {
                VALIDATE(WriteJsonRealMember(DEFAULT_CHANNEL, VALUE, value));
            }
            else
            {
                VALIDATE(WriteJsonNullMember(DEFAULT_CHANNEL, VALUE));
            }
            VALIDATE(WriteJsonBooleanMember(DEFAULT_CHANNEL, SIMULATION, measurement->simulationValue != NULL));
            VALIDATE(WriteJsonObjectEnd(DEFAULT_CHANNEL));
        }
    }
    return WriteJsonObjectEnd(DEFAULT_CHANNEL);
}


Status CallGetMeasurementProperties(const JsonNode* parameters, Status* status)
{
    Status transportStatus = OK;
    char* measurementName;
    if (AllocateString(parameters, MEASUREMENT_NAME, &measurementName) == JSON_OK)
    {
        transportStatus = PutMeasurementProperties(measurementName, status);
        free(measurementName);
    }
    else
    {
        *status = INVALID_PARAMETER;
    }
    return transportStatus;
}


Status CallSetMeasurementSimulation(const JsonNode* parameters, Status* status)
{
    char* measurementName;
    float value;
    if ((GetFloat(parameters, SIMULATION_VALUE, &value) == JSON_OK) && (AllocateString(parameters, MEASUREMENT_NAME, &measurementName) == JSON_OK))
    {
        *status = SetMeasurementSimulation(measurementName, value);
        free(measurementName);
    }
    else
    {
        *status = INVALID_PARAMETER;
    }
    return OK;
}


Status CallResetMeasurementSimulation(const JsonNode* parameters, Status* status)
{
    char* measurementName;
    if (AllocateString(parameters, MEASUREMENT_NAME, &measurementName) == JSON_OK)
    {
        *status = ResetMeasurementSimulation(measurementName);
        free(measurementName);
    }
    else
    {
        *status = INVALID_PARAMETER;
    }
    return OK;
}


Status CallGetTableNames(const JsonNode* parameters, Status* status)
{
    RETURN_WHEN_INVALID
    int count = GetMeasurementTableCount();
    int i;
    VALIDATE(WriteJsonMemberName(DEFAULT_CHANNEL, RETURN_VALUE));
    VALIDATE(WriteJsonArrayStart(DEFAULT_CHANNEL));
    for (i = 0; i < count; ++i)
    {
        MeasurementTable* measurementTable;
        if (GetMeasurementTable(i, &measurementTable) == OK)
        {
            VALIDATE(WriteJsonStringElement(DEFAULT_CHANNEL, measurementTable->name));
        }
    }
    return WriteJsonArrayEnd(DEFAULT_CHANNEL);
}


Status CallGetTableProperties(const JsonNode* parameters, Status* status)
{
    Status transportStatus = OK;
    char* tableName;
    if (AllocateString(parameters, TABLE_NAME, &tableName) == JSON_OK)
    {
        transportStatus = PutMeasurementTableProperties(tableName, status);
        free(tableName);
    }
    else
    {
        *status = INVALID_PARAMETER;
    }
    return transportStatus;
}


Status CallGetTableFields(const JsonNode* parameters, Status* status)
{
    Status transportStatus = OK;
    char* tableName;
    if (AllocateString(parameters, TABLE_NAME, &tableName) == JSON_OK)
    {
        transportStatus = PutMeasurementTableFields(tableName, status);
        free(tableName);
    }
    else
    {
        *status = INVALID_PARAMETER;
    }
    return transportStatus;
}


Status CallSetTableEnabled(const JsonNode* parameters, Status* status)
{
    char* tableName;
    bool enabled;
    if ((GetBoolean(parameters, ENABLED, &enabled) == JSON_OK) && (AllocateString(parameters, TABLE_NAME, &tableName) == JSON_OK))
    {
        *status = SetMeasurementTableEnabled(tableName, enabled);
        free(tableName);
    }
    else
    {
        *status = INVALID_PARAMETER;
    }
    return OK;
}


Status CallSetTableField(const JsonNode* parameters, Status* status)
{
    char* tableName;
    int column, row;
    float value;
    if ((GetInt(parameters, COLUMN, &column) == JSON_OK) &&
        (GetInt(parameters, ROW, &row) == JSON_OK) && 
        (GetFloat(parameters, VALUE, &value) == JSON_OK) && 
        (AllocateString(parameters, TABLE_NAME, &tableName) == JSON_OK))
    {
        *status = SetMeasurementTableField(tableName, column, row, value);
        free(tableName);
    }
    else
    {
        *status = INVALID_PARAMETER;
    }
    return OK;
}


Status CallGetEngineProperties(const JsonNode* parameters, Status* status)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonMemberName(DEFAULT_CHANNEL, RETURN_VALUE));
    VALIDATE(WriteJsonObjectStart(DEFAULT_CHANNEL));
    VALIDATE(WriteJsonIntegerMember(DEFAULT_CHANNEL, CYLINDER_COUNT, GetCylinderCount()));
    VALIDATE(PutCogwheel());
    VALIDATE(PutDeadPoints());
    return WriteJsonObjectEnd(DEFAULT_CHANNEL);
}


Status CallSetCylinderCount(const JsonNode* parameters, Status* status)
{
    int count;
    if (GetInt(parameters, CYLINDER_COUNT, &count) == JSON_OK)
    {
        *status = SetCylinderCount(count);
    }
    else
    {
        *status = INVALID_PARAMETER;
    }
    return OK;
}


Status CallSetCogwheelProperties(const JsonNode* parameters, Status* status)
{
    int cogTotal, gapSize, offset;
    if ((GetInt(parameters, COG_TOTAL, &cogTotal) == JSON_OK) && (GetInt(parameters, GAP_SIZE, &gapSize) == JSON_OK) && (GetInt(parameters, OFFSET, &offset) == JSON_OK))
    {
        *status = SetGogwheelProperties(cogTotal, gapSize, offset);
    }
    else
    {
        *status = INVALID_PARAMETER;
    }
    return OK;
}


Status CallGetPersistentElements(const JsonNode* parameters, Status* status)
{
    RETURN_WHEN_INVALID
    VALIDATE(PutPersistentElements(status));
    return WriteJsonObjectEnd(DEFAULT_CHANNEL);
}


Function functions[] =
{
    { "GetMeasurements", &CallGetMeasurements },
    { "GetMeasurementProperties", &CallGetMeasurementProperties },
    { "SetMeasurementSimulation", &CallSetMeasurementSimulation },
    { "ResetMeasurementSimulation", &CallResetMeasurementSimulation },
    { "GetTableNames", &CallGetTableNames },
    { "GetTableProperties", &CallGetTableProperties },
    { "GetTableFields", &CallGetTableFields },
    { "SetTableEnabled", &CallSetTableEnabled },
    { "SetTableField", &CallSetTableField },
    { "GetEngineProperties", &CallGetEngineProperties },
    { "SetCylinderCount", &CallSetCylinderCount },
    { "SetCogwheelProperties", &CallSetCogwheelProperties },
    { "GetPersistentElements", &CallGetPersistentElements }
};

#define FUNCTION_COUNT (sizeof(functions) / sizeof(Function))


JsonStatus EmptyObject(JsonNode* node)
{
    node->source = "{}";
    node->type = JSON_OBJECT;
    node->length = 2;
    return JSON_OK;
}


Function* FindFunction(const char* name) 
{
    int i;
    for (i = 0; i < FUNCTION_COUNT; ++i)
    {
        if (strcmp(name, functions[i].name) == 0)
        {
            return &(functions[i]);
        }
    }
    return NULL;
}


Status HandleCall(const JsonNode* message, Status* error) 
{
    Status transportStatus = OK;
    char* functionName;
    if (AllocateString(message, FUNCTION, &functionName) == JSON_OK)
    {
        transportStatus = WriteJsonStringMember(DEFAULT_CHANNEL, FUNCTION, functionName);
        Function* function = FindFunction(functionName);
        if ((transportStatus == OK) && (function != NULL))
        {
            JsonNode parameters;
            JsonStatus jsonStatus = GetObject(message, PARAMETERS, &parameters);
            if (jsonStatus == JSON_NAME_NOT_PRESENT)
            {
                jsonStatus = EmptyObject(&parameters);
            }
            if (jsonStatus == JSON_OK) 
            {
                transportStatus = function->Call(&parameters, error);
            }
            free(functionName);
        }
        else 
        {
            *error = "UnknownFunction";
        }
    }
    else
    {
        *error = "NoFunction";
    }
    return transportStatus;
}


/*
** Interface
*/

Status HandleMessage(const char* jsonString)
{
    JsonNode message;
    char* direction;
    Status status = UNINITIALIZED;
    Status transportStatus = WriteJsonRootStart(DEFAULT_CHANNEL);
    if (transportStatus == OK)
    {
        Initialize(jsonString, &message);
        if (AllocateString(&message, DIRECTION, &direction) == JSON_OK)
        {
            if (strcmp(direction, CALL) == 0)
            {
                transportStatus = WriteJsonStringMember(DEFAULT_CHANNEL, DIRECTION, RETURN);
                if (transportStatus == OK) 
                {
                    transportStatus = HandleCall(&message, &status);
                }
            }
            else
            {
                transportStatus = WriteJsonStringMember(DEFAULT_CHANNEL, DIRECTION, FIRE);
                status = "InvalidDirection";
            }
            free(direction);
        }
        else
        {
            transportStatus = WriteJsonStringMember(DEFAULT_CHANNEL, DIRECTION, FIRE);
            status =  "InvalidMessageReceived";
        }
        if ((transportStatus == OK) && (status != UNINITIALIZED))
        {
            transportStatus = WriteJsonStringMember(DEFAULT_CHANNEL, STATUS, status);
        }
        if (transportStatus == OK)
        {
            transportStatus = WriteJsonRootEnd(DEFAULT_CHANNEL);
        }
    }
    return transportStatus;
}


Status FireTextEvent(const char* name, const char* value)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonRootStart(DEFAULT_CHANNEL))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, DIRECTION, FIRE))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, name, value))
    return WriteJsonRootEnd(DEFAULT_CHANNEL);
}


Status FireIntegerEvent(const char* name, int value)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonRootStart(DEFAULT_CHANNEL))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, DIRECTION, FIRE))
    VALIDATE(WriteJsonIntegerMember(DEFAULT_CHANNEL, name, value))
    return WriteJsonRootEnd(DEFAULT_CHANNEL);
}


Status FireRealEvent(const char* name, double value)
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonRootStart(DEFAULT_CHANNEL))
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, DIRECTION, FIRE))
    VALIDATE(WriteJsonRealMember(DEFAULT_CHANNEL, name, value))
    return WriteJsonRootEnd(DEFAULT_CHANNEL);
}


Status FireErrorEvent(const char* error)
{
    return FireTextEvent(ERROR, error);
}
