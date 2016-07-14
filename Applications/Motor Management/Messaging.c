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

const char* MEASUREMENT_NAME = "MeasurementName";
const char* TABLE_NAME = "TableName";
const char* COLUMN_MEASUREMENT_NAME = "ColumnMeasurementName";
const char* ROW_MEASUREMENT_NAME = "RowMeasurementName";
const char* FIELDS = "Fields";
const char* VALUE = "Value";
const char* SIMULATION = "Simulation";
const char* COLUMN = "Column";
const char* ROW = "Row";
const char* CURRENT_COLUMN = "CurrentColumn";
const char* CURRENT_ROW = "CurrentRow";
const char* ENABLED = "Enabled";
const char* MINIMUM = "Minimum";
const char* MAXIMUM = "Maximum";
const char* PRECISION = "Precision";
const char* DECIMALS = "Decimals";
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


bool FitsByte(int value)
{
    return (0x00 <= value) && (value <= 0xFF);
}


bool FitsWord(int value)
{
    return (0x0000 <= value) && (value <= 0xFFFF);
}


Status PutMeasurementSimulation(const char* measurementName, Status* status)
{
    RETURN_WHEN_INVALID
    Measurement* measurement;
    *status = FindMeasurement(measurementName, &measurement);
    VALIDATE(WriteJsonObjectStart(DEFAULT_CHANNEL));
    if (*status == OK)
    {
        if (measurement->simulationValue != NULL)
        {
            VALIDATE(WriteJsonRealMember(DEFAULT_CHANNEL, SIMULATION_VALUE, *(measurement->simulationValue)));
            VALIDATE(WriteJsonBooleanMember(DEFAULT_CHANNEL, SIMULATION, TRUE));
        }
        else
        {
            VALIDATE(WriteJsonBooleanMember(DEFAULT_CHANNEL, SIMULATION, FALSE));
        }
    }
    return WriteJsonObjectEnd(DEFAULT_CHANNEL);
}


Status PutMeasurementProperties(const char* measurementName, Status* status)
{
    RETURN_WHEN_INVALID
    Measurement* measurement;
    VALIDATE(WriteJsonObjectStart(DEFAULT_CHANNEL));
    *status = FindMeasurement(measurementName, &measurement);
    if (*status == OK)
    {
        VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, MEASUREMENT_NAME, measurementName));
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


Status PutMeasurementTableProperties(const char* tableName, Status* status)
{
    RETURN_WHEN_INVALID
    MeasurementTable* measurementTable;
    VALIDATE(WriteJsonObjectStart(DEFAULT_CHANNEL));
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, TABLE_NAME, tableName));
    *status = FindMeasurementTable(tableName, &measurementTable);
    if (*status == OK)
    {
        bool enabled;
        *status = GetMeasurementTableEnabled(tableName, &enabled);
        if (*status == OK)
        {
            VALIDATE(WriteJsonBooleanMember(DEFAULT_CHANNEL, ENABLED, enabled));
        }
        VALIDATE(WriteJsonRealMember(DEFAULT_CHANNEL, MINIMUM, measurementTable->minimum));
        VALIDATE(WriteJsonRealMember(DEFAULT_CHANNEL, MAXIMUM, measurementTable->maximum));
        VALIDATE(WriteJsonRealMember(DEFAULT_CHANNEL, PRECISION, measurementTable->precision));
        VALIDATE(WriteJsonIntegerMember(DEFAULT_CHANNEL, DECIMALS, measurementTable->decimals));
        if (measurementTable->columnMeasurement != NULL)
        {
            VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, COLUMN_MEASUREMENT_NAME, measurementTable->columnMeasurement->name));
        }
        if (measurementTable->rowMeasurement != NULL)
        {
            VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, ROW_MEASUREMENT_NAME, measurementTable->rowMeasurement->name));
        }
    }
    return (WriteJsonObjectEnd(DEFAULT_CHANNEL));    
}


