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
const char* RETURN_VALUE = "ReturnValue";
const char* NOTIFICATION = "Notification";

const char* PROCEDURE = "Procedure";
const char* REQUEST = "Request";
const char* MODIFY = "Modify";
const char* FUNCTION = "Function";
const char* PARAMETERS = "Parameters";

const char* DATA_TYPE = "DataType";
const char* MEASUREMENT = "Measurement";
const char* MEASUREMENT_TABLE = "MeasurementTable";
const char* ENGINE = "Engine";
const char* ELEMENTS = "Elements";

const char* INSTANCES = "Instances";
const char* ATTRIBUTES = "Attributes";
const char* VALUES = "Values";

const char* TABLE_NAME = "TableName";
const char* VALUE = "Value";
const char* SIMULATION = "Simulation";
const char* ROW = "Row";
const char* COLUMN = "Column";
const char* CURRENT_ROW = "CurrentRow";
const char* CURRENT_COLUMN = "CurrentColumn";
const char* TABLE = "Table";
const char* FIELDS = "Fields";
const char* ENABLED = "Enabled";

const char* CYLINDER_COUNT = "CylinderCount";
const char* DEAD_POINTS = "DeadPoints";
const char* COGWHEEL = "Cogwheel";
const char* COG_TOTAL = "CogTotal";
const char* GAP_SIZE = "GapSize";
const char* OFFSET = "Offset";

const char*  PERSISTENT = "Persistent";
const char* TYPE_ID = "TypeId";
const char* REFERENCE = "Reference";
const char* SIZE = "Size";

const char* ERROR = "Error";
const char* STATUS = "Status";


typedef struct
{
    const char* name;
    Status (*Call) (const JsonNode* parameters, Status* status);
} Function;


bool NameRequested(const JsonNode* attributes, const char* attributeName)
{
    int index;
    return (attributes == NULL) || (IndexOfString(attributes, attributeName, &index) == JSON_OK);
}


Status PutMeasurementValue(const Measurement* measurement, Status* error)
{
    float value;
    Status status = OK;
    Status measurementStatus = GetMeasurementValue(measurement, &value);
    if (measurementStatus == OK)
    {
        status = WriteJsonRealMember(DEFAULT_CHANNEL, VALUE, value);
    }
    else
    {
        *error = measurementStatus;
    }
    return status;
}


Status PutMeasurementAttributes(const Measurement* measurement, const JsonNode* attributes, Status* error)
{
    Status status = OK;
    if (NameRequested(attributes, VALUE))
    {
       status = PutMeasurementValue(measurement, error); 
    }
    if (NameRequested(attributes, SIMULATION))
    {
       status = WriteJsonBooleanMember(DEFAULT_CHANNEL, SIMULATION, measurement->simulationValue != NULL);
    }
    return status;
}


Status PutMeasurement(const JsonNode* message, const Measurement* measurement, Status* error)
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
                status = PutMeasurementAttributes(measurement, &attributes, error);
            }
            else if (jsonStatus == JSON_NAME_NOT_PRESENT)
            {
                status = PutMeasurementAttributes(measurement, NULL, error);
            }
        }
        if (status == OK)
        {
            status = WriteJsonObjectEnd(DEFAULT_CHANNEL);
        }
    }
    return status;
}


Status PutMeasurements(const JsonNode* message, Status* error)
{
    JsonNode instanceArray;
    Status status = OK;
    JsonStatus jsonStatus = GetArray(message, INSTANCES, &instanceArray);
    if ((jsonStatus == JSON_OK) || (jsonStatus == JSON_NAME_NOT_PRESENT))
    {
        JsonNode* instances = (jsonStatus == JSON_OK) ? &instanceArray : NULL;
        int count = GetMeasurementCount();
        int i;
        for (i = 0; (i < count) && (status == OK); ++i)
        {
            Measurement* measurement;
            if (GetMeasurement(i, &measurement) == OK)
            {
                if (NameRequested(instances, measurement->name))
                {
                    status = PutMeasurement(message, measurement, error);
                }
            }
        }
    }
    else
    {
        *error = "InvalidInstances";
    }
    return status;
}


