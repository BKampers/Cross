#include "Measurements.h"

#include <stdlib.h>
#include <string.h>

#include "Crank.h"
#include "AnalogInput.h"


/*
** Private
*/

const char RPM[] = "RPM";
const char LOAD[] = "Load";
const char MAP_SENSOR[] = "Map";
const char SPARE[] = "Spare";


Status GetRpmValue(float* value);
Status GetLoadValue(float* value);
Status GetMapSensorValue(float* value);
Status GetSpareValue(float* value);


Measurement measurements[] =
{ /*  Name                 GetValue                 format  minimum   maximum  simulation*/
    { RPM,               &GetRpmValue,              "%5d",     0.0f, 10000.0f, NULL },
    { LOAD,              &GetLoadValue,             "%3.1f",   0.0f,   100.0f, NULL },
    { MAP_SENSOR,        &GetMapSensorValue,        "%3.1f",   0.0f,   100.0f, NULL },
    { SPARE,             &GetSpareValue,            "%3.1f",   0.0f,   100.0f, NULL }
};

#define MEASUREMENT_COUNT (sizeof(measurements) / sizeof(Measurement))


Status GetAnalogMeasurement(int index, float* value)
{
    Measurement* measurement = &(measurements[index + 1]);
    int adcValue;
    Status status = GetAnalogValue(index, &adcValue);
    if (status == OK)
    {
        *value = measurement->minimum + (measurement->maximum - measurement->minimum) / ADC_MAX * adcValue;
    }
    return status;
}


Status GetLoadValue(float* value)
{
    return GetAnalogMeasurement(0, value);
}


Status GetMapSensorValue(float* value)
{
    return GetAnalogMeasurement(1, value);
}


Status GetSpareValue(float* value)
{
    return GetAnalogMeasurement(2, value);
}


Status GetRpmValue(float* value)
{
    *value = GetRpm();
    return OK;
}


/*
** Interface
*/


int GetMeasurementCount()
{
    return MEASUREMENT_COUNT;
}


Status GetMeasurement(int index, Measurement** measurement)
{
    if ((0 <= index) && (index < MEASUREMENT_COUNT))
    {
        *measurement = &(measurements[index]);
        return OK;
    }
    else
    {
        return "InvalidMeasurementIndex";
    }
}


Status FindMeasurement(const char* name, Measurement** measurement)
{
    int i;
    for (i = 0; i < MEASUREMENT_COUNT; ++i)
    {
        if (strcmp(name, measurements[i].name) == 0)
        {
            *measurement = &(measurements[i]);
            return OK;
        }
    }
    return "NoSuchMeasurement";
}


float GetMeasurementRange(const Measurement* measurement)
{
    return measurement->maximum - measurement->minimum;
}


Status GetMeasurementValue(const Measurement* measurement, float* value)
{
    if (measurement->simulationValue == NULL)
    {
        float measurementValue;
        Status status = measurement->GetValue(&measurementValue);
        if (status == OK)
        {
            *value = maxf(measurement->minimum, minf(measurementValue, measurement->maximum));
        }
        return status;
    }
    else
    {
        *value = *(measurement->simulationValue);
        return OK;
    }
}


Status SetMeasurementSimulation(const char* name, float value)
{
    Measurement* measurement;
    Status status = FindMeasurement(name, &measurement);
    if (status == OK)
    {
        if (measurement->simulationValue == NULL)
        {
            measurement->simulationValue = malloc(sizeof(float));
        }
        *(measurement->simulationValue) = value;
    }
    return status;
}


Status ResetMeasurementSimulation(const char* name)
{
    Measurement* measurement;
    Status status = FindMeasurement(name, &measurement);
    if (status == OK)
    {
        if (measurement->simulationValue != NULL)
        {
            free(measurement->simulationValue);
            measurement->simulationValue = NULL;
        }
    }
    return status;
}