Status PutMeasurementTableActualValues(const char* tableName, Status* status)
{
    RETURN_WHEN_INVALID
    MeasurementTable* measurementTable;
    VALIDATE(WriteJsonObjectStart(DEFAULT_CHANNEL));
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, TABLE_NAME, tableName));
    *status = FindMeasurementTable(tableName, &measurementTable);
    if (*status == OK)
    {
        VALIDATE(WriteJsonIntegerMember(DEFAULT_CHANNEL, CURRENT_ROW, measurementTable->rowIndex));
        VALIDATE(WriteJsonIntegerMember(DEFAULT_CHANNEL, CURRENT_COLUMN, measurementTable->columnIndex));
    }
    return (WriteJsonObjectEnd(DEFAULT_CHANNEL));    
}


Status PutMeasurementTableFields(const char* tableName, Status* status)
{
    RETURN_WHEN_INVALID
    MeasurementTable* measurementTable;
    VALIDATE(WriteJsonObjectStart(DEFAULT_CHANNEL));
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, TABLE_NAME, tableName))
    VALIDATE(WriteJsonMemberName(DEFAULT_CHANNEL, FIELDS));
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
    VALIDATE(WriteJsonArrayEnd(DEFAULT_CHANNEL));
    return (WriteJsonObjectEnd(DEFAULT_CHANNEL));
}


Status PutMeasurementTableField(const char* tableName, int column, int row, float value) {
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonObjectStart(DEFAULT_CHANNEL));
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, TABLE_NAME, tableName));
    VALIDATE(WriteJsonIntegerMember(DEFAULT_CHANNEL, COLUMN, column));
    VALIDATE(WriteJsonIntegerMember(DEFAULT_CHANNEL, ROW, row));
    VALIDATE(WriteJsonRealMember(DEFAULT_CHANNEL, VALUE, value));
    return WriteJsonObjectEnd(DEFAULT_CHANNEL);
}


Status PutMeasurementTableEnabled(const char* tableName, bool enabled) {
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonObjectStart(DEFAULT_CHANNEL));
    VALIDATE(WriteJsonStringMember(DEFAULT_CHANNEL, TABLE_NAME, tableName));
    VALIDATE(WriteJsonBooleanMember(DEFAULT_CHANNEL, ENABLED, enabled));
    return WriteJsonObjectEnd(DEFAULT_CHANNEL);
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