Status HandleMeasurementRequest(const JsonNode* message, Status* error)
{
    Status status = WriteJsonMemberName(DEFAULT_CHANNEL, VALUES);
    if (status == OK)
    {
        status = WriteJsonObjectStart(DEFAULT_CHANNEL);
        if (status == OK)
        {
            status = PutMeasurements(message, error);
            if (status == OK)
            {
                status = WriteJsonObjectEnd(DEFAULT_CHANNEL);
            }
        }
    }
    return status;
}


Status PutMeasurmentTableFields(const MeasurementTable* measurementTable, Status* error) 
{
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
                    *error = valueStatus;
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
    return status; 
}


Status PutMeasurementTableAttributes(const MeasurementTable* measurementTable, const JsonNode* attributes, Status* error)
{
    Status status = OK;
    if (NameRequested(attributes, CURRENT_ROW))
    {
        status = WriteJsonIntegerMember(DEFAULT_CHANNEL, CURRENT_ROW, measurementTable->rowIndex);
    }
    if ((status == OK) && NameRequested(attributes, CURRENT_COLUMN))
    {
        status = WriteJsonIntegerMember(DEFAULT_CHANNEL, CURRENT_COLUMN, measurementTable->columnIndex);
    }
    if ((status == OK) && NameRequested(attributes, TABLE))
    {
        status = PutMeasurmentTableFields(measurementTable, error);
    }
    if ((status == OK) && NameRequested(attributes, ENABLED))
    {
        bool enabled;
        *error = GetMeasurementTableEnabled(measurementTable->name, &enabled);
        if (*error == OK)
        {
            status = WriteJsonBooleanMember(DEFAULT_CHANNEL, ENABLED, enabled);
        }
    }
    return status;
}


Status PutMeasurementTable(const JsonNode* message, const MeasurementTable* measurementTable, Status* error)
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
                status = PutMeasurementTableAttributes(measurementTable, &attributes, error);
            }
            else if (jsonStatus == JSON_NAME_NOT_PRESENT)
            {
                status = PutMeasurementTableAttributes(measurementTable, NULL, error);
            }
        }
    }
    if (status == OK)
    {
        status = WriteJsonObjectEnd(DEFAULT_CHANNEL);
    }
    return status;
}


Status PutMeasurementTables(const JsonNode* message, Status* error)
{
    JsonNode instanceArray;
    Status status = OK;
    JsonStatus jsonStatus = GetArray(message, INSTANCES, &instanceArray);
    if ((jsonStatus == JSON_OK) || (jsonStatus == JSON_NAME_NOT_PRESENT))
    {
        JsonNode* instances = (jsonStatus == JSON_OK) ? &instanceArray : NULL;
        int count = GetMeasurementTableCount();
        int i;
        for (i = 0; (i < count) && (status == OK); ++i)
        {
            MeasurementTable* measurementTable;
            if (GetMeasurementTable(i, &measurementTable) == OK)
            {
                if (NameRequested(instances, measurementTable->name))
                {
                    status = PutMeasurementTable(message, measurementTable, error);
                }
            }
        }
    }
    else
    {
        *error = "InvalidInstances";
    }
    return status;
}


Status HandleMeasurementTableRequest(const JsonNode* message, Status* error)
{
    Status status = WriteJsonMemberName(DEFAULT_CHANNEL, VALUES);
    if (status == OK)
    {
        status = WriteJsonObjectStart(DEFAULT_CHANNEL);
        if (status == OK)
        {
            status = PutMeasurementTables(message, error);
            if (status == OK)
            {
                status = WriteJsonObjectEnd(DEFAULT_CHANNEL);
            }
        }
    }
    return status;
}


