#ifndef _MEASUREMENTS_H_
#define _MEASUREMENTS_H_

#include "ApiStatus.h"


extern const char TEMPERATURE[];

typedef struct
{
    const char* name;
    Status (*GetValue) (float* value);
    const char* format;
    float minimum;
    float maximum;
    float* simulationValue;
} Measurement;


int GetMeasurementCount();
Status GetMeasurement(int index, Measurement** measurement);
Status FindMeasurement(const char* name, Measurement** measurement);

Status GetMeasurementValue(const Measurement* measurement, float* value);
float GetMeasurementRange(const Measurement* measurement);

Status SetMeasurementSimulation(const char* name, float value);
Status ResetMeasurementSimulation(const char* name);

#endif /* _MEASUREMENTS_H_ */
