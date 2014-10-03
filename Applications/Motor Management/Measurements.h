#ifndef _MEASUREMENTS_H_
#define _MEASUREMENTS_H_

#include "ApiStatus.h"


extern const char RPM[];
extern const char LOAD[];
extern const char WATER_TEMPERATURE[];
extern const char AIR_TEMPERATURE[];
extern const char BATTERY_VOLTAGE[];
extern const char MAP_SENSOR[];
extern const char LAMBDA[];
extern const char AUX1[];
extern const char AUX2[];

typedef struct
{
    Status (*GetValue) (float* value);
    const char* format;
    float minimum;
    float maximum;
    float* simulationValue;
} Measurement;


Status FindMeasurement(const char* name, Measurement** measurement);

Status GetMeasurementValue(Measurement* measurement, float* value);

Status SetMeasurementSimulation(const char* name, float value);
Status ResetMeasurementSimulation(const char* name);

#endif /* _MEASUREMENTS_H_ */
