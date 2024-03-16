/*
 ** Engine properties
 ** Author: Bart Kampers
 */

#include "Engine.h"

#include <stdint.h>
#include <math.h>

#include "Configuration.h"
#include "PersistentElementManager.h"
#include "Crank.h"
#include "Ignition.h"
#include "InjectionTimer.h"


/*
** Private 
*/

typedef struct
{
    uint8_t cogTotal;
    uint8_t gapSize;
    uint8_t offset;
} Cogwheel;


typedef struct
{
    Cogwheel cogwheel;
    uint8_t cylinderCount;
} Engine;


char ENGINE_IS_RUNNING[] = "EngineIsRunning";

int deadPointCogs[DEAD_POINT_MAX] = { 0, 0, 0, 0 };


Engine engine;
TypeId engineTypeId = INVALID_TYPE_ID;


void InitDefaultCogwheel()
{
    engine.cogwheel.cogTotal = GetDefaultCogTotal();
    engine.cogwheel.gapSize = GetDefaultGapSize();
    engine.cogwheel.offset = GetDefaultDeadPointOffset();
}

int ValidCog(int number)
{
    int total = GetCogTotal();
    int effective = GetEffectiveCogCount();
    number %= total;
    if (number == 0)
    {
        return 1;
    }
    if (number <= effective)
    {
        return number;
    }
    if (total - number < number - effective)
    {
        return 1;
    }
    return effective;
}


void DeadPointCallback(int cogNumber)
{
    StartIgnition(cogNumber);
    StartInjection(cogNumber);
}


Status InitDeadPointCallbacks()
{
    Status status = OK;
    int count = GetDeadPointCount();
    int i;
    InitCrankCallbacks();
    for (i = 0; (i < count) && (status == OK); ++i)
    {
        int cogNumber = GetDeadPointCog(i);
        deadPointCogs[i] = cogNumber;
        status = SetCogCountCallback(&DeadPointCallback, cogNumber);
    }
    return status;
}


bool ValidCogwheelProperties(int cogTotal, int gapSize, int offset)
{
	return
        ((3 <= cogTotal) && (cogTotal <= 255) && (1 <= gapSize) && (gapSize <= cogTotal - 2) && (1 <= offset) && (offset < cogTotal - gapSize)) ||
        ((2 <= cogTotal) && (cogTotal <= 255) && (gapSize == 0));
}


Status StoreEngine()
{
    Status status = InitDeadPointCallbacks();
    if (status == OK)
    {
        Reference reference;
        status = FindFirst(engineTypeId, &reference);
        if (status == OK)
        {
            status = StoreElement(&engine, reference, sizeof(engine));
        }
    }
    return status;
}


/*
** Interface
*/

void RegisterEngineType()
{
    RegisterType(&engineTypeId);
}


Status InitEngine()
{
    Reference reference;
    Status status = FindFirst(engineTypeId, &reference);
	if (status == OK)
	{
		status = GetElement(reference, sizeof(engine), &engine);
	}
	else
	{
		InitDefaultCogwheel();
	    engine.cylinderCount = GetDefaultCylinderCount();
		status = AllocateElement(engineTypeId, sizeof(engine), &reference);
		if (status == OK)
		{
			status = StoreElement(&engine, reference, sizeof(engine));
		}
    }
	if (!IsCogwheelMutable())
	{
		InitDefaultCogwheel();
	}
    InitCrank();
    if (status == OK)
    {
        status = InitDeadPointCallbacks();
    }
    return status;
}


int GetCogTotal()
{
    return engine.cogwheel.cogTotal;
}


int GetGapSize()
{
    return engine.cogwheel.gapSize;
}


int GetEffectiveCogCount()
{
    return engine.cogwheel.cogTotal - engine.cogwheel.gapSize;
}


int GetDeadPointOffset()
{
    return engine.cogwheel.offset;
}


int GetDeadPointCount()
{
	if (engine.cogwheel.gapSize == 0)
	{
		return 0;
	}
    switch (engine.cylinderCount)
    {
        case 8:
            return 4;
        case 6:
            return 3;
        default:
            return 2;
    }
}


int GetDeadPointIndex(int cogNumber)
{
    int i;
    for (i = 0; i < DEAD_POINT_MAX; ++i)
    {
        if (deadPointCogs[i] == cogNumber)
        {
            return i;
        }
    }
    return -1;
}


int GetDeadPointCog(int index)
{
    return ValidCog(engine.cogwheel.offset + roundf((float) engine.cogwheel.cogTotal / GetDeadPointCount() * index));
}


int GetCylinderCount()
{
    return engine.cylinderCount;
}


Status SetGogwheelProperties(int cogTotal, int gapSize, int offset)
{
	if (!IsCogwheelMutable())
	{
		return DISABLED;
	}
    if (EngineIsRunning())
    {
        return ENGINE_IS_RUNNING;
    }
    if (!ValidCogwheelProperties(cogTotal, gapSize, offset))
    {
    	return "InvalidCogwheelProperties";
	}
	engine.cogwheel.cogTotal = (uint8_t) cogTotal;
	engine.cogwheel.gapSize = (uint8_t) gapSize;
	engine.cogwheel.offset = (uint8_t) offset;
	return StoreEngine();
}


Status SetCylinderCount(int count)
{
    if (EngineIsRunning())
    {
        return ENGINE_IS_RUNNING;
    }
    if ((count != 4) && (count != 6) && (count != 8))
    {
        return "InvalidCylinderCount";
    }
    engine.cylinderCount = (uint8_t) count;
    return StoreEngine();
}
