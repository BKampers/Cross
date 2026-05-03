/*
** Interface for low power detection
** Copyright 2014, Bart Kampers
*/
#ifndef __VOLTAGEDETECTION_H__
#define __VOLTAGEDETECTION_H__

#include "Types.h"

void EnableLowVoltageDetection();
bool LowVoltageDetected();

#endif /* __VOLTAGEDETECTION_H__ */