Status PutDeadPoints()
{
    Status status = WriteJsonMemberName(DEFAULT_CHANNEL, DEAD_POINTS);
    if (status == OK)
    {
        status = WriteJsonArrayStart(DEFAULT_CHANNEL);
        if (status == OK)
        {
            int i;
            for (i = 0; (i < GetDeadPointCount()) && (status == OK); ++i)
            {
                status = WriteJsonIntegerElement(DEFAULT_CHANNEL, GetDeadPointCog(i));
            }
        }            
        if (status == OK)
        {
            status = WriteJsonArrayEnd(DEFAULT_CHANNEL);
        }
    }
    return status;
}


Status PutCogwheel()
{
    Status status = WriteJsonMemberName(DEFAULT_CHANNEL, COGWHEEL);
    if (status == OK)
    {
        status = WriteJsonObjectStart(DEFAULT_CHANNEL);
        if (status == OK)
        {
            status = WriteJsonIntegerMember(DEFAULT_CHANNEL, COG_TOTAL, GetCogTotal());
            if (status == OK)
            {
                status = WriteJsonIntegerMember(DEFAULT_CHANNEL, GAP_SIZE, GetGapSize());
                if (status == OK)
                {
                    status = WriteJsonIntegerMember(DEFAULT_CHANNEL, OFFSET, GetDeadPointOffset());
                    if (status == OK)
                    {
                        status = WriteJsonObjectEnd(DEFAULT_CHANNEL);
                    }
                }
            }
        }
    }
    return status;
}


Status PutEngineAttributes(const JsonNode* attributes)
{
    Status status = WriteJsonMemberName(DEFAULT_CHANNEL, "");
    if (status == OK)
    {
        status = WriteJsonObjectStart(DEFAULT_CHANNEL);
        if (status == OK) 
        {
            if (NameRequested(attributes, CYLINDER_COUNT))
            {
                status = WriteJsonIntegerMember(DEFAULT_CHANNEL, CYLINDER_COUNT, GetCylinderCount());
            }
            if ((status == OK) && NameRequested(attributes, DEAD_POINTS))
            {
                status = PutDeadPoints();
            }
            if ((status == OK) && NameRequested(attributes, COGWHEEL))
            {
                status = PutCogwheel();
            }
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
    Status status = WriteJsonMemberName(DEFAULT_CHANNEL, VALUES);
    if (status == OK)
    {
        status = WriteJsonObjectStart(DEFAULT_CHANNEL);
        if (status == OK)
        {
            JsonNode attributes;
            JsonStatus jsonStatus = GetArray(message, ATTRIBUTES, &attributes);
            if (jsonStatus == JSON_OK)
            {
                status = PutEngineAttributes(&attributes);
            }
            else if (jsonStatus == JSON_NAME_NOT_PRESENT)
            {
                status = PutEngineAttributes(NULL);
            }
        }    
        if (status == OK)
        {
            status = WriteJsonObjectEnd(DEFAULT_CHANNEL);
        }
    }
    return status;
}


Status PutElement(TypeId typeId, Reference reference, ElementSize size)
{
    Status status = WriteJsonObjectStart(DEFAULT_CHANNEL);
    if (status == OK)
    {
        status = WriteJsonIntegerMember(DEFAULT_CHANNEL, TYPE_ID, typeId);
        if (status == OK)
        {
            status = WriteJsonIntegerMember(DEFAULT_CHANNEL, REFERENCE, reference);
            if (status == OK)
            {
                status = WriteJsonIntegerMember(DEFAULT_CHANNEL, SIZE, size);
                if (status == OK)
                {
                    status = WriteJsonObjectEnd(DEFAULT_CHANNEL);
                }
            }
        }
    }
    return status;
}


Status PutElementTypes(TypeId typeId)
{
    Status status = OK;
    Reference reference;
    Status memoryStatus = FindFirst(typeId, &reference);
    while ((memoryStatus == OK) && (status == OK))
    {
        ElementSize size;
        memoryStatus = GetSize(reference, &size);
        if (memoryStatus == OK)
        {
            status = PutElement(typeId, reference, size);
            memoryStatus = FindNext(typeId, &reference);
        }
    }
    return status;
}


Status PutPersistentElements()
{
    Status status = WriteJsonMemberName(DEFAULT_CHANNEL, PERSISTENT);
    if (status == OK)
    {
        WriteJsonArrayStart(DEFAULT_CHANNEL);
        if (status == OK)
        {
            int count = GetTypeCount();
            int i;
            for (i = 0; (i < count) && (status == OK); ++i)
            {
                status = PutElementTypes((TypeId) i);
            }
            if (status == OK)
            {
                status = WriteJsonArrayEnd(DEFAULT_CHANNEL);
            }
        }
    }
    return status;
}


Status HandleElementsRequest(const JsonNode* message)
{
    Status status = WriteJsonMemberName(DEFAULT_CHANNEL, VALUES);
    if (status == OK)
    {
        status = WriteJsonObjectStart(DEFAULT_CHANNEL);
        if (status == OK)
        {
            JsonNode attributes;
            JsonStatus jsonStatus = GetArray(message, ATTRIBUTES, &attributes);
            if ((jsonStatus == JSON_NAME_NOT_PRESENT) || ((jsonStatus == JSON_OK) && NameRequested(&attributes, PERSISTENT)))
            {
                status = PutPersistentElements();
            }
            if (status == OK)
            {
                status = WriteJsonObjectEnd(DEFAULT_CHANNEL);
            }
        }
    }
    return status;
}


Status HandleRequest(const JsonNode* message, Status* error)
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
                    status = HandleMeasurementRequest(message, error);
                }
                else if (strcmp(dataType, MEASUREMENT_TABLE) == 0)
                {
                    status = HandleMeasurementTableRequest(message, error);
                }
                else if (strcmp(dataType, ENGINE) == 0)
                {
                    status = HandleEngineRequest(message);
                }
                else if (strcmp(dataType, ELEMENTS) == 0)
                {
                    status = HandleElementsRequest(message);
                }
                else
                {
                    *error = "InvalidDataType";
                }
            }
            free(dataType);
        }
        else
        {
            *error = "NoDataType";
        }
    }
    return status;
}


