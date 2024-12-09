#include "Measurements.h"

#include <stdlib.h>
#include <string.h>

#include "AnalogInput.h"


/*
** Private
*/

const char TEMPERATURE[] = "Temperature";


Status GetTemperatureValue(float* value);


Measurement measurements[] =
{ /*  Name          GetValue             format  minimum   maximum  simulation*/
    { TEMPERATURE, &GetTemperatureValue, "%3.1f",   0.0f,   100.0f, NULL }
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


Status GetWaterTemperatureValue(float* value)
{
    return GetAnalogMeasurement(1, value);
}


Status GetAirTemperatureValue(float* value)
{
    return GetAnalogMeasurement(2, value);
}


Status GetBatteryVoltageValue(float* value)
{
    return GetAnalogMeasurement(3, value);
}


Status GetMapSensorValue(float* value)
{
    return GetAnalogMeasurement(4, value);
}


Status GetLambdaValue(float* value)
{
    return GetAnalogMeasurement(5, value);
}


Status GetAux1Value(float* value)
{
    return GetAnalogMeasurement(6, value);
}


Status GetAux2Value(float* value)
{
    return GetAnalogMeasurement(7, value);
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
