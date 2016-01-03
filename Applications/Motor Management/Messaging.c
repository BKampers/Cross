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
const char* NOTIFICATION = "Notification";

const char* PROCEDURE = "Procedure";
const char* REQUEST = "Request";
const char* MODIFY = "Modify";

const char* DATA_TYPE = "DataType";
const char* MEASUREMENT = "Measurement";
const char* MEASUREMENT_TABLE = "MeasurementTable";
const char* ENGINE = "Engine";
const char*  ELEMENTS = "Elements";

const char* INSTANCES = "Instances";
const char* ATTRIBUTES = "Attributes";
const char* VALUES = "Values";

const char* VALUE = "Value";
const char* SIMULATION = "Simulation";
const char* CURRENT_ROW = "CurrentRow";
const char* CURRENT_COLUMN = "CurrentColumn";
const char* TABLE = "Table";

const char* CYLINDER_COUNT = "CylinderCount";
const char* DEAD_POINTS = "DeadPoints";
const char* COGWHEEL = "Cogwheel";
const char* COG_TOTAL = "CogTotal";
const char* GAP_SIZE = "GapSize";
const char* OFFSET = "Offset";

const char*  PERSISTENT = "Persistent";
const char* TYPE = "Type";
const char* REFERENCE = "Reference";
const char* SIZE = "Size";

const char* ERROR = "Error";
const char* STATUS = "Status";


bool NameRequested(const JsonNode* attributes, const char* attributeName)
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
        status =  WriteJsonStringMember(DEFAULT_CHANNEL, ERROR, status);
    }
    return status;
}


Status PutMeasurementAttributes(const Measurement* measurement, const JsonNode* attributes)
{
    Status status = OK;
    if (NameRequested(attributes, VALUE))
    {
       status = PutMeasurementValue(measurement); 
    }
    if (NameRequested(attributes, SIMULATION))
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
                    status = PutMeasurement(message, measurement);
                }
            }
        }
    }
    else
    {
        status = WriteJsonStringMember(DEFAULT_CHANNEL, ERROR, "InvalidInstances");
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
    if (NameRequested(attributes, CURRENT_ROW))
    {
        status = WriteJsonIntegerMember(DEFAULT_CHANNEL, CURRENT_ROW, measurementTable->rowIndex);
    }
    if (NameRequested(attributes, CURRENT_COLUMN))
    {
        status = WriteJsonIntegerMember(DEFAULT_CHANNEL, CURRENT_COLUMN, measurementTable->columnIndex);
    }
    if (NameRequested(attributes, TABLE))
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
                    status = PutMeasurementTable(message, measurementTable);
                }
            }
        }
    }
    else
    {
        status = WriteJsonStringMember(DEFAULT_CHANNEL, ERROR, "InvalidInstances");
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
                        status = WriteJsonObjectStart(DEFAULT_CHANNEL);
                    }
                }
            }
        }
    }
    return status;
}


Status PutEngineAttributes(const JsonNode* attributes)
{
    Status status = OK;
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
    return status;
}


Status HandleEngineRequest(const JsonNode* message)
{
    Status status = WriteJsonMemberName(DEFAULT_CHANNEL, VALUE);
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
        status = WriteJsonIntegerMember(DEFAULT_CHANNEL, TYPE, typeId);
        if (status == OK)
        {
            status = WriteJsonIntegerMember(DEFAULT_CHANNEL, REFERENCE, reference);
            if (status == OK)
            {
                status = WriteJsonIntegerMember(DEFAULT_CHANNEL, SIZE, size);
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
        JsonNode attributes;
        JsonStatus jsonStatus = GetArray(message, ATTRIBUTES, &attributes);
        if ((jsonStatus == JSON_OK) && NameRequested(&attributes, PERSISTENT))
        {
             status = PutPersistentElements();
        }
        if (status == OK)
        {
            status = WriteJsonObjectEnd(DEFAULT_CHANNEL);
        }
    }
    return status;
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
                else if (strcmp(dataType, ELEMENTS) == 0)
                {
                    status = HandleElementsRequest(message);
                }
                else
                {
                    status = WriteJsonStringMember(DEFAULT_CHANNEL, ERROR, "InvalidDataType");
                }
            }
            free(dataType);
        }
        else
        {
            status = WriteJsonStringMember(DEFAULT_CHANNEL, ERROR, "INoDataType");
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
            status = WriteJsonStringMember(DEFAULT_CHANNEL, ERROR, "InvalidProcedure");
        }
        free(procedure);
    }
    else
    {
        status = WriteJsonStringMember(DEFAULT_CHANNEL, ERROR, "NoProcedure");
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