Status HandleMeasurementModification(const JsonNode* instances, const JsonNode* values, Status* error)
{
    return NOT_IMPLEMENTED;
}


void ModifyFields(const MeasurementTable* measurementTable, const JsonNode* values, Status* error) 
{
    JsonNode fields;
    if (GetArray(values, FIELDS, &fields) == JSON_OK)
    {
        int i = 0;
        JsonStatus next;
        do 
        {
            float value;
            int column, row;
            JsonNode field;
            next = GetObjectAt(&fields, i, &field);
            if (next == JSON_OK)
            {
                if ((GetInt(&field, COLUMN, &column) == JSON_OK) && (GetInt(&field, ROW, &row) == JSON_OK) && (GetFloat(&field, VALUE, &value) == JSON_OK))
                {
                    *error = SetMeasurementTableField(measurementTable->name, column, row, value);
                }
                else
                {
                    *error = "InvalidField";
                }
            }
            ++i;
        } while ((*error == OK) && (next == JSON_OK));
    }
}


void HandleMeasurementTableModification(const JsonNode* instances, const JsonNode* values, Status* error)
{
    int instanceCount;
    JsonStatus jsonStatus = GetCount(instances, &instanceCount);
    if ((jsonStatus == JSON_OK) && (instanceCount == 1))
    {
        char* tableName;
        jsonStatus = AllocateStringAt(instances, 0, &tableName);
        if (jsonStatus == JSON_OK)
        {
            MeasurementTable* measurementTable;
            *error = FindMeasurementTable(tableName, &measurementTable);
            if (*error == OK)
            {
                ModifyFields(measurementTable, values, error); 
            }
            free(tableName);
        }
    }
    else
    {
        *error = "InvalidInstanceCount";
    }
}