Status PutEngineProperties()
{
    RETURN_WHEN_INVALID
    VALIDATE(WriteJsonObjectStart(DEFAULT_CHANNEL));
    VALIDATE(WriteJsonIntegerMember(DEFAULT_CHANNEL, CYLINDER_COUNT, GetCylinderCount()));
    VALIDATE(PutCogwheel());
    VALIDATE(PutDeadPoints());
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


Status CallGetMeasurements(const JsonNode* parameters, Status* status)
{
    RETURN_WHEN_INVALID
    int i;
    int count = GetMeasurementCount();
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
    Status transportStatus;
    char* measurementName;
    if (AllocateString(parameters, MEASUREMENT_NAME, &measurementName) == JSON_OK)
    {
        transportStatus = PutMeasurementProperties(measurementName, status);
        free(measurementName);
    }
    else
    {
        transportStatus = WriteJsonNull(DEFAULT_CHANNEL);
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
    return PutMeasurementSimulation(measurementName, status);
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
    return PutMeasurementSimulation(measurementName, status);
}


Status CallGetTableNames(const JsonNode* parameters, Status* status)
{
    RETURN_WHEN_INVALID
    int count = GetMeasurementTableCount();
    int i;
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
    Status transportStatus;
    char* tableName;
    if (AllocateString(parameters, TABLE_NAME, &tableName) == JSON_OK)
    {
        transportStatus = PutMeasurementTableProperties(tableName, status);
        free(tableName);
    }
    else
    {
        transportStatus = WriteJsonNull(DEFAULT_CHANNEL);
        *status = INVALID_PARAMETER;
    }
    return transportStatus;
}


Status CallGetTableActualValues(const JsonNode* parameters, Status* status)
{
    Status transportStatus;
    char* tableName;
    if (AllocateString(parameters, TABLE_NAME, &tableName) == JSON_OK)
    {
        transportStatus = PutMeasurementTableActualValues(tableName, status);
        free(tableName);
    }
    else
    {
        transportStatus = WriteJsonNull(DEFAULT_CHANNEL);
        *status = INVALID_PARAMETER;
    }
    return transportStatus;
}


Status CallGetTableFields(const JsonNode* parameters, Status* status)
{
    Status transportStatus;
    char* tableName;
    if (AllocateString(parameters, TABLE_NAME, &tableName) == JSON_OK)
    {
        transportStatus = PutMeasurementTableFields(tableName, status);
        free(tableName);
    }
    else
    {
        transportStatus = WriteJsonNull(DEFAULT_CHANNEL);
        *status = INVALID_PARAMETER;
  }
    return transportStatus;
}


Status CallSetTableEnabled(const JsonNode* parameters, Status* status)
{
    Status transportStatus;
    char* tableName;
    bool enabled;
    if ((GetBoolean(parameters, ENABLED, &enabled) == JSON_OK) && (AllocateString(parameters, TABLE_NAME, &tableName) == JSON_OK))
    {
        *status = SetMeasurementTableEnabled(tableName, enabled);
        transportStatus = PutMeasurementTableEnabled(tableName, enabled);
        free(tableName);
    }
    else
    {
        transportStatus = WriteJsonNull(DEFAULT_CHANNEL);
        *status = INVALID_PARAMETER;
    }
    return transportStatus;
}


Status CallSetTableField(const JsonNode* parameters, Status* status)
{
    Status transportStatus;
    char* tableName;
    int column, row;
    float value;
    if ((GetInt(parameters, COLUMN, &column) == JSON_OK) &&
        (GetInt(parameters, ROW, &row) == JSON_OK) && 
        (GetFloat(parameters, VALUE, &value) == JSON_OK) && 
        (AllocateString(parameters, TABLE_NAME, &tableName) == JSON_OK))
    {
        *status = SetMeasurementTableField(tableName, column, row, value);
        transportStatus = PutMeasurementTableField(tableName, column, row, value);
        free(tableName);
    }
    else
    {
        transportStatus = WriteJsonNull(DEFAULT_CHANNEL);
        *status = INVALID_PARAMETER;
    }
    return transportStatus;
}


Status CallGetEngineProperties(const JsonNode* parameters, Status* status)
{
    return PutEngineProperties();
}


Status CallIsEngineRunning(const JsonNode* parameters, Status* status)
{
    return WriteJsonBoolean(DEFAULT_CHANNEL, EngineIsRunning());
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
    return PutEngineProperties();
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
    return PutEngineProperties();
}


Status CallGetPersistentElements(const JsonNode* parameters, Status* status)
{
    RETURN_WHEN_INVALID
    int i;
    VALIDATE(WriteJsonArrayStart(DEFAULT_CHANNEL));
    int count = GetTypeCount();
    for (i = 1; i <= count; ++i)
    {
        VALIDATE(PutElementTypes((TypeId) i, status));
    }
    VALIDATE(WriteJsonArrayEnd(DEFAULT_CHANNEL));
    return WriteJsonObjectEnd(DEFAULT_CHANNEL);
}


#define MEMORY_BUFFER_SIZE 0x100
Status CallGetPersistentMemoryBytes(const JsonNode* parameters, Status* status)
{    
    RETURN_WHEN_INVALID
    byte bytes[MEMORY_BUFFER_SIZE];
    Reference limit = PersistentMemoryLimit();
    Reference reference = 0;
    *status = OK;
    VALIDATE(WriteJsonArrayStart(DEFAULT_CHANNEL));
    for (reference = 0; (reference < limit) && (*status == OK); reference += MEMORY_BUFFER_SIZE)
    {
        *status = ReadPersistentMemory(reference, MEMORY_BUFFER_SIZE, bytes);
        if (*status == OK)
        {
            int i;
            for (i = 0; i < MEMORY_BUFFER_SIZE; ++i)
            {
                VALIDATE(WriteJsonIntegerElement(DEFAULT_CHANNEL, bytes[i]));
            }
        }
    }
    VALIDATE(WriteJsonArrayEnd(DEFAULT_CHANNEL));
    return WriteJsonObjectEnd(DEFAULT_CHANNEL);
}


Status CallSetPersistentMemoryByte(const JsonNode* parameters, Status* status)
{
    int reference, value;
    if ((GetInt(parameters, REFERENCE, &reference) == JSON_OK) && (GetInt(parameters, VALUE, &value) == JSON_OK) && (0x0000 <= reference) && (reference <= 0xFFFF) && (0x00 <= value) && (value <= 0xFF))
    {
        *status = WritePersistentMemoryByte((Reference) reference, (byte) value);
    }
    else
    {
        *status = INVALID_PARAMETER;
    }
    return WriteJsonNull(DEFAULT_CHANNEL);
}



Status FillByteArray(const JsonNode* jsonArray, byte** byteArray, int count)
{
    int i;
    for (i = 0; i < count; ++i)
    {
        int value;
        if ((GetIntAt(jsonArray, i, &value) == JSON_OK) && FitsByte(value))
        {
            (*byteArray)[i] = (byte) value;
        }
        else
        {
            return "InvalidByte";
        }
    }
    return OK;
}


Status AllocateByteArray(const JsonNode* jsonArray, byte** byteArray, int* count)
{
    Status status = INVALID_PARAMETER;
    if (GetCount(jsonArray, count) == JSON_OK)
    {
        *byteArray = malloc(*count);
        if (*byteArray != NULL)
        {
            status = FillByteArray(jsonArray, byteArray, *count);
            if (status != OK)
            {
                free(*byteArray);
            }
        }
    }
    return status;
}


Status CallSetPersistentMemoryBytes(const JsonNode* parameters, Status* status)
{
    JsonNode values;
    int reference;
    if ((GetInt(parameters, REFERENCE, &reference) == JSON_OK) && FitsWord(reference) && (GetArray(parameters, VALUE, &values) == JSON_OK))
    {
        byte* buffer;
        int length;
        *status = AllocateByteArray(&values, &buffer, &length);
        if (*status == OK)
        {
            *status = WritePersistentMemory((Reference) reference, length, buffer);
            free(buffer);
        }           
    }
    else
    {
        *status = INVALID_PARAMETER;
    }
    return WriteJsonNull(DEFAULT_CHANNEL);
}


Function functions[] =
{
    { "GetMeasurements", &CallGetMeasurements },
    { "GetMeasurementProperties", &CallGetMeasurementProperties },
    { "SetMeasurementSimulation", &CallSetMeasurementSimulation },
    { "ResetMeasurementSimulation", &CallResetMeasurementSimulation },
    { "GetTableNames", &CallGetTableNames },
    { "GetTableActualValues", &CallGetTableActualValues },
    { "GetTableProperties", &CallGetTableProperties },
    { "GetTableFields", &CallGetTableFields },
    { "SetTableEnabled", &CallSetTableEnabled },
    { "SetTableField", &CallSetTableField },
    { "GetEngineProperties", &CallGetEngineProperties },
    { "IsEngineRunning", &CallIsEngineRunning },
    { "SetCylinderCount", &CallSetCylinderCount },
    { "SetCogwheelProperties", &CallSetCogwheelProperties },
    { "GetPersistentElements", &CallGetPersistentElements },
    { "GetPersistentMemoryBytes", &CallGetPersistentMemoryBytes },
    { "SetPersistentMemoryByte", &CallSetPersistentMemoryByte },
    { "SetPersistentMemoryBytes", &CallSetPersistentMemoryBytes }
};


/*
** Interface
*/

JsonRpcApi motorManagementApi =
{
    functions,
    sizeof(functions) / sizeof(Function)
};
