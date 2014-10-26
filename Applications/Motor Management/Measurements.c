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
const char WATER_TEMPERATURE[] = "Water";
const char AIR_TEMPERATURE[] = "Air";
const char BATTERY_VOLTAGE[] = "Battery";
const char MAP_SENSOR[] = "Map";
const char LAMBDA[] = "Lambda";
const char AUX1[] = "Aux1";
const char AUX2[] = "Aux2";


Status GetRpmValue(float* value);
Status GetLoadValue(float* value);
Status GetWaterTemperatureValue(float* value);
Status GetAirTemperatureValue(float* value);
Status GetBatteryVoltageValue(float* value);
Status GetMapSensorValue(float* value);
Status GetLambdaValue(float* value);
Status GetAux1Value(float* value);
Status GetMapAux2Value(float* value);


Measurement measurements[] =
{ /*  Name                 GetValue                 format  minimum   maximum  simulation*/
    { RPM,               &GetRpmValue,              "%5d",     0.0f, 10000.0f, NULL },
    { LOAD,              &GetLoadValue,             "%3.1f",   0.0f,   100.0f, NULL },
    { WATER_TEMPERATURE, &GetWaterTemperatureValue, "%3.1f",   0.0f,   100.0f, NULL },
    { AIR_TEMPERATURE,   &GetAirTemperatureValue,   "%3.1f", -50.0f,   200.0f, NULL },
    { BATTERY_VOLTAGE,   &GetBatteryVoltageValue,   "%2.1f",   0.0f,    30.0f, NULL },
    { MAP_SENSOR,        &GetMapSensorValue,        "%3.1f",   0.0f,   100.0f, NULL }
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


Status GetMapAux2Value(float* value)
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
        return measurement->GetValue(value);
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