Status HandleEngineModification(const JsonNode* instances, const JsonNode* values, Status* error)
{
    return NOT_IMPLEMENTED;
}


Status HandleModify(const JsonNode* message, Status* error)
{
    Status status = WriteJsonStringMember(DEFAULT_CHANNEL, PROCEDURE, MODIFY);
    if (status == OK)
    {
        JsonNode instances;        
        if (GetArray(message, INSTANCES, &instances) == JSON_OK)
        {
            JsonNode values;
            if (GetObject(message, VALUES, &values) == JSON_OK)
            {
                char* dataType;
                if (AllocateString(message, DATA_TYPE, &dataType) == JSON_OK)
                {
                    status = WriteJsonStringMember(DEFAULT_CHANNEL, DATA_TYPE, dataType);
                    if (status == OK)
                    {
                        if (strcmp(dataType, MEASUREMENT) == 0)
                        {
                            status = HandleMeasurementModification(&instances, &values, error);
                        }
                        else if (strcmp(dataType, MEASUREMENT_TABLE) == 0)
                        {
                            HandleMeasurementTableModification(&instances, &values, error);
                        }
                        else if (strcmp(dataType, ENGINE) == 0)
                        {
                            status = HandleEngineModification(&instances, &values, error);
                        }
                        else
                        {
                            *error = "InvalidDataType";
                        }
                    }
                    free(dataType);
                }
                else
                {
                    *error = "NoDataType";
                }
            }
            else
            {
                *error = "NoValues";
            }
        }
        else
        {
            *error = "NoInstances";
        }
    }
    return status;
}


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


Status CallGetMeasurementTableNames(const JsonNode* parameters, Status* status)
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


Status CallGetMeasurementTableProperties(const JsonNode* parameters, Status* status)
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


Status CallGetMeasurementTableFields(const JsonNode* parameters, Status* status)
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


Status CallSetMeasurementTableEnabled(const JsonNode* parameters, Status* status)
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

Status CallSetMeasurementTableField(const JsonNode* parameters, Status* status)
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

Function functions[] =
{
    { "GetMeasurementTableNames", &CallGetMeasurementTableNames },
    { "GetMeasurementTableProperties", &CallGetMeasurementTableProperties },
    { "GetMeasurementTableFields", &CallGetMeasurementTableFields },
    { "SetMeasurementTableEnabled", &CallSetMeasurementTableEnabled },
    { "SetMeasurementTableField", &CallSetMeasurementTableField }
};

#define FUNCTION_COUNT (sizeof(functions) / sizeof(Function))


char* EMPTY_OBJECT = "{}";
JsonStatus EmptyObject(JsonNode* node)
{
    node->source = EMPTY_OBJECT;
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
    if (AllocateString(message, PROCEDURE, &functionName) == JSON_OK)
    {
        if (strcmp(functionName, REQUEST) == 0)
        {
            transportStatus = HandleRequest(message, error);
        }
        else if (strcmp(functionName, MODIFY) == 0)
        {
            transportStatus = HandleModify(message, error);
        }
        else
        {
            *error = "InvalidProcedure";
        }
        free(functionName);
    }
    else if (AllocateString(message, FUNCTION, &functionName) == JSON_OK)
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

void HandleMessage(const char* jsonString)
{
    JsonNode message;
    char* direction;
    Status error = UNINITIALIZED;
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
                    transportStatus = HandleCall(&message, &error);
                }
            }
            else
            {
                error = "InvalidDirection";
            }
            free(direction);
        }
        else
        {
            error =  "NoDirection";
        }
        if ((transportStatus == OK) && (error != UNINITIALIZED))
        {
            transportStatus = WriteJsonStringMember(DEFAULT_CHANNEL, ((error == OK) ? STATUS : ERROR), error);
        }
        if (transportStatus == OK)
        {
            transportStatus = WriteJsonRootEnd(DEFAULT_CHANNEL);
        }
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
